/**
 * \file   ReactCollisionBody.cpp
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#include "AEngine/Core/Logger.h"
#include "ReactCollider.h"
#include "ReactCollisionBody.h"
#include "ReactPhysics.h"

namespace AEngine
{
//--------------------------------------------------------------------------------
// ReactCollisionBody
//--------------------------------------------------------------------------------
	ReactCollisionBody::ReactCollisionBody(
		ReactPhysicsWorld* world,
		const Math::vec3& position,
		const Math::quat& orientation):
		m_world(world)
	{
		m_body = world->GetNative()->createCollisionBody({ AEMathToRP3D(position), AEMathToRP3D(orientation) });
		m_lastTransform = m_body->getTransform();
	}

	ReactCollisionBody::~ReactCollisionBody()
	{
		rp3d::RigidBody* rigidBody = dynamic_cast<rp3d::RigidBody*>(m_body);
		if(rigidBody)
		{
			m_world->GetNative()->destroyRigidBody(rigidBody);
		}
		else
		{
			m_world->GetNative()->destroyCollisionBody(m_body);
		}
	}

	rp3d::CollisionBody* ReactCollisionBody::GetNative() const
	{
		return m_body;
	}

	void ReactCollisionBody::SetTransform(const Math::vec3& position, const Math::quat& orientation)
	{
		rp3d::Transform transform;
		transform.setPosition(AEMathToRP3D(position));
		transform.setOrientation(AEMathToRP3D(orientation));
		m_body->setTransform(transform);
	}

	void ReactCollisionBody::GetTransform(Math::vec3& position, Math::quat& orientation) const
	{
		const rp3d::Transform& transform = m_body->getTransform();
		position = RP3DToAEMath(transform.getPosition());
		orientation = RP3DToAEMath(transform.getOrientation());
	}

	UniquePtr<Collider> ReactCollisionBody::AddBoxCollider(const Math::vec3& size, const Math::vec3& offset, const Math::quat& orientation)
	{
		rp3d::PhysicsCommon* common = dynamic_cast<ReactPhysicsAPI&>(PhysicsAPI::Instance()).GetCommon();
		rp3d::BoxShape* box = common->createBoxShape(AEMathToRP3D(size));
		rp3d::Transform transform(AEMathToRP3D(offset), AEMathToRP3D(orientation));
		rp3d::Collider* collider = m_body->addCollider(box, transform);
		return MakeUnique<ReactBoxCollider>(collider);
	}

	UniquePtr<Collider> ReactCollisionBody::AddCapsuleCollider(float radius, float height, const Math::vec3& offset, const Math::quat& orientation)
	{
		rp3d::PhysicsCommon* common = dynamic_cast<ReactPhysicsAPI&>(PhysicsAPI::Instance()).GetCommon();
		rp3d::CapsuleShape* capsule = common->createCapsuleShape(radius, height);
		rp3d::Transform transform(AEMathToRP3D(offset), AEMathToRP3D(orientation));
		rp3d::Collider* collider = m_body->addCollider(capsule, transform);
		return MakeUnique<ReactCapsuleCollider>(collider);
	}

	UniquePtr<Collider> ReactCollisionBody::AddSphereCollider(float radius, const Math::vec3& offset, const Math::quat& orientation)
	{
		rp3d::PhysicsCommon* common = dynamic_cast<ReactPhysicsAPI&>(PhysicsAPI::Instance()).GetCommon();
		rp3d::SphereShape* sphere = common->createSphereShape(radius);
		rp3d::Transform transform(AEMathToRP3D(offset), AEMathToRP3D(orientation));
		rp3d::Collider* collider = m_body->addCollider(sphere, transform);
		return MakeUnique<ReactSphereCollider>(collider);
	}

	void ReactCollisionBody::GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation)
	{
		rp3d::Transform current = m_body->getTransform();
		rp3d::Transform interpolated = rp3d::Transform::interpolateTransforms(
			m_lastTransform,
			current,
			m_world->GetAccumulatorVal() / m_world->GetUpdateStep()
		);

		m_lastTransform = current;
		position = RP3DToAEMath(interpolated.getPosition());
		orientation = RP3DToAEMath(interpolated.getOrientation());
	}

	UniquePtr<Collider> ReactCollisionBody::GetCollider()
	{
		rp3d::uint32 numColliders = m_body->getNbColliders();
		if (numColliders == 0)
		{
			return nullptr;
		}

		rp3d::Collider* collider = m_body->getCollider(0);
		if (!collider)
		{
			return nullptr;
		}

		rp3d::CollisionShapeName type = collider->getCollisionShape()->getName();
		switch (type)
		{
		case rp3d::CollisionShapeName::BOX:
			return MakeUnique<ReactBoxCollider>(collider);
		case rp3d::CollisionShapeName::CAPSULE:
			return MakeUnique<ReactCapsuleCollider>(collider);
		case rp3d::CollisionShapeName::SPHERE:
			return MakeUnique<ReactSphereCollider>(collider);
		default:
			AE_LOG_FATAL("ReactCollisionBody::GetCollider::Invalid_type");
		}
	}

	void ReactCollisionBody::RemoveCollider()
	{
		rp3d::Collider* collider = m_body->getCollider(0);
		if (collider)
		{
			m_body->removeCollider(collider);
		}
	}

//--------------------------------------------------------------------------------
// ReactRigidBody
//--------------------------------------------------------------------------------
	ReactRigidBody::ReactRigidBody(ReactPhysicsWorld* world, const Math::vec3& position, const Math::quat& orientation)
	{
		m_body = MakeUnique<ReactCollisionBody>(world, position, orientation);
	}

	void ReactRigidBody::SetType(Type type)
	{
		m_type = type;
	}

	RigidBody::Type ReactRigidBody::GetType() const
	{
		return m_type;
	}

	void ReactRigidBody::SetMass(float massKg)
	{
		m_mass = massKg;
	}

	float ReactRigidBody::GetMass() const
	{
		return m_mass;
	}

	void ReactRigidBody::SetLinearDamping(float damping)
	{
		m_linearDamping = std::clamp(damping, 0.0f, 1.0f);
	}

	float ReactRigidBody::GetLinearDamping() const
	{
		return m_linearDamping;
	}

	void ReactRigidBody::SetAngularDamping(float damping)
	{
		m_angularDamping = std::clamp(damping, 0.0f, 1.0f);
	}

	float ReactRigidBody::GetAngularDamping() const
	{
		return m_angularDamping;
	}

	void ReactRigidBody::SetHasGravity(bool hasGravity)
	{
		m_hasGravity = hasGravity;
	}

	bool ReactRigidBody::GetHasGravity() const
	{
		return m_hasGravity;
	}

	void ReactRigidBody::SetLinearAcceleration(Math::vec3 acceleration)
	{
		m_linearAcceleration = acceleration;
	}

	Math::vec3 ReactRigidBody::GetLinearAcceleration() const
	{
		return m_linearAcceleration;
	}

	void ReactRigidBody::SetAngularAcceleration(Math::vec3 acceleration)
	{
		m_angularAcceleration = acceleration;
	}

	Math::vec3 ReactRigidBody::GetAngularAcceleration() const
	{
		return m_angularAcceleration;
	}

	void ReactRigidBody::SetLinearVelocity(const Math::vec3& velocity)
	{
		m_linearVelocity = velocity;
	}

	const Math::vec3 ReactRigidBody::GetLinearVelocity() const
	{
		return m_linearVelocity;
	}

	void ReactRigidBody::SetAngularVelocity(const Math::vec3& velocity)
	{
		m_angularVelocity = velocity;
	}

	const Math::vec3 ReactRigidBody::GetAngularVelocity() const
	{
		return m_angularVelocity;
	}


//--------------------------------------------------------------------------------
// From ReactCollisionBody
//--------------------------------------------------------------------------------
	void ReactRigidBody::SetTransform(const Math::vec3& position, const Math::quat& orientation)
	{
		m_body->SetTransform(position, orientation);
	}

	void ReactRigidBody::GetTransform(Math::vec3& position, Math::quat& orientation) const
	{
		m_body->GetTransform(position, orientation);
	}

	UniquePtr<Collider> ReactRigidBody::AddBoxCollider(const Math::vec3& size, const Math::vec3& offset, const Math::quat& orientation)
	{
		return m_body->AddBoxCollider(size, offset, orientation);
	}

	UniquePtr<Collider> ReactRigidBody::AddCapsuleCollider(float radius, float height, const Math::vec3& offset, const Math::quat& orientation)
	{
		return m_body->AddCapsuleCollider(radius, height);
	}

	UniquePtr<Collider> ReactRigidBody::AddSphereCollider(float radius, const Math::vec3& offset, const Math::quat& orientation)
	{
		return m_body->AddSphereCollider(radius);
	}

	UniquePtr<Collider> ReactRigidBody::GetCollider()
	{
		return m_body->GetCollider();
	}

	void ReactRigidBody::RemoveCollider()
	{
		m_body->RemoveCollider();
	}

	void ReactRigidBody::GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation)
	{
		m_body->GetInterpolatedTransform(position, orientation);
	}
}
