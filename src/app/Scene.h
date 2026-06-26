#pragma once
#ifndef NSCENE_H
#define NSCENE_H
#include "AppCtx.h"
#include "src/ui/render/WindowView.h"
#include "src/ui/widget/RootWidget.h"
#include "src/ui/widget/Widget.h"

namespace flx::app {
	class Scene {
	protected:
		Unique<ui::RootWidget> widget{};
		Logger logger{};
		AppCtx ctx;
		// const ui::Viewport& viewport;
		u64 layer{}; // Call UpdateLayer if changed dynamically
					 // std::string nextScene = "";

	public:
		const std::string name;

		explicit Scene(AppCtx context, std::string_view name)
			: logger(Logger::makeAsync(name, true)),
			  ctx(context),
			  name(name) {}

		virtual ~Scene() {
		}

		virtual void draw(ui::RenderBuffer& buffer) {
			if (widget) {
				widget->draw(buffer);
			}
		}

		virtual void update(float dt) {
			if (widget) {
				widget->update(dt);
			}
		}

		virtual bool handleEvent(const ui::WindowEvent& event) {
			if (widget) {
				return widget->handleEvent(event);
			}
			return false;
		}

		virtual void makeImGuiContent() {}

		void createWidget() {
			widget = std::make_unique<ui::RootWidget>(ctx.windowView.viewport);
		}

		virtual void onWindowResized(const ui::WindowView& view) {
			if (widget) {
				widget->onWindowResized(view);
			}
		}

		virtual void enter() {
		}

		virtual void exit() {
		}

		u64 getLayer() const {
			return layer;
		}
	};

	// template <class T>
	// class SceneBase : public Scene {
	// 	explicit SceneBase()
	// 		: Scene(Util::makeContentID<T>()) {}
	// };

} // namespace flx::app

#endif