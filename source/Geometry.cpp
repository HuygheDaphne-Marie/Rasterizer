#include "Geometry.h"

#include <utility>

Geometry::Geometry(FPoint3 position, FVector3 forward)
	: m_Transform()
	, m_Position(std::move(position))
	, m_Forward(std::move(forward))
{
	RecalculateTransform();
}

const FPoint3& Geometry::GetPosition() const
{
	return m_Position;
}
void Geometry::SetPosition(const FPoint3& position)
{
	m_Position = position;
	RecalculateTransform();
}

const FVector3& Geometry::GetForward() const
{
	return  m_Forward;
}
void Geometry::SetForward(const FVector3& forward)
{
	m_Forward = forward;
	RecalculateTransform();
}

void Geometry::OnRecalculateTransform() {} // Does nothing by default

void Geometry::RecalculateTransform()
{
	const FVector3 worldUp{ 0,1,0 };

	const FVector3 right{ Cross(worldUp, m_Forward) };
	const FVector3 up{ Cross(m_Forward, right) };

	m_Transform[0] = FVector4{ right, 0 };
	m_Transform[1] = FVector4{ up, 0 };
	m_Transform[2] = FVector4{ m_Forward, 0 };
	m_Transform[3] = FVector4{ m_Position.x, m_Position.y, m_Position.z, 1.f };

	OnRecalculateTransform();
}
