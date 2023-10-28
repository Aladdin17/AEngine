--affordanceTesting.lua

dofile ("assets/scripts/affordanceCommon.lua")

local messageAgent
local elapsedTime = 0

function OnStart()
	messageAgent = MessageService.CreateAgent(entity:GetTagComponent().ident)

	-- Register message handlers
	messageAgent:RegisterMessageHandler(
		MessageType.DoorOpened,
		function(msg)
			if (elapsedTime >= 3.0) then
				elapsedTime = 0.0
				print("Door opened!!!")
				messageAgent:SendMessageToAgent(
					msg.sender,
					MessageType.DoorInteracted,
					{}
				)
			end
		end
	)

	messageAgent:RegisterMessageHandler(
		MessageType.DoorClosed,
		function(msg)
			if(elapsedTime >= 5.0) then
				elapsedTime = 0.0
				print("Door closed!!!")
				messageAgent:SendMessageToAgent(
					msg.sender,
					MessageType.DoorInteracted,
					{}
				)
			end
		end
	)
end

function OnUpdate(dt)
	elapsedTime = elapsedTime + dt
end

function OnStop()
	messageAgent:Destroy()
end
