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
		const Math::quat& orientation)
		: m_world(world)
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
		m_body->GetNative()->setUserData(static_cast<void*>(this));
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
		if (massKg <= 0.0f)
		{
			AE_LOG_ERROR("ReactRigidBody::SetMass::Mass_must_be_greater_than_zero");
			return;
		}

		m_mass = massKg;
		m_inverseMass = 1.0f / massKg;
	}

	float ReactRigidBody::GetMass() const
	{
		return m_mass;
	}

	void ReactRigidBody::SetInertiaTensor(const Math::mat3& inertiaTensor)
	{
		/// \todo Find a way to detect invalid inverse inertia tensors
		m_inertiaTensor = inertiaTensor;
		m_inverseInertiaTensor = Math::inverse(inertiaTensor);
	}

	Math::mat3 ReactRigidBody::GetInertiaTensor() const
	{
		return m_inertiaTensor;
	}

	void ReactRigidBody::SetHasGravity(bool hasGravity)
	{
		m_hasGravity = hasGravity;
	}

	bool ReactRigidBody::GetHasGravity() const
	{
		return m_hasGravity;
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

	void ReactRigidBody::SetCenterOfMass(const Math::vec3 &centerOfMass)
	{
		m_centerOfMass = centerOfMass;
	}

	Math::vec3 ReactRigidBody::GetCenterOfMass() const
	{
		return m_centerOfMass;
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
		Math::mat3 inertiaTensor = RectangleInertiaTensor(m_mass, size);
		this->SetInertiaTensor(inertiaTensor);

		return m_body->AddBoxCollider(size, offset, orientation);
	}

	UniquePtr<Collider> ReactRigidBody::AddCapsuleCollider(float radius, float height, const Math::vec3& offset, const Math::quat& orientation)
	{
		return m_body->AddCapsuleCollider(radius, height);
	}

	UniquePtr<Collider> ReactRigidBody::AddSphereCollider(float radius, const Math::vec3& offset, const Math::quat& orientation)
	{
		Math::mat3 inertiaTensor = SphereInertiaTensor(m_mass, radius);
		this->SetInertiaTensor(inertiaTensor);

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

	//convert center of mass at world space
	Math::vec3 ReactRigidBody::ConvertCOMToWorldSpace()
	{;
		Math::vec3 pos;
		Math::quat orientation;
		m_body->GetTransform(pos, orientation);
		return pos + orientation * m_centerOfMass;
	}

	float ReactRigidBody::GetInverseMass() const
	{
		return m_inverseMass;
	}

	Math::mat3 ReactRigidBody::GetInverseInertiaTensor() const
	{
		return m_inverseInertiaTensor;
	}

	//rectangular prism inertia tensor
	Math::mat3 ReactRigidBody::RectangleInertiaTensor(const float& mass, const Math::vec3& size)
	{
		float x = size.x * 2;
		float y = size.y * 2;
		float z = size.z * 2;

		float Ixx = (mass / 12.0f) * (y * y + z * z);
		float Iyy = (mass / 12.0f) * (x * x + z * z);
		float Izz = (mass / 12.0f) * (x * x + y * y);

		return Math::mat3{ Ixx, 0.0f, 0.0f,
						   0.0f, Iyy, 0.0f,
						   0.0f, 0.0f, Izz };
	}

	//sphere inertia tensor
	Math::mat3 ReactRigidBody::SphereInertiaTensor(const float& mass, const float& radius)
	{
		float I = (2.0f / 5.0f) * mass * radius * radius;
		return Math::mat3{ I, 0.0f, 0.0f,
						   0.0f, I, 0.0f,
						   0.0f, 0.0f, I };
	}

	//sphereshell inertia tensor

	//cylinder inertia tensor

	//cylindershell inertia tensor
}
