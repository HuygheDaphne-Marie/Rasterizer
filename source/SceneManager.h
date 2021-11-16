#pragma once
#include <vector>

#include "Singleton.h"
#include "SceneGraph.h"

class SceneManager final : public Singleton<SceneManager>
{
public:
	SceneManager(const SceneManager& other) = delete;
	SceneManager(SceneManager&& other) = delete;
	SceneManager& operator=(const SceneManager& other) = delete;
	SceneManager& operator=(SceneManager&& other) = delete;
	~SceneManager() override = default;

	SceneGraph& AddNewScene();

	SceneGraph& GetActiveScene();
	SceneGraph& GotoScene(const int index);
	SceneGraph& GotoNextScene();
	SceneGraph& GotoPreviousScene();

private:
	friend class Singleton<SceneManager>;
	SceneManager();

	int m_CurrentlyActiveSceneIndex = 0;
	std::vector<SceneGraph> m_SceneGraphs{};
};