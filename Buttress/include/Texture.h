#pragma once
#include "pch.h"
#include <glad/glad.h>
class Texture
{
public:
	Texture(std::string simpleName, std::string path);
	Texture(std::string simpleName, std::array<std::string, 6> paths);
	void Use();
	~Texture();
	void Debug();
private:
	int m_width, m_height, m_channel;
	GLuint m_textureNo;
	std::string m_path;
	std::string m_name;
	std::array<std::string, 6> m_paths;
	bool m_is3DTexture = false;
};

