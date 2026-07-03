<#
.SYNOPSIS
    生成 clang-tidy 配置并对项目源码运行 clang-tidy。

.DESCRIPTION
    此脚本用于在当前项目中批量运行 clang-tidy。

    脚本启动后会先读取 scripts/clang-tidy/clang-tidy.fon，并调用
    scripts/generate_clang_tidy.py 生成 scripts/clang-tidy/.clang-tidy。
    clang-tidy.fon 中的 Checks 以数组形式维护，生成 .clang-tidy 时会展开为
    clang-tidy 兼容的 YAML 折叠块格式。

    运行 clang-tidy 时，脚本会通过 --config-file 显式指定
    scripts/clang-tidy/.clang-tidy；不会使用项目根目录下的 .clang-tidy。

    生成配置后，脚本会查找指定构建目录中的 compile_commands.json，并基于该
    编译数据库运行 clang-tidy。默认构建目录为 cmake-build-dev。

    如果传入的是源文件路径，脚本只检查该文件。
    如果传入的是目录路径，脚本会把它当作构建目录。
    如果不传参数，脚本会递归检查 src/ 下的所有 .cpp、.cc、.cxx 文件。

    脚本不会执行 CMake 配置、构建或编译命令；它只依赖已有的
    compile_commands.json。

.PARAMETER Arg1
    可选参数。
    可以是要检查的单个源文件，也可以是包含 compile_commands.json 的构建目录。
    如果路径不存在，脚本会按构建目录处理。

.PARAMETER Arg2
    可选参数。
    当 Arg1 是单个源文件时，Arg2 用于指定构建目录。

.ENVIRONMENT
    CLANG_TIDY
        可选。指定 clang-tidy 可执行文件路径。
        未设置时使用 PATH 中的 clang-tidy。

    MAX_JOBS
        可选。指定并行运行的 clang-tidy 进程数量。
        未设置时默认为 8；小于 1 时按 1 处理。

    FON_ROOT
        可选。供 scripts/generate_clang_tidy.py 使用。
        指定 FON 工具目录。未设置时默认使用 E:\FON。

.EXAMPLE
    .\run_clang_tidy.ps1

    生成 .clang-tidy，然后使用 cmake-build-dev/compile_commands.json
    检查 src/ 下的所有 C++ translation units。

.EXAMPLE
    .\run_clang_tidy.ps1 src\main.cpp

    生成 .clang-tidy，然后只检查 src/main.cpp，
    构建目录使用默认的 cmake-build-dev。

.EXAMPLE
    .\run_clang_tidy.ps1 src\main.cpp cmake-build-release

    生成 .clang-tidy，然后只检查 src/main.cpp，
    编译数据库来自 cmake-build-release/compile_commands.json。

.EXAMPLE
    .\run_clang_tidy.ps1 cmake-build-release

    生成 .clang-tidy，然后检查 src/ 下的所有 C++ translation units，
    编译数据库来自 cmake-build-release/compile_commands.json。

.NOTES
    脚本会并行启动 clang-tidy，并在终端显示进度、诊断数量和失败数量。
    clang-tidy 的外部头文件统计摘要会被过滤；只有项目源码诊断或真实执行失败会输出。
    临时日志文件会写入系统临时目录，并在脚本结束时清理。
#>

param(
    [string]$Arg1 = "",
    [string]$Arg2 = ""
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = $ScriptDir

if ((Split-Path -Leaf $ScriptDir) -eq ".agents") {
    $CandidateRoot = Split-Path -Parent $ScriptDir
    if ((Test-Path -LiteralPath (Join-Path $CandidateRoot "src") -PathType Container) -and
        (Test-Path -LiteralPath (Join-Path $CandidateRoot "CMakeLists.txt") -PathType Leaf)) {
        $Root = $CandidateRoot
    }
}

$SrcDir = Join-Path $Root "src"
$BuildDir = Join-Path $Root "cmake-build-dev"
$TargetFile = $null
$CppSourceExtensions = @(".cpp", ".cc", ".cxx")
$AnsiEscapePattern = [string]([char]27) + "\[[0-9;]*[A-Za-z]"

function Resolve-ProjectPath([string]$Path) {
    if ([System.IO.Path]::IsPathRooted($Path)) {
        return [System.IO.Path]::GetFullPath($Path)
    }

    return [System.IO.Path]::GetFullPath((Join-Path $Root $Path))
}

function Get-DisplayPath([string]$Path) {
    $fullRoot = [System.IO.Path]::GetFullPath($Root).TrimEnd('\', '/')
    $fullPath = [System.IO.Path]::GetFullPath($Path)

    if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        $relative = $fullPath.Substring($fullRoot.Length).TrimStart('\', '/')
        return $relative.Replace("\", "/")
    }

    return $fullPath.Replace("\", "/")
}

function Quote-Arg([string]$Value) {
    if ($null -eq $Value) {
        return '""'
    }

    $escaped = $Value.Replace('\', '\\').Replace('"', '\"')
    return '"' + $escaped + '"'
}

function Test-CppSourceFile([string]$Path) {
    $extension = [System.IO.Path]::GetExtension($Path).ToLowerInvariant()
    return $CppSourceExtensions -contains $extension
}

function Get-ConsoleWidth {
    try {
        if ([Console]::BufferWidth -gt 0) {
            return [Console]::BufferWidth - 1
        }
    } catch {
    }

    return 100
}

function Clear-ProgressLine {
    if (-not $script:ProgressLineVisible) {
        return
    }

    $width = Get-ConsoleWidth
    [Console]::Write("`r" + (" " * $width) + "`r")
    $script:ProgressLineVisible = $false
}

function Remove-AnsiEscape([string]$Text) {
    return $Text -replace $AnsiEscapePattern, ""
}

function Test-ProjectDiagnosticPath([string]$Path) {
    $normalized = (Remove-AnsiEscape $Path).Replace("\", "/")
    return $normalized -match "(^|/)src/"
}

function Remove-ClangTidyNoise([string]$Text) {
    if ([string]::IsNullOrWhiteSpace($Text)) {
        return ""
    }

    $kept = New-Object System.Collections.Generic.List[string]
    $currentBlock = New-Object System.Collections.Generic.List[string]
    $currentBlockHasDiagnostic = $false
    $keepCurrentBlock = $true

    function Flush-DiagnosticBlock {
        if ($currentBlock.Count -eq 0) {
            return
        }

        if ($keepCurrentBlock) {
            foreach ($blockLine in $currentBlock) {
                [void]$kept.Add($blockLine)
            }
        }

        $currentBlock.Clear()
        Set-Variable -Name currentBlockHasDiagnostic -Value $false -Scope 1
    }

    foreach ($line in ($Text -split "`r?`n")) {
        $cleanLine = Remove-AnsiEscape $line

        if ([string]::IsNullOrWhiteSpace($cleanLine)) {
            continue
        }

        if ($cleanLine -match "^[0-9]+ warnings? generated\.$") {
            continue
        }

        if ($cleanLine -match "^Suppressed [0-9]+ warnings? ") {
            continue
        }

        if ($cleanLine -match "^Use -header-filter=") {
            continue
        }

        if ($cleanLine -match "^(In file included from |                 from )") {
            [void]$currentBlock.Add($line)
            continue
        }

        if ($cleanLine -match "^(.+):[0-9]+:[0-9]+: (warning|error): ") {
            if ($currentBlockHasDiagnostic) {
                Flush-DiagnosticBlock
            }

            $keepCurrentBlock = Test-ProjectDiagnosticPath $Matches[1]
            $currentBlockHasDiagnostic = $true
            [void]$currentBlock.Add($line)
            continue
        }

        if ($currentBlock.Count -gt 0) {
            [void]$currentBlock.Add($line)
        } else {
            [void]$kept.Add($line)
        }
    }

    Flush-DiagnosticBlock

    return ($kept -join [Environment]::NewLine).Trim()
}

if ($Arg1) {
    $p1 = Resolve-ProjectPath $Arg1

    if (Test-Path -LiteralPath $p1 -PathType Container) {
        $BuildDir = $p1
    } elseif (Test-Path -LiteralPath $p1 -PathType Leaf) {
        $TargetFile = $p1
    } else {
        $BuildDir = $p1
    }
}

if ($TargetFile -and $Arg2) {
    $BuildDir = Resolve-ProjectPath $Arg2
}

$ClangTidy = if ($env:CLANG_TIDY) { $env:CLANG_TIDY } else { "clang-tidy" }

$ClangTidyDir = Join-Path $Root "scripts\clang-tidy"
$ClangTidyFon = Join-Path $ClangTidyDir "clang-tidy.fon"
$ClangTidyConfig = Join-Path $ClangTidyDir ".clang-tidy"
$GenerateClangTidy = Join-Path $Root "scripts\generate_clang_tidy.py"

if (-not (Test-Path -LiteralPath $ClangTidyFon -PathType Leaf)) {
    Write-Error "clang-tidy FON config not found: $ClangTidyFon"
    exit 1
}

if (-not (Test-Path -LiteralPath $GenerateClangTidy -PathType Leaf)) {
    Write-Error "clang-tidy config generator not found: $GenerateClangTidy"
    exit 1
}

python $GenerateClangTidy $ClangTidyFon $ClangTidyConfig
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$MaxJobs = 8
if ($env:MAX_JOBS) {
    $parsedJobs = 0
    if ([int]::TryParse($env:MAX_JOBS, [ref]$parsedJobs)) {
        $MaxJobs = $parsedJobs
    }
}

if ($MaxJobs -lt 1) {
    $MaxJobs = 1
}

if (-not (Test-Path -LiteralPath $SrcDir -PathType Container)) {
    Write-Error "src directory not found: $SrcDir"
    exit 1
}

if ($TargetFile) {
    if (-not (Test-Path -LiteralPath $TargetFile -PathType Leaf)) {
        Write-Error "target file not found: $TargetFile"
        exit 1
    }

    if (-not (Test-CppSourceFile $TargetFile)) {
        Write-Error "target file is not a C++ source file: $TargetFile"
        exit 1
    }
}

$CompileCommands = Join-Path $BuildDir "compile_commands.json"
if (-not (Test-Path -LiteralPath $CompileCommands -PathType Leaf)) {
    Write-Host "compile_commands.json not found: $CompileCommands"
    Write-Host ""
    Write-Host "Usage:"
    Write-Host "  .\run_clang_tidy.ps1"
    Write-Host "  .\run_clang_tidy.ps1 src\main.cpp"
    Write-Host "  .\run_clang_tidy.ps1 src\main.cpp cmake-build-release"
    Write-Host "  .\run_clang_tidy.ps1 cmake-build-release"
    exit 1
}

$clangCommand = Get-Command $ClangTidy -ErrorAction SilentlyContinue
if (-not $clangCommand -and -not (Test-Path -LiteralPath $ClangTidy -PathType Leaf)) {
    Write-Error "clang-tidy not found. Set CLANG_TIDY to clang-tidy.exe path, or add it to PATH."
    exit 1
}

$Files = @()
if ($TargetFile) {
    $Files += (Get-Item -LiteralPath $TargetFile).FullName
} else {
    $Files += Get-ChildItem -LiteralPath $SrcDir -Recurse -File | Where-Object {
        Test-CppSourceFile $_.FullName
    } | ForEach-Object {
        $_.FullName
    }
}

$TotalFiles = $Files.Count
if ($TotalFiles -eq 0) {
    Write-Error "no translation units found in $SrcDir"
    exit 1
}

$JobDir = Join-Path ([System.IO.Path]::GetTempPath()) ("clang_tidy_{0}_{1}" -f $PID, ([guid]::NewGuid().ToString("N")))
New-Item -ItemType Directory -Path $JobDir | Out-Null

$Running = New-Object System.Collections.Generic.List[object]
$NextIndex = 0
$Completed = 0
$Failed = 0
$HadDiagnostic = 0
$IgnoredExternalOnlyFailures = 0
$ProgressLineVisible = $false
$PrintedReportThisTick = $false

function New-ClangTidyProcess([string]$File, [int]$Id) {
    $displayFile = Get-DisplayPath $File
    $stdoutFile = Join-Path $JobDir "$Id.stdout.log"
    $stderrFile = Join-Path $JobDir "$Id.stderr.log"

    $args = @(
        (Quote-Arg $displayFile),
        ("-p=" + (Quote-Arg $BuildDir)),
        ("--config-file=" + (Quote-Arg $ClangTidyConfig)),
        "--header-filter=.*[\\/]src[\\/].*",
        "--use-color",
        "--quiet",
        "--extra-arg=/Y-",
        "--extra-arg=-fcolor-diagnostics"
    )

    $proc = Start-Process `
        -FilePath $ClangTidy `
        -ArgumentList $args `
        -WorkingDirectory $Root `
        -RedirectStandardOutput $stdoutFile `
        -RedirectStandardError $stderrFile `
        -NoNewWindow `
        -PassThru

    [pscustomobject]@{
        Id = $Id
        File = $File
        DisplayFile = $displayFile
        StdoutFile = $stdoutFile
        StderrFile = $stderrFile
        Process = $proc
    }
}

function Write-ProgressLine {
    $percent = [int][math]::Floor(($Completed * 100.0) / $TotalFiles)
    $filled = [int][math]::Floor($percent / 5)
    if ($filled -gt 20) {
        $filled = 20
    }

    $empty = 20 - $filled
    $bar = ("#" * $filled) + ("-" * $empty)
    $line = "clang-tidy [$bar] $Completed/$TotalFiles $percent%, running: $($Running.Count), diagnostics: $HadDiagnostic, failures: $Failed"

    $width = Get-ConsoleWidth
    if ($line.Length -lt $width) {
        $line = $line + (" " * ($width - $line.Length))
    }

    [Console]::Write("`r$line")
    $script:ProgressLineVisible = $true
}

function Write-ClangTidyReport($job, [string]$Text, [bool]$IsFailure) {
    Clear-ProgressLine

    if ($script:Completed -gt 0 -or $script:HadDiagnostic -gt 0 -or $script:Failed -gt 0) {
        Write-Host ""
    }

    Write-Host "[$($job.Id)] clang-tidy `"$($job.DisplayFile)`""
    if (-not [string]::IsNullOrWhiteSpace($Text)) {
        Write-Host $Text
    }

    if ($IsFailure) {
        Write-Host "clang-tidy failed: `"$($job.DisplayFile)`""
    }

    $script:PrintedReportThisTick = $true
}

function Complete-One($job) {
    $job.Process.WaitForExit()

    $stdout = ""
    $stderr = ""

    if (Test-Path -LiteralPath $job.StdoutFile -PathType Leaf) {
        $stdout = Get-Content -LiteralPath $job.StdoutFile -Raw
    }

    if (Test-Path -LiteralPath $job.StderrFile -PathType Leaf) {
        $stderr = Get-Content -LiteralPath $job.StderrFile -Raw
    }

    $text = $stdout + $stderr
    $filteredText = Remove-ClangTidyNoise $text
    $plainFilteredText = Remove-AnsiEscape $filteredText
    $exitCode = $job.Process.ExitCode

    $hasProjectDiagnostic =
        $plainFilteredText -match "[\\/]src[\\/].*:[0-9]+:[0-9]+: warning:" -or
        $plainFilteredText -match "[\\/]src[\\/].*:[0-9]+:[0-9]+: error:" -or
        $plainFilteredText -match "^src[\\/].*:[0-9]+:[0-9]+: warning:" -or
        $plainFilteredText -match "^src[\\/].*:[0-9]+:[0-9]+: error:"

    if ($exitCode -ne 0) {
        if ([string]::IsNullOrWhiteSpace($filteredText)) {
            $script:IgnoredExternalOnlyFailures++
        } else {
            $script:Failed++
            Write-ClangTidyReport $job $filteredText $true
        }
    } elseif ($hasProjectDiagnostic) {
        $script:HadDiagnostic++
        Write-ClangTidyReport $job $filteredText $false
    }

    Remove-Item -LiteralPath $job.StdoutFile -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $job.StderrFile -Force -ErrorAction SilentlyContinue

    $script:Completed++
}

try {
    Write-Host "clang-tidy parallel jobs: $MaxJobs"

    while ($NextIndex -lt $TotalFiles -or $Running.Count -gt 0) {
        $script:PrintedReportThisTick = $false

        while ($NextIndex -lt $TotalFiles -and $Running.Count -lt $MaxJobs) {
            $NextIndex++
            $job = New-ClangTidyProcess -File $Files[$NextIndex - 1] -Id $NextIndex
            [void]$Running.Add($job)
        }

        for ($i = $Running.Count - 1; $i -ge 0; --$i) {
            $job = $Running[$i]
            if ($job.Process.HasExited) {
                Complete-One $job
                $Running.RemoveAt($i)
            }
        }

        if (-not $script:PrintedReportThisTick) {
            Write-ProgressLine
        }

        Start-Sleep -Milliseconds 150
    }

    Write-ProgressLine
    [Console]::WriteLine("")
    Write-Host ""
    Write-Host "clang-tidy checked $Completed translation units, diagnostics: $HadDiagnostic, failures: $Failed, ignored external-only failures: $IgnoredExternalOnlyFailures"

    if ($Completed -ne $TotalFiles -or $Failed -ne 0) {
        exit 1
    }

    exit 0
}
finally {
    foreach ($job in $Running) {
        try {
            if (-not $job.Process.HasExited) {
                $job.Process.Kill()
            }
        } catch {
        }
    }

    Remove-Item -LiteralPath $JobDir -Recurse -Force -ErrorAction SilentlyContinue
}
