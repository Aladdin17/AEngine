#include "FCM.h"
#include <algorithm>
#include <stdexcept>

namespace AEngine
{
	void FCM::AddNode(
		const std::string &name,
		float initialValue,
		float activationThreshold,
		std::function<void(float)> onActivate,
		std::function<void(float)> onDeactivate)
	{
		// create the concept
		Concept conc(
			name,
			initialValue,
			activationThreshold,
			onActivate,
			onDeactivate	
		);

		// add the concept to the list
		m_concepts.push_back(conc);
		m_count++;
	}

	void FCM::AddEdge(unsigned int from, unsigned int to, float weight)
	{
		m_arcs.push_back({ from, to, weight });
	}

	void FCM::Init()
	{
		// resize and zero the weight matrix
		m_weights.resize(m_count * m_count);
		std::fill(m_weights.begin(), m_weights.end(), 0.0f);

		// set the weights, column-major order
		for (auto &arc : m_arcs)
		{
			if (arc.from >= m_count || arc.to >= m_count)
			{
				throw std::runtime_error("Invalid arc");
			}

			m_weights[arc.from * m_count + arc.to] = arc.weight;
		}

		// resize and zero activation levels 
		m_activationLevels.resize(m_count);
		m_activationLevelsLast.resize(m_count);
		std::fill(m_activationLevels.begin(), m_activationLevels.end(), 0.0f);
		std::fill(m_activationLevelsLast.begin(), m_activationLevelsLast.end(), 0.0f);

		// set initial activation levels
		for (int i = 0; i < m_count; i++)
		{
			m_activationLevels[i] = m_concepts[i].initialValue;
		}
	}

	void FCM::OnUpdate()
	{
		// make a copy of the concepts previous state (t - 1)
		m_activationLevelsLast = m_activationLevels;

		// apply matrix multiplication, then add old concept value
		for (int ai = 0; ai < m_count; ai++)
		{
			float activationDelta = 0.0f;
			for (int row = 0; row < m_count; row++)
			{
				// may be able to remove this check
				// this means we can't be connected to ourselves
				if (ai == row)
				{
					continue;
				}

				// delta += A_j * W_ji
				activationDelta += m_activationLevelsLast[row] * m_weights[row * m_count + ai];
			}

			// update the activation level
			// Ai = delta + Ai(t - 1)
			m_activationLevels[ai] = activationDelta + m_activationLevelsLast[ai];
		}

		// clamp the new activation levels
		for(float &level : m_activationLevels)
		{
			level = std::clamp(level, 0.0f, 1.0f);
		}

		// activate/deactivate concepts
		for (int i = 0; i < m_count; i++)
		{
			float level = m_activationLevels[i];
			float threshold = m_concepts[i].activationThreshold;
			bool active = m_concepts[i].active;

			// moving from inactive to active
			if (level >= threshold && !active)
			{
				m_concepts[i].onActivate(m_activationLevels[i]);
				m_concepts[i].active = true;
			}

			// moving from active to inactive
			else if (level < threshold && active)
			{
				m_concepts[i].onDeactivate(m_activationLevels[i]);
				m_concepts[i].active = false;
			}
		}
	}

	float FCM::GetConceptValue(const std::string &name) const
	{
		for (int i = 0; i < m_count; i++)
		{
			if (m_concepts[i].name == name)
			{
				return m_activationLevels[i];
			}
		}

		return -1.0f;
	}

	float FCM::GetConceptValue(unsigned int index) const
	{
		if (index >= m_count)
		{
			return -1.0f;
		}

		return m_activationLevelsLast[index];
	}

	bool FCM::SetConceptValue(const std::string &name, float value)
	{
		for (int i = 0; i < m_count; i++)
		{
			if (m_concepts[i].name == name)
			{
				m_activationLevels[i] = value;
				return true;
			}
		}

		return false;
	}

	bool FCM::SetConceptValue(unsigned int index, float value)
	{
		if (index >= m_count)
		{
			return false;
		}

		// set the value
		m_activationLevels[index] = value;
		return true;
	}

	unsigned int FCM::GetConceptCount() const
	{
		return m_count;
	}

	const std::vector<float>& FCM::GetWeights() const
	{
		return m_weights;
	}
}
