#pragma once
#include "pch.h"
#include "core/Texture.h"
#include "components/Camera.h"
#include "ECS/ECS.h"
class TerrainConfig
{
public:
	TerrainConfig() {}
	TerrainConfig(float scaleY, float scaleXZ, std::array<int, 8>& lodRanges);
	float GetScaleY() const { return m_scaleY; }
	float GetScaleXZ() const { return m_scaleXZ; }
	void SetLodRange(int idx, int lodRange);
	int UpdateMorphingArea(int lodIdx);
	int GetLodRange(int idx) { return m_lodRange[idx]; }

	Entity camera;

private:
	float m_scaleY;
	float m_scaleXZ;
	std::shared_ptr<TextureData> m_heightMap;
	std::shared_ptr<TextureData> m_normalMap;
	
	std::array<int, 8> m_lodRange;

	std::array<int, 8> m_lodMorphingArea;

	
};