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
	Message m;
	InputEvent input;
	input.key = key;
	input.scanCode = scancode;
	input.keyAction = (KeyboardButtonState) action;
	m.inputEvent = input;
	m.tag = "keyboard";
	Bus::Instance().SendMessage(m);
}

Input::~Input()
{
}