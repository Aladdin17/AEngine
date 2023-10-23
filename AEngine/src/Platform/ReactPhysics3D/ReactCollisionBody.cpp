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
    ReactCollisionBody::ReactCollisionBody(ReactPhysicsWorld* world, const Math::vec3& position,
        const Math::quat& orientation, bool isRigid):
        m_world(world)
    {
        if (isRigid)
        {
            m_body = world->GetNative()->createRigidBody({ AEMathToRP3D(position), AEMathToRP3D(orientation) });
        }
        else
        {
            m_body = world->GetNative()->createCollisionBody({ AEMathToRP3D(position), AEMathToRP3D(orientation) });
        }

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
        m_body = MakeUnique<ReactCollisionBody>(world, position, orientation, true);
    }

	rp3d::RigidBody* ReactRigidBody::GetNative() const
    {
        return dynamic_cast<rp3d::RigidBody*>(m_body->GetNative());
    }

    // rigidbody stuff

    void ReactRigidBody::SetHasGravity(bool hasGravity)
    {
    	GetNative()->enableGravity(hasGravity);
    }

    bool ReactRigidBody::GetHasGravity() const
    {
        return GetNative()->isGravityEnabled();
    }

    void ReactRigidBody::SetVelocity(const Math::vec3& velocity)
    {
        GetNative()->setLinearVelocity(AEMathToRP3D(velocity));
    }

    const Math::vec3 ReactRigidBody::GetVelocity() const
    {
        const rp3d::Vector3& velocity = GetNative()->getLinearVelocity();
        return RP3DToAEMath(velocity);
    }

    void ReactRigidBody::SetDrag(float factor)
    {
        GetNative()->setLinearDamping(factor);
    }

    void ReactRigidBody::SetMass(float massKg)
    {
        GetNative()->setMass(massKg);
    }

    float ReactRigidBody::GetMass() const
    {
        return GetNative()->getMass();
    }

    void ReactRigidBody::SetType(Type type)
    {
        switch (type)
        {
        case Type::STATIC:
            GetNative()->setType(rp3d::BodyType::STATIC);
            break;
        case Type::KINEMATIC:
            GetNative()->setType(rp3d::BodyType::KINEMATIC);
            break;
        case Type::DYNAMIC:
            GetNative()->setType(rp3d::BodyType::DYNAMIC);
            break;
        default:
        {
            AE_LOG_FATAL("ReactRigidBody::SetType::Invalid_type");
        }
        }
    }

    RigidBody::Type ReactRigidBody::GetType() const
    {
        rp3d::BodyType type = GetNative()->getType();
        switch (type)
        {
        case rp3d::BodyType::DYNAMIC:
            return Type::DYNAMIC;
        case rp3d::BodyType::STATIC:
            return Type::STATIC;
        case rp3d::BodyType::KINEMATIC:
            return Type::KINEMATIC;
        }
    }

    // collision body stuff

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

    void ReactRigidBody::GetInterpolatedTransform(Math::vec3& position, Math::quat& orientation)
    {
        m_body->GetInterpolatedTransform(position, orientation);
    }

    UniquePtr<Collider> ReactRigidBody::GetCollider()
    {
        return m_body->GetCollider();
    }

	void ReactRigidBody::RemoveCollider()
    {
        m_body->RemoveCollider();
    }
}
