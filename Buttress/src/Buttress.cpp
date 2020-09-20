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
		//instance->universe->sendevent...mouse event, x, y
		//Input::Instance().TickMouse(window, x, y);
		//Input::Instance().TickKey(window, key, scancode, action, mods);
	};

	glfwSetKeyCallback(m_window.get(), keyEvent);



	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	m_currentDirectory = converter.to_bytes(std::filesystem::current_path().c_str());

	//Bus::Instance().Debug();

	PRINT("CURRENT DIRECTORY:", m_currentDirectory);
	
	float frameBegin = 0;
	float frameEnd = 0;
	double lasttime = glfwGetTime();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(m_window.get()))
	{
		frameBegin = glfwGetTime();
		float deltaTime = frameBegin - frameEnd;
		frameEnd = frameBegin;
		glClearColor(0.3, 0.4, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		m_universe->Render(deltaTime);

		glfwSwapBuffers(m_window.get());
		glfwPollEvents();
		while (glfwGetTime() < lasttime + 1.0 / 60) {
		}
		lasttime += 1.0 / 60;
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