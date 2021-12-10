#pragma once
#include <SDL_surface.h>

#include "EMath.h"
#include "ERGBColor.h"
#include <vector>

#include "RenderInfo.h"

#include "Vertex.h"

using namespace Elite;

class Geometry
{
public:
	explicit Geometry(FPoint3 position, FVector3 forward = FVector3{0,0,1});
	virtual ~Geometry() = default;

	virtual void Hit(const RenderInfo& renderInfo) const = 0;

	const FPoint3& GetPosition() const;
	void SetPosition(const FPoint3& position); // Maybe add option to avoid recalculation

	const FVector3& GetForward() const;
	void SetForward(const FVector3& forward); // Maybe add option to avoid recalculation

	const FMatrix4& GetTransform() const;

	virtual std::vector<Vertex> GetModelVertices() const = 0;

	// The great rework begins..
	virtual void Project(std::vector<Vertex>& vertices) const = 0;
	virtual bool Rasterize(std::vector<Vertex>& vertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const = 0;

protected:
	virtual void OnRecalculateTransform();
	void RecalculateTransform();

private:
	FPoint3 m_Position;
	FVector3 m_Forward;
	FMatrix4 m_Transform;
};

