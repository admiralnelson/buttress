#pragma once
#include "pch.h"
#include "Material.h"

class Mesh
{
public:
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indices, Material mats);
	void Draw();
private:
	std::vector<Material> mats;
};