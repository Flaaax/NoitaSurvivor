using System;
using System.Diagnostics;

namespace CodeGenerator {

	/// <summary>
	/// 基于 RAII 特性的计时类（C# 需通过 IDisposable + using 实现）
	/// 作用域内自动计时，离开作用域自动输出耗时
	/// </summary>
	public class TimingScope : IDisposable {
		// 核心计时组件（标准库的 Stopwatch，仅负责计时逻辑）
		private readonly Stopwatch _stopwatch;
		// 可选：为计时任务命名，输出更清晰
		private readonly string _taskName;
		// 标记是否已释放（避免重复调用 Dispose）
		private bool _disposed;

		/// <summary>
		/// 获取总耗时（可在 Dispose 前手动获取）
		/// </summary>
		public TimeSpan Elapsed => _stopwatch.Elapsed;

		/// <summary>
		/// 构造函数（创建时自动开始计时）
		/// </summary>
		/// <param name="taskName">计时任务名称（可选）</param>
		public TimingScope(string taskName = "未命名任务") {
			_taskName = taskName;
			_stopwatch = Stopwatch.StartNew(); // 初始化并启动计时
			Console.WriteLine($"[{_taskName}] 开始计时...");
		}

		/// <summary>
		/// 释放资源（离开 using 作用域时自动调用）
		/// </summary>
		public void Dispose() {
			Dispose(true);
			// 无需终结器（无未托管资源），抑制 GC 回收
			GC.SuppressFinalize(this);
		}

		/// <summary>
		/// 实际释放逻辑（符合 IDisposable 标准实现）
		/// </summary>
		/// <param name="disposing">是否为手动调用（true=using/手动 Dispose，false=GC 触发）</param>
		protected virtual void Dispose(bool disposing) {
			if (_disposed) return;
			if (disposing) {
				_stopwatch.Stop();
				Console.WriteLine(
					$"[{_taskName}] 运行完成，" +
					$" 总耗时：{_stopwatch.Elapsed.TotalMilliseconds:F2} 毫秒 " +
					$"(≈ {_stopwatch.Elapsed.TotalSeconds:F2} 秒)"
				);
			}
			_disposed = true;
		}

		// 无需终结器（Finalizer）：本类仅使用托管资源（Stopwatch），GC 可自动回收
		// ~TimingScope() => Dispose(false);
	}
}