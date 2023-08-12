/**
 * @file
 * @author Ben Hawkins (34112619)
**/

#pragma once

#include <vector>
#include <string>
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AEngine/Render/Bone.h"

namespace AEngine
{

		/**
		 * @struct Bone
		 * @brief Stores animation data for a Bone
		**/
	class AssimpBone : public Bone
	{
	public:
			/**
			 * @brief Contructor
			 * @param[in] animNode Assimp aiNodeAnim object
			**/
		AssimpBone(const aiNodeAnim* animNode);
			/**
			 * @brief Update and return a bone transform
			 * @param[in] animationTime current time in animation
			**/
		const Math::mat4 GetLocalTransform(float animationTime) const;
			/**
			 * @brief Return a bone name
			 * @return String bone name
			**/
		std::string GetBoneName() const;

	private:
			/**
			 * @brief Load bone keyframes
			 * @param[in] animNode Assimp animNode object
			**/
		void LoadData(const aiNodeAnim* animNode);
			/**
			 * @brief Calculates the scale between two timestamps
			 * @param[in] currentTimeStamp first keyframe timestamp
			 * @param[in] nextTimeStamp next keyframe timestamp
			 * @param[in] animationTime time of animation
			**/
		const float GetScaleFactor(float currentTimeStamp, float nextTimeStamp, float animationTime) const;
			/**
			 * @brief Interpolate a postion between two keyframes
			 * @param[in] animationTime time of animation
			 * @return position transform
			**/
		const Math::mat4 InterpolatePosition(float animationTime) const;
			/**
			 * @brief Interpolate a rotation between two keyframes
			 * @param[in] animationTime time of animation
			 * @return rotation transform
			**/
		const Math::mat4 InterpolateRotation(float animationTime) const;
			/**
			 * @brief Interpolate a scale between two keyframes
			 * @param[in] animationTime time of animation
			 * @return scale transform
			**/
		const Math::mat4 InterpolateScaling(float animationTime) const;

			/// @brief vector of position keyframes
		std::vector<KeyPosition> m_positions;
			/// @brief vector of rotation keyframes
		std::vector<KeyRotation> m_rotations;
			/// @brief vector of scale keyframes
		std::vector<KeyScale> m_scales;
			/// @brief bone name
		std::string m_name;

	};
}