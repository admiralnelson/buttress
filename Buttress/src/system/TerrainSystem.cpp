#include "pch.h"
#include "system/TerrainSystem.h"
#include "system/CameraSystem.h"
#include "components/Terrain.h"
#include "components/TerrainNode.h"
#include "core/Texture.h"
void TerrainSystem::Init(Universe* universe)
{
}


void TerrainSystem::CreateTerrain(Entity ent, std::string path)
{

}

void TerrainSystem::CreateQuadtree(Entity parent, Terrain& terrainConfig)
{
	//create 8x8 patches

	for (size_t i = 0; i < ROOT_NODES; i++)
	{
		for (size_t j = 0; j < ROOT_NODES; j++)
		{
			Entity child = m_universe->CreateEntity("terrain node");
			parent.AttachChild(child);
			Vec2 location = { (float)i / (float)ROOT_NODES, (float)j / (float)ROOT_NODES };
			unsigned int level = TerrainPatchLoader::Instance().Create(8);
			AddTerrainNode(child, terrainConfig, location, 0, level);
		}
	}
}

void TerrainSystem::AddTerrainNode(Entity entity, const Terrain& terrainConfig, const Vec2& location, unsigned int lod, unsigned int terrainLodPatch)
{
	TerrainNode terrainNode;
	terrainNode.terrainLodPatch = terrainLodPatch;
	terrainNode.location = location;
	terrainNode.gap = 1.0f / (ROOT_NODES) * (float)pow(2, lod);
	entity.AddComponent<TerrainNode>(terrainNode);
}

void TerrainSystem::Render()
{
	while (!m_sortedRenderQueue.empty())
	{
		auto& queue = m_sortedRenderQueue.front();



		m_sortedRenderQueue.pop();
	}
}

void TerrainSystem::Process(size_t index)
{
	if (m_isFirstTick)
	{
		CameraSystem* cam = m_universe->GetSystem<CameraSystem>();
		m_camera = cam->FindPrimaryCamera();
		m_isFirstTick = false;
	}
	EntityId id = *std::next(m_entity.begin(), index);
	Entity entToProcess = m_universe->QueryByEntityId(id);
	Process(entToProcess);
}

void TerrainSystem::Process(Entity ent)
{
	
	Terrain& terrain = ent.GetComponent<Terrain>();
	if (terrain.isTextureReady)
	{
		size_t len = terrain.textures.size();
		tbb::parallel_for(tbb::blocked_range<int>(0, len),
			[&](tbb::blocked_range<int> r)
		{
			for (int i = r.begin(); i < r.end(); ++i)
			{
				if (terrain.textures[i].texture == nullptr)
				{
					terrain.textures[i].texture = TextureLoader::Instance().LoadTexture(terrain.textures[i].path);
				}
			}
		});
	}
	if (terrain.isLoaded)
	{
		Node& node = ent.GetComponent<Node>();
		if (node.IsLeaf())
		{
			TerrainNode& quad = ent.GetComponent<TerrainNode>();
			m_terrainRenderQueue.emplace(TerrainQueue{ quad });
		}
		size_t len = node.TotalChild();
		for (size_t i = 0; i < len; i++)
		{
			Process(ent);
		}
	}
	else
	{
		Entity parent = ent;
		CreateQuadtree(parent, terrain);
		terrain.isLoaded = true;
	}
}


