#include "Application.h"

#include "Scene.h"
#include "global/DebugVariables.h"
#include "global/Loader.h"
#include "src/ui/global/Global.h"
#include "src/ui/render/RenderBuffer.h"
#include "src/ui/shapes/RichTextShape.h"
#include "src/utils/Timer.h"

#include <SFML/System/Clock.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <numeric>

namespace flx::app {
	namespace fs = std::filesystem;

	int Application::loop() {
		if (runGuard) {
			logger.error_and_throw("Do not run the application more than once!");
		}
		runGuard = true;

		// Logger::info("Entering main loop...");
		// fps
		sf::Clock clock;
		Array<float, 1000> frameTimes = {};
		float averageFPS = 0;
		u64 index = 0;
		ui::RenderBuffer buffer(window.getView());

		// Logger::info("Initializing scenes...");
		// sceneManager.setCurrentScene("menu_scene");

		const auto font = Loader::loadFont(defaultFont, true);
		auto FPSText = ui::RichTextShape(*font);
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

		ui::RichTextShape richText(*font, "中文测试 ABC xyz 12345 ０１２３ ￥$%&!?.,，\n。！中文2？；：「」『』（）()[]{} / \n | +-= * _ ~ @ # ");
		richText.setPosition({200, 200});
		richText.setLineWidth(1000.f);
		sf::RectangleShape rectangle;
		rectangle.setPosition(richText.getPosition());
		rectangle.setSize(richText.getLayoutSize());
		rectangle.setFillColor(sf::Color::Cyan);

		if (imguiEnabled) {
			ImGui::GetStyle().ScaleAllSizes(1.5f);
		}

		while (window.isOpen() && isRunning) {
			// calculate deltatime
			constexpr float MAX_DELTA_TIME = 0.1f;
			auto sfDt = clock.restart();
			const float dt = std::min(sfDt.asSeconds(), MAX_DELTA_TIME);

			if (runtime.showDebugFPS) {
				frameTimes[index] = dt;
				index++;
				if (index >= frameTimes.size())
					index = 0;
				fpsCalcTimer.update(dt);
			}

			// window.updateMousePos(); // this is critical!!!

			// Logger::info("New frame started, deltaTime: {}", deltaTime);
			// Logger::info("Current scene: {}", currentScene ? currentScene->getName() : "null");

			sceneManager.handleCommands();

			auto onWindowResized = [&] {
				buffer.onWindowResized(window.getView());
				sceneManager.onWindowResized(window.getView());
			};

			// handle event
			while (const auto event = window.pollEvent()) {
				const auto& raw = event->rawEvent;
				if (shouldDisplayImgui()) {
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
				} else if (const auto e = raw.getIf<sf::Event::KeyPressed>()) {
					if (e->code == sf::Keyboard::Key::Escape) {
						isRunning = false;
						logger.info("Window closed by pressing Esc");
					} else if (e->code == sf::Keyboard::Key::F) {
						const bool isFullscreen = window.getMode() == ui::Window::Borderless;
						if (isFullscreen) {
							window.setMode(ui::Window::Windowed);
						} else {
							window.setMode(ui::Window::Borderless);
						}

						onWindowResized();
					}
				} else if (raw.is<sf::Event::Resized>()) {
					onWindowResized();
				}

				(void)sceneManager.handleEvent(*event);
			}

			if (shouldDisplayImgui()) {
				ImGui::SFML::Update(window.getWindow(), sfDt);
			}

			// update
			sceneManager.update(dt);

			// draw
			sceneManager.draw(buffer);

			if (runtime.showDebugFPS) {
				buffer.drawUI(FPSText);
			}

			static bool& showDebugText = DebugVariables::emplace<bool>("showDebugText", false);

			if (showDebugText) {
				buffer.drawUI(rectangle);
				buffer.drawUI(richText);
			}

			window.draw(buffer);

			if (shouldDisplayImgui()) {
				ImGui::Begin(runtime.imguiTitle.c_str());

				// ImGui::SeparatorText("渲染");
				//
				// if (ImGui::Button("显示Debug文本")) {
				// 	showDebugText = !showDebugText;
				// }

				sceneManager.makeImGuiContent();

				ImGui::End();

				// if (showDemo) {
				// 	ImGui::ShowDemoWindow(&showDemo);
				// }

				ImGui::SFML::Render(window.getWindow());
			}

			window.display();
		}

		if (imguiEnabled) {
			ImGui::SFML::Shutdown();
		}

		return 0;
	}

	void Application::initImgui() {
		if (!ImGui::SFML::Init(window.getWindow(), false)) {
			logger.error_and_throw("Failed to initialize ImGui-SFML");
		}
		logger.info("ImGui-SFML initialized");

		const fs::path imguiFontPath = fs::path(Loader::resource_path) / defaultFont;

		ImGuiIO& io = ImGui::GetIO();

		const auto font = io.Fonts->AddFontFromFileTTF(
			imguiFontPath.string().c_str(),
			22.f,
			nullptr,
			io.Fonts->GetGlyphRangesChineseFull());

		if (!font) {
			logger.error_and_throw("Failed to load Chinese font for ImGui");
		}

		io.FontDefault = font;

		if (!ImGui::SFML::UpdateFontTexture()) {
			logger.error_and_throw("Imgui-SFML Update font texture failed!");
		}
	}

	AppCtx Application::getContext() {
		return {
			.windowView = window.getView(),
			.input = window.input,
			.sceneManager = sceneManager,
			.runtime = runtime,
		};
	}

	bool Application::shouldDisplayImgui() const {
		return imguiEnabled && runtime.imguiDisplay;
	}

	Application::Application(AppInfo info)
		: window(info.defaultWindowSize, info.name),
		  logger(Logger::makeAsync("App", true)) {

		defaultFont = info.defaultFont;
		if (info.imguiEnabled) {
			imguiEnabled = true;
			runtime.imguiDisplay = true;
			initImgui();
		}

		logger.info("App {} initializing...", info.name);

		ui::Global::setDefaultFont(*Loader::loadFont(defaultFont, true));

		logger.info("App initialization done.");
	}

	Application::~Application() {}
} // namespace flx::app
