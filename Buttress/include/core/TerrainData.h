#include "pch.h"
#include "glad/glad.h"
#include "ecs/ECS.h"
#include "core/Shader.h"
struct TerrainBuffer
{
	TerrainBuffer(unsigned int dimension, IntVec2 index, std::shared_ptr<Shader> terrainShader);
	void Draw();
	void Destroy();
private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_size;
};

class TerrainLoader
{
public:
	std::shared_ptr<Shader> shader;
	static TerrainLoader& Instance() 
	{
		static TerrainLoader loader;
		return loader;
	}

	//ONLY CALL THIS ON MAIN THREAD
	void AllocateQuadtree(Entity rootEntity, int lodLevel, int dimension);
	void AllocateVertices(Entity rootEntity);
	void Debug(Entity rootEntity);
	void Clear();
	bool IsEntityAssociatedToTerrain(EntityId ent);
private:
	void AllocateRecursively(Entity mainParent, Entity node, int dimension, int lodLevel, int currentLevel, IntVec2& indexOffset);
	TerrainLoader() {}
	tbb::concurrent_unordered_map<EntityId, std::vector<TerrainBuffer>, std::hash<EntityId>> m_terrainPatchesCaches;
};