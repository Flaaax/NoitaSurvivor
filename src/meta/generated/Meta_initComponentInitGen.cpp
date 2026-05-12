#include "../ComponentMeta.h"
#include "../CustomFieldParser.h"
#include "src/ecs/entity.h"
#include "src/game/Components/EntityComponents.h"
#include "src/game/Components/PhysicsComponents.h"
#include "src/game/GameContext.h"

template <class T> struct ValueWrapper {
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

void ComponentMeta::initGeneratedComponentInitializers() {
	componentInitializerFactories["EntityComponent"] =
		[](const json& jsonData) -> ComponentInitializer {
		using __layer_t = ValueWrapper<decltype(EntityComponent::layer)>;
		using __health_t = ValueWrapper<decltype(EntityComponent::health)>;
		struct __EntityComponentParser {
			__layer_t layer{};
			__health_t health{};
		} p;

		if constexpr (__layer_t::enabled) {
			if (jsonData.contains("layer")) {
				p.layer.storage = std::move(__layer_t::Parser::parse(jsonData["layer"]));
			}
		}
		if constexpr (__health_t::enabled) {
			if (jsonData.contains("health")) {
				p.health.storage = std::move(__health_t::Parser::parse(jsonData["health"]));
			}
		}

		return [p = std::move(p)](const GameCtx& ctx, myecs::entity e) -> void {
			auto& c = ctx.reg.emplace<EntityComponent>(e);

			if (p.layer.storage) {
				c.layer = p.layer.value();
			}
			if (p.health.storage) {
				c.health = p.health.value();
			}
		};
	};

	componentInitializerFactories["ProjectileComponent"] =
		[](const json& jsonData) -> ComponentInitializer {
		using __damage_t = ValueWrapper<decltype(ProjectileComponent::damage)>;
		using __impulse_t = ValueWrapper<decltype(ProjectileComponent::impulse)>;
		using __maxSpeed_t = ValueWrapper<decltype(ProjectileComponent::maxSpeed)>;
		using __pierce_t = ValueWrapper<decltype(ProjectileComponent::pierce)>;
		struct __ProjectileComponentParser {
			__damage_t damage{};
			__impulse_t impulse{};
			__maxSpeed_t maxSpeed{};
			__pierce_t pierce{};
		} p;

		if constexpr (__damage_t::enabled) {
			if (jsonData.contains("damage")) {
				p.damage.storage = std::move(__damage_t::Parser::parse(jsonData["damage"]));
			}
		}
		if constexpr (__impulse_t::enabled) {
			if (jsonData.contains("impulse")) {
				p.impulse.storage = std::move(__impulse_t::Parser::parse(jsonData["impulse"]));
			}
		}
		if constexpr (__maxSpeed_t::enabled) {
			if (jsonData.contains("maxSpeed")) {
				p.maxSpeed.storage = std::move(__maxSpeed_t::Parser::parse(jsonData["maxSpeed"]));
			}
		}
		if constexpr (__pierce_t::enabled) {
			if (jsonData.contains("pierce")) {
				p.pierce.storage = std::move(__pierce_t::Parser::parse(jsonData["pierce"]));
			}
		}

		return [p = std::move(p)](const GameCtx& ctx, myecs::entity e) -> void {
			auto& c = ctx.reg.emplace<ProjectileComponent>(e);

			if (p.damage.storage) {
				c.damage = p.damage.value();
			}
			if (p.impulse.storage) {
				c.impulse = p.impulse.value();
			}
			if (p.maxSpeed.storage) {
				c.maxSpeed = p.maxSpeed.value();
			}
			if (p.pierce.storage) {
				c.pierce = p.pierce.value();
			}
		};
	};

	componentInitializerFactories["EnemyComponent"] =
		[](const json& jsonData) -> ComponentInitializer {
		struct __EnemyComponentParser {
		} p;

		return [p = std::move(p)](const GameCtx& ctx, myecs::entity e) -> void {
			auto& c = ctx.reg.emplace<EnemyComponent>(e);
		};
	};

	componentInitializerFactories["DirectionComponent"] =
		[](const json& jsonData) -> ComponentInitializer {
		struct __DirectionComponentParser {
		} p;

		return [p = std::move(p)](const GameCtx& ctx, myecs::entity e) -> void {
			auto& c = ctx.reg.emplace<DirectionComponent>(e);
		};
	};

	componentInitializerFactories["MultiContactComponent"] =
		[](const json& jsonData) -> ComponentInitializer {
		using __contact_interval_t =
			ValueWrapper<decltype(MultiContactComponent::default_duration)>;
		struct __MultiContactComponentParser {
			__contact_interval_t contact_interval{};
		} p;

		if constexpr (__contact_interval_t::enabled) {
			if (jsonData.contains("contact_interval")) {
				p.contact_interval.storage =
					std::move(__contact_interval_t::Parser::parse(jsonData["contact_interval"]));
			}
		}

		return [p = std::move(p)](const GameCtx& ctx, myecs::entity e) -> void {
			auto& c = ctx.reg.emplace<MultiContactComponent>(e);

			if (p.contact_interval.storage) {
				c.default_duration = p.contact_interval.value();
			}
		};
	};

	componentInitializerFactories["SmoothMovementComponent"] =
		[](const json& jsonData) -> ComponentInitializer {
		using __acc_t = ValueWrapper<decltype(SmoothMovementComponent::acc)>;
		using __dec_t = ValueWrapper<decltype(SmoothMovementComponent::dec)>;
		using __spd_t = ValueWrapper<decltype(SmoothMovementComponent::spd)>;
		struct __SmoothMovementComponentParser {
			__acc_t acc{};
			__dec_t dec{};
			__spd_t spd{};
		} p;

		if constexpr (__acc_t::enabled) {
			if (jsonData.contains("acc")) {
				p.acc.storage = std::move(__acc_t::Parser::parse(jsonData["acc"]));
			}
		}
		if constexpr (__dec_t::enabled) {
			if (jsonData.contains("dec")) {
				p.dec.storage = std::move(__dec_t::Parser::parse(jsonData["dec"]));
			}
		}
		if constexpr (__spd_t::enabled) {
			if (jsonData.contains("spd")) {
				p.spd.storage = std::move(__spd_t::Parser::parse(jsonData["spd"]));
			}
		}

		return [p = std::move(p)](const GameCtx& ctx, myecs::entity e) -> void {
			auto& c = ctx.reg.emplace<SmoothMovementComponent>(e);

			if (p.acc.storage) {
				c.acc = p.acc.value();
			}
			if (p.dec.storage) {
				c.dec = p.dec.value();
			}
			if (p.spd.storage) {
				c.spd = p.spd.value();
			}
		};
	};
}
