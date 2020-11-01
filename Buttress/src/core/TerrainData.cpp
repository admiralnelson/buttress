#include "pch.h"
#include "core/TerrainData.h"
#include "components/Terrain.h"

void TerrainLoader::Allocate(Entity rootEntity, int lodLevel, int dimension)
{
	if (shader == nullptr)
	{
		PRINT("ERROR", "terrain shader is not set!");
		std::runtime_error("terrain shader is not set!");
	}
	//checks for any dead entities
	//DeleteDeadEntities();
	if (IsEntityAssociatedToTerrain(rootEntity.GetId()))
	{
		PRINT("WARN", "entity", rootEntity.GetId(), " has a terrain alerady!");
	}

	IntVec2 v = { 0,0 };
	AllocateRecursively(rootEntity, rootEntity, dimension, lodLevel, 0, v);
	
}

void TerrainLoader::Debug(Entity rootEntity)
{
	PRINT("terrain test---------------");
	
	Node& node = rootEntity.GetComponent<Node>();

	for (size_t i = 0; i < node.TotalChild(); i++)
	{
		Debug(node.GetChild(i));
	}

	Terrain::TerrainNode& terrainNode = rootEntity.GetComponent<Terrain::TerrainNode>();
	PRINT("terrain level -- ", terrainNode.currentLevel, "terrain index:", glm::to_string(terrainNode.index), "terrain buffer id", terrainNode.terrainBufferIndex);
}

bool TerrainLoader::IsEntityAssociatedToTerrain(EntityId ent)
{
	for (const auto& i : m_terrainPatchesCaches)
	{
		if (i.first == ent) return true;
	}
	return false;
}

Entity lastEntity;
void TerrainLoader::AllocateRecursively(Entity mainParent, Entity parent, int dimension, int lodLevel, int currentLevel, IntVec2 &indexOffset)
{

	if (currentLevel > lodLevel) 
	{
		TerrainBuffer buffer(dimension, indexOffset, TerrainLoader::Instance().shader);
		m_terrainPatchesCaches[mainParent.GetId()].emplace_back(buffer);

		Terrain::TerrainNode node;
		node.index = indexOffset;
		node.currentLevel = currentLevel;
		node.terrainBufferIndex = m_terrainPatchesCaches[mainParent.GetId()].size();

		parent.AddComponent<Terrain::TerrainNode>(node);
		return;
	}

	TerrainBuffer buffer(dimension, indexOffset, TerrainLoader::Instance().shader);
	m_terrainPatchesCaches[mainParent.GetId()].emplace_back(buffer);
	
	Terrain::TerrainNode node;
	node.index = indexOffset;
	node.currentLevel = currentLevel;
	node.terrainBufferIndex = m_terrainPatchesCaches[mainParent.GetId()].size();

	parent.AddComponent<Terrain::TerrainNode>(node);


	for (size_t i = 0; i < 4; i++)
	{
		Entity child = parent.CreateEntity("terrain");
		parent.AttachChild(child);
	}


	if (indexOffset.x > 0)
	{
		indexOffset.x = indexOffset.x * 2;
	}

	if (indexOffset.y > 0)
	{
		indexOffset.y = indexOffset.y * 2;
	}

	int newCurrentLevel = currentLevel + 1;
	Entity child = parent.GetChild(0);
	IntVec2 newIndex;
	newIndex.x = indexOffset.x;
	newIndex.y = indexOffset.y;
	AllocateRecursively(mainParent, child, dimension, lodLevel, newCurrentLevel, newIndex);

	Entity child1 = parent.GetChild(1);
	IntVec2 newIndex1;
	newIndex1.x = indexOffset.x + 1;
	newIndex1.y = indexOffset.y;
	AllocateRecursively(mainParent, child1, dimension, lodLevel, newCurrentLevel, newIndex1);

	Entity child2 = parent.GetChild(2);
	IntVec2 newIndex2;
	newIndex2.x = indexOffset.x;
	newIndex2.y = indexOffset.y + 1;
	AllocateRecursively(mainParent, child2, dimension, lodLevel, newCurrentLevel, newIndex2);

	Entity child3 = parent.GetChild(3);
	IntVec2 newIndex3;
	newIndex3.x = indexOffset.x + 1;
	newIndex3.y = indexOffset.y + 1;
	AllocateRecursively(mainParent, child3, dimension, lodLevel, newCurrentLevel, newIndex3);

	parent.GetComponent<Terrain::TerrainNode>().index.x = newIndex.x;
	parent.GetComponent<Terrain::TerrainNode>().index.y = newIndex.y;

}

TerrainBuffer::TerrainBuffer(unsigned int dimension,  IntVec2 index, std::shared_ptr<Shader> terrainShader)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	for (int z = 0; z < dimension; z++) 
	{
		for (int x = 0; x < dimension; x++) 
		{
			Vertex vert;
			vert.position = Vec3(x, 0, z);
			vert.normal = Vec3(0, 1, 0);
			float uOffset = 0;
			float vOffset = 0;
			if (x != index.x && x > 0)
			{
				uOffset = ((float)index.x / (float)x);
			}
			if (z != index.y && z > 0)
			{
				vOffset = ((float)index.y / (float)z);
			}
			float u = uOffset + ((float)x / (float)(dimension - 1));
			float v = vOffset + ((float)z / (float)(dimension - 1));
			vert.uv = Vec2(u, v);
			vert.uv2 = Vec2(0, 0); //for splat texture
			vertices.emplace_back(vert);

		}
		
	}

	int ptr = 0;
	indices.resize(6 * (dimension - 1) * (dimension - 1));
	for (int z = 0; z< dimension - 1; z++)
	{
		for (int x = 0; x < dimension - 1; x++)
		{
			int topLeft = z * dimension + x;
			int topRight = topLeft + 1;
			int bottomLeft = (z+1) * dimension + x;
			int bottomRight = bottomLeft + 1;


			indices[ptr++] = topLeft;
			indices[ptr++] = bottomLeft;
			indices[ptr++] = topRight;
			indices[ptr++] = topRight;
			indices[ptr++] = bottomLeft;
			indices[ptr++] = bottomRight;

			vertices[topLeft].uv2     = Vec2(0, 0);
			vertices[bottomLeft].uv2  = Vec2(0, 1);
			vertices[topRight].uv2    = Vec2(1, 0);
			vertices[bottomRight].uv2 = Vec2(1, 1);
		}
	}



	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexArrayAttrib(m_vao, terrainShader->GetAttributeLocation(ATTRIBUTE_POS));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, terrainShader->GetAttributeLocation(ATTRIBUTE_NORMAL));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));
	glEnableVertexArrayAttrib(m_vao, terrainShader->GetAttributeLocation(ATTRIBUTE_UV));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3) + sizeof(Vec2)));
	glEnableVertexArrayAttrib(m_vao, terrainShader->GetAttributeLocation(ATTRIBUTE_UV2));

	

	int err = 0;

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	PRINT("INFO", "terrain patch has been created vao:", m_vao, "vbo:", m_vbo, "ibo:", m_ibo, "index size:", indices.size(), "index offset:", glm::to_string(index));
	err = glGetError();
	if (err != 0)
	{
		PRINT("ERROR");
	}
	
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

}

void TerrainBuffer::Draw()
{
}

void TerrainBuffer::Destroy()
{
}
