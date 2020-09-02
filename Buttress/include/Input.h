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
	void Tick(GLFWwindow* window, double xpos, double ypos);
	~Input();

private:
	Input();
};

