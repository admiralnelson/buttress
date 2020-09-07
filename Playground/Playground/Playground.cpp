// Playground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <PrimitiveDraw.h>
#include <Buttress.h>
#include <Texture.h>
#include <Util.h>
#include <Geometry.h>
#include <Model.h>
#include <Bus.h>
#include <Input.h>
#include <Object.h>
#include <Camera.h>
int main()
{
	{
		Buttress b;
		b.Init(800, 600, "tetst");
		Texture* t = new Texture("test", "../../resource/media/test.jpg");
		//Texture* t2 = new Texture("test 2", "../../resource/media/Wood_Wall_002_basecolor.jpg");


		std::shared_ptr<Shader> s;
		s.reset(new Shader("test"));
		s->AddVertexShader(ReadFileAsString("../../resource/shader/core.txt"));
		s->AddFragmentShader(ReadFileAsString("../../resource/shader/core_material.txt"));
		s->CompileShader();
		s->Validate();
		s->Debug();
		std::shared_ptr<Material> material;
		material.reset(new Material("test material", s));
		material->diffuse = std::shared_ptr<Texture>(t);
		material->Debug();
		std::shared_ptr<Model> model;
		model.reset(new Model("a box", s,"../../resource/obj/test.obj"));
		model->material = material;
		Object object{ "test", std::vector<std::shared_ptr<Model>> {model}, Transformation() };
		Transformation camTransform;
		camTransform.position = Vec3(0.0f, 0.0f, 3.0f);
		Camera cam("main", s, 60, Vec2{ Buttress::ButtressInstance()->Width(), Buttress::ButtressInstance()->Height() }, camTransform );
		int err = 0;
		cam.speed = 30;
		
		b.OnLoop = [&]()
		{
			cam.Use();
			object.Draw();
			err = glGetError();
			if (err)
			{
				PRINT("error");
			}
		};


		b.OnShutdown = [&]()
		{

		};

		bool firstMouse = false;
		float x = 0, y = 0;
		b.OnStart = [&]()
		{
			Camera& camera = cam;
			bool& mouse = firstMouse;
			float& lastX = x, &lastY = y;
			Bus::Instance().AddReceiver("key", "mouse" , [&](Message& m)
			{
				PRINT("mouse", m.inputEvent.x);
				PRINT("mouse", m.inputEvent.y);
				
				if (firstMouse)
				{
					lastX = m.inputEvent.x;
					lastY = m.inputEvent.y;
					firstMouse = false;
				}

				float xoffset = m.inputEvent.x - lastX;
				float yoffset = lastY - m.inputEvent.y; // reversed since y-coordinates go from bottom to top

				lastX = m.inputEvent.x;
				lastY = m.inputEvent.y;

				cam.MouseLook(Vec2((float)xoffset, (float)yoffset));
				//cam.Debug(); //causes input lag!
			});

			Bus::Instance().AddReceiver("key1", "keyboard", [&](Message& m) 
			{
				if (m.inputEvent.keyAction == KeyboardButtonState::TAP || m.inputEvent.keyAction == KeyboardButtonState::HOLD)
				{
					PRINT("tap ", glfwGetKeyName(m.inputEvent.key, m.inputEvent.scanCode), "dt", m.inputEvent.deltaT);
					if (m.inputEvent.key == GLFW_KEY_W)
					{
						cam.Move(Camera::FORWARD, m.inputEvent.deltaT);
					}
					else if (m.inputEvent.key == GLFW_KEY_A)
					{
						cam.Move(Camera::LEFT, m.inputEvent.deltaT);
					}
					else if (m.inputEvent.key == GLFW_KEY_D)
					{
						cam.Move(Camera::RIGHT, m.inputEvent.deltaT);
					}
					else if (m.inputEvent.key == GLFW_KEY_S)
					{
						cam.Move(Camera::BACKWARD, m.inputEvent.deltaT);
					}
				}
			});
			return true;
		};

		b.Start();
	}
	_CrtDumpMemoryLeaks();
	
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
