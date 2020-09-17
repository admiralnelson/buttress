#pragma once
#include "pch.h"
#include "core\Texture.h"
#include "Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::unordered_map<std::string, std::shared_ptr<Texture>> Texture::m_texturesList;

Texture::Texture(std::string path)
{
	m_width = 0;
	m_height = 0;
	m_textureNo = 0;
	m_channel = 0;
	m_path = path;
	std::shared_ptr<unsigned char> data;
	stbi_set_flip_vertically_on_load(true);
	data.reset(stbi_load(path.c_str(), &m_width, &m_height, &m_channel, 0));
	if (data == nullptr)
	{
		PRINT("ERROR", "unable to load file", path);
		throw new std::exception("texture fail to load", -1);
	}
	glGenTextures(1, &m_textureNo);
	glBindTexture(GL_TEXTURE_2D, m_textureNo);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	switch (m_channel)
	{
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.get());
		break;
	default:
		PRINT("ERROR", "unsupported nr of channel. expected 4 or 3 got", m_channel, "filename:", path);
		throw new std::exception("texture fail to load", -1);
		break;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	PRINT("INFO", "texture type 2D has been created. filename: ", m_path, "texture nr:", m_textureNo);
}

void Texture::Use()
{
	//todo: for multiple texture!
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureNo);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureNo);
}

void Texture::Debug()
{
	PRINT("INFO", "-----------------");
	PRINT("INFO", "texture path:", m_path);
	PRINT("INFO", "texture nr:", m_textureNo);
}
