#pragma once
#include "pch.h"
#include <glad/glad.h>


class TextureData
{
	friend class TextureLoader;
public:
	TextureData() {}
	void Load(std::string path);
	//MUST BE EXECUTED ON MAIN THREAD!. Returns false, if it's not ready yet!
	void Use(unsigned int glTextureIndex = 0);
	bool IsReady();
	~TextureData();
	void Debug();
	
private:
	int m_width = 0, m_height = 0, m_channel = 0;
	GLuint m_textureNo = -1;
	std::string m_path;
	std::shared_ptr<unsigned char> m_data;
	bool m_isReady = false;
	bool m_is3DTexture = false;
};


class TextureLoader
{
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
private: 
	std::unordered_map<std::string, std::shared_ptr<TextureData>> m_texturesList;
	TextureLoader() {}
};
