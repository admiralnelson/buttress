#pragma once
#include "pch.h"
#include <glad/glad.h>

class Shader
{
public:
	Shader(std::string name);
	void Use();
	bool AddUniform(std::string name, std::string type = "", GLint arraySize = 0);
	bool AddAttribute(std::string name, std::string type = "");
	GLint GetAttributeLocation(std::string name);
	GLint GetUniformLocation(std::string name);
	bool SetUniformValueF(std::string name, float x);
	bool SetUniformValueF(std::string name, float x, float y);
	bool SetUniformValueF(std::string name, float x, float y, float z);
	bool SetUniformValueF(std::string name, float x, float y, float z, float w);
	bool SetUniformValueF(std::string name, std::vector<float> v);

	bool SetUniformValueI(std::string name, int x);
	bool SetUniformValueI(std::string name, int x, int y);
	bool SetUniformValueI(std::string name, int x, int y, int z);
	bool SetUniformValueI(std::string name, int x, int y, int z, int w);
	bool SetUniformValueI(std::string name, std::vector<int> v);

	void Debug();
	void AddVertexShader(std::string source);
	void AddFragmentShader(std::string source);
	void CompileShader();
	bool IsShaderReady();
	bool Validate();
	bool IsUniformDefined(std::string name);
	bool IsAttributeDefined(std::string name);
	~Shader();
private:
	void AddProgram(std::string source, int type);
	void FindAndLocateAttributes(std::string source);
	void FindAndLocateUniforms(std::string source);
	void CheckVaryings(std::string source);
	std::vector<std::string> SplitToVector(std::string input);
	bool CheckError(std::string lastOperationName, std::vector<std::string> params);
private:
	struct ShaderParam 
	{
		std::string type;
		GLint valuePos;
		GLint arraySize; 
	};
	std::string m_name;
	std::unordered_map<std::string, ShaderParam> m_uniforms;
	std::unordered_map<std::string, ShaderParam> m_attributes;
	std::unordered_map<std::string, ShaderParam> m_varyings;
	GLuint m_program = -1;	
	bool m_isReady = false;
};
