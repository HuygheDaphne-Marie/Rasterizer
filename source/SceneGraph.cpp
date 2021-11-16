#include "SceneGraph.h"

SceneGraph::~SceneGraph()
{
	for (Geometry* pGeometry : m_pGeometries)
	{
		delete pGeometry;
		pGeometry = nullptr;
	}
}

void SceneGraph::AddGeometryToScene(Geometry* geometry)
{
	m_pGeometries.push_back(geometry);
}

const std::vector<Geometry*>& SceneGraph::GetGeometries() const
{
	return m_pGeometries;
}
