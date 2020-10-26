#pragma once
#include "pch.h"
#include "core/Texture.h"
#include "components/Camera.h"
#include "ECS/ECS.h"
class TerrainConfig
{
public:
	TerrainConfig() {}
	TerrainConfig(float scaleY, float scaleXZ, std::array<int, 8>& lodRanges): 
		m_scaleY(scaleY), m_scaleXZ(m_scaleXZ), m_lodRange(lodRanges)
	{
		for (size_t i = 0; i < 8; i++)
		{
			SetLodRange(i, lodRanges[i]);
		}	
	}

	float GetScaleY() const { return m_scaleY; }
	float GetScaleXZ() const { return m_scaleXZ; }
	void SetLodRange(int idx, int lodRange)
	{
		m_lodRange[idx] = lodRange;
		m_lodMorphingArea[idx] = lodRange - UpdateMorphingArea(idx + 1);
	}
	int UpdateMorphingArea(int lodIdx)
	{
		return (int)((m_scaleXZ / MAX_ROOT_NODES) / pow(2, lodIdx));
	}
	int GetLodRange(int idx)
	{
		return m_lodRange[idx];
	}

	Entity camera;

private:
	float m_scaleY;
	float m_scaleXZ;
	std::shared_ptr<TextureData> m_heightMap;
	std::shared_ptr<TextureData> m_normalMap;
	std::shared_ptr<TextureData> m_heightMap;
	std::array<int, 8> m_lodRange;

	std::array<int, 8> m_lodMorphingArea;

	
};