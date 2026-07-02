#pragma once
#ifndef NSCENE_H
#define NSCENE_H
#include "AppCtx.h"
#include "src/ui/Window.h"
#include "src/ui/render/WindowView.h"
#include "src/ui/widget/RootWidget.h"
#include "src/ui/widget/Widget.h"

namespace flx::app {
	class Scene : public EnableSWeakFromThis<Scene> {
	protected:
		SUnique<ui::RootWidget> widget{};
		Logger logger{};
		AppCtx ctx;
		// const ui::Viewport& viewport;
		// Call UpdateLayer if changed dynamically
		u64 layer{};

		template <std::derived_from<Scene> Obj, class Callback>
		decltype(auto) bindCallback(Callback Obj::* member) {
			auto* self = static_cast<Obj*>(this);

			return [self, member]<typename... Args>(Args&&... args) -> decltype(auto) {
				return std::invoke(self->*member, FLX_FORWARD);
			};
		}

	public:
		const std::string name;

		SWeak<Scene> ref() {
			return sweakFromThis();
		}

		SWeak<const Scene> ref() const {
			return sweakFromThis();
		}

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
			widget = makeSUnique<ui::RootWidget>(ctx.window.getView().viewport);
		}

		virtual void onWindowResized() {
			if (widget) {
				widget->onWindowResized(ctx.window.getView());
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