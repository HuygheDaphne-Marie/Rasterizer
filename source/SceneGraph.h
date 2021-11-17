#pragma once
#include <vector>
#include "Geometry.h"
#include "ECamera.h"

using namespace Elite;

class SceneGraph final
{
public:
	// Todo: rule of 5
	~SceneGraph();

	void AddGeometryToScene(Geometry* geometry);
	const std::vector<Geometry*>& GetGeometries() const;

	void SetCamera(Camera* pCamera);
	Camera* GetCamera() const;

private:
	std::vector<Geometry*> m_pGeometries{};
	Camera* m_pCamera{ nullptr };
};