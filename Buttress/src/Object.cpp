#include "pch.h"
#include "Object.h"

void Object::Draw()
{
	if (models.size() > 0)
	{
		models[0]->shader->Use();

		if (!camera)
		{
			PRINT("WARNING", "object has no camera assigned! object name:", name);
			return;
		}

		Matrix4 proj, view;
		proj = camera->Projection();
		models[0]->shader->SetUniformMat4x4("projection", proj);
		//PRINT("projection set");
		view = camera->View();
		models[0]->shader->SetUniformMat4x4("view", view);
		//PRINT("view set");
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
