#pragma once
#include <vector>
#include "Geometry.h"

using namespace Elite;

class SceneGraph final
{
public:
	// Todo: rule of 5
	~SceneGraph();

	void AddGeometryToScene(Geometry* geometry);
	const std::vector<Geometry*>& GetGeometries() const;

private:
	std::vector<Geometry*> m_pGeometries{};
};