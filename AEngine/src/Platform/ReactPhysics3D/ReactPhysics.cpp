/**
 * \file   ReactPhysics.cpp
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/
#include "AEngine/Core/TimeStep.h"
#include "AEngine/Core/Types.h"
#include "ReactPhysics.h"
#include "ReactCollisionBody.h"
#include "AEngine/Core/Logger.h"

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
// ReactEventListener
//--------------------------------------------------------------------------------
	void ReactEventListener::onContact(const CollisionCallback::CallbackData& callbackData)
	{
		for (unsigned int p = 0; p < callbackData.getNbContactPairs(); p++)
		{
			CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

			if(contactPair.getEventType() == CollisionCallback::ContactPair::EventType::ContactExit)
			{
				continue;
			}

			ReactRigidBody* body1 = static_cast<ReactRigidBody*>(contactPair.getBody1()->getUserData());
			ReactRigidBody* body2 = static_cast<ReactRigidBody*>(contactPair.getBody2()->getUserData());
			
			if(body1 == nullptr || body2 == nullptr)
			{
				continue;
			}
			// check if either body is static implement later
			//if (body1->IsStatic() || body2->IsStatic()) something like that

			for(unsigned int c = 0; c < contactPair.getNbContactPoints(); c++)
			{
				CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(0);
				float penetration = contactPoint.getPenetrationDepth();
				Math::vec3 normal = RP3DToAEMath(contactPoint.getWorldNormal());

				AE_LOG_DEBUG("Normal: '{}', '{}', '{}'", normal.x, normal.y, normal.z);

				rp3d::Vector3 body1Contact = contactPair.getCollider1()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider1();
				rp3d::Vector3 body2Contact = contactPair.getCollider2()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider2();

				Math::vec3 body1ContactPoint = RP3DToAEMath(body1Contact);
				Math::vec3 body2ContactPoint = RP3DToAEMath(body2Contact);

				AE_LOG_DEBUG("Collision: '{}', '{}', '{}'", body1ContactPoint.x, body1ContactPoint.y, body1ContactPoint.z);
				AE_LOG_DEBUG("Collision: '{}', '{}', '{}'", body2ContactPoint.x, body2ContactPoint.y, body2ContactPoint.z);

				
				ResolvePenetration(body1, body2, penetration, normal);
				CollisionResolution(body1, body2, body1ContactPoint, body2ContactPoint, normal);
			}
		}
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
		: m_world(nullptr), m_accumulator(0), m_renderer{ nullptr }, m_eventListener{}
	{
		m_world = common->createPhysicsWorld();
		m_world->setIsDebugRenderingEnabled(false);
		m_world->setEventListener(&m_eventListener);
		m_renderer = MakeUnique<ReactPhysicsRenderer>(m_world->getDebugRenderer());
	}

	ReactPhysicsWorld::~ReactPhysicsWorld()
	{
		// This ensures that the world is properly destroyed within the PhysicsCommon.
		ReactPhysicsAPI::Instance().GetCommon()->destroyPhysicsWorld(m_world);
	}

	void ReactPhysicsWorld::Init(const Props& settings)
	{
		m_props = settings;
	}

	void ReactPhysicsWorld::SetUpdateStep(TimeStep step)
	{
		m_props.updateStep = step;
	}

	void ReactPhysicsWorld::OnUpdate(TimeStep deltaTime)
	{
		m_accumulator += deltaTime;

		while (m_accumulator >= m_props.updateStep)
		{
			// remove the update step from the accumulator
			m_accumulator -= m_props.updateStep;

			// run the update step on each of the rigidbodies in the world
			// this will update their positions and rotations
			// as well as any other physics calculations
			for (auto it = m_rigidBodies.begin(); it != m_rigidBodies.end();)
			{
				SharedPtr<ReactRigidBody> rb = it->lock();
				if (!rb)
				{
					it = m_rigidBodies.erase(it);
					if (it == m_rigidBodies.end())
					{
						break;
					}

					// don't ++it here otherwise we will skip the element after the erased one
					continue;
				}

				UpdateRigidBody(m_props.updateStep, rb.get());
				++it;
			}

			// update the rp3d physics world to detect collisions
			// inside here the collision callbacks will be called
			// and the properties of the rigidbodies will be updated
			// to reflect the collisions; however, the positions and
			// rotations will not be updated, only the immediate collision
			// resolution will be performed and the positions and rotations
			// will be updated in the next step
			m_world->update(m_props.updateStep.Seconds());

			// update the render data if debug rendering is enabled
			if (m_world->getIsDebugRenderingEnabled())
			{
				m_renderer->GenerateRenderData();
			}
		}
	}

	SharedPtr<CollisionBody> ReactPhysicsWorld::AddCollisionBody(const Math::vec3& position, const Math::quat& orientation)
	{
		SharedPtr<ReactCollisionBody> body = MakeShared<ReactCollisionBody>(this, position, orientation);
		m_collisionBodies.push_back(MakeWeak(body));
		return body;
	}

	SharedPtr<RigidBody> ReactPhysicsWorld::AddRigidBody(const Math::vec3& position, const Math::quat& orientation)
	{
		SharedPtr<ReactRigidBody> body = MakeShared<ReactRigidBody>(this, position, orientation);
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

	void ReactPhysicsWorld::UpdateRigidBody(TimeStep deltaTime, ReactRigidBody* body)
	{
		// to be implemented
		
		//add linear velocity
		Math::vec3 position;
		Math::quat orientation;

		body->GetTransform(position, orientation);
		Math::vec3 newPos = position + body->GetLinearVelocity() * deltaTime.Seconds();

		//add angular velocity
		Math::quat newOrientation = orientation * Math::quat(body->GetAngularVelocity() * deltaTime.Seconds());

		body->SetTransform(newPos, newOrientation);
	}


//--------------------------------------------------------------------------------
//Physics Resolution
//--------------------------------------------------------------------------------

	void ReactEventListener::ResolvePenetration(ReactRigidBody* body1, ReactRigidBody* body2, float penetration, const Math::vec3& normal)
	{
		if(body1->GetType() != RigidBody::Type::Static)
		{
			Math::vec3 pos1;
			Math::quat orient1;
			body1->GetTransform(pos1, orient1);
			Math::vec3 newPos1 = pos1 - penetration * normal;
			body1->SetTransform(newPos1, orient1);
		}

		if(body2->GetType() != RigidBody::Type::Static)
		{
			Math::vec3 pos2;
			Math::quat orient2;
			body2->GetTransform(pos2, orient2);
			Math::vec3 newPos2 = pos2 + penetration * normal;
			body2->SetTransform(newPos2, orient2);
		}
	}

	void ReactEventListener::CollisionResolution(ReactRigidBody* body1, ReactRigidBody* body2, const Math::vec3& body1ContactPoint, const Math::vec3& body2ContactPoint, 
												const Math::vec3& normal)
	{
		float coefficeintOfRestitution = 0.8f;

		Math::vec3 body1Linear = body1->GetLinearVelocity();
		Math::vec3 body1Angular = body1->GetAngularVelocity();

		Math::vec3 body2Linear = body2->GetLinearVelocity();
		Math::vec3 body2Angular = body2->GetAngularVelocity();

		Math::vec3 relativeVelocity = body1Linear - body2Linear;

		Math::vec3 r1 = body1ContactPoint - body1->ConvertCOMToWorldSpace();
		Math::vec3 r2 = body2ContactPoint - body2->ConvertCOMToWorldSpace();

		float restitution = -(1.0f + coefficeintOfRestitution);
		float combinedInverseMass = body1->GetInverseMass() + body2->GetInverseMass();

		Math::vec3 r1CrossN = Math::cross(r1, normal);
		Math::vec3 r2CrossN = Math::cross(r2, normal);

		float numerator = restitution * (Math::dot(normal, relativeVelocity) + Math::dot(body1->GetAngularVelocity(), r1CrossN) 
							- Math::dot(body2->GetAngularVelocity(), r2CrossN));

		float denominator = combinedInverseMass + (Math::dot(r1CrossN, body1->GetInverseInertiaTensor() * r1CrossN) 
							+ Math::dot(r2CrossN, body2->GetInverseInertiaTensor() * r2CrossN));

		float lambda = numerator / denominator;

		Math::vec3 impulse = lambda * normal;

		if(lambda < 0)
		{
			body1Linear += impulse * body1->GetInverseMass();
			body1Angular += (lambda * body1->GetInverseInertiaTensor()) * r1CrossN;

			body2Linear -= impulse * body2->GetInverseMass();
			body2Angular -= (lambda * body2->GetInverseInertiaTensor()) * r2CrossN;

			if(body1->GetType() != RigidBody::Type::Static)
			{
				body1->SetLinearVelocity(body1Linear);
				body1->SetAngularVelocity(body1Angular);
			}
			
			if(body2->GetType() != RigidBody::Type::Static)
			{
				body2->SetLinearVelocity(body2Linear);
				body2->SetAngularVelocity(body2Angular);
			} 
		}
	}
}
