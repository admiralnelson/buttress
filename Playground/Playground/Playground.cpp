// Playground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <PrimitiveDraw.h>
#include <Buttress.h>
#include <core\Texture.h>
#include <core\ModelData.h>
#include <Util.h>
#include <Geometry.h>
#include <Object.h>
#include <system\CameraSystem.h>
#include <components\Camera.h>
#include <ecs\ECS.h>
#include <components\EntityName.h>
#include <components\Model.h>
#include <components\Animation.h>

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

		std::shared_ptr<Shader> animationShader;
		animationShader.reset(new Shader("animationShader"));
		animationShader->AddVertexShader(ReadFileAsString("../../resource/shader/core_animation.txt"));
		animationShader->AddFragmentShader(ReadFileAsString("../../resource/shader/core_material.txt"));
		animationShader->CompileShader();
		animationShader->Validate();
		animationShader->Debug();

		ModelData::defaultShader = baseShader;
		ModelData::defaultAnimatedShader = animationShader;
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

			Entity guard = universe.CreateEntity("a guard");
			Model model;
			model.objectPath = "../../resource/bob_lamp/boblampclean.md5mesh";
			guard.AddComponent<Model>(model);
			guard.GetComponent<Transform>().RotateDeg({ -90, -90, 0 });
			guard.Debug();
			guard.GetComponent<Transform>().scale = { 0.2, 0.2, 0.2 };

			for (size_t i = 1; i < 100; i++)
			{
				Entity guardInstance = universe.CreateEntity("a guard" + std::to_string(i));
				Model model;
				model.objectPath = "../../resource/bob_lamp/boblampclean.md5mesh";
				guardInstance.AddComponent<Model>(model);
				guardInstance.GetComponent<Transform>().RotateDeg({ -90, -90, 0 });
				guardInstance.GetComponent<Transform>().position = { 0, 0, i * 1.5 };
				guardInstance.GetComponent<Transform>().scale = { 0.1, 0.1, 0.1 };
				guardInstance.Debug();
			}
			
			for (size_t i = 1; i < 10; i++)
			{
				Entity gunInstance = universe.CreateEntity("a gun" + std::to_string(i));
				Model model;
				model.objectPath = "../../resource/gun_model/Handgun_obj.obj";
				gunInstance.AddComponent<Model>(model);
				gunInstance.GetComponent<Transform>().position = { 2, 0, i * 1.5 };
				
			}



			//Entity guard2 = universe.CreateEntity("another guard");
			//guard2.GetComponent<Transform>().position = { 10, 2, 2 };
			//guard2.GetComponent<Transform>().RotateDeg({ -90, 90, 0 });
			//guard2.AddComponent<Model>(model);

			//Entity gun = universe.CreateEntity("a gun");
			//Model gunMesh;
			////gunMesh.objectPath = "../../resource/full_model/backpack.obj";;
			////gun.AddComponent<Model>(gunMesh);
			//gun.GetComponent<Transform>().position = { 1, 1, 1};
			//gun.GetComponent<Transform>().scale = { 107, 10, 10 };
			//Entity gun2 = gun.CreateEntity("child of gun");
			//Entity gun3 = gun.CreateEntity("child of gun 2");

			//guard.Debug();

			//auto gunModel = universe.CreateEntity("gun");
			//Model gunM;
			//gunM.objectPath = "../../resource/gun_model/Handgun_obj.obj";
			//gunModel.AddComponent<Model>(gunM);
			//
			//

			universe.MemoryDebug();

			//universe.AddEventListener(KEYBOARD_EVENT::KEYBOARD_PRESS, [&guard](Event & event)
			//{
			//	unsigned int key = event.GetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON);
			//	if (key == GLFW_KEY_P)
			//	{
			//		Animation& anim = guard.GetComponent<Animation>();
			//		PRINT("check anim value");
			//	}
			//});

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
				//PRINT("mouse move, x", x, "y", y);
			});

			universe.AddEventListener(KEYBOARD_EVENT::KEYBOARD_PRESS, [/*&guard, &gun*/](Event& e)
			{
				unsigned int key = e.GetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON);
				if (key == GLFW_KEY_T)
				{
					/*guard.AttachChild(gun);
					gun.GetComponent<Transform>().scale = { 0.1, 0.1, 0.1 };*/
				}
				if (key == GLFW_KEY_Y)
				{
					/*guard.RemoveChild(gun);
					gun.GetComponent<Transform>().scale = { 1, 1, 1 };*/
				}
			});


			universe.AddEventListener(KEYBOARD_EVENT::KEYBOARD_PRESS, [](Event& e)
			{
				unsigned int key = e.GetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON);
				//PRINT("keyboard press", key);
			});

			universe.AddEventListener(KEYBOARD_EVENT::KEYBOARD_PRESS, [&](Event &e)
			{
				float speed = 200.5;
				float dt = universe.GetLastDeltaTime();
				unsigned int keyPress = e.GetParam<unsigned int>(KEYBOARD_EVENT::PARAMS::KEYBOARD_BUTTON);
				Transform& transform = ent.GetComponent<Transform>();
				float velocity = speed * dt;
				//PRINT("velocity", velocity, "dt", dt);
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
				PRINT("cam pos ", glm::to_string(transform.position));
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
