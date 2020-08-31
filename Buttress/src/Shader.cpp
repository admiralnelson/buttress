#include "pch.h"
#include "Shader.h"
#include "Logger.h"

 static std::unordered_map<std::string, Shader*> shaderList;

Shader::Shader(std::string name)
{
	m_name = name;
	if (shaderList.find(name) != shaderList.end())
	{
		PRINT("ERROR", "shader", name, "has been created already!");
		for (auto& s : shaderList)
		{
			PRINT("   ", "name ", s.first, " location :", s.second);
		}
		throw new std::exception("shader error", -1);
	}
	m_program = glCreateProgram();
	PRINT("INFO", "shader program has been created:", m_name, "program nr:", m_program);
	shaderList[m_name] = this;
}

void Shader::Use()
{
	if (!IsShaderReady())
	{
		PRINT("ERROR", "shader is not compiled", m_name);
		throw new std::exception("shader error", -1);
	}
	glUseProgram(m_program);
}


//TODO: automatic parse list of uniform and vertex
bool Shader::AddUniform(std::string name)
{
	int uniformLocation = glGetUniformLocation(m_program, name.c_str());
	if (uniformLocation == -1)
	{
		PRINT("WARNING", "unable to locate uniform in the shader:", name, "from shader:", m_name);
		return false;
	}
	m_uniforms[name] =  uniformLocation;
	return true;
}

bool Shader::AddAttribute(std::string name)
{
	int uniformLocation = glGetAttribLocation(m_program, name.c_str());
	if (uniformLocation == -1)
	{
		PRINT("WARNING", "unable to locate attribute in the shader:", name, "from shader:", m_name);
		return false;
	}
	m_attributes[name] = uniformLocation;
	return true;
}

GLint Shader::GetAttributeLocation(std::string name) 
{
	return m_attributes[name];
}

GLint Shader::GetUniformLocation(std::string name)
{
	return m_attributes[name];
}

void Shader::SetUniformValueF(std::string name, float x)
{
}

void Shader::SetUniformValueF(std::string name, float x, float y)
{
}

void Shader::SetUniformValueF(std::string name, float x, float y, float z)
{
}

void Shader::SetUniformValueF(std::string name, float x, float y, float z, float w)
{
}

void Shader::SetUniformValueF(std::string name, std::vector<float> v)
{
}

void Shader::SetUniformValueI(std::string name, int x)
{
}

void Shader::SetUniformValueI(std::string name, int x, int y)
{
}

void Shader::SetUniformValueI(std::string name, int x, int y, int z)
{
}

void Shader::SetUniformValueI(std::string name, int x, int y, int z, int w)
{
}

void Shader::SetUniformValueI(std::string name, std::vector<int> v)
{
}

void Shader::Debug()
{
	PRINT("INFO", "-----------------");
	PRINT("INFO", "shader name:", m_name);
	PRINT("INFO", "is shader ready: ", m_isReady ? "yes" : "no");
	PRINT("INFO", "shader program nr:", m_program);
	PRINT("INFO", "registered uniforms:");
	for (auto& i : m_uniforms)
	{
		PRINT("    ", i.first);
	}
	PRINT("INFO", "registered attributes:");
	for (auto& i : m_attributes)
	{
		PRINT("    ", i.first, "location at shader:", GetAttributeLocation(i.first));
	}
	

}

void Shader::AddVertexShader(std::string source)
{
	AddProgram(source, GL_VERTEX_SHADER);
}

void Shader::AddFragmentShader(std::string source)
{
	AddProgram(source, GL_FRAGMENT_SHADER);
}

void Shader::CompileShader()
{
	glLinkProgram(m_program);
	GLint status;
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if (!status)
	{
		PRINT("ERROR", "shader", m_name, "is not linkable.");
		GLint maxLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(m_program, maxLength, &maxLength, &errorLog[0]);
		

		PRINT("ERROR LINKAGE INFO", std::string(errorLog.begin(), errorLog.end()));		
		throw new std::exception("program shader error", -1);
	}
	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
	if (!status)
	{
		PRINT("ERROR", "shader", m_name, "is not valid.");
		GLint maxLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(m_program, maxLength, &maxLength, &errorLog[0]);


		PRINT("ERROR VALIDATION INFO", std::string(errorLog.begin(), errorLog.end()));
		throw new std::exception("program shader error", -1);
	}
	m_isReady = true;
}

bool Shader::IsShaderReady()
{
	return m_isReady;
}

Shader::~Shader()
{
	shaderList.erase(m_name);
	glDeleteProgram(m_program);
	PRINT("INFO", "shader has been erased", m_name);
}

void Shader::AddProgram(std::string source, int type)
{
	int shader = glCreateShader(type);
	if (!shader)
	{
		PRINT("ERROR", "shader type", type, "is not compilable.", "faulting shader:", m_name);
		throw new std::exception("shader error", -1);
	}
	const char* data[1] = { source.c_str() };
	glShaderSource(shader, 1, data, 0);
	glCompileShader(shader);

	GLint isCompiled = false;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled)
	{
		PRINT("ERROR", "shader type", type, "is not compilable due to compilation error.", "faulting shader:", m_name);
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		glDeleteShader(shader);		

		PRINT("ERROR COMPILATION INFO", std::string(errorLog.begin(), errorLog.end()));
		throw new std::exception("shader error", -1);
	}
	glAttachShader(m_program, shader);
	glDeleteShader(shader);
}
