// Buttress.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "Logger.h"
#include "Buttress.h"
#include "Shader.h"


Buttress::Buttress()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

bool Buttress::Init(int width, int height, std::string title)
{
	m_width = width;
	m_height = height;
	m_window.reset(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr));
	if (m_window == nullptr)
	{
		PRINT("failed to init the window!");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_window.get());
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		PRINT("failed to init the glad!");
		glfwTerminate();
		return false;
	}
	glViewport(0, 0, m_width, m_height);
	glfwMakeContextCurrent(m_window.get());
	glfwSetWindowUserPointer(m_window.get(), reinterpret_cast<void*>(this));
	auto resizeCallback = [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Buttress* instance = reinterpret_cast<Buttress*>(glfwGetWindowUserPointer(window));
		if (instance->OnResize != nullptr)
		{
			instance->OnResize(width, height);
		}
	};
	glfwSetFramebufferSizeCallback(m_window.get(), resizeCallback);

	return true;
}


void Buttress::Start()
{
	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	const char* vertexShaderSource = 
	"#version 330 core\n"
	"layout(location = 0) in vec3 aPos;\n"   // the position variable has attribute position 0
	"layout(location = 1) in vec3 aColor;\n" // the color variable has attribute position 1
	"out vec3 ourColor;\n" // output a color to the fragment shader
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos, 1.0);\n"
		"ourColor = aColor;\n" // set ourColor to the input color we got from the vertex data
	"}\n\0";

	const char* fragmentShaderSource = 
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"
	"void main()\n"
	"{\n"
		"FragColor = vec4(ourColor, 1.0);\n"
	"}\n\0";

	Shader s("test");
	s.AddVertexShader(vertexShaderSource);
	s.AddFragmentShader(fragmentShaderSource);
	s.CompileShader();
	s.AddAttribute("aPos");
	s.AddAttribute("aColor");
	s.Debug();
	GLuint vbo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices ,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexArrayAttrib(vao, s.GetAttributeLocation("aPos"));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexArrayAttrib(vao, s.GetAttributeLocation("aColor"));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	while (!glfwWindowShouldClose(m_window.get()))
	{
		glClearColor(0.3, 0.4, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		s.Use();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(m_window.get());
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
}

Buttress::~Buttress()
{
}