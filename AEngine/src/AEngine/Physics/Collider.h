/**
 * \file   Collider.h
 * \author Lane O'Rafferty (33534304)
 * \author Christien Alden (34119981)
*/

#pragma once

#include "AEngine/Math/Math.h"

namespace AEngine
{
		/**
		 * \class Collider
		 * \brief Represents a collider in the physics simulation.
		 *
		 * This class provides an interface for managing collider properties.
		 */
	class Collider
	{
	public:
		enum class Type
		{
			Box, Capsule, Sphere
		};

	public:
		virtual ~Collider() = default;
			/**
			 * \brief Sets whether the collider is a trigger.
			 *
			 * \param[in] isTrigger True to set the collider as a trigger, false otherwise.
			 */
		virtual void SetIsTrigger(bool isTrigger) = 0;
			/**
			 * \brief Checks if the collider is a trigger.
			 *
			 * \return True if the collider is a trigger, false otherwise.
			 */
		virtual bool GetIsTrigger() const = 0;
		virtual Math::vec3 GetOffset() const = 0;
		virtual void SetOffset(const Math::vec3& offset) = 0;
		virtual Math::quat GetOrientation() const = 0;
		virtual void SetOrientation(const Math::quat& orientation) = 0;
			/**
			 * \brief Gets the type of the collider.
			 * \return The type of the collider.
			*/
		virtual Type GetType() const = 0;
		virtual const char* GetName() const = 0;
	};


	class BoxCollider : public Collider
	{
	public:
		virtual void Resize(const Math::vec3& size) = 0;
		virtual Math::vec3 GetSize() const = 0;
	};

	class CapsuleCollider : public Collider
	{
	public:
		virtual void SetRadius(float radius) = 0;
		virtual float GetRadius() const = 0;
		virtual void SetHeight(float height) = 0;
		virtual float GetHeight() const = 0;
	};

	class SphereCollider : public Collider
	{
	public:
		virtual void SetRadius(float radius) = 0;
		virtual float GetRadius() const = 0;
	};
}
