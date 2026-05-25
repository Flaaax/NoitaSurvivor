#pragma warning(disable : 5105)
#include "NWindow.h"
#include "../global/AssetManager.h"
#include "../scenes/GameScene.h"
#include "../scenes/MenuScene.h"
#include "../utils/Timer.h"
#include "NScene.h"
#include "NText.h"
#include "global/InputManager.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include "shapes/NRichTextShape.h"
#include "src/game/Services/EntityService.h"
#include "src/global/DebugVariables.h"
#include "src/render/Renderer.h"
#include "src/utils/Logger.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <format>
#include <numeric>

bool DrawStringCombo(const char* label, const Util::Vector<std::string_view>& items, int& current_index) {
	if (items.empty())
		return false;

	const char* preview = items[current_index].data();
	bool changed = false;

	if (ImGui::BeginCombo(label, preview)) {
		for (const int i : items.indices<int>()) {
			const bool selected = (current_index == i);

			if (ImGui::Selectable(items[i].data(), selected)) {
				current_index = i;
				changed = true;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	return changed;
}

void NWindow::updateWindowSize() const {
	const auto size = static_cast<nvec2>(window->getSize());
	scale.updateWindowSize(size);
	// glViewport(0, 0, sizeu.x, sizeu.y);

	sf::View view;
	view.setCenter(size / 2.f);
	view.setSize(size);
	window->setView(view);
}

void NWindow::updateMousePos() const {
	auto& state = InputManager::state;
	state.mouseScreen = static_cast<nvec2>(sf::Mouse::getPosition(*window));
	state.mouseRender = scale.toRenderPos(state.mouseScreen);
}

NWindow::NWindow() {
	constexpr auto windowSize = NScale::defaultWindowSize.to<sf::Vector2u>();
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(windowSize), "NoitaSurvivor: Test");

	Logger::info("Application starting...");

	AssetMgr::init(); // init

	if (!ImGui::SFML::Init(*window, false)) {
		Logger::error("Failed to initialize ImGui-SFML");
	} else {
		Logger::info("ImGui-SFML initialized");
	}

	ImGuiIO& io = ImGui::GetIO();

	const auto font = io.Fonts->AddFontFromFileTTF(
		"./resources/fonts/msyh.ttc",
		22.f,
		nullptr,
		io.Fonts->GetGlyphRangesChineseFull());

	if (!font) {
		Logger::error_and_throw("Failed to load Chinese font for ImGui");
	}
	io.FontDefault = font;

	if (!ImGui::SFML::UpdateFontTexture()) {
		Logger::error("Imgui-SFML Update font texture failed!");
	}

	updateWindowSize();

	Logger::info("Window initialization complete");
}

NWindow::~NWindow() {
}

int NWindow::loop() {
	// Logger::info("Entering main loop...");
	// fps
	sf::Clock clock;
	std::array<float, 1000> frameTimes = {};
	float averageFPS = 0;
	size_t index = 0;
	Renderer rdr;

	// Logger::info("Initializing scenes...");
	sceneManager.addScene(std::make_unique<GameScene>());
	sceneManager.addScene(std::make_unique<MenuScene>());
	sceneManager.setCurrentScene("menu_scene");

	auto& font = AssetMgr::getFont("consola");
	auto text = new NLineText(font);
	text->setPosition({0, 0});
	text->sfText.setFillColor(sf::Color::Black);
	text->setSize(2);
	CTimer fpsCalcTimer(1.f, [&] {
		const float total = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.F);
		averageFPS = static_cast<float>(frameTimes.size()) / total;
		(*text)[0] = (std::format("FPS: {}", static_cast<int>(trunc(averageFPS))));
	});
	fpsCalcTimer.start(CTimer::infinite_trigger);

	const auto globalWidget = Util::makeUnique(new NRootWidget());
	globalWidget->addToTop(Util::makeUnique(std::move(text)));

	bool isRunning = true;
	bool enableImgui = true;

	// NRichTextShape richText(font, "Hello, [blue]world[/]!\n[i]Rich[/] [sine]Text[/] 1");
	// richText.setPosition({200, 200});
	// sf::RectangleShape rectangle;
	// rectangle.setPosition(richText.getPosition());
	// rectangle.setSize(richText.getLayoutSize());
	// rectangle.setFillColor(sf::Color::Cyan);

	ImGui::GetStyle().ScaleAllSizes(1.5f);

	while (window->isOpen() && isRunning) {
		// calculate deltatime
		constexpr float MAX_DELTA_TIME = 0.1f;
		auto sfDt = clock.restart();
		const float dt = std::min(sfDt.asSeconds(), MAX_DELTA_TIME);

		frameTimes[index] = dt;
		index++;
		if (index >= frameTimes.size())
			index = 0;
		fpsCalcTimer.update(dt);

		updateMousePos(); // this is critical!!!

		// Logger::info("New frame started, deltaTime: {}", deltaTime);
		if (sceneManager.shouldChangeScene()) {
			sceneManager.changeScene();
		}

		const auto currentScene = sceneManager.getCurrentScene();
		// Logger::info("Current scene: {}", currentScene ? currentScene->getName() : "null");

		// handle event
		while (const auto event = window->pollEvent()) {
			if (enableImgui) {
				ImGui::SFML::ProcessEvent(*window, *event);
				const bool isMouseEvent =
					event->is<sf::Event::MouseButtonPressed>() ||
					event->is<sf::Event::MouseButtonReleased>() ||
					event->is<sf::Event::MouseMoved>() ||
					event->is<sf::Event::MouseWheelScrolled>();

				if (isMouseEvent && ImGui::GetIO().WantCaptureMouse) {
					continue; // ImGui 吃掉鼠标事件
				}
			}

			if (event->is<sf::Event::Closed>()) {
				isRunning = false;
				Logger::info("Window closed directly by user");
			} else if (event->is<sf::Event::Resized>()) {
				updateWindowSize();
			} else if (const auto e = event->getIf<sf::Event::KeyPressed>(); e && e->code == sf::Keyboard::Key::Escape) {
				isRunning = false;
				Logger::info("Window closed by pressing Esc");
			}
			if (currentScene) {
				NEventCtx eventCtx{
					.rawEvent = *event,
					.scale = scale,
					.input = InputManager::getState(),
				};
				currentScene->handleEvent(eventCtx);
			}
		}

		if (enableImgui) {
			ImGui::SFML::Update(*window, sfDt);
		}

		// update
		if (currentScene) {
			currentScene->update(dt);
		}

		// draw
		rdr.clear();

		if (currentScene) {
			currentScene->draw(rdr);
		}

		globalWidget->draw(rdr);

		// rdr.drawUI(rectangle);
		// rdr.drawUI(richText);

		rdr.draw(*window);

		if (enableImgui) {
			static bool showDemo = false;

			ImGui::Begin("Imgui");

			ImGui::SeparatorText("游戏内容");

			if (ImGui::Button("清除实体")) {
				static bool& shouldClear = DebugVariables::try_emplace<bool>("shouldClearEntities", true);
				shouldClear = true;
			}

			static bool& enableEnemySpawn = DebugVariables::try_emplace<bool>("enableEnemySpawn", true);
			if (ImGui::Button(!enableEnemySpawn ? "启用怪物生成" : "禁用怪物生成")) {
				enableEnemySpawn = !enableEnemySpawn;
			}
			if (enableEnemySpawn) {
				static float& enemySpawnFreq = DebugVariables::try_emplace<float>("enemySpawnFreq", 1.f);
				ImGui::SliderFloat("怪物生成速率", &enemySpawnFreq, 0.5f, 10.f);
			}

			static Util::Vector<std::string_view> trackers = {
				"none",
				"circle",
				"seek",
				"weakSeek",
				"leadSeek",
				"lateral",
				"navigation",
			};

			static int& selectedTracker = DebugVariables::try_emplace("tracker", 1);

			if (DrawStringCombo("跟踪算法", trackers, selectedTracker)) {
				// Logger::info("选择了 {}", trackers[selected]);
			}

			ImGui::End();

			if (showDemo) {
				ImGui::ShowDemoWindow(&showDemo);
			}

			ImGui::SFML::Render(*window);
		}

		window->display();
	}

	window->close();
	ImGui::SFML::Shutdown();
	return 0;
}
