#include "pch.h"
#include "Terrain.h"

Terrain::Terrain(float _scaleY, float _scaleXZ, std::array<int, 8>& _lodRanges)
	: scaleY(_scaleY), scaleXZ(_scaleXZ)
{
	//std::copy(std::begin(_lodRange), std::end(_lodRange), std::begin(lodRange));
	//std::copy(std::begin(_lodMorphingArea), std::end(_lodMorphingArea), std::begin(lodMorphingArea));
	for (size_t i = 0; i < _lodRanges.size(); i++)
	{
		if (_lodRanges[i] == 0)
		{
			lodRanges[i] = 0;
			lodMorphingAreas[i] = 0;
		}
		else
		{
			SetLodRange(i, _lodRanges[i]);
		}
	}
}

void Terrain::SetLodRange(int idx, int lodRange)
{
	lodRanges[idx] = lodRange;
	lodMorphingAreas[idx] = lodRange - UpdateMorhpingArea(idx + 1);
}

int Terrain::UpdateMorhpingArea(int lod)
{
	return (int) (scaleXZ / ROOT_NODES) / pow(2, lod);
}
