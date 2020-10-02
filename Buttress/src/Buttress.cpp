// Buttress.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "Logger.h"
#include "Buttress.h"
#include "Util.h"
#include "ecs/EventManager.h"

Buttress::Buttress()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

bool Buttress::Init(int width, int height, std::string title)
{
	m_width = width;
	m_height = height;
	m_window.reset(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr));
	if (m_window == nullptr)
	{
		PRINT("failed to init the window!");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_window.get());
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		PRINT("failed to init the glad!");
		glfwTerminate();
		return false;
	}

	glViewport(0, 0, m_width, m_height);
	glfwMakeContextCurrent(m_window.get());
	glfwSetWindowUserPointer(m_window.get(), reinterpret_cast<void*>(this));
	glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPos(m_window.get(), m_width/ 2, m_height/ 2);
	return true;
}

void Buttress::Start(Universe *universe)
{
	if (!universe)
	{
		PRINT("ERROR", "no universe to simulate with!");
		Shutdown();
	}
	m_universe = universe;

	auto resizeCallback = [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Buttress* instance = reinterpret_cast<Buttress*>(glfwGetWindowUserPointer(window));
		Event event(WINDOW_EVENT::WINDOW_RESIZE);
		event.SetParam<unsigned int>(WINDOW_EVENT::PARAMS::PARAM_WIDTH, width);
		event.SetParam<unsigned int>(WINDOW_EVENT::PARAMS::PARAM_HEIGHT, height);
		instance->GetUniverse()->SendEvent(event);
	};
	glfwSetFramebufferSizeCallback(m_window.get(), resizeCallback);

	auto mouseEvent = [](GLFWwindow* window, double x, double y)
	{
		Buttress* instance = reinterpret_cast<Buttress*>(glfwGetWindowUserPointer(window));
		Event event(MOUSE_EVENT::MOUSE_MOVE);
		event.SetParam<double>(MOUSE_EVENT::PARAMS::MOUSE_X, x);
		event.SetParam<double>(MOUSE_EVENT::PARAMS::MOUSE_Y, y);
		instance->GetUniverse()->SendEvent(event);
	};
	glfwSetCursorPosCallback(m_window.get(), mouseEvent);

	auto keyEvent = [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Buttress* instance = reinterpret_cast<Buttress*>(glfwGetWindowUserPointer(window));
		Event event(KEYBOARD_EVENT::KEYBOARD_PRESS);
		event.SetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON, (unsigned int)key);
		instance->GetUniverse()->SendEvent(event);
	};

	glfwSetKeyCallback(m_window.get(), keyEvent);



	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	m_currentDirectory = converter.to_bytes(std::filesystem::current_path().c_str());

	//Bus::Instance().Debug();

	PRINT("CURRENT DIRECTORY:", m_currentDirectory);
	
	double currentTime = glfwGetTime();
	double lastTime = 0;
	double accumulateDt = 0;
	int frameCount = 0;
	int FRAME_RATE = 60;
	while (!glfwWindowShouldClose(m_window.get()))
	{
		currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		accumulateDt += delta;
		frameCount++;
		if (accumulateDt >= 1)
		{
			glfwSetWindowTitle(m_window.get(), (const char*)std::to_string(frameCount).c_str());
			frameCount = 0;
			accumulateDt = 0;
		}
		
		m_universe->Render(delta);

		glfwSwapBuffers(m_window.get());
		glfwPollEvents();
		lastTime = currentTime;
		std::chrono::duration<double, std::milli> sleepTime(1000 / 60);
		std::this_thread::sleep_for(sleepTime);

	}
	Shutdown();
}

void Buttress::Shutdown()
{
	//send shutdown event....
	glfwTerminate();
}

Buttress::~Buttress()
{
}