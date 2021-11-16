#include "SceneGraph.h"

void SceneGraph::AddGeometryToScene(Geometry&& geometry)
{
	m_pGeometries.push_back(geometry);
}

const std::vector<Geometry>& SceneGraph::GetGeometries() const
{
	return m_pGeometries;
}
