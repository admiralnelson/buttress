#pragma once
#include "pch.h"
#include "Texture.h"
#include "Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
static std::unordered_map<std::string, Texture*> textureList;

Texture::Texture(std::string simpleName, std::string path)
{
	m_width = 0;
	m_height = 0;
	m_textureNo = 0;
	m_channel = 0;
	m_path = path;
	m_name = simpleName;
	std::shared_ptr<unsigned char> data;
	data.reset(stbi_load(path.c_str(), &m_width, &m_height, &m_channel, 0));
	if (data == nullptr)
	{
		PRINT("ERROR", "unable to load file", path);
		throw new std::exception("texture fail to load", -1);
	}
	glGenTextures(1, &m_textureNo);
	glBindTexture(GL_TEXTURE_2D, m_textureNo);
	switch (m_channel)
	{
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNALED, data.get());
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNALED, data.get());
		break;
	default:
		PRINT("ERROR", "unsupported nr of channel. expected 4 or 3 got", m_channel, "filename:", path);
		throw new std::exception("texture fail to load", -1);
		break;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	
	PRINT("INFO", "texture type 2D has been created. name:", m_name, "filename: ", m_path, "texture nr:", m_textureNo);
	textureList[m_name] = this;
}

Texture::Texture(std::string simpleName, std::array<std::string, 6> paths)
{
}

Texture::~Texture()
{

}

void Texture::Debug()
{
	PRINT("INFO", "-----------------");
	PRINT("INFO", "texture name:", m_name);
	PRINT("INFO", "texture path:", m_path);
	PRINT("INFO", "texture nr:", m_textureNo);
}
