--doorAffordance.lua
dofile ("assets/scripts/affordanceCommon.lua")

local messageAgent
local elapsedTime = 0

local State = {
	Opened = 0,
	Closed = 1
}

local fsm = FSM.new({
	-- States
	FSMState.new(
		"Opened",
		{ State.Closed },	--transitions to
		function(dt) -- OnUpdate
			if elapsedTime > 2.5 then
				elapsedTime = 0
				print("Door would like to be closed!!!")
				messageAgent:BroadcastMessage(
					MessageType.DoorOpened,
					{}
				)
			end
			return State.Opened
		end,
		function() -- OnEntry
			elapsedTime = 0
		end
	),

	FSMState.new(
		"Closed",
		{ State.Opened },
		function(dt) -- OnUpdate
			if elapsedTime > 2.5 then
				elapsedTime = 0
				print("Door would like to be opened!!!")
				messageAgent:BroadcastMessage(
					MessageType.DoorClosed,
					{}
				)
			end
			return State.Closed
		end,
		function() -- OnEntry
			elapsedTime = 0
		end
	)},
	-- Initial State
	State.Closed
)

function OnStart()
	messageAgent = MessageService.CreateAgent(entity:GetTagComponent().ident)
	fsm:Init()

	messageAgent:RegisterMessageHandler(
		MessageType.DoorInteracted,
		function(msg)
			if (fsm:GetCurrentState() == State.Opened) then
				fsm:GoToState(State.Closed)
				print("Door closed by " .. msg.sender)
			else
				fsm:GoToState(State.Opened)
				print("Door opened by " .. msg.sender)
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
