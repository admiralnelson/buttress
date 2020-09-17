#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PrimitiveDraw.h"
#include "ecs/ECS.h"

class Buttress
{
	friend class Universe;
public:
	Buttress();
	bool Init(int width, int height, std::string title);
	//universe deletion is managed by client program
	void Start(Universe *universe);
	void Shutdown();
	int Width() { return m_width; };
	int Height() { return m_height; };
	~Buttress();
	
private:
	struct DestroyglfwWin {
		void operator()(GLFWwindow* ptr)
		{
			glfwDestroyWindow(ptr);
		}
	};

	Universe* m_universe; //deletion is managed by client program
	std::unique_ptr<GLFWwindow, DestroyglfwWin> m_window;
	std::string m_currentDirectory;
	int m_width = 0, m_height = 0;
	bool m_running = false;


};
