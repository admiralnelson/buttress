#include "pch.h"
#include "Shader.h"
#include "Logger.h"

 static std::unordered_map<std::string, Shader*> shaderList;

Shader::Shader(std::string name)
{
	this->name = name;
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
	PRINT("INFO", "shader program has been created:", this->name, "program nr:", m_program);
	shaderList[this->name] = this;
}

void Shader::Use()
{
	if (!IsShaderReady())
	{
		PRINT("ERROR", "shader is not compiled", this->name);
		throw new std::exception("shader error", -1);
	}
	glUseProgram(m_program);
}


//TODO: automatic parse list of uniform and vertex
bool Shader::AddUniform(std::string name, std::string type, GLint arraySize)
{
	int uniformLocation = glGetUniformLocation(m_program, name.c_str());
	if (uniformLocation == -1)
	{
		PRINT("WARNING", "unable to locate uniform in the shader:", name, "from shader:", this->name);
		return false;
	}
	m_uniforms[name] = { type, uniformLocation, arraySize };
	return true;
}

bool Shader::AddAttribute(std::string name, std::string type)
{
	int uniformLocation = glGetAttribLocation(m_program, name.c_str());
	if (uniformLocation == -1)
	{
		PRINT("WARNING", "unable to locate attribute in the shader:", name, "from shader:", this->name);
		return false;
	}
	m_attributes[name] = { type, uniformLocation };
	return true;
}

GLint Shader::GetAttributeLocation(std::string name) 
{
	if (!IsAttributeDefined(name))
	{
		PRINT("WARNING", "undefined attribute", name, "shader source:", this->name);
		return -1;
	}
	return m_attributes[name].valuePos;
}

GLint Shader::GetUniformLocation(std::string name)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return -1;
	}
	return m_uniforms[name].valuePos;
}

bool Shader::SetUniformValueF(std::string name, float x)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	glUniform1f(m_uniforms[name].valuePos, x);
	CheckError(__FUNCTION__, { "x:", std::to_string(x) });
	return true;
}

bool Shader::SetUniformValueF(std::string name, float x, float y)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE:::
	return true;
}

bool Shader::SetUniformValueF(std::string name, float x, float y, float z)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE:::
	return true;
}

bool Shader::SetUniformValueF(std::string name, float x, float y, float z, float w)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE:::
	return true;
}

bool Shader::SetUniformValueF(std::string name, std::vector<float> v)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE:::
	return true;
}

bool Shader::SetUniformValueI(std::string name, int x)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	glUniform1i(m_uniforms[name].valuePos, x);
	CheckError(__FUNCTION__, { "x:", std::to_string(x) });
	return true;
}

bool Shader::SetUniformValueI(std::string name, int x, int y)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE
	return true;
}

bool Shader::SetUniformValueI(std::string name, int x, int y, int z)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE:::
	return true;
}

bool Shader::SetUniformValueI(std::string name, int x, int y, int z, int w)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE:::
	return true;
}

bool Shader::SetUniformValueI(std::string name, std::vector<int> v)
{
	if (!IsUniformDefined(name))
	{
		PRINT("WARNING", "undefined uniform", name, "shader source:", this->name);
		return false;
	}
	//DEFINE HERE:::
	return true;
}

void Shader::Debug()
{
	PRINT("INFO", "-----------------");
	PRINT("INFO", "shader name:", this->name);
	PRINT("INFO", "is shader ready: ", m_isReady ? "yes" : "no");
	PRINT("INFO", "shader program nr:", m_program);
	PRINT("INFO", "registered uniforms:");
	for (auto& i : m_uniforms)
	{
		if (i.second.arraySize > 0)
		{
			PRINT("    ", i.first, "type", i.second.type, "array size", i.second.arraySize , "location at shader:", GetUniformLocation(i.first));
		}
		else
		{
			PRINT("    ", i.first, "type", i.second.type, "location at shader:", GetUniformLocation(i.first));
		}
	}
	PRINT("INFO", "registered attributes:");
	for (auto& j : m_attributes)
	{
		PRINT("    ",j.first, "type", j.second.type , "location at shader:", GetAttributeLocation(j.first));
	}
	

}

void Shader::AddVertexShader(std::string source)
{
	AddProgram(source, GL_VERTEX_SHADER);
	FindAndLocateAttributes(source);
	FindAndLocateUniforms(source);
}

void Shader::AddFragmentShader(std::string source)
{
	AddProgram(source, GL_FRAGMENT_SHADER);
	FindAndLocateUniforms(source);
}

void Shader::CompileShader()
{
	glLinkProgram(m_program);
	GLint status;
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if (!status)
	{
		PRINT("ERROR", "shader", this->name, "is not linkable.");
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
		PRINT("ERROR", "shader", this->name, "is not valid.");
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

bool Shader::Validate()
{
	if (!m_isReady)
	{
		PRINT("WARNING", "shader", this->name, "is not ready to validate");
		return false;
	}
	bool result = true;
	for (auto &s : m_attributes)
	{
		std::string name = s.first;
		std::string type= s.second.type;
		if (!AddAttribute(name, type))
		{
			PRINT("WARNING","attribute is optimised away:", name, "- type", s.second.type, "from shader:", this->name);
			result = false;
		}
	}

	for (auto& s : m_uniforms)
	{
		std::string name = s.first;
		std::string type = s.second.type;
		GLint arraySize = s.second.arraySize;
		if (!AddUniform(name, type, arraySize))
		{
			PRINT("WARNING", "uniform is optimised away:", name, "- type", s.second.type, "from shader:", this->name);
			result = false;
		}
	}
	return result;
}

bool Shader::IsUniformDefined(std::string name)
{
	return m_uniforms.find(name) != m_uniforms.end();
}

bool Shader::IsAttributeDefined(std::string name)
{
	return m_attributes.find(name) != m_attributes.end();
}

Shader::~Shader()
{
	shaderList.erase(this->name);
	glDeleteProgram(m_program);
	PRINT("INFO", "shader has been erased", this->name);
}

void Shader::AddProgram(std::string source, int type)
{
	int shader = glCreateShader(type);
	if (!shader)
	{
		PRINT("ERROR", "shader type", type, "is not compilable.", "faulting shader:", this->name);
		throw new std::exception("shader error", -1);
	}
	const char* data[1] = { source.c_str() };
	glShaderSource(shader, 1, data, 0);
	glCompileShader(shader);

	GLint isCompiled = false;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled)
	{
		PRINT("ERROR", "shader type", type, "is not compilable due to compilation error.", "faulting shader:", this->name);
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

void Shader::FindAndLocateAttributes(std::string source)
{
	std::regex reg("layout\\(location = \\d\\) in (\\w+) (\\w+).*");
	std::vector<std::string> processed = SplitToVector(source);
	for (std::string& s : processed)
	{
		std::smatch result;
		if (std::regex_match(s, result, reg))
		{
			m_attributes[result[2]] = { result[1], -1, 0 };
		}
		
	}

}

void Shader::FindAndLocateUniforms(std::string source)
{
	std::regex reg("uniform (\\w+) (\\w+)\\[?(\\d+)?\\]?.*");
	std::vector<std::string> processed = SplitToVector(source);
	for (std::string& s : processed)
	{
		std::smatch result;
		if (std::regex_match(s, result, reg))
		{
			if (result.length() == 3)
			{
				m_uniforms[result[2]] = { result[1], -1, 0 };
			}
			else
			{
				GLint arraySize = atoi(std::string(result[3]).c_str());
				m_uniforms[result[2]] = { result[1], -1, arraySize };
			}
		}

	}

}

std::vector<std::string> Shader::SplitToVector(std::string input)
{
	std::string token;
	std::vector<std::string> output;
	std::string delim = "\n";
	size_t pos = input.find(delim);
	while (pos != std::string::npos)
	{
		token = input.substr(0, pos);
		output.push_back(token);
		input.erase(0, pos + delim.length());
		pos = input.find("\n");
	}
	return output;
}

bool Shader::CheckError(std::string lastOperationName, std::vector<std::string> params)
{
	int error = glGetError();
	if (error == 0) return false;
	else
	{
		std::string paramString;
		for (std::string& s : params)
		{
			paramString += s + " ";
		}
		PRINT("WARNING", "error", std::to_string(error) ,"operation in shader:", this->name ,"last operation:", lastOperationName, "params:", paramString);
	}
	return false;
}
