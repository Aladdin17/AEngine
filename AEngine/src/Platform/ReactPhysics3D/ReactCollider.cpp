/**
 * \file   ReactCollider.cpp
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#include <reactphysics3d/reactphysics3d.h>
#include "ReactCollider.h"

namespace AEngine
{
    ReactCollider::ReactCollider(rp3d::Collider* collider)
        : m_collider{ collider }
    {
        // no body
    }

    void ReactCollider::SetIsTrigger(bool isTrigger)
    {
        m_collider->setIsTrigger(isTrigger);
    }

    bool ReactCollider::GetIsTrigger() const
    {
        return m_collider->getIsTrigger();
    }

    Math::vec3 ReactCollider::GetOffset() const
    {
        rp3d::Transform transform = m_collider->getLocalToBodyTransform();
        return RP3DToAEMath(transform.getPosition());
    }

	void ReactCollider::SetOffset(const Math::vec3& offset)
    {
        // get transform, set position, then set transform in collider
        rp3d::Transform transform = m_collider->getLocalToBodyTransform();
        transform.setPosition(AEMathToRP3D(offset));
        m_collider->setLocalToBodyTransform(transform);
    }

	Math::quat ReactCollider::GetOrientation() const
    {
        rp3d::Transform transform = m_collider->getLocalToBodyTransform();
        return RP3DToAEMath(transform.getOrientation());
    }

	void ReactCollider::SetOrientation(const Math::quat& orientation)
    {
        // get transform, set orientation, then set transform in collider
        rp3d::Transform transform = m_collider->getLocalToBodyTransform();
        transform.setOrientation(AEMathToRP3D(orientation));
        m_collider->setLocalToBodyTransform(transform);
    }

    rp3d::CollisionShape* ReactCollider::GetNativeShape() const
    {
        return m_collider->getCollisionShape();
    }

    ReactBoxCollider::ReactBoxCollider(rp3d::Collider* collider)
    {
        m_collider = MakeUnique<ReactCollider>(collider);
    }

    void ReactBoxCollider::SetIsTrigger(bool isTrigger)
    {
        m_collider->SetIsTrigger(isTrigger);
    }

    bool ReactBoxCollider::GetIsTrigger() const
    {
        return m_collider->GetIsTrigger();
    }

    void ReactBoxCollider::Resize(const Math::vec3& size)
    {
        rp3d::BoxShape* box = dynamic_cast<rp3d::BoxShape*>(m_collider->GetNativeShape());
        box->setHalfExtents(AEMathToRP3D(size));
    }

    Math::vec3 ReactBoxCollider::GetOffset() const
    {
        return m_collider->GetOffset();
    }

    void ReactBoxCollider::SetOffset(const Math::vec3& offset)
    {
        m_collider->SetOffset(offset);
    }

    Math::quat ReactBoxCollider::GetOrientation() const
    {
        return m_collider->GetOrientation();
    }

    void ReactBoxCollider::SetOrientation(const Math::quat& orientation)
    {
        m_collider->SetOrientation(orientation);
    }

    Math::vec3 ReactBoxCollider::GetSize() const
    {
        rp3d::BoxShape* box = dynamic_cast<rp3d::BoxShape*>(m_collider->GetNativeShape());
        return RP3DToAEMath(box->getHalfExtents());
    }

	Collider::Type ReactBoxCollider::GetType() const
    {
        return Type::Box;
    }

	const char* ReactBoxCollider::GetName() const
    {
        return "Box";
    }

//--------------------------------------------------------------------------------
// ReactCapsuleCollider
//--------------------------------------------------------------------------------

    ReactCapsuleCollider::ReactCapsuleCollider(rp3d::Collider* collider)
    {
        m_collider = MakeUnique<ReactCollider>(collider);
    }

    Collider::Type ReactCapsuleCollider::GetType() const
    {
        return Type::Box;
    }

	const char* ReactCapsuleCollider::GetName() const
    {
        return "Box";
    }

    void ReactCapsuleCollider::SetIsTrigger(bool isTrigger)
    {
        m_collider->SetIsTrigger(isTrigger);
    }

    bool ReactCapsuleCollider::GetIsTrigger() const
    {
        return m_collider->GetIsTrigger();
    }

    Math::vec3 ReactCapsuleCollider::GetOffset() const
    {
        return m_collider->GetOffset();
    }

    void ReactCapsuleCollider::SetOffset(const Math::vec3& offset)
    {
        m_collider->SetOffset(offset);
    }

    Math::quat ReactCapsuleCollider::GetOrientation() const
    {
        return m_collider->GetOrientation();
    }

    void ReactCapsuleCollider::SetOrientation(const Math::quat& orientation)
    {
        m_collider->SetOrientation(orientation);
    }

    float ReactCapsuleCollider::GetRadius() const
    {
        rp3d::CapsuleShape* capsule = dynamic_cast<rp3d::CapsuleShape*>(m_collider->GetNativeShape());
        return capsule->getRadius();
    }

    void ReactCapsuleCollider::SetRadius(float radius)
    {
        rp3d::CapsuleShape* capsule = dynamic_cast<rp3d::CapsuleShape*>(m_collider->GetNativeShape());
        capsule->setRadius(radius);
    }

    float ReactCapsuleCollider::GetHeight() const
    {
        rp3d::CapsuleShape* capsule = dynamic_cast<rp3d::CapsuleShape*>(m_collider->GetNativeShape());
        return capsule->getHeight();
    }

    void ReactCapsuleCollider::SetHeight(float height)
    {
        rp3d::CapsuleShape* capsule = dynamic_cast<rp3d::CapsuleShape*>(m_collider->GetNativeShape());
        capsule->setHeight(height);
    }

    //--------------------------------------------------------------------------------
// ReactSphereCollider
//--------------------------------------------------------------------------------

    ReactSphereCollider::ReactSphereCollider(rp3d::Collider* collider)
    {
        m_collider = MakeUnique<ReactCollider>(collider);
    }

    Collider::Type ReactSphereCollider::GetType() const
    {
        return Type::Sphere;
    }

	const char* ReactSphereCollider::GetName() const
    {
        return "Sphere";
    }

    void ReactSphereCollider::SetIsTrigger(bool isTrigger)
    {
        m_collider->SetIsTrigger(isTrigger);
    }

    bool ReactSphereCollider::GetIsTrigger() const
    {
        return m_collider->GetIsTrigger();
    }

    Math::vec3 ReactSphereCollider::GetOffset() const
    {
        return m_collider->GetOffset();
    }

    void ReactSphereCollider::SetOffset(const Math::vec3& offset)
    {
        m_collider->SetOffset(offset);
    }

    Math::quat ReactSphereCollider::GetOrientation() const
    {
        return m_collider->GetOrientation();
    }

    void ReactSphereCollider::SetOrientation(const Math::quat& orientation)
    {
        m_collider->SetOrientation(orientation);
    }

    float ReactSphereCollider::GetRadius() const
    {
        rp3d::SphereShape* sphere = dynamic_cast<rp3d::SphereShape*>(m_collider->GetNativeShape());
        return sphere->getRadius();
    }

    void ReactSphereCollider::SetRadius(float radius)
    {
        rp3d::SphereShape* sphere = dynamic_cast<rp3d::SphereShape*>(m_collider->GetNativeShape());
        sphere->setRadius(radius);
    }
}
