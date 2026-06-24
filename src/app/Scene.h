#pragma once
#ifndef NSCENE_H
#define NSCENE_H
#include "AppCtx.h"
#include "src/ui/render/NWindowView.h"
#include "src/ui/widget/NRootWidget.h"
#include "src/ui/widget/NWidget.h"

namespace flx::app {
	class Scene {
		// friend class SceneManager;

	protected:
		Unique<ui::RootWidget> widget{};
		Logger logger{};
		AppCtx context;
		const ui::Viewport& viewport;
		// std::string nextScene = "";

	public:
		const std::string name;

		explicit Scene(AppCtx context, std::string_view name)
			: logger(Logger::makeAsync(name, true)),
			  context(context),
			  viewport(context.windowViewport.viewport),
			  name(name) {}

		virtual ~Scene() {
		}

		virtual void draw(ui::RenderBuffer& rdr) {
			if (widget) {
				widget->draw(rdr);
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
			widget = flx::makeUnique(new ui::RootWidget(viewport));
		}

		void onWindowResized(const ui::NWindowView& view) const {
			if (widget) {
				widget->onWindowResized(view);
			}
		}

		virtual void enter() {
		}

		virtual void exit() {
		}
	};

	// template <class T>
	// class SceneBase : public Scene {
	// 	explicit SceneBase()
	// 		: Scene(Util::makeContentID<T>()) {}
	// };

} // namespace flx::app

#endif