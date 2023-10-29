--bookAffordance.lua
dofile ("assets/scripts/affordanceCommon.lua")

local messageAgent
local elapsedTime = 0

local State = {
	Safe = 0,
	PickedUp = 1,
	Dropped = 2,
	Thrown = 3,
	Lost = 4
}

local fsm = FSM.new({
	-- States
	FSMState.new(
		"Safe",
		{ State.PickedUp },	--transitions to
		function(dt) -- OnUpdate
			return State.Safe
		end,
		function() -- OnEntry
		end
	),

	FSMState.new(
		"PickedUp",
		{ State.Dropped },
		function(dt) -- OnUpdate
			return State.PickedUp
		end,
		function() -- OnEntry
		end
	),

	FSMState.new(
		"Dropped",
		{ State.PickedUp },
		function(dt) -- OnUpdate
			return State.Dropped
		end,
		function() -- OnEntry
		end
	),

	FSMState.new(
		"Thrown",
		{ State.PickedUp },
		function(dt) -- OnUpdate
			return State.Thrown
		end,
		function() -- OnEntry
		end
	),

	FSMState.new(
		"Lost",
		{ State.PickedUp },
		function(dt) -- OnUpdate
			return State.Lost
		end,
		function() -- OnEntry	
		end
	)},
	-- Initial State
	State.Safe
)

function OnStart()
	messageAgent = MessageService.CreateAgent(entity:GetTagComponent().ident)
	fsm:Init()

	messageAgent:RegisterMessageHandler(
		MessageType.BookInteracted,
		function(msg)
			if (fsm:GetCurrentState() == State.Safe) then
				fsm:GoToState(State.PickedUp)
				print("Book Picked up by " .. msg.sender)
			end
		end
	)
end

function OnFixedUpdate(dt)
	elapsedTime = elapsedTime + dt
	fsm:OnUpdate(dt)
end

function OnDestroy()
	messageAgent:Destroy()
end