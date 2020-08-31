// Buttress.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "Logger.h"
#include "Buttress.h"
#include "Shader.h"
#include "Util.h"

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
	auto resizeCallback = [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Buttress* instance = reinterpret_cast<Buttress*>(glfwGetWindowUserPointer(window));
		if (instance->OnResize != nullptr)
		{
			instance->OnResize(width, height);
		}
	};
	glfwSetFramebufferSizeCallback(m_window.get(), resizeCallback);

	m_primitiveDraw.reset(new PrimitiveDraw());

	return true;
}

void Buttress::Start()
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	m_currentDirectory = converter.to_bytes(std::filesystem::current_path().c_str());
	
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



	while (!glfwWindowShouldClose(m_window.get()))
	{
		glClearColor(0.3, 0.4, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
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
		glfwSwapBuffers(m_window.get());
		glfwPollEvents();
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