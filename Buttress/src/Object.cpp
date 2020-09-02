#include "pch.h"
#include "Object.h"

void Object::Draw()
{
	if (models.size() > 0)
	{
		models[0].Draw();
	}
	else
	{
		PRINT("WARNING", "object", name, "has no models in it!");
	}
}
