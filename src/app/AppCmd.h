#pragma once
#include "src/ui/context/WindowMode.h"
#include "src/utils/Container/Variant.h"

namespace flx::app {
	struct AppCmd {
		struct SetWindowMode {
			ui::WindowMode mode{};
		};

		struct Exit {};

		using Cmd = Variant<std::monostate,
							SetWindowMode,
							Exit>;
	};
} // namespace flx::app