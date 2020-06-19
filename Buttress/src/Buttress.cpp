// Buttress.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "Buttress.h"
#include "Logger.h"
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

	return true;
}


void Buttress::Start()
{
	while (!glfwWindowShouldClose(m_window.get()))
	{
		glfwSwapBuffers(m_window.get());
		glfwPollEvents();
	}
	glfwTerminate();
}

Buttress::~Buttress()
{
}