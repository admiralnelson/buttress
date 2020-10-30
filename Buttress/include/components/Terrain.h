#include "core/Texture.h"
struct Terrain
{
	friend class TerrainSystem;
	friend class TerrainLoader;
	friend class TerrainData;
	Terrain(): m_blocksX(0),
		m_blocksY(0),
		m_verticesCount(0),
		m_maxLevel(0) {}
	Terrain(unsigned int blocksX, unsigned int blocksY, unsigned int verticesCount, unsigned int maxLevel) :
		m_blocksX(blocksX), 
		m_blocksY(blocksY), 
		m_verticesCount(verticesCount), 
		m_maxLevel(maxLevel)  {}
	Terrain(const Terrain& t) :
		m_blocksX(t.m_blocksX),
		m_blocksY(t.m_blocksY),
		m_verticesCount(t.m_verticesCount),
		m_maxLevel(t.m_maxLevel),
		heightMap(t.heightMap)
	{

	}
	Terrain operator=(const Terrain& t)
	{
		return Terrain(t);
	}

	~Terrain() {}
	std::shared_ptr<TextureData> heightMap;
	struct TerrainNode
	{
		IntVec2 index;
		unsigned int terrainBufferIndex;
		unsigned int currentLevel;
	};
private:
	const unsigned int m_blocksX, m_blocksY, m_verticesCount, m_maxLevel;


};