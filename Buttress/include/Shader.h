#pragma once
#include "pch.h"
#include <glad/glad.h>

class Shader
{
public:
	Shader(std::string name);
	void Use();
	void AddUniform(std::string name);
	void AddAttribute(std::string name);
	GLint GetAttributeLocation(std::string name);
	GLint GetUniformLocation(std::string name);
	void SetUniformValueF(std::string name, float x);
	void SetUniformValueF(std::string name, float x, float y);
	void SetUniformValueF(std::string name, float x, float y, float z);
	void SetUniformValueF(std::string name, float x, float y, float z, float w);
	void SetUniformValueF(std::string name, std::vector<float> v);

	void SetUniformValueI(std::string name, int x);
	void SetUniformValueI(std::string name, int x, int y);
	void SetUniformValueI(std::string name, int x, int y, int z);
	void SetUniformValueI(std::string name, int x, int y, int z, int w);
	void SetUniformValueI(std::string name, std::vector<int> v);

	void Debug();
	void AddVertexShader(std::string source);
	void AddFragmentShader(std::string source);
	void CompileShader();
	bool IsShaderReady();
	~Shader();
private:
	void AddProgram(std::string source, int type);
	
private:
	std::string m_name;
	std::unordered_map<std::string, int> m_uniforms;
	std::unordered_map<std::string, int> m_attributes;
	GLuint m_program = -1;	
	bool m_isReady = false;
};
