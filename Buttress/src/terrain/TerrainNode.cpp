#include "pch.h"
#include "terrain/TerrainNode.h"

TerrainNode::TerrainNode(TerrainVBO vbo, TerrainConfig config, Vec2 location, int lod, Vec2 index) :
	TerrainContainer(),
	m_config(config), m_location(location), m_lod(lod), m_index(index)
{
	m_vbo = vbo;
	m_gap = (float)1.0 / (MAX_ROOT_NODES * (float)(pow(2, lod)));
	Vec3 localScalling = Vec3(m_gap, 0, m_gap);
	Vec3 localTranslation = Vec3(location.x, 0, location.y);
	localTransform.scale = localScalling;
	localTransform.position = localTranslation;
	worldTransform.scale = Vec3(config.GetScaleXZ(), config.GetScaleY(), config.GetScaleXZ());
	worldTransform.position = Vec3(-config.GetScaleXZ() / 2.0f, 0, -config.GetScaleXZ() / 2.0f);

	ComputeWorldPos();
	UpdateQuadtree();
}

void TerrainNode::Render()
{
	if (m_isLeaf)
	{
		m_vbo.Draw();
	}
	for (auto& node : nodes)
	{
		node->Render();
	}
}

void TerrainNode::UpdateQuadtree()
{
	Transform& cameraPos = m_config.camera.GetComponent<Transform>();
	if (cameraPos.position.y > m_config.GetScaleY())
	{
		m_worldPos.y = m_config.GetScaleY();
	}
	else
	{
		m_worldPos.y = cameraPos.position.y;
	}

	UpdateChildNodes();
	for (auto node : nodes)
	{
		dynamic_cast<TerrainNode*>(node)->UpdateQuadtree();
	}
}

void TerrainNode::UpdateChildNodes()
{
	Transform& cameraPos = m_config.camera.GetComponent<Transform>();
	Vec3 delta = cameraPos.position - m_worldPos;
	float distance = delta.length();

	if (distance < m_config.GetLodRange(m_lod))
	{
		Add4Childs(m_lod + 1);
	}
	else if (distance >= m_config.GetLodRange(m_lod))
	{
		RemoveChildNodes();
	}
}

void TerrainNode::Add4Childs(int lod)
{
	if (m_isLeaf)
	{
		m_isLeaf = false;
	}

	if (nodes.size() == 0)
	{
		for (size_t i = 0; i < 2; i++)
		{
			for (size_t j = 0; j < 2; j++)
			{
				// BAD 
				// NO HEAP PLS
				// perhaps preallocate and use flags? 
				AddChild(new TerrainNode(m_vbo, m_config, m_location + Vec2(i * m_gap / 2.0f, j * m_gap / 2.0f), lod, Vec2(i, j)));
			}
		}
	}
}

void TerrainNode::RemoveChildNodes()
{
	if (!m_isLeaf)
	{
		m_isLeaf = true;
	}
	if (nodes.size() == 0)
	{
		// BAD 
		// NO HEAP PLS
		for (size_t i = 0; i < nodes.size(); i++)
		{
			delete nodes[i];
		}
		nodes.clear();
	}
}

void TerrainNode::ComputeWorldPos()
{
	Vec2 location = ((m_location + m_gap / 2.0f) * m_config.GetScaleXZ()) - m_config.GetScaleXZ() / 2.0f;
	m_worldPos = Vec3(location.x, 0, location.y);
}