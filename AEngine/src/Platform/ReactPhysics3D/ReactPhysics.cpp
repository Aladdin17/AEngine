/**
 * \file   ReactPhysics.cpp
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/
#include "AEngine/Core/TimeStep.h"
#include "AEngine/Core/Types.h"
#include "ReactPhysics.h"
#include "ReactCollisionBody.h"

namespace AEngine
{
//--------------------------------------------------------------------------------
// Math Conversions
//--------------------------------------------------------------------------------
	rp3d::Vector3 AEMathToRP3D(const Math::vec3& vec)
	{
		return { vec.x, vec.y, vec.z };
	}

	Math::vec3 RP3DToAEMath(const rp3d::Vector3& vec)
	{
		return { vec.x, vec.y, vec.z };
	}

	rp3d::Quaternion AEMathToRP3D(const Math::quat& quat)
	{
		return { quat.x, quat.y, quat.z, quat.w };
	}

	Math::quat RP3DToAEMath(const rp3d::Quaternion quat)
	{
		return { quat.w,  quat.x, quat.y, quat.z };
	}

//--------------------------------------------------------------------------------
// ReactPhysicsAPI
//--------------------------------------------------------------------------------
	ReactPhysicsAPI& ReactPhysicsAPI::Instance()
	{
		static ReactPhysicsAPI s_instance;
		return s_instance;
	}

	ReactPhysicsAPI::ReactPhysicsAPI()
		: m_common()
	{

	}

	UniquePtr<PhysicsWorld> ReactPhysicsAPI::CreateWorld(const PhysicsWorld::Props& props)
	{
		// create the world, set the initial properties, then return it
		UniquePtr<PhysicsWorld> world = MakeUnique<ReactPhysicsWorld>(&m_common);
		world->Init(props);
		return std::move(world);
	}

	rp3d::PhysicsCommon* ReactPhysicsAPI::GetCommon()
	{
		return &m_common;
	}

//--------------------------------------------------------------------------------
// ReactPhysicsWorld
//--------------------------------------------------------------------------------
	ReactPhysicsWorld::ReactPhysicsWorld(rp3d::PhysicsCommon* common)
		: m_world(nullptr), m_accumulator(0), m_updateStep(0), m_renderer{ nullptr }
	{
		m_world = common->createPhysicsWorld();
		m_world->setIsDebugRenderingEnabled(false);
		m_renderer = MakeUnique<ReactPhysicsRenderer>(m_world->getDebugRenderer());
	}

	ReactPhysicsWorld::~ReactPhysicsWorld()
	{
		// This ensures that the world is properly destroyed within the PhysicsCommon.
		ReactPhysicsAPI::Instance().GetCommon()->destroyPhysicsWorld(m_world);
	}

	void ReactPhysicsWorld::Init(const Props& settings)
	{
		m_updateStep = settings.updateStep;
	}

	void ReactPhysicsWorld::SetUpdateStep(TimeStep step)
	{
		m_updateStep = step;
	}

	void ReactPhysicsWorld::OnUpdate(TimeStep deltaTime)
	{
		m_accumulator += deltaTime;

		while (m_accumulator >= m_updateStep)
		{
			// remove the update step from the accumulator
			m_accumulator -= m_updateStep;

			// run the update step on each of the rigidbodies in the world
			// this will update their positions and rotations
			// as well as any other physics calculations
			for (auto body : m_rigidBodies)
			{
				// body->OnUpdate(m_updateStep);
			}

			// update the rp3d physics world to detect collisions
			// inside here the collision callbacks will be called
			// and the properties of the rigidbodies will be updated
			// to reflect the collisions; however, the positions and
			// rotations will not be updated, only the immediate collision
			// resolution will be performed and the positions and rotations
			// will be updated in the next step
			m_world->update(m_updateStep.Seconds());

			// update the render data if debug rendering is enabled
			if (m_world->getIsDebugRenderingEnabled())
			{
				m_renderer->GenerateRenderData();
			}
		}
	}

	SharedPtr<CollisionBody> ReactPhysicsWorld::AddCollisionBody(const Math::vec3& position, const Math::quat& orientation)
	{
		SharedPtr<CollisionBody> body = MakeShared<ReactCollisionBody>(this, position, orientation);
		m_collisionBodies.push_back(MakeWeak(body));
		return body;
	}

	SharedPtr<RigidBody> ReactPhysicsWorld::AddRigidBody(const Math::vec3& position, const Math::quat& orientation)
	{
		SharedPtr<RigidBody> body = MakeShared<ReactRigidBody>(this, position, orientation);
		m_rigidBodies.push_back(MakeWeak(body));
		return body;
	}

	void ReactPhysicsWorld::Render(const Math::mat4& projectionView) const
	{
		m_renderer->Render(projectionView);
	}

	bool ReactPhysicsWorld::IsRenderingEnabled() const
	{
		return m_world->getIsDebugRenderingEnabled();
	}

	void ReactPhysicsWorld::SetRenderingEnabled(bool enable)
	{
		m_world->setIsDebugRenderingEnabled(enable);
	}

	const ReactPhysicsRenderer* ReactPhysicsWorld::GetRenderer() const
	{
		return m_renderer.get();
	}

	void ReactPhysicsWorld::ForceRenderingRefresh()
	{
		m_world->forceGenerateRenderingPrimitives();
		if (m_world->getIsDebugRenderingEnabled())
		{
			m_renderer->GenerateRenderData();
		}
	}

	rp3d::PhysicsWorld* ReactPhysicsWorld::GetNative()
	{
		return m_world;
	}
}
