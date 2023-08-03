/**
 * \file
 * \author Christien Alden (34119981)
*/
#pragma once
#include "AEngine/Core/TimeStep.h"
#include "FSMGraph.h"
#include "FSMState.h"
#include <initializer_list>

namespace AEngine
{
		/**
		 * \class FSM
		 * \brief A finite state machine
		 * \details
		 * This object takes a either a vector of states or an initializer list of states and constructs a finite state machine from them.
		 * When OnUpdate() is called, the current states FSMState::OnUpdate() is called, handling all the transitions and state changes.
		*/
	class FSM
	{
	public:
		FSM() = default;
			/**
			 * \param[in] states The states in the FSM
			 * \param[in] initialState The initial state
			 * \note The \p initialState parameter is optional and defaults to 0
			 * \throw std::invalid_argument if the states vector is empty
			*/
		FSM(std::initializer_list<FSMState> states, int initialState = 0);
			/**
			 * \copydoc FSM::FSM(std::initializer_list<FSMState>, int)
			*/
		FSM(std::vector<FSMState> states, int initialState = 0);
		~FSM() = default;

			/**
			 * \brief Runs the FSMState::OnEntry() function of the initial state
			*/
		void Init();
			/**
			 * \brief Updates the FSM object
			 * \param[in] deltaTime for this frame
			*/
		void OnUpdate(TimeStep deltaTime);
			/**
			 * \brief Go to the given state
			 * \param[in] state The state to go to
			 * \retval True The state was changed
			 * \retval False The state was not changed
			*/
		bool GoToState(int nextState);
			/**
			 * \brief Get the current state
			 * \return The current state
			*/
		int GetCurrentState() const;

	private:
			/**
			 * \brief Holds the state switching logic
			*/
		FSMGraph m_graph;
	};
}
