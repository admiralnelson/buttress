#pragma once
#include "pch.h"
//terrain config
struct Terrain
{
	friend class TerrainSystem;
	Terrain(float _scaleY, float _scaleXZ, std::array<int, 8>& _lodRanges);
	Terrain() {}
private:
	void SetLodRange(int idx, int lodRange);
	int UpdateMorhpingArea(int lod);
	std::array<int, 8> lodRanges = {};
	std::array<int, 8> lodMorphingAreas = {};
	float scaleY = 0;
	float scaleXZ = 0;
	MaterialId materialIndex;
	bool isLoaded = false;
};