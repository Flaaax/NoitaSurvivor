#pragma once
#include "src/utils/Container/Map.h"
#include "src/utils/Macro.h"

#include <filesystem>
#include <functional>
#include <string_view>

namespace flx::app {
	template <class T>
	using AssetTable = StrMap<T>;

	struct Loader {
		using Path = std::filesystem::path;
		using FileCallback = std::function<void(const Path& path)>;

		FLX_CONSTEXPR std::string_view ignore_file = ".flx_ignore";

		static void traverseFolder(const Path& folder, const FileCallback& onFile);
		static std::string makeEntry(const Path& file, const Path& root);
	};
} // namespace flx::app