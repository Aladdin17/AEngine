/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "FSMGraph.h"
#include "AEngine/Core/Types.h"
#include <map>
#include <sstream>
#include <stdexcept>

namespace AEngine
{
	FSMGraph::FSMGraph(std::vector<FSMState> states, int initialState)
		: m_states{ states }, m_currentState{ initialState }, m_previousState{ initialState }
	{
		if (m_states.size() == 0)
		{
			throw std::invalid_argument("FSMGraph must have at least one state");
		}
	}

	void FSMGraph::Init()
	{
		GetCurrentState().OnEntry();
	}

	FSMState& FSMGraph::GetCurrentState()
	{
		return m_states[m_currentState];
	}

	int FSMGraph::GetCurrentStateIndex() const
	{
		return m_currentState;
	}

	bool FSMGraph::GoToState(int nextState)
	{
		// don't change state if already in it
		if (nextState == m_currentState)
		{
			return false;
		}

		// check if the state transition is valid
		if (!m_states[m_currentState].HasTransition(nextState))
		{
			return false;
		}

		// detect a blip back to last state
		int next = (nextState == -1) ? m_previousState : nextState;

		// update nextStates
		m_previousState = m_currentState;
		m_currentState = next;

		m_states[m_previousState].OnExit();
		m_states[m_currentState].OnEntry();
		return true;
	}
}
