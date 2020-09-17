#pragma once
#include "pch.h"
#include <glad/glad.h>
class Texture
{
friend class Material;
public:
	Texture(std::string path);
	void Use();
	~Texture();
	void Debug();
private:
	int m_width, m_height, m_channel;
	GLuint m_textureNo;
	std::string m_path;
	bool m_is3DTexture = false;
	static std::unordered_map<std::string, std::shared_ptr<Texture>> m_texturesList;
};

