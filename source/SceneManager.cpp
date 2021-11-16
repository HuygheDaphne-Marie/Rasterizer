#include "SceneManager.h"

SceneGraph& SceneManager::AddNewScene()
{
	m_SceneGraphs.emplace_back(); // Adds new empty scene
	return m_SceneGraphs[static_cast<int>(m_SceneGraphs.size()) - 1];
}

SceneGraph& SceneManager::GetActiveScene()
{
	return m_SceneGraphs[m_CurrentlyActiveSceneIndex];
}
SceneGraph& SceneManager::GotoScene(const int index)
{
	m_CurrentlyActiveSceneIndex = index % static_cast<int>(m_SceneGraphs.size());
	return GetActiveScene();
}
SceneGraph& SceneManager::GotoNextScene()
{
	return GotoScene(m_CurrentlyActiveSceneIndex + 1);
}
SceneGraph& SceneManager::GotoPreviousScene()
{
	return GotoScene(m_CurrentlyActiveSceneIndex - 1);
}

SceneManager::SceneManager()
{
	m_SceneGraphs.reserve(10);
	m_SceneGraphs.emplace_back(); // first Scene;
}
