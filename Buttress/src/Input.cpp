#include "pch.h"
#include "Input.h"

Input::Input()
{
}


void Input::Tick(GLFWwindow* window, double xpos, double ypos)
{
	Message m;
	MouseEvent mouse;
	mouse.x = xpos;
	mouse.y = ypos;
	m.event = &mouse;
	m.tag = "mouse";
	PRINT("x", xpos, "y", ypos);
	PRINT("got event! sending to the bus!");
	Bus::Instance().SendMessage(m);
	PRINT(m.event->ToString());
}

Input::~Input()
{
}