// Parse a C++ files
using System.Diagnostics;
using System.Text;

namespace CodeGenerator {

	public class MyStringBuilder {
		public readonly StringBuilder str = new();
		public static MyStringBuilder operator +(MyStringBuilder self, string s) {
			self.str.AppendLine(s);
			return self;
		}
		public void NextLine(int times = 1) {
			for (int i = 0; i < times; i++) {
				str.AppendLine();
			}
		}
		public override string ToString() {
			return str.ToString();
		}
	}
	public class Program {

		static string GenerateCode(List<CppStructInfo> structs, string[] include_files) {
			MyStringBuilder content = new();
			content += """
				#include"../ComponentMeta.h"
				#include"../CustomFieldParser.h"
				#include"src/ecs/entity.h"
				#include"src/game/GameContext.h"
				""";
			foreach (var file in include_files) {
				content += $"#include\"{file}\"";
			}
			content.NextLine(2);
			content += """
				template<class T>
				struct ValueWrapper {
					using Parser = FieldParser<T>;
					static constexpr bool enabled = Parser::enabled;
					using Storage = std::conditional_t<enabled, std::optional<T>, EmptyFieldType>;
					Storage storage{};

					const T& value()const {
						if constexpr (enabled) {
							return storage.value();
						}
						throw "Not supposed to be here...";
					}
				};
				""";
			content.NextLine();
			content += "void ComponentMeta::initGeneratedComponentInitializers() {";
			foreach (var info in structs) {
				var name = info.Name;
				content += $"componentInitializerFactories[\"{name}\"] = [](const json& jsonData)->ComponentInitializer {{";
				foreach (var field in info.Fields) {
					content += $"using __{field}_t = ValueWrapper<decltype({name}::{field})>;";
				}
				content += $"struct __{name}Parser {{";
				foreach (var field in info.Fields) {
					content += $"__{field}_t {field}{{}};";
				}
				content += "} p;";
				content.NextLine();
				foreach (var field in info.Fields) {
					content += $@"if constexpr (__{field}_t::enabled) {{
							if (jsonData.contains(""{field}"")) {{
								p.{field}.storage = std::move(__{field}_t::Parser::parse(jsonData[""{field}""]));
							}}
						}}";
				}
				content.NextLine();
				content += "return [p = std::move(p)](GameCtx& ctx, myecs::entity e)->void {";
				content += $"auto& c = ctx.reg.emplace<{name}>(e);";
				content.NextLine();
				foreach (var field in info.Fields) {
					content += $@"if (p.{field}.storage) {{
							c.{field} = p.{field}.value();
						}}";
				}
				content.NextLine();
				content += "};";
				content += "};";
				content.NextLine();
			};
			content += "}";
			return content.str.ToString();
		}

		static void FormatFile(string target_file) {
			if (!File.Exists(target_file)) {
				Console.WriteLine($"错误：目标文件不存在！路径：{target_file}");
				return;
			}
			string quotedOutputFile = $"\"{target_file}\"";
			var startInfo = new ProcessStartInfo {
				FileName = "clang-format",
				Arguments = $"-i {quotedOutputFile}", // 带引号的路径避免空格问题
				CreateNoWindow = true,
				RedirectStandardError = true, // 启用 stderr 捕获
				UseShellExecute = false, // 必须设为 false 才能重定向流
				StandardErrorEncoding = System.Text.Encoding.UTF8 // 确保中文错误信息正常显示
			};

			using var process = Process.Start(startInfo);
			if (process == null) {
				Console.WriteLine("错误：启动 clang-format 失败！");
				return;
			}
			string errorOutput = process.StandardError.ReadToEnd();
			bool isDone = process.WaitForExit(10000);
			if (!isDone) {
				process.Kill();
				throw new TimeoutException("clang-format 运行超时，已强制终止");
			}
			int exitCode = process.ExitCode;
			if (exitCode != 0) {
				throw new Exception($"clang-format 未正常退出！退出码：{exitCode}\n错误详情：{errorOutput}");
			}
			if (!string.IsNullOrEmpty(errorOutput)) {
				Console.WriteLine($"警告：clang-format 输出了额外信息：{errorOutput}");
			}

			//Console.WriteLine("格式化完成！");
		}

		public static void UpdateTarget(string targetPath, string newCode) {
			try {
				if (!File.Exists(targetPath)) {
					File.WriteAllText(targetPath, newCode);
					Console.WriteLine($"目标文件不存在，已创建：{targetPath}");
					return;
				}
				string existingCode = File.ReadAllText(targetPath);
				if (existingCode != newCode) {
					File.WriteAllText(targetPath, newCode);
					Console.WriteLine($"已完成代码更新：{targetPath}");
				}
				else Console.WriteLine($"代码为最新，无需更新");
			}
			catch (Exception ex) {
				Console.WriteLine($"更新失败! → {ex.Message}");
			}
		}

		public static void SwitchToDir(string[] args) {
			try {
				if (args.Length > 0) {
					string targetDir = args[0];
					if (Directory.Exists(targetDir)) {
						Directory.SetCurrentDirectory(targetDir);
						Console.WriteLine($"已切换到指定目录：{targetDir}");
					}
					else throw new DirectoryNotFoundException($"错误：指定的目录不存在 -> {targetDir}");
				}
				else Console.WriteLine("未传入目录参数，使用当前默认目录");
				Console.WriteLine($"当前运行目录：{Directory.GetCurrentDirectory()}");
			}
			catch (Exception ex) {
				Console.WriteLine($"切换目录失败：{ex.Message}");
				throw;
			}
		}

		static List<CppStructInfo> GetStructInfo(string file) {
			string code = File.ReadAllText(file);
			var tokens = CodeParser.Parse(code);
			return CodeParser.GetCppStructs(tokens);
		}

		public static int Main(string[] args) {
			//string oldDir = Directory.GetCurrentDirectory();
			SwitchToDir(args);
			using var timer = new TimingScope("CodeGenerator");
			Directory.SetCurrentDirectory("../../../../");  //CodeGenerator
			string output_dir = Path.GetFullPath("../src/meta/generated/");
			string output_file = Path.Combine(output_dir, "Meta_initComponentInitGen.cpp");
			string temp_file = output_file + "_temp";
			string[] include_files = [
				"src/game/Components/EntityComponents.h",
				"src/game/Components/PhysicsComponents.h",
				//"src/game/Components/PlayerComponent.h",
			];
			string[] source_files = [.. include_files.Select(file => Path.GetFullPath(Path.Combine("..", file)))];
			try {
				List<CppStructInfo> structs = [];
				foreach (var file in source_files) {
					structs.AddRange(GetStructInfo(file));
				}
				string code = GenerateCode(structs, include_files);
				File.WriteAllText(temp_file, code);
				FormatFile(temp_file);
				string formatted = File.ReadAllText(temp_file);
				File.Delete(temp_file);
				UpdateTarget(output_file, formatted);
			}
			catch (Exception e) {
				Console.WriteLine("运行失败！");
				Console.WriteLine(e.ToString());
				return 1;
			}
			Console.Out.Flush();
			//Console.WriteLine("完成!");
			//Directory.SetCurrentDirectory(oldDir);
			return 0;
		}
	}
}
