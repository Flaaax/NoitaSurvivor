#pragma warning(disable : 5105)
#include "NWindow.h"
#include "../global/AssetManager.h"
#include "../scenes/GameScene.h"
#include "../scenes/MenuScene.h"
#include "../utils/Timer.h"
#include "./shapes/NRoundRectShape.h"
#include "NScene.h"
#include "NText.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include "src/utils/Logger.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <format>
#include <numeric>

void NWindow::updateWindowSize() const {
	const auto sizeu = window->getSize();
	scale.updateWindowSize(sizeu);
	// glViewport(0, 0, sizeu.x, sizeu.y);

	sf::View view;
	nvec2 size = sizeu;
	view.setCenter(size / 2.f);
	view.setSize(size);
	window->setView(view);
}

void NWindow::updateMousePos() const {
	mouseRealPos = sf::Mouse::getPosition(*window);
	mouseRenderPos = scale.realPosToRenderPos(mouseRealPos);
}

NWindow::NWindow() {
	constexpr auto windowSize = NScale::defaultWindowSize.to<sf::Vector2u>();
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(windowSize), "NoitaSurvivor: Test");

	Logger::info("Application starting...");

	AssetMgr::init(); // init

	if (!ImGui::SFML::Init(*window)) {
		Logger::error("Failed to initialize ImGui-SFML");
	} else {
		Logger::info("ImGui-SFML initialized");
	}

	updateWindowSize();

	Logger::info("Window initialization complete");
}

NWindow::~NWindow() {
}

int NWindow::loop() const {
	// Logger::info("Entering main loop...");
	// fps
	sf::Clock clock;
	std::array<float, 1000> frameTimes = {};
	float averageFPS = 0;
	size_t index = 0;
	Renderer renderer;

	// Logger::info("Initializing scenes...");
	sceneManager.addScene(std::make_unique<GameScene>());
	sceneManager.addScene(std::make_unique<MenuScene>());
	sceneManager.setCurrentScene("MenuScene");

	auto& font = AssetMgr::getFont("consola");
	NLineText text(font);
	text.setPosition({0, 0});
	text.sfText.setFillColor(sf::Color::Black);
	text.setSize(2);
	CTimer fpsCalcTimer(1.f, [&] {
		float total = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.F);
		averageFPS = static_cast<float>(frameTimes.size()) / total;
		text[0] = (std::format("FPS: {}", static_cast<int>(trunc(averageFPS))));
	});
	fpsCalcTimer.start(CTimer::infinite_trigger);

	bool isRunning = true;
	bool enableImgui = true;

	ImGui::GetStyle().ScaleAllSizes(1.5f);
	//  window->setFramerateLimit(120);

	while (window->isOpen() && isRunning) {
		// calculate deltatime
		constexpr float MAX_DELTA_TIME = 0.1f;
		auto dt = clock.restart();
		float deltaTime = std::min(dt.asSeconds(), MAX_DELTA_TIME);

		frameTimes[index] = deltaTime;
		index++;
		if (index >= frameTimes.size())
			index = 0;
		fpsCalcTimer.update(deltaTime);

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
			}

			if (event->is<sf::Event::Closed>()) {
				isRunning = false;
				Logger::info("Window closed directly by user");
			} else if (event->is<sf::Event::Resized>()) {
				updateWindowSize();
				renderer.updateGuiRender();
			} else if (const auto e = event->getIf<sf::Event::KeyPressed>(); e && e->code == sf::Keyboard::Key::Escape) {
				isRunning = false;
				Logger::info("Window closed by pressing Esc");
			}
			if (currentScene) {
				if (currentScene->m_widget) {
					const bool handled = currentScene->m_widget->handleEvent(*event);
					if (!handled) {
						currentScene->handleEvent(*event);
					}
				} else
					currentScene->handleEvent(*event);
			}
		}

		updateMousePos(); // this is critical!!!

		if (enableImgui) {
			ImGui::SFML::Update(*window, dt);
		}

		// update
		if (currentScene) {
			if (currentScene->m_widget) {
				currentScene->m_widget->update(deltaTime);
			}
			currentScene->update(deltaTime);
		}

		// draw
		renderer.clear();

		if (currentScene) {
			currentScene->draw(renderer);

			if (currentScene->m_widget) {
				currentScene->m_widget->draw(renderer);
			}
		}

		text.draw(renderer);

		renderer.draw(*window);

		if (enableImgui) {
			static bool showDemo = false;
			static float value = 0.5f;
			static int clickCount = 0;

			ImGui::Begin("Imgui");

			ImGui::Text("If you can see this, ImGui-SFML is working.");
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

			ImGui::SliderFloat("Test slider", &value, 0.0f, 1.0f);

			if (ImGui::Button("Click me")) {
				clickCount++;
				Logger::info("ImGui button clicked: {}", clickCount);
			}

			ImGui::Text("Button clicks: %d", clickCount);
			ImGui::Checkbox("Show ImGui demo window", &showDemo);

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
