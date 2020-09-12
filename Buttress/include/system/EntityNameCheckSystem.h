#pragma once
#include "pch.h"
#include "ECS.h"
#include "system/EntityNameCheckSystem.h"
#include "components/EntityName.h"

class EntityNameCheckSystem : public System
{
public: 
	void Init(Universe *universe) override;
	bool CheckNow(std::string name);

};