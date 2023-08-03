/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "FSM.h"

namespace AEngine
{
	FSM::FSM(std::initializer_list<FSMState> states, int initialState)
		: FSM{ std::vector<FSMState>{ states }, initialState }
	{

	}

	FSM::FSM(std::vector<FSMState> states, int initialState)
		: m_graph{ states, initialState }
	{

	}

	void FSM::Init()
	{
		m_graph.Init();
	}

	void FSM::OnUpdate(TimeStep deltaTime)
	{
		int nextState = m_graph.GetCurrentState().OnUpdate(deltaTime);
		m_graph.GoToState(nextState);
	}

	bool FSM::GoToState(int nextState)
	{
		return m_graph.GoToState(nextState);
	}

	int FSM::GetCurrentState() const
	{
		return m_graph.GetCurrentStateIndex();
	}
}
