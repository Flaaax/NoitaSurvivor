#pragma once
#include "src/ui/context/WindowMode.h"
#include "src/utils/Container/Variant.h"

namespace flx::app {
	struct AppCmd {
		struct ToggleWindowMode {
			Optional<ui::WindowMode> mode{};
		};

		struct Exit {};

		using Cmd = Variant<std::monostate,
							ToggleWindowMode,
							Exit>;
	};
} // namespace flx::app