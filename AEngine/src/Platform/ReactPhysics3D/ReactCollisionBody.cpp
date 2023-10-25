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

		// don't update the mass if it's the same, prevents unnecessary calculations of the inertia tensor
		if (abs(massKg - m_mass) < std::numeric_limits<float>::epsilon())
		{
			return;
		}

		m_mass = massKg;
		m_inverseMass = 1.0f / massKg;
		CalculateInertiaTensor();
	}

	float ReactRigidBody::GetMass() const
	{
		return m_mass;
	}

	float ReactRigidBody::GetInverseMass() const
	{
		return m_inverseMass;
	}

	void ReactRigidBody::SetRestitution(float restitution)
	{
		m_restitution = std::clamp(restitution, 0.0f, 1.0f);
	}

	float ReactRigidBody::GetRestitution() const
	{
		return m_restitution;
	}

	Math::mat3 ReactRigidBody::GetLocalInertiaTensor() const
	{
		return m_inertiaTensor;
	}

	Math::mat3 ReactRigidBody::GetWorldInertiaTensor() const
	{
		Math::vec3 position;
		Math::quat orientation;
		m_body->GetTransform(position, orientation);
		Math::mat3 rotation = Math::mat3_cast(orientation);
		return rotation * m_inertiaTensor * Math::transpose(rotation);
	}

	Math::mat3 ReactRigidBody::GetLocalInverseInertiaTensor() const
	{
		return m_inverseInertiaTensor;
	}

	Math::mat3 ReactRigidBody::GetWorldInverseInertiaTensor() const
	{
		Math::vec3 position;
		Math::quat orientation;
		m_body->GetTransform(position, orientation);
		Math::mat3 rotation = Math::mat3_cast(orientation);
		return rotation * m_inverseInertiaTensor * Math::transpose(rotation);
	}

	void ReactRigidBody::SetCentreOfMass(const Math::vec3 &centreOfMass)
	{
		m_centreOfMass = centreOfMass;
	}

	Math::vec3 ReactRigidBody::GetCentreOfMass() const
	{
		return m_centreOfMass;
	}

	Math::vec3 ReactRigidBody::GetCentreOfMassWorldSpace() const
	{
		Math::vec3 position;
		Math::quat orientation;
		m_body->GetTransform(position, orientation);
		return position + (orientation * m_centreOfMass);
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

	void ReactRigidBody::SetLinearMomentum(const Math::vec3 &momentum)
	{
		m_linearMomentum = momentum;
	}

	const Math::vec3 ReactRigidBody::GetLinearMomentum() const
	{
		return m_linearMomentum;
	}

	void ReactRigidBody::SetAngularMomentum(const Math::vec3 &momentum)
	{
		m_angularMomentum = momentum;
	}

	const Math::vec3 ReactRigidBody::GetAngularMomentum() const
	{
		return m_angularMomentum;
	}

	void ReactRigidBody::SetLinearVelocity(const Math::vec3& velocity)
	{
		m_linearMomentum = m_mass * velocity;
	}

	const Math::vec3 ReactRigidBody::GetLinearVelocity() const
	{
		return m_linearMomentum * m_inverseMass;
	}

	void ReactRigidBody::SetAngularVelocity(const Math::vec3& velocity)
	{
		m_angularMomentum = m_inertiaTensor * velocity;
	}

	const Math::vec3 ReactRigidBody::GetAngularVelocity() const
	{
		return m_angularMomentum * m_inverseInertiaTensor;
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
		// attach the collider and update the inertia tensor
		UniquePtr<Collider> collider = m_body->AddBoxCollider(size, offset, orientation);
		CalculateInertiaTensor();
		return std::move(collider);
	}

	UniquePtr<Collider> ReactRigidBody::AddCapsuleCollider(float radius, float height, const Math::vec3& offset, const Math::quat& orientation)
	{
		UniquePtr<Collider> collider = m_body->AddCapsuleCollider(radius, height, offset, orientation);
		CalculateInertiaTensor();
		return collider;
	}

	UniquePtr<Collider> ReactRigidBody::AddSphereCollider(float radius, const Math::vec3& offset, const Math::quat& orientation)
	{
		UniquePtr<Collider> collider = m_body->AddSphereCollider(radius, offset, orientation);
		CalculateInertiaTensor();
		return collider;
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

	void ReactRigidBody::CalculateInertiaTensor()
	{
		UniquePtr<Collider> collider = this->GetCollider();
		if (!collider)
		{
			return;
		}

		switch (collider->GetType())
		{
		case Collider::Type::Box:
			{
				ReactBoxCollider* box = dynamic_cast<ReactBoxCollider*>(collider.get());
				Math::vec3 halfExtents = box->GetSize();

				// calculate inertia tensor assuming uniform density and no offset
				m_inertiaTensor[0][0] = (1.0f / 3.0f) * m_mass * (pow(halfExtents.y, 2.0f) + pow(halfExtents.z, 2.0f));
				m_inertiaTensor[1][1] = (1.0f / 3.0f) * m_mass * (pow(halfExtents.x, 2.0f) + pow(halfExtents.z, 2.0f));
				m_inertiaTensor[2][2] = (1.0f / 3.0f) * m_mass * (pow(halfExtents.x, 2.0f) + pow(halfExtents.y, 2.0f));
				m_inverseInertiaTensor = Math::inverse(m_inertiaTensor);
			}
			break;

		case Collider::Type::Capsule:
			{
				ReactCapsuleCollider* capsule = dynamic_cast<ReactCapsuleCollider*>(collider.get());
				float radius = capsule->GetRadius();
				float height = capsule->GetHeight();

				// calculate inertia tensor assuming uniform density and no offset
				float hemisphereI = (2.0f / 5.0f) * m_mass * radius * radius;
				float cylinderI = (1.0f / 12.0f) * m_mass * (3.0f * radius * radius + height * height);
				m_inertiaTensor[0][0] = 2 * hemisphereI + cylinderI;
				m_inertiaTensor[1][1] = 2 * hemisphereI + cylinderI;
				m_inertiaTensor[2][2] = hemisphereI + cylinderI;
				m_inverseInertiaTensor = Math::inverse(m_inertiaTensor);
			}
			break;
		case Collider::Type::Sphere:
			{
				ReactSphereCollider* sphere = dynamic_cast<ReactSphereCollider*>(collider.get());
				float radius = sphere->GetRadius();

				// calculate inertia tensor assuming uniform density and no offset
				m_inertiaTensor[0][0] = (2.0f / 5.0f) * m_mass * radius * radius;
				m_inertiaTensor[1][1] = (2.0f / 5.0f) * m_mass * radius * radius;
				m_inertiaTensor[2][2] = (2.0f / 5.0f) * m_mass * radius * radius;
				m_inverseInertiaTensor = Math::inverse(m_inertiaTensor);
			}
			break;
		default:
			{
				AE_LOG_FATAL("ReactRigidBody::CalculateInertiaTensor::Invalid_collider_type");
			}
		}
	}
}
