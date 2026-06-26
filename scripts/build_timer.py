import sys
import time
from pathlib import Path

mode = sys.argv[1]
stamp = Path(sys.argv[2])

if mode == "start":
	stamp.write_text(str(time.perf_counter()), encoding="utf-8")
	# print("Build timer started.")

elif mode == "end":
	if not stamp.exists():
		print("计时失败：没有找到计时缓存")
		sys.exit(0)

	start = float(stamp.read_text(encoding="utf-8"))
	elapsed = time.perf_counter() - start

	print(f"此次构建耗时: {elapsed:.3f} 秒")