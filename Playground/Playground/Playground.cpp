// Playground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <PrimitiveDraw.h>
#include <Buttress.h>
#include <core\Texture.h>
#include <core\Model.h>
#include <Util.h>
#include <Geometry.h>
#include <Object.h>
#include <system\CameraSystem.h>
#include <components\Camera.h>
#include <ecs\ECS.h>
#include <components\EntityName.h>
#include <components\Mesh.h>

int main()
{
	{
		Universe universe;
		Buttress b;
		b.Init(800, 600, "tetst");

		std::shared_ptr<Shader> baseShader;
		baseShader.reset(new Shader("test"));
		baseShader->AddVertexShader(ReadFileAsString("../../resource/shader/core.txt"));
		baseShader->AddFragmentShader(ReadFileAsString("../../resource/shader/core_material.txt"));
		baseShader->CompileShader();
		baseShader->Validate();
		baseShader->Debug();

		std::shared_ptr<Shader> lampIndicatorShader;
		lampIndicatorShader.reset(new Shader("lamp indicator"));
		lampIndicatorShader->AddVertexShader(ReadFileAsString("../../resource/shader/core.txt"));
		lampIndicatorShader->AddFragmentShader(ReadFileAsString("../../resource/shader/core_lamp_indicator.txt"));
		lampIndicatorShader->CompileShader();
		lampIndicatorShader->Validate();
		lampIndicatorShader->Debug();

		Model::defaultShader = baseShader;

		//ECS TEST
		{
			universe.GetSystem<CameraSystem>()->windowDimension = { b.Width(), b.Height() };
			Entity ent = universe.CreateEntity("camera");
			
			Camera cam;
			cam.IsPrimary = true;
			ent.AddComponent<Camera>(cam);
			ent.AttachToSystem<CameraSystem>();
			PRINT("ENTITY NAME IS ", ent.GetComponent<EntityName>().name);
			ent.Debug();

			Entity backpack = universe.CreateEntity("a backpack");
			Mesh mesh;
			mesh.objectPath = "../../resource/full_model/backpack.obj";
			backpack.AddComponent<Mesh>(mesh);
			backpack.Debug();
			backpack.GetComponent<Transform>().RotateDeg({ 0, 0, 180 });
			backpack.GetComponent<Transform>().scale = { 0.5, 0.5, 0.5 };

			Entity backpack2 = universe.CreateEntity("a backpack 2");
			backpack2.AddComponent<Mesh>(mesh);
			backpack2.GetComponent<Transform>().position = { 1, 1, 1 };
			//backpack2.GetComponent<Transform>().RotateDeg({ 0, 0, 190});

			//backpack.AttachChild(backpack2);
			backpack.GetComponent<Transform>().scale = { 1, 1, 1 };

			universe.MemoryDebug();

			//register an event listener
			universe.AddEventListener(WINDOW_EVENT::WINDOW_RESIZE, [](Event& e)
			{
				unsigned int height = e.GetParam<unsigned int>(WINDOW_EVENT::PARAMS::PARAM_HEIGHT);
				unsigned int width = e.GetParam<unsigned int>(WINDOW_EVENT::PARAMS::PARAM_WIDTH);
				PRINT("window on resize, w", width, "h", height);
			});

			universe.AddEventListener(MOUSE_EVENT::MOUSE_MOVE, [](Event& e)
			{
				double x = e.GetParam<double>(MOUSE_EVENT::PARAMS::MOUSE_X);
				double y = e.GetParam<double>(MOUSE_EVENT::PARAMS::MOUSE_Y);
				PRINT("mouse move, x", x, "y", y);
			});

			universe.AddEventListener(KEYBOARD_EVENT::KEYBOARD_PRESS, [&backpack, &backpack2](Event& e)
			{
				unsigned int key = e.GetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON);
				if (key == GLFW_KEY_T)
				{
					backpack.AttachChild(backpack2);
					backpack2.GetComponent<Transform>().scale = { 0.1, 0.1, 0.1 };
				}
				if (key == GLFW_KEY_Y)
				{
					backpack.RemoveChild(backpack2);
					backpack2.GetComponent<Transform>().scale = { 1, 1, 1 };
				}
			});


			universe.AddEventListener(KEYBOARD_EVENT::KEYBOARD_PRESS, [](Event& e)
			{
				unsigned int key = e.GetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON);
				PRINT("keyboard press", key);
			});

			universe.AddEventListener(KEYBOARD_EVENT::KEYBOARD_PRESS, [&](Event &e)
			{
				float speed = 2.5;
				float dt = universe.GetLastDeltaTime();
				unsigned int keyPress = e.GetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON);
				Transform& transform = ent.GetComponent<Transform>();
				float velocity = speed * dt;
				switch (keyPress)
				{
				case GLFW_KEY_W:
					transform.position += transform.front * velocity;
					break;
				case GLFW_KEY_S:
					transform.position -= transform.front * velocity;
					break;
				case GLFW_KEY_A:
					transform.position -= transform.right * velocity;
					break;
				case GLFW_KEY_D:
					transform.position += transform.right * velocity;
					break;
				default:
					break;
				}
			});
			

			b.Start(&universe);
		}

	}
	
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
