#include "DebugScene.h"

#include "src/app/SceneManager.h"

#include <imgui.h>

namespace flx::app {

	DebugScene::DebugScene(AppCtx ctx)
		: Scene(ctx, makeContentID<DebugScene>()) {
	}

	void DebugScene::makeImGuiContent() {
		if (!ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
			return;
		}

		const auto mousePos = ctx.input.mouseRender;
		ImGui::Text("鼠标位置：（%.1f, %.1f）", mousePos.x, mousePos.y);

		ImGui::SeparatorText("活动场景");
		if (ImGui::BeginTable("ActiveScenesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("场景");
			ImGui::TableSetupColumn("层");
			ImGui::TableHeadersRow();

			for (const auto& scene : ctx.sceneManager.getActiveScenes()) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(scene->name.c_str());

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%d", scene->getLayer());
			}

			ImGui::EndTable();
		}
	}

	bool DebugScene::handleEvent(const ui::WindowEvent& event) {
		if (const auto e = event.rawEvent.getIf<sf::Event::KeyPressed>()) {
			if (e->code == sf::Keyboard::Key::Escape) {
				ctx.runtime.cmds += AppCmd::Exit{};
				logger.info("debug_scene requested exit by key Escape");
			} else if (e->code == sf::Keyboard::Key::Tab) {
				ctx.runtime.cmds += AppCmd::ToggleWindowMode{};
				logger.info("debug_scene requested toggle window mode by key F");
			}
		}

		return Scene::handleEvent(event);
	}
} // namespace flx::app