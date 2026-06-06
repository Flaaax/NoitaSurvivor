#include "Application.h"

#include "Scene.h"
#include "global/AssetManager.h"
#include "global/DebugVariables.h"
#include "src/ui/global/NGlobal.h"
#include "src/ui/shapes/NRichTextShape.h"
#include "src/utils/Timer.h"

#include <SFML/System/Clock.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <numeric>

namespace flx::app {
	namespace {
		bool drawStringCombo(const char* label, const Util::Vector<std::string_view>& items, int& current_index) {
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
	} // namespace

	int Application::loop() {
		if (runGuard) {
			logger.error_and_throw("Do not run the application more than once!");
		}
		runGuard = true;

		// Logger::info("Entering main loop...");
		// fps
		sf::Clock clock;
		std::array<float, 1000> frameTimes = {};
		float averageFPS = 0;
		u64 index = 0;
		NRenderBuffer rdr(window.getViewport());

		// Logger::info("Initializing scenes...");
		// sceneManager.setCurrentScene("menu_scene");

		auto& font = AssetMgr::getDefaultFont();
		auto FPSText = NRichTextShape(font);
		FPSText.setPosition({5, 5});
		FPSText.setCharacterSize(22u);
		CTimer fpsCalcTimer(1.f, [&] {
			const float total = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.F);
			averageFPS = static_cast<float>(frameTimes.size()) / total;
			FPSText.setString(std::format("FPS: {}", static_cast<u32>(trunc(averageFPS))));
		});
		fpsCalcTimer.start(CTimer::infinite_trigger);

		// const auto globalWidget = Util::makeUnique(new NRootWidget());
		// globalWidget->addToTop(Util::makeUnique(text));

		bool isRunning = true;

		// NRichTextShape richText(font, "Very Very Ver Verrrrry long TEEx ver y"
		// 							  " long not end yet bruh fr fr\n中文1非常长文本Text中文？？？？？\nYeah");
		// richText.setPosition({200, 200});
		// richText.setLineWidth(200.f);
		// sf::RectangleShape rectangle;
		// rectangle.setPosition(richText.getPosition());
		// rectangle.setSize(richText.getLayoutSize());
		// rectangle.setFillColor(sf::Color::Cyan);

		ImGui::GetStyle().ScaleAllSizes(1.5f);

		while (window.isOpen() && isRunning) {
			// calculate deltatime
			constexpr float MAX_DELTA_TIME = 0.1f;
			auto sfDt = clock.restart();
			const float dt = std::min(sfDt.asSeconds(), MAX_DELTA_TIME);

			if (showDebugFPS) {
				frameTimes[index] = dt;
				index++;
				if (index >= frameTimes.size())
					index = 0;
				fpsCalcTimer.update(dt);
			}

			// window.updateMousePos(); // this is critical!!!

			// Logger::info("New frame started, deltaTime: {}", deltaTime);
			if (sceneManager.shouldChangeScene()) {
				sceneManager.changeScene();
			}

			const auto currentScene = sceneManager.getCurrentScene();
			// Logger::info("Current scene: {}", currentScene ? currentScene->getName() : "null");

			// handle event
			while (const auto event = window.pollEvent()) {
				const auto& raw = event->rawEvent;
				if (imguiEnabled) {
					ImGui::SFML::ProcessEvent(window.getWindow(), event->rawEvent);
					const bool isMouseEvent =
						raw.is<sf::Event::MouseButtonPressed>() ||
						raw.is<sf::Event::MouseButtonReleased>() ||
						raw.is<sf::Event::MouseMoved>() ||
						raw.is<sf::Event::MouseWheelScrolled>();

					if (isMouseEvent && ImGui::GetIO().WantCaptureMouse) {
						continue; // ImGui 吃掉鼠标事件
					}
				}

				if (raw.is<sf::Event::Closed>()) {
					isRunning = false;
					logger.info("App window is closed by user");
				} else if (const auto e = raw.getIf<sf::Event::KeyPressed>(); e && e->code == sf::Keyboard::Key::Escape) {
					isRunning = false;
					logger.info("Window closed by pressing Esc");
				}
				if (currentScene) {
					currentScene->handleEvent(*event);
				}
			}

			if (imguiEnabled) {
				ImGui::SFML::Update(window.getWindow(), sfDt);
			}

			// update
			if (currentScene) {
				currentScene->update(dt);
			}

			// draw

			if (currentScene) {
				currentScene->draw(rdr);
			}

			rdr.draw(FPSText);

			// static bool& showDebugText = DebugVariables::try_emplace<bool>("showDebugText", false);
			//
			// if (showDebugText) {
			// 	rdr.drawUI(rectangle);
			// 	rdr.drawUI(richText);
			// }

			window.draw(rdr);

			if (imguiEnabled) {
				ImGui::Begin("Imgui");

				// ImGui::SeparatorText("渲染");
				//
				// if (ImGui::Button("显示Debug文本")) {
				// 	showDebugText = !showDebugText;
				// }

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

				if (drawStringCombo("跟踪算法", trackers, selectedTracker)) {
					// Logger::info("选择了 {}", trackers[selected]);
				}

				ImGui::End();

				// if (showDemo) {
				// 	ImGui::ShowDemoWindow(&showDemo);
				// }

				ImGui::SFML::Render(window.getWindow());
			}
		}

		ImGui::SFML::Shutdown();
		return 0;
	}

	AppContext Application::getContext() {
		return {
			.windowViewport = window.getViewport(),
			.input = window.input,
			.sceneManager = sceneManager,
		};
	}

	Application::Application(const AppInit& info)
		: window(info.defaultWindowSize, info.name),
		  logger(Logger::makeAsync("App", true)) {

		imguiEnabled = info.imguiEnabled;
		showDebugFPS = info.displayDebugFPS;

		logger.info("App {} initializing...", info.name);

		AssetMgr::init();

		if (imguiEnabled) {
			if (!ImGui::SFML::Init(window.getWindow(), false)) {
				logger.error("Failed to initialize ImGui-SFML");
			} else {
				logger.info("ImGui-SFML initialized");
			}

			ImGuiIO& io = ImGui::GetIO();

			const auto font = io.Fonts->AddFontFromFileTTF(
				info.imGuiFontPath.c_str(),
				22.f,
				nullptr,
				io.Fonts->GetGlyphRangesChineseFull());

			if (!font) {
				logger.error_and_throw("Failed to load Chinese font for ImGui");
			}

			io.FontDefault = font;

			if (!ImGui::SFML::UpdateFontTexture()) {
				logger.error("Imgui-SFML Update font texture failed!");
			}

		} else {
			logger.info("Skip ImGui initialization");
		}

		NGlobal::setDefaultFont(AssetMgr::getDefaultFont());

		logger.info("App initialization done.");
	}
} // namespace flx::app
