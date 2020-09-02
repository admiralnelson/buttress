#include "pch.h"
#include "Input.h"

Input::Input()
{
}


void Input::Tick(GLFWwindow* window, double xpos, double ypos)
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

Input::~Input()
{
}