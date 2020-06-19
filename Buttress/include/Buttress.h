#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Buttress
{
public:
	Buttress();
	bool Init(int width, int height, std::string title);
	void Start();
	std::function<void(int width, int height)> OnResize;
	~Buttress();

private:
	std::shared_ptr<GLFWwindow> m_window;
	int m_width = 0, m_height = 0;
};
