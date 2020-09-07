// Buttress.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "Logger.h"
#include "Buttress.h"
#include "Shader.h"
#include "Util.h"

Buttress* Buttress::m_thisInstance;

Buttress::Buttress()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_thisInstance = this;
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
	Input::Instance();
	m_primitiveDraw.reset(new PrimitiveDraw());
	glViewport(0, 0, m_width, m_height);
	glfwMakeContextCurrent(m_window.get());
	glfwSetWindowUserPointer(m_window.get(), reinterpret_cast<void*>(this));
	glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(m_window.get(), m_width/ 2, m_height/ 2);
	auto resizeCallback = [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Buttress* instance = reinterpret_cast<Buttress*>(glfwGetWindowUserPointer(window));
		if (instance->OnResize != nullptr)
		{
			instance->OnResize(width, height);
			Message msg;
			msg.windowEvent = { 0, 0, width, height, true };
			msg.tag = "OnResize";
			msg.msg = "Buttress";
			Bus::Instance().SendMessage(msg);
		}
	};
	glfwSetFramebufferSizeCallback(m_window.get(), resizeCallback);
	
	auto mouseEvent = [](GLFWwindow* window, double x, double y)
	{
		Input::Instance().TickMouse(window, x, y);
	};
	glfwSetCursorPosCallback(m_window.get(), mouseEvent);

	auto keyEvent = [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Input::Instance().TickKey(window, key, scancode, action, mods);
	};

	glfwSetKeyCallback(m_window.get(), keyEvent);


	return true;
}

void Buttress::Start()
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	m_currentDirectory = converter.to_bytes(std::filesystem::current_path().c_str());

	//Bus::Instance().Debug();

	PRINT("CURRENT DIRECTORY:", m_currentDirectory);
	
	if (OnStart)
	{
		if (!OnStart())
		{
			Shutdown();
			PRINT("ERROR", "OnStart returns false, closing program....");
			return;
		}
	}


	for (size_t i = 0; i < m_primitiveDraw->GetBeginQueueSize(); i++)
	{
		DrawCommand cmd = m_primitiveDraw->GetBeginQueue(i);
		if (cmd.Command)
		{
			cmd.Command();
		}
		else
		{
			PRINT("WARN", "EMPTY command at index ", i, "desc: ", cmd.Description);
		}
	}

	Bus::Instance().Start();
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
		if (OnLoop)
		{
			OnLoop();
		}
		if (m_primitiveDraw->GetLoopQueueSize() > 0)
		{
			DrawCommand cmd = m_primitiveDraw->GetCurrentLoopQueue();
			if (cmd.Command)
			{
				cmd.Command();
			}
			else
			{
				PRINT("WARN", "EMPTY command ", "desc: ", cmd.Description);
			}
		}
		Bus::Instance().SetDeltaTime(deltaTime);
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
	if (OnShutdown)
	{
		OnShutdown();
	}
	glfwTerminate();
}

Buttress::~Buttress()
{
}