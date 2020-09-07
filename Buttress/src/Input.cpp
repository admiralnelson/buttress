#include "pch.h"
#include "Input.h"

Input::Input()
{
}


void Input::TickMouse(GLFWwindow* window, double xpos, double ypos)
{
	Message m;
	InputEvent input;
	input.x = xpos;
	input.y = ypos;
	m.inputEvent = input;
	m.tag = "mouse";
	PRINT("x", xpos, "y", ypos);
	PRINT("got event! sending to the bus!");
	Bus::Instance().SendMessage(m);
}

void Input::TickKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key > KEYS_END) return; 
	if (action == GLFW_PRESS)
	{
		m_pressedKeys[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
		m_pressedKeys[key] = false;
	}

	for (auto& j : m_handler)
	{
		if (m_pressedKeys[j.key])
		{
			j.callback(j.key, Bus::Instance().GetDeltaTime());
		}
	}

	Message m;
	InputEvent input;
	input.lastKey = key;
	m.inputEvent = input;
	m.tag = "keyboard";
	Bus::Instance().SendMessage(m);
}

void Input::RegisterKey(std::string name, int key, std::function<void(int key, float dT)> callback)
{
	if (!IsKeyHandlerRegistered(name))
	{
		m_handler.push_back(KeyHandler{ name, callback, key });
	}
	else
	{
		PRINT("WARNING", "handler name", name, "already exist");
	}
}

bool Input::IsKeyHandlerRegistered(std::string name)
{
	for (auto& i : m_handler)
	{
		if (i.name == name) return true;
	}
	return false;
}


Input::~Input()
{
}