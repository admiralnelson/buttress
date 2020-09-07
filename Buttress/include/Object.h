#pragma once
#include "pch.h"
#include "Model.h"
#include "Geometry.h"
class Object
{
// TODO: RELATIVE POSITIONING 
public:
	std::string name;
	std::vector<std::shared_ptr<Model>> models;
	Transformation transform;
	void Draw();
};