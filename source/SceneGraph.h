#pragma once
#include <vector>
#include "Geometry.h"

using namespace Elite;

class SceneGraph final
{
public:
	void AddGeometryToScene(Geometry&& geometry);
	const std::vector<Geometry>& GetGeometries() const;

private:
	std::vector<Geometry> m_pGeometries{};
};