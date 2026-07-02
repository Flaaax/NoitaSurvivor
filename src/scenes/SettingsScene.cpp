#include "SettingsScene.h"

#include "src/app/global/LocManager.h"
#include "src/ui/Window.h"
#include "src/ui/elements/Button.h"
#include "src/ui/layout/BoxLayout.h"

namespace flx::ui {
	class VBoxLayout;
}

namespace flx::app {
	SettingsScene::SettingsScene(AppCtx ctx)
		: Scene(ctx, makeContentID<SettingsScene>()) {

		layer = 1;

		createWidget();

		// using namespace ui;
		using enum ui::Policy;

		const auto& table = *LocManager::getTable("ui");

		auto leftLayout = ui::VBoxLayout::create({
			.alignY = Center,
			.heightPolicy = Fixed,
			.size = {0, 600},
			.spacing = 15.f,
		});

		{
			auto button1 = ui::Button::create({
				.size = {200, 100},
				.text = table["pause.continue"],
				.onClick = bindCallback(&SettingsScene::requestResume),
			});

			auto button2 = ui::Button::create({
				.size = {200, 100},
				.text = table["pause.display"],
				//.onClick = bindCallback(&SettingsScene::requestResume),
			});

			auto button3 = ui::Button::create({
				.size = {200, 100},
				.text = table["pause.exit"],
				.onClick = [this] {
					this->ctx.runtime.cmds += AppCmd::Exit{};
				},
			});

			leftLayout->add(button1 | move);
			leftLayout->add(button2 | move);
			leftLayout->add(button3 | move);
		}

		auto rightLayout = ui::VBoxLayout::create({
			.alignY = Center,
			.heightPolicy = Fill,
			.spacing = 5.f,
		});

		{
			auto button1 = makeSUnique<ui::Button>();
			button1->setText(table["pause.toggle_fullscreen"]);
			button1->setSize({200, 100});
			button1->onClick = [this] {
				using enum ui::WindowMode;
				const auto nextMode = this->ctx.window.getMode() == Borderless ? Windowed : Borderless;
				this->ctx.runtime.cmds += AppCmd::SetWindowMode{nextMode};
			};

			rightLayout->add(button1 | move);
		}

		auto wrapperLayout = ui::HBoxLayout::create({
			.alignY = Center,
			.widthPolicy = Shrink,
			.heightPolicy = Shrink,
			.spacing = 20.f,
		});

		wrapperLayout->add(leftLayout | move);
		wrapperLayout->add(rightLayout | move);

		auto panel = ui::Panel::create({
			.sizePolicy = Shrink,
			.outlineThickness = 2.f,
			.layout = wrapperLayout | move,
		});
		auto frame = panel->getFrame();
		frame.setCenter(ctx.window.getView().canvasRect.center());
		panel->setPosition(frame.position);

		widget->add(panel | move);
	}

	void SettingsScene::setVisible(bool visible) const {
		widget->isVisible = visible;
	}
} // namespace flx::app