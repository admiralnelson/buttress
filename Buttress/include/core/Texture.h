#pragma once
#include "pch.h"
#include <glad/glad.h>


class TextureData
{
	friend class Material;
	friend class TextureLoader;
public:
	TextureData() {}
	void Use();
	~TextureData();
	void Debug();
	
private:
	void Load(std::string path);
	int m_width, m_height, m_channel;
	GLuint m_textureNo;
	std::string m_path;
	bool m_is3DTexture = false;
};


class TextureLoader
{
private: 
	std::unordered_map<std::string, std::shared_ptr<TextureData>> m_texturesList;
	TextureLoader() {}
public:
	static TextureLoader& Instance()
	{
		static TextureLoader instance;
		return instance;
	}
	std::shared_ptr<TextureData> LoadTexture(std::string path)
	{
		if (m_texturesList.find(path) == m_texturesList.end())
		{
			std::shared_ptr texture = std::make_shared<TextureData>();
			texture->Load(path);
			m_texturesList[path] = texture ;
		}
		return m_texturesList[path];
	}
};
