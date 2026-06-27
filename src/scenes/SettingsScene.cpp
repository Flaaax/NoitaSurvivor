#include "SettingsScene.h"

#include "src/app/global/LocManager.h"
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
		});

		{
			auto button1 = makeSUnique<ui::Button>();
			button1->onClick = requestResume;
			button1->setText(table["pause.continue"]);
			button1->setSize({200, 100});

			auto button2 = makeSUnique<ui::Button>();
			button2->setText(table["pause.display"]);
			button2->setSize({200, 100});

			leftLayout->add(button1 | move);
			leftLayout->add(button2 | move);
		}

		auto rightLayout = ui::VBoxLayout::create({
			.alignY = Center,
			.heightPolicy = Fill,
		});

		{
			auto button1 = makeSUnique<ui::Button>();
			button1->setText(table["pause.toggle_fullscreen"]);
			button1->setSize({200, 100});

			rightLayout->add(button1 | move);
		}

		auto wrapperLayout = ui::HBoxLayout::create({
			.widthPolicy = Shrink,
			.heightPolicy = Shrink,
			.spacing = 20.f,
		});

		wrapperLayout->add(leftLayout | move);
		wrapperLayout->add(rightLayout | move);

		auto panel = makeSUnique<ui::Panel>();
		panel->sizePolicy = Fixed;
		panel->setLayout(wrapperLayout | move);
		auto frame = panel->getFrame();
		frame.setCenter(ctx.windowView.canvasRect.center());
		panel->setFrame(frame);

		widget->add(panel | move);
	}
} // namespace flx::app