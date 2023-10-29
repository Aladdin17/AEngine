--buttonAffordance.lua
dofile ("assets/scripts/affordanceCommon.lua")

local messageAgent
local elapsedTime = 0

local State = {
	Idle = 0,
	Pressed = 1,
	Released = 2
}

local fsm = FSM.new({
	-- States
	FSMState.new(
		"Idle",
		{ State.Pressed },	--transitions to
		function(dt) -- OnUpdate
			if elapsedTime > 2.5 then
				elapsedTime = 0
				messageAgent:BroadcastMessage(
					MessageType.ButtonInteracted,
					{}
				)
			end
			return State.Pressed
		end,
		function() -- OnEntry
			elapsedTime = 0
		end
	),

	FSMState.new(
		"Pressed",
		{ State.Released },
		function(dt) -- OnUpdate
			if elapsedTime > 2.5 then
				elapsedTime = 0
				messageAgent:BroadcastMessage(
					MessageType.ButtonInteracted,
					{}
				)
			end
			return State.Released
		end,
		function() -- OnEntry
			elapsedTime = 0
		end
	),

	FSMState.new(
		"Released",
		{ State.Idle },
		function(dt) -- OnUpdate
			if elapsedTime > 2.5 then
				elapsedTime = 0
				messageAgent:BroadcastMessage(
					MessageType.ButtonInteracted,
					{}
				)
			end
			return State.Idle
		end,
		function() -- OnEntry
			elapsedTime = 0
		end
	)},
	-- Initial State
	State.Idle
)

function OnStart()
	messageAgent = MessageService.CreateAgent(entity:GetTagComponent().ident)
	fsm:Init()

	messageAgent:RegisterMessageHandler(
		MessageType.ButtonInteracted,
		function(msg)
			if (fsm:GetCurrentState() == State.Idle) then
				fsm:GoToState(State.Pressed)
				print("Button Pressed by " .. msg.sender)
			elseif (fsm:GetCurrentState() == State.Pressed) then
				fsm:GoToState(State.Released)
				print("Button Released.")
			elseif (fsm:GetCurrentState() == State.Released) then
				fsm:GoToState(State.Idle)
				print("Button Idle.")
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