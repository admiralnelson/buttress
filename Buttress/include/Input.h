#pragma once
#include "pch.h"
#include "glfw/glfw3.h"
#include "Bus.h"

class Input
{
public:
	static Input& Instance()
	{
		static Input instance;
		return instance;
	}
	void TickMouse(GLFWwindow* window, double xpos, double ypos);
	void TickKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	~Input();

private:
	Input();
};

