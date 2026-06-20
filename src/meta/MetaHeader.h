// ReSharper disable CppUnusedIncludeDirective
#pragma once
#include "src/ecs/entity.h"
#include "src/game/GameContext.h"
#include "src/meta/ComponentMeta.h"
#include "src/meta/CustomFieldParser.h"
#include "src/utils/TypeName.h"

using namespace flx::fon;
using namespace flx::game;
using namespace flx::meta;

namespace flx::meta {
	template <class T>
	struct ValueWrapper {
		using Parser = FieldParser<T>;
		static constexpr bool enabled = Parser::enabled;
		using Storage = std::conditional_t<enabled, std::optional<T>, EmptyFieldType>;
		Storage storage{};

		const T& value() const {
			if constexpr (enabled) {
				return storage.value();
			}
			throw "Not supposed to be here...";
		}
	};
} // namespace flx::meta