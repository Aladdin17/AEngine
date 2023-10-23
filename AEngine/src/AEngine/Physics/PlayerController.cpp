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
		m_currentVelocity{ Math::vec3{ 0.0f } },
		m_inGroundedState{ false },
		m_inFallingState{ true },
		m_body{ nullptr },
		m_groundRay{ nullptr },
		m_forwardRay{ nullptr },
		m_forwardRayLength{ m_properties.radius * 0.5f },
		m_groundRayLength{ m_properties.height }
	{
		m_body = world->AddRigidBody(startPosition, Math::quat(1, 0, 0, 0));
		m_body->SetType(RigidBody::Type::Kinematic);
		m_body->AddCapsuleCollider(m_properties.radius, m_properties.height);
		m_groundRay = Raycaster::Create(world);
		m_forwardRay = Raycaster::Create(world);
	}

	Math::vec3 PlayerController::GetTransform() const
	{
		// get the interpolated transform for the collision body
		Math::vec3 position;
		Math::quat orientation;
		m_body->GetInterpolatedTransform(position, orientation);

		// return the position component
		return position;
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

		// applies movement on the body based on the current direction
		m_body->SetLinearVelocity(m_currentDirection);
	}

	void PlayerController::OnUpdate(float dt)
	{
		// check if player has hit a wall
		if (DetectWall())
		{
			// if hit, set the state and stop moving in the horizontal directions
			if (!m_hitWall)
			{
				m_body->SetLinearVelocity({0.0f, m_fallingSpeed, 0.0f});
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
				m_body->SetLinearDamping(m_properties.moveDrag);
				m_body->SetLinearVelocity({ m_currentDirection.x, 0, m_currentDirection.z });
				m_fallingSpeed = 0.0f;
				m_inGroundedState = true;
				m_inFallingState = false;

				float hitFraction = m_groundRay->GetInfo().hitFraction;

				// if the hit was after the collsion, correct for overlap
				if (hitFraction < 1.0f)
				{
					Math::vec3 position;
					Math::quat orientation;
					m_body->GetTransform(position, orientation);

					float correction = 0.99f - hitFraction;
					position.y += correction * m_groundRayLength;
					m_body->SetTransform(position, orientation);
				}
			}
		}
		else
		{
			// if not hit, set the state and start falling
			if (!m_inFallingState)
			{
				m_body->SetLinearDamping(m_properties.fallDrag);
				m_inFallingState = true;
				m_inGroundedState = false;
			}

			// set the y component of velocity to the falling speed
			m_currentDirection.y = m_fallingSpeed;
			m_body->SetLinearVelocity(m_currentDirection);

			// apply falling acceleration
			m_fallingSpeed += -9.8f * dt;
		}
	}

	bool PlayerController::DetectGround()
	{
		// get the current transform
		Math::vec3 capsCenter;
		Math::quat orientation;
		m_body->GetTransform(capsCenter, orientation);

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
		Math::vec3 capsRadius = capsNearBottom + m_properties.radius * m_unitDirection;
		return (m_forwardRay->CastRay(capsNearBottom, capsRadius));
	}

	void PlayerController::MoveAlongNormal(const Math::vec3& normal)
	{
		float factor = Math::dot(m_currentDirection, normal) / glm::length2(normal);
		m_currentDirection -= factor * normal;
	}
}
