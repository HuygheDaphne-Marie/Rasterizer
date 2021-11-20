#pragma once
#include <SDL_surface.h>

#include "EMath.h"
#include "ERGBColor.h"
#include <vector>

using namespace Elite;

class Geometry
{
public:
	explicit Geometry(FPoint3 position, FVector3 forward = FVector3{0,0,1});
	virtual ~Geometry() = default;

	virtual void Hit(std::vector<float>& depthBuffer, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels) const = 0;

	const FPoint3& GetPosition() const;
	void SetPosition(const FPoint3& position); // Maybe add option to avoid recalculation

	const FVector3& GetForward() const;
	void SetForward(const FVector3& forward); // Maybe add option to avoid recalculation

	const FMatrix4& GetTransform() const;

protected:
	virtual void OnRecalculateTransform();
	void RecalculateTransform();

private:
	FPoint3 m_Position;
	FVector3 m_Forward;
	FMatrix4 m_Transform;
};

