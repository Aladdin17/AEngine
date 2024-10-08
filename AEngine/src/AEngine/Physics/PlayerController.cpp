/**
 * \file PlayerController.cpp
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#include "PlayerController.h"
#include "AEngine/Core/Logger.h"

namespace AEngine
{
	PlayerController::PlayerController(
		PhysicsWorld* world,
		const Math::vec3& startPosition,
		const Properties& properties)
		: m_properties{ properties },
		m_capsuleHeight{ 0 },
		m_fallingSpeed{ 0 },
		m_currentDirection{ Math::vec3{0.0f} },
		m_unitDirection{ Math::vec3{0.0f} },
		m_inGroundedState{ false },
		m_inFallingState{ true },
		m_body{ nullptr },
		m_groundRay{ nullptr },
		m_forwardRay{ nullptr },
		m_forwardRayLength{ m_properties.radius * 0.5f },
		m_groundRayLength{ m_properties.height * 0.5f }
	{

		if(m_properties.fallDrag > 1.0f || m_properties.moveDrag > 1.0f)
		{
			AE_LOG_WARN("Fall Drag and Move Drag should be less than 1.0");
		}

		m_body = world->AddRigidBody(startPosition, Math::quat(1, 0, 0, 0));
		m_body->SetType(RigidBody::Type::Kinematic);
		m_body->AddCapsuleCollider(m_properties.radius, m_properties.height, m_properties.capsuleOffset);
		m_body->SetMass(7.0f);
		m_groundRay = Raycaster::Create(world);
		m_forwardRay = Raycaster::Create(world);
	}

	void PlayerController::SetTransform(const Math::vec3& position, const Math::quat& orientation)
	{
		m_body->SetTransform(position, orientation);
	}

	Math::vec3 PlayerController::GetTransform() const
	{
		// get the interpolated transform for the collision body
		Math::vec3 position;
		Math::quat orientation;
		m_body->GetTransform(position, orientation);

		// return the position component
		return position;
	}

	Properties PlayerController::GetControllerProperties() const
	{
		return m_properties;
	}

	Math::vec3 PlayerController::GetUnitDirection() const
	{
		return m_unitDirection;
	}

	void PlayerController::SetSpeed(float speed)
	{
		m_properties.moveFactor = speed;
	}

	void PlayerController::SetDrag(float drag)
	{
		m_properties.moveDrag = drag;
	}

	void PlayerController::SetFallDrag(float drag)
	{
		m_properties.fallDrag = drag;
	}

	void PlayerController::SetRadius(float radius)
	{
		if(m_body->GetColliders().empty())
			return;

		m_properties.radius = radius;

		SharedPtr<CapsuleCollider> collider = std::static_pointer_cast<CapsuleCollider>(m_body->GetColliders().front());
		collider->SetRadius(radius);
	}

	void PlayerController::SetHeight(float height)
	{
		if(m_body->GetColliders().empty())
			return;

		m_properties.height = height;
		m_groundRayLength = height * 0.5f;

		SharedPtr<CapsuleCollider> collider = std::static_pointer_cast<CapsuleCollider>(m_body->GetColliders().front());
		collider->SetHeight(height);
	}

	void PlayerController::SetCapsuleOffset(const Math::vec3& offset)
	{
		if(m_body->GetColliders().empty())
			return;

		m_properties.capsuleOffset = offset;

		SharedPtr<CapsuleCollider> collider = std::static_pointer_cast<CapsuleCollider>(m_body->GetColliders().front());
		collider->SetOffset(offset);
	}

	void PlayerController::ApplyForce(Math::vec3& direction)
	{
		// check for zero vector
		if (Math::all(glm::equal(direction, Math::vec3(0))))
		{
			return;
		}

		// remove the y component from the direction (lock to ground)
		direction.y = 0.0f;

		// set the magnitude of the direction vector based off moveFactor and gravity
		m_unitDirection = glm::normalize(direction);

		// set the current direction off the computed unitDirection scaled by the move factor
		m_currentDirection = m_unitDirection * m_properties.moveFactor;

		// if the player hit a wall, project their movement parallel to the collision surface
		if (m_hitWall)
		{
			MoveAlongNormal(m_forwardRay->GetInfo().hitNormal);
		}

		// if on the ground, project movement along the surface
		if (!m_inFallingState)
		{
			MoveAlongNormal(m_groundRay->GetInfo().hitNormal);
		}

		// add the falling speed to the y component
		m_currentDirection.y += m_fallingSpeed;
	}

	void PlayerController::OnUpdate(float dt)
	{
		Math::vec3 position;
		Math::quat orientation;
		m_body->GetTransform(position, orientation);

		// check if player has hit a wall
		if (DetectWall())
		{
			// if hit, set the state and stop moving in the horizontal directions
			if (!m_hitWall)
			{
				m_currentDirection = {0.0f, m_fallingSpeed, 0.0f};
				m_hitWall = true;
			}
		}
		else
		{
			// if not hit, set the state
			if (m_hitWall)
			{
				m_hitWall = false;
			}
		}

		// check if player has hit the ground
		if (DetectGround())
		{
			if (!m_inGroundedState)
			{
				// if hit, set the state and stop falling
				m_currentDirection = { m_currentDirection.x, 0, m_currentDirection.z };
				m_fallingSpeed = 0.0f;
				m_inGroundedState = true;
				m_inFallingState = false;

				float hitFraction = m_groundRay->GetInfo().hitFraction;

				// if the hit was after the collsion, correct for overlap
				if (hitFraction < 1.0f)
				{
					float correction = 0.99f - hitFraction;
					position.y += correction * m_groundRayLength;
				}
			}
		}
		else
		{
			// if not hit, set the state and start falling
			if (!m_inFallingState)
			{
				m_inFallingState = true;
				m_inGroundedState = false;
			}

			// set the y component of velocity to the falling speed
			m_currentDirection.y = m_fallingSpeed;

			// apply falling acceleration
			m_fallingSpeed += -9.8f * dt;
		}

		Math::vec3 deltaLinearVelocity = m_currentDirection * dt;
		Math::vec3 newPosition = position + deltaLinearVelocity;

		m_currentDirection.x *= m_properties.moveDrag;
		m_currentDirection.y *= m_properties.fallDrag;
		m_currentDirection.z *= m_properties.moveDrag;

		m_body->SetTransform(newPosition, orientation);
	}

	bool PlayerController::DetectGround()
	{
		// get the current transform
		Math::vec3 capsCenter;
		Math::quat orientation;
		m_body->GetTransform(capsCenter, orientation);

		capsCenter += m_properties.capsuleOffset;

		// create a ray from the center to the bottom of the capsule and test for collision
		Math::vec3 capsBottom = capsCenter;
		capsBottom.y -= m_groundRayLength;
		return (m_groundRay->CastRay(capsCenter, capsBottom));
	}

	bool PlayerController::DetectWall()
	{
		// get the current transform
		Math::vec3 capsNearBottom;
		Math::quat orientation;
		m_body->GetTransform(capsNearBottom, orientation);

		// create a ray from near the bottom of the capsule to the edge of the capsule and test for collision
		capsNearBottom.y -= 0.9f * 0.5f * m_properties.height;
		capsNearBottom.y -= m_properties.capsuleOffset.y;
		Math::vec3 capsRadius = capsNearBottom + m_properties.radius * m_unitDirection;
		return (m_forwardRay->CastRay(capsNearBottom, capsRadius));
	}

	void PlayerController::MoveAlongNormal(const Math::vec3& normal)
	{
		if (normal == glm::vec3(0.0f))
			return;

		float factor = Math::dot(m_currentDirection, normal) / glm::length2(normal);
		m_currentDirection -= factor * normal;
	}
}
