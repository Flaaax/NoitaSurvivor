#pragma once
#ifndef NSCENE_H
#define NSCENE_H
#include "src/ui/widget/NRootWidget.h"
#include "src/ui/widget/NWidget.h"

namespace flx::app {
	class Scene {
	protected:
		n_unique<NRootWidget> widget{};
		Logger logger{};

	public:
		const std::string_view name;

		explicit Scene(std::string_view name)
			: logger(Logger::makeAsync(name, true)),
			  name(name) {}

		virtual ~Scene() {
		}

		virtual void draw(NRenderBuffer& rdr) {
			if (widget) {
				widget->draw(rdr);
			}
		}

		virtual void update(float dt) {
			if (widget) {
				widget->update(dt);
			}
		}

		virtual bool handleEvent(const NWindowEvent& event) {
			if (widget) {
				return widget->handleEvent(event);
			}
			return false;
		}

		virtual void makeImGuiContent() {}

		void createWidget() {
			widget = Util::makeUnique(new NRootWidget());
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