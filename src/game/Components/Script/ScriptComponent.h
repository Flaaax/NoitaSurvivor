#pragma once
#include "../../../utils/Container/Vector.h"
#include "src/ecs/entity.h"
#include "src/game/States/ContactState.h"

namespace flx::game {
	class Script {
	public:
		bool isDone = false;

		virtual ~Script() {
		}

		virtual void onUpdate(const GameCtx& ctx, myecs::entity self, float dt) {
		}

		// triggered on removal of the entity
		virtual void onDeath(const GameCtx& ctx, myecs::entity self) {
		}

		// Should NOT handle game logic!
		virtual void configureContact(const GameCtx& ctx, myecs::entity self, myecs::entity other, ContactSettings& settings) {
		}

		virtual void onContact(const GameCtx& ctx, myecs::entity self, myecs::entity other, const ContactEvent& event) {
		}
	};

	struct ScriptComponent {
		Vector<n_shared<Script>> scripts;
	};
} // namespace flx::game