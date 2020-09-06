#include "pch.h"
#include "Object.h"

void Object::Draw()
{
	if (models.size() > 0)
	{
		models[0]->shader->Use();
		Matrix4 modelMatrix;
		modelMatrix = transform.GetTransformation();
		models[0]->shader->SetUniformMat4x4("model", modelMatrix);
		//PRINT("model set");
		models[0]->Draw();
	}
	else
	{
		PRINT("WARNING", "object", name, "has no models in it!");
	}
}
