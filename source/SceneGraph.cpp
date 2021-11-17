#include "SceneGraph.h"

SceneGraph::~SceneGraph()
{
	for (Geometry* pGeometry : m_pGeometries)
	{
		delete pGeometry;
		pGeometry = nullptr;
	}

	delete m_pCamera;
	m_pCamera = nullptr;
}

void SceneGraph::AddGeometryToScene(Geometry* geometry)
{
	m_pGeometries.push_back(geometry);
}
const std::vector<Geometry*>& SceneGraph::GetGeometries() const
{
	return m_pGeometries;
}

void SceneGraph::SetCamera(Camera* pCamera)
{
	if (m_pCamera != pCamera)
	{
		delete m_pCamera;
	}
	m_pCamera = pCamera;
}
Camera* SceneGraph::GetCamera() const
{
	return m_pCamera;
}
