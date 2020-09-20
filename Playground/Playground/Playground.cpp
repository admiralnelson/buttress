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
		std::shared_ptr<TextureData> t;
		t = TextureLoader::Instance().LoadTexture("../../resource/media/test.jpg");
		//Texture* t2 = new Texture("test 2", "../../resource/media/Wood_Wall_002_basecolor.jpg");

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

		Material material("test material", baseShader);
		material.diffuse = t;
		material.Debug();

		Material materialLamp("test lamp", lampIndicatorShader);
		materialLamp.Debug();


		//Test copy constructor
		Material materialA;
		Material materialB;
		materialA = material;
		materialB = materialLamp;
		materialA.Debug();
		materialB.Debug();

		Model::defaultShader = baseShader;


		/*Transformation camTransform;
		camTransform.position = Vec3(0.0f, 0.0f, 3.0f);
		std::shared_ptr<Camera> cam;
		cam.reset(new Camera("main", 60, Vec2{ Buttress::ButtressInstance()->Width(), Buttress::ButtressInstance()->Height() }, camTransform));


		Object object{ "test", cam, std::vector<std::shared_ptr<Model>> {model}, Transformation() };
		Object object2{ "test2", cam, std::vector<std::shared_ptr<Model>> {model}, Transformation() };
		Object object3{ "lamp", cam, std::vector<std::shared_ptr<Model>> {lampModel}, Transformation() };
		object.transform.position = Vec3(1.0f, 0.0f, 0.0);
		object.transform.Rotate(Vec3(90, 0, 0));
		object2.transform.position = Vec3(0.0f, 1.0f, 0.0);
		object3.transform.position = Vec3(0, 2.0f, 0 );
		object3.transform.scale = Vec3(0.1f, 0.1f, 0.1f);
		*/

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



			b.Start(&universe);
		}



		/*
		int err = 0;
		cam->speed = 30;
		
		b.OnLoop = [&]()
		{
			object.Draw();
			object2.Draw();
			object3.Draw();
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
		int tapKey = 0;

		Input::Instance().RegisterKey("pressW", GLFW_KEY_W, [&](int key, float dT)
		{
			cam->Move(Camera::FORWARD, dT);
		});
		Input::Instance().RegisterKey("pressA", GLFW_KEY_A, [&](int key, float dT)
		{
			cam->Move(Camera::LEFT, dT);
		});
		Input::Instance().RegisterKey("pressD", GLFW_KEY_D, [&](int key, float dT)
		{
			cam->Move(Camera::RIGHT, dT);
		});
		Input::Instance().RegisterKey("pressS", GLFW_KEY_S, [&](int key, float dT)
		{
			cam->Move(Camera::BACKWARD, dT);
		});


		b.OnStart = [&]()
		{
			//Camera& camera = cam.get;
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

				cam->MouseLook(Vec2((float)xoffset, (float)yoffset));
				//cam.Debug(); //causes input lag!
			});

			return true;
		};

		b.Start();*/
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
