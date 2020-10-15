#pragma once
#include "pch.h"
#include "core/Texture.h"
struct Terrain
{
	friend class TerrainSystem;
	struct TerrainTexture
	{
		friend class TerrainSystem;
		std::string path;
		int heightBegin;
		int heightEnd;
	private:
		std::shared_ptr<TextureData> texture;
	};
	Terrain(float _scaleY, float _scaleXZ, std::array<int, 8>& _lodRanges);
	void AddTexture(TerrainTexture t);
	TerrainTexture& GetTexture(size_t idx);
	Terrain() {}
private:
	tbb::concurrent_vector<TerrainTexture> textures;
	void SetLodRange(int idx, int lodRange);
	int UpdateMorhpingArea(int lod);
	std::array<int, 8> lodRanges = {};
	std::array<int, 8> lodMorphingAreas = {};
	float scaleY = 0;
	float scaleXZ = 0;
	bool isLoaded = false;
	bool isTextureReady = false;
};