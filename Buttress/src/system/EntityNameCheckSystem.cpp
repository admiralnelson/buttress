#include "pch.h"
#include "system/EntityNameCheckSystem.h"


void EntityNameCheckSystem::Init(Universe* universe)
{
	m_universe = universe;
}

bool EntityNameCheckSystem::CheckNow(std::string name)
{
	for (auto const& i : m_entity)
	{
		Entity e = m_universe->QueryByEntityId(i);
		if (e.GetComponent<EntityName>().name == name)
		{
			return true;
		}
	}
	return false;
}
