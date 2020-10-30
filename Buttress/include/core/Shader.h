#pragma once
#include "pch.h"
#include <glad/glad.h>

#define ATTRIBUTE_POS "aPos"
#define ATTRIBUTE_NORMAL "aNormal"
#define ATTRIBUTE_UV "aUv"
#define ATTRIBUTE_UV2 "aUv2"
#define ATTRIBUTE_BONE_IDS "aBoneIds"
#define ATTRIBUTE_BONE_WEIGHTS "aWeights"

#define UNIFORM_SAMPLER2D_DIFFUSE "material.diffuse"
#define UNIFORM_SAMPLER2D_SPECULAR "material.specular"
#define UNIFORM_FLOAT_SHININESS "material.shininess"
#define UNIFORM_ARRAY_MATRIX4_BONES "bones[0..99]"
#define UNIFORM_ARRAY_MATRIX4_BONE "bones"

#define UNIFORM_MATRIX4_PVM "pvm"
#define UNIFORM_MATRIX4_MODEL "model"


class Shader
{
	friend class ModelData;
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
	bool SetUniformMat4x4Array(std::string _name, unsigned int size, Matrix4& mat);

	bool SetUniformValueI(std::string _name, int x);
	bool SetUniformValueI(std::string _name, int x, int y);
	bool SetUniformValueI(std::string _name, int x, int y, int z);
	bool SetUniformValueI(std::string _name, int x, int y, int z, int w);
	bool SetUniformValueI(std::string _name, std::vector<int>& v);

	void Debug();
	void LoadShaderPack(std::string jsonString);
	void AddVertexShader(std::string source);
	void AddGeometryShader(std::string source);
	void AddTesselationControlShader(std::string source);
	void AddTesselationEvaluationShader (std::string source);
	void AddFragmentShader(std::string source);
	void AddComputeShader(std::string source);
	void CompileShader();
	bool IsShaderReady();
	bool Validate();
	bool IsButtressConstantDefined(std::string name);
	bool IsUniformDefined(std::string _name);
	bool IsUniformArrayDefined(std::string name);
	bool IsAttributeDefined(std::string _name);
	bool IsStructExist(std::string name);
	~Shader();
private:
	struct ShaderParam
	{
		std::string type;
		GLint valuePos;
		GLint arraySize;
	};
	struct ShaderKeyValue
	{
		std::string name;
		ShaderParam param;
	};

	void AddProgram(std::string source, int type);
	bool AddUniformAux(std::string _name, std::string type = "", GLint arraySize = 0);
	void FindAndLocateStructs(std::string source);
	void FindAndLocateAttributes(std::string source);
	void FindAndLocateUniforms(std::string source);
	void FindButressConstants(std::string source);
	bool ValidateUniformRecursively(std::string name, ShaderParam type, std::string parent = "");
	
	void CheckVaryings(std::string source);
	std::vector<std::string> SplitToVector(std::string input);
	bool CheckError(std::string lastOperation_name, std::vector<std::string> params);
private:
	std::unordered_map<std::string, std::vector<ShaderKeyValue>> m_structs;
	std::unordered_map<std::string, ShaderParam> m_uniforms;
	std::unordered_map<std::string, ShaderParam> m_attributes;
	std::unordered_map<std::string, ShaderParam> m_varyings;
	std::unordered_map<std::string, GLint> m_buttressConstants;
	std::vector<ShaderKeyValue> m_uniformsAux;
	GLuint m_program = -1;	
	bool m_isReady = false;
	static std::shared_ptr<Shader> m_defaultShader;
};
