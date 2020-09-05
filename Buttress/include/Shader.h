#pragma once
#include "pch.h"
#include <glad/glad.h>

#define ATTRIBUTE_POS "aPos"
#define ATTRIBUTE_COLOR "aColor"
#define ATTRIBUTE_UV "aUv"

#define UNIFORM_SAMPLER2D_DIFFUSE "diffuse"
#define UNIFORM_SAMPLER2D_SPECULAR "specular"
#define UNIFORM_FLOAT_SHININESS "shininess"


class Shader
{
public:
	Shader(std::string _name);
	std::string name;
	void Use();
	bool AddUniform(std::string _name, std::string type = "", GLint arraySize = 0);
	bool AddAttribute(std::string _name, std::string type = "");
	GLint GetAttributeLocation(std::string _name);
	GLint GetUniformLocation(std::string _name);
	bool SetUniformValueF(std::string _name, float x);
	bool SetUniformValueF(std::string _name, float x, float y);
	bool SetUniformValueF(std::string _name, float x, float y, float z);
	bool SetUniformValueF(std::string _name, float x, float y, float z, float w);
	bool SetUniformValueF(std::string _name, std::vector<float> &v);

	bool SetUniformMat4x4(std::string _name, Matrix4 &mat);

	bool SetUniformValueI(std::string _name, int x);
	bool SetUniformValueI(std::string _name, int x, int y);
	bool SetUniformValueI(std::string _name, int x, int y, int z);
	bool SetUniformValueI(std::string _name, int x, int y, int z, int w);
	bool SetUniformValueI(std::string _name, std::vector<int>& v);

	void Debug();
	void AddVertexShader(std::string source);
	void AddFragmentShader(std::string source);
	void CompileShader();
	bool IsShaderReady();
	bool Validate();
	bool IsUniformDefined(std::string _name);
	bool IsAttributeDefined(std::string _name);
	~Shader();
private:
	void AddProgram(std::string source, int type);
	void FindAndLocateAttributes(std::string source);
	void FindAndLocateUniforms(std::string source);
	void CheckVaryings(std::string source);
	std::vector<std::string> SplitToVector(std::string input);
	bool CheckError(std::string lastOperation_name, std::vector<std::string> params);
private:
	struct ShaderParam 
	{
		std::string type;
		GLint valuePos;
		GLint arraySize; 
	};
	std::unordered_map<std::string, ShaderParam> m_uniforms;
	std::unordered_map<std::string, ShaderParam> m_attributes;
	std::unordered_map<std::string, ShaderParam> m_varyings;
	GLuint m_program = -1;	
	bool m_isReady = false;
};
