#include "pch.h"
#include "terrain/TerrainConfig.h"

TerrainConfig::TerrainConfig(float scaleY, float scaleXZ, std::array<int, 8>& lodRanges):
m_scaleY(scaleY), m_scaleXZ(scaleXZ), m_lodRange(lodRanges)
{
	for (size_t i = 0; i < 8; i++)
	{
		SetLodRange(i, lodRanges[i]);
	}
}

void TerrainConfig::SetLodRange(int idx, int lodRange)
{
	m_lodRange[idx] = lodRange;
	m_lodMorphingArea[idx] = lodRange - UpdateMorphingArea(idx + 1);
}

int TerrainConfig::UpdateMorphingArea(int lodIdx)
{
	return (int)((m_scaleXZ / MAX_ROOT_NODES) / pow(2, lodIdx));
}