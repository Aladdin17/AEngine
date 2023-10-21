/**
 * \file   ReactCollider.h
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/
#pragma once
#include "ReactCollisionBody.h"
#include "AEngine/Physics/Collider.h"
#include <reactphysics3d/reactphysics3d.h>

namespace AEngine
{
		/**
		 * \class ReactCollider
		 * \brief Represents a collider using the ReactPhysics3D library.
		 * \details
		 * Does not inherit from AEngine::Collider as this causes issues with diamond inheritance.
		 * Instead this class is used as a factory to generate the rp3d collider objects.
		 * Also to wrap the common functionality of the rp3d collider objects.
		*/
	class ReactCollider
	{
	public:
			/**
			 * \brief Constructs a ReactCollider object with the given collision body and shape.
			 *
			 * \param body The collision body associated with the collider.
			 * \param shape The collision shape of the collider.
			 */
		ReactCollider(rp3d::Collider* collider);
		~ReactCollider() = default;
			/**
			 * \brief Sets whether the collider is a trigger or not.
			 *
			 * Overrides the base class function SetIsTrigger.
			 *
			 * \param[in] isTrigger Specifies if the collider is a trigger or not.
			 */
		void SetIsTrigger(bool isTrigger);
			/**
			 * \brief Returns whether the collider is a trigger or not.
			 *
			 * Overrides the base class function GetIsTrigger.
			 *
			 * \return True if the collider is a trigger, false otherwise.
			 */
		bool GetIsTrigger() const;

		Math::vec3 GetOffset() const;
		void SetOffset(const Math::vec3& offset);
		Math::quat GetOrientation() const;
		void SetOrientation(const Math::quat& orientation);
			/**
			 * \brief Returns the native ReactPhysics3D CollisionShape object.
			 *
			 * \return The native ReactPhysics3D CollisionShape object.
			 */
		rp3d::CollisionShape* GetNativeShape() const;

	private:
		rp3d::Collider* m_collider;      ///< The native ReactPhysics3D Collider object.
    };


	class ReactBoxCollider : public BoxCollider
	{
	public:
		ReactBoxCollider(rp3d::Collider* collider);
		virtual ~ReactBoxCollider() override = default;
		virtual void SetIsTrigger(bool isTrigger) override;
		virtual bool GetIsTrigger() const override;
		virtual void Resize(const Math::vec3& size) override;
		virtual Math::vec3 GetOffset() const override;
		virtual void SetOffset(const Math::vec3& offset) override;
		virtual Math::quat GetOrientation() const override;
		virtual void SetOrientation(const Math::quat& orientation) override;
		virtual Math::vec3 GetSize() const override;
		Type GetType() const override;
		const char* GetName() const override;

	private:
		UniquePtr<ReactCollider> m_collider;
	};

	class ReactCapsuleCollider : public CapsuleCollider
	{
	public:
		ReactCapsuleCollider(rp3d::Collider* collider);
		virtual ~ReactCapsuleCollider() override = default;
		virtual void SetIsTrigger(bool isTrigger) override;
		virtual bool GetIsTrigger() const override;
		virtual Type GetType() const override;
		virtual const char* GetName() const override;
		virtual Math::vec3 GetOffset() const override;
		virtual void SetOffset(const Math::vec3& offset) override;
		virtual Math::quat GetOrientation() const override;
		virtual void SetOrientation(const Math::quat& orientation) override;

		virtual float GetRadius() const override;
		virtual void SetRadius(float radius) override;
		virtual float GetHeight() const override;
		virtual void SetHeight(float height) override;

	private:
		UniquePtr<ReactCollider> m_collider;
	};

	class ReactSphereCollider : public SphereCollider
	{
	public:
		ReactSphereCollider(rp3d::Collider* collider);
		virtual ~ReactSphereCollider() override = default;
		virtual void SetIsTrigger(bool isTrigger) override;
		virtual bool GetIsTrigger() const override;
		virtual Type GetType() const override;
		virtual const char* GetName() const override;
		virtual Math::vec3 GetOffset() const override;
		virtual void SetOffset(const Math::vec3& offset) override;
		virtual Math::quat GetOrientation() const override;
		virtual void SetOrientation(const Math::quat& orientation) override;

		virtual float GetRadius() const override;
		virtual void SetRadius(float radius) override;

	private:
		UniquePtr<ReactCollider> m_collider;
	};
}
