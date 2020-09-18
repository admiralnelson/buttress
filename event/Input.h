#pragma once
#include "pch.h"
#include "glfw/glfw3.h"
#include "Bus.h"

#define KEYS_END GLFW_KEY_LAST + 1
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
	void RegisterKey(std::string name, int key, std::function<void(int key, float dT)> callback);
	void RegisterMouse(std::string name, std::function<void(Vec2 pos, float dT)> callback);
	bool IsKeyHandlerRegistered(std::string name);
	bool forwardToBus = true;
	~Input();

private:
	Input();
	struct KeyHandler
	{
		std::string name;
		std::function<void(int key, float dT)> callback;
		int key;
	};
	int lastKey = 0;
	std::unordered_map<int, bool> m_pressedKeys;
	std::vector<KeyHandler> m_handler;
};

