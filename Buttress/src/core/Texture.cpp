#pragma once
#include "pch.h"
#include "core\Texture.h"
#include "Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void TextureData::Load(std::string path)
{
	m_width = 0;
	m_height = 0;
	m_textureNo = 0;
	m_channel = 0;
	m_path = path;
	stbi_set_flip_vertically_on_load(true);
	m_data.reset(stbi_load(path.c_str(), &m_width, &m_height, &m_channel, 0));
	if (m_data == nullptr)
	{
		PRINT("ERROR", "unable to load file", path);
		throw std::runtime_error("texture fail to load");
	}
	PRINT("INFO", "texture type 2D has been loaded to RAM. filename: ", m_path);
}

void TextureData::Use(unsigned int glTextureIndex)
{
	if (m_textureNo == 0)
	{
		glGenTextures(1, &m_textureNo);
		glBindTexture(GL_TEXTURE_2D, m_textureNo);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		switch (m_channel)
		{
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data.get());
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data.get());
			break;
		default:
			PRINT("ERROR", "unsupported nr of channel. expected 4 or 3 got", m_channel, "filename:", m_path);
			throw std::runtime_error("texture fail to load");
			break;
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		PRINT("INFO", "texture type 2D has been loadded to GPU. filename: ", m_path, "texture nr:", m_textureNo);
		m_data.reset();
		glActiveTexture(GL_TEXTURE0 + glTextureIndex);
		glBindTexture(GL_TEXTURE_2D, m_textureNo);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0 + glTextureIndex);
		glBindTexture(GL_TEXTURE_2D, m_textureNo);
	}
}

bool TextureData::IsReady()
{
	return m_isReady;
}



TextureData::~TextureData()
{
	glDeleteTextures(1, &m_textureNo);
}

void TextureData::Debug()
{
	PRINT("INFO", "-----------------");
	PRINT("INFO", "texture path:", m_path);
	PRINT("INFO", "texture nr:", m_textureNo);
}
