#pragma once
#include "pch.h"
#include "Model.h"
#include "Geometry.h"


//Entity
class Object
{
// TODO: RELATIVE POSITIONING 
public:
	std::string name;
	//std::shared_ptr<Camera> camera;
	std::vector<std::shared_ptr<Model>> models;
	//std::vector<std::shared_ptr<Object>> objects;
	//Transformation transform;
	void Draw();
};