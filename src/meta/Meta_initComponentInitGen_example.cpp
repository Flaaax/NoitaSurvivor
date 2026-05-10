#include"ComponentMeta.h"
#include"src/ecs/entity.h"
#include"CustomFieldParser.h"
#include"src/game/Components/EntityComponents.h"	//auto includeS

/*
	initData["ContactLayer"] = [](void* instance, const json& data) {
		auto os = json_parse<std::string_view>(data, "ContactLayer");
		if (!os.has_value()) return;
		auto& s = os.value();
		ContactLayer& inst = *reinterpret_cast<ContactLayer*>(instance);
		std::unordered_map<std::string_view, ContactLayer> map = {
			{"None", ContactLayer::None},
			{"Player", ContactLayer::Player},
			{"Enemy", ContactLayer::Enemy},
			{"Projectile", ContactLayer::Projectile},
			{"PlayerProjectile", ContactLayer::PlayerProjectile},
			{"EnemyProjectile", ContactLayer::EnemyProjectile},
			{"Wall", ContactLayer::Wall}
		};
		Util::find_key_if(map, s, [&](const auto val) {
			inst = val;
		});
	};
*/

//template<class T>
//struct ValueWrapper {
//	using Parser = FieldParser<T>;
//	static constexpr bool enabled = Parser::enabled;
//	using Storage = std::conditional_t<enabled, std::optional<T>, EmptyFieldType>;
//	Storage storage;
//
//	const T& value()const {
//		if constexpr (enabled) {
//			return storage.value();
//		}
//		static_assert(false, "Invalid function call");
//	}
//};
//
//void ComponentMeta::initGeneratedComponentInitializers() {
//	//example: TestComponent
//	componentInitializerFactories["TestComponent"] = [](const json& jsonData)->ComponentInitializer {
//		using __size_t = ValueWrapper<decltype(TestComponent::size)>;
//		using __width_t = ValueWrapper<decltype(TestComponent::width)>;
//		//...
//
//		struct __TestComponentParser {
//			__size_t size;
//			__width_t width;
//			//...
//		} p;
//
//		if constexpr (__size_t::enabled) {
//			if (jsonData.contains("size")) {
//				p.size.storage = std::move(__size_t::Parser::parse(jsonData["size"]));
//			}
//		}
//		if constexpr (__width_t::enabled) {
//			if (jsonData.contains("width")) {
//				p.width.storage = std::move(__width_t::Parser::parse(jsonData["width"]));
//			}
//		}
//		//...
//
//		return [p = std::move(p)](GameCtx& ctx, myecs::entity e)->void {
//			auto& c = ctx.reg.emplace<TestComponent>(e);
//			if (p.size.storage) {
//				c.size = p.size.value();
//			}
//			if (p.width.storage) {
//				c.width = p.width.value();
//			}
//			//...
//		};
//	};
//}
