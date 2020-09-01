#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PrimitiveDraw.h"
#include "Bus.h"
#include "Input.h"

struct DestroyglfwWin {
	void operator()(GLFWwindow* ptr)
	{
		glfwDestroyWindow(ptr);
	}
};


class Buttress
{
public:
	Buttress();
	bool Init(int width, int height, std::string title);
	void Start();
	void Shutdown();
	std::function<void(int width, int height)> OnResize;
	std::function<bool()> OnStart;
	std::function<void()> OnShutdown;
	std::function<void()> OnLoop;	
	static std::shared_ptr<Buttress> ButtressInstance() 
	{
		return m_thisInstance;
	}
	~Buttress();
private:
	std::unique_ptr<GLFWwindow, DestroyglfwWin> m_window;
	std::shared_ptr<PrimitiveDraw> m_primitiveDraw;
	static std::shared_ptr<Buttress> m_thisInstance;
	std::string m_currentDirectory;
	std::thread m_logicLoop;
	int m_width = 0, m_height = 0;
	bool m_running = false;
};
