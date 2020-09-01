// Playground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <PrimitiveDraw.h>
#include <Buttress.h>
#include <Texture.h>
#include <Util.h>
#include <Geometry.h>
#include <Model.h>
int main()
{
    
    Buttress b;
    b.Init(800, 600, "tetst");
    Texture t("test", "../../resource/media/test.jpg");
	Texture t2("test 2", "../../resource/media/Wood_Wall_002_basecolor.jpg");

	std::vector<Vertex> vert =
	{
		// positions          // colors           // texture coords
		{ Vec3( 0.5f,  0.5f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f) }, // top right
		{ Vec3( 0.5f, -0.5f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f) }, // bottom right
		{ Vec3(-0.5f, -0.5f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f) }, // bottom left
		{ Vec3(-0.5f,  0.5f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f) }, // top left 
	};

    std::vector<unsigned int> indices = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

	Shader s("test");
	s.AddVertexShader(ReadFileAsString("../../resource/shader/core.txt"));
	s.AddFragmentShader(ReadFileAsString("../../resource/shader/core_material.txt"));
	s.CompileShader();
	s.Validate();
	s.Debug();
	Material material("test material", std::shared_ptr<Shader>(&s));
	material.diffuse = std::shared_ptr<Texture>(&t);
	if (material.IsReady())
	{
		PRINT("yes it is");
	}
	Model model("a square", std::shared_ptr<Material>(&material), vert, indices);

	DrawCommand startup;
	startup.Description = "Startup procedures";
	startup.Command = [&]()
	{
		
	};


	DrawCommand loop;
	loop.Description = "Draw loop";
	loop.Command = [&]()
	{
		model.Draw();
	};

	b.PrimitiveDrawInstance()->PushBegin(startup);
	b.PrimitiveDrawInstance()->PushLoop(loop);
	b.PrimitiveDrawInstance()->Debug();



	b.OnShutdown = [&]()
	{

	};

    b.OnStart = []()
    {
        return true;
    };
    b.Start();

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
