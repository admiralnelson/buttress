#include "pch.h"
#include "system/TerrainSystem.h"
#include "system/CameraSystem.h"
#include "components/Terrain.h"
#include "core/Texture.h"
void TerrainSystem::Init(Universe* universe)
{
}


void TerrainSystem::CreateTerrain(Entity ent, std::string path)
{

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
	Terrain& terrain = m_universe->QueryByEntityId(id).GetComponent<Terrain>();
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
	if (!terrain.isLoaded)
	{
		terrain.isLoaded = true;
		
	}
	
}


