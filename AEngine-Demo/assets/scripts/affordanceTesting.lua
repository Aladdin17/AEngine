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
			if (elapsedTime >= 10.0) then
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
			if(elapsedTime >= 15.0) then
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

	messageAgent:RegisterMessageHandler(
		MessageType.ButtonPressed,
		function(msg)
			print("Button pressed!!!")
		end
	)

	messageAgent:RegisterMessageHandler(
		MessageType.ButtonReleased,
		function(msg)
				print("Button Released!!!")
				messageAgent:SendMessageToAgent(
					msg.sender,
					MessageType.ButtonInteracted,
					{}
			)
		end
	)

end

function OnUpdate(dt)
	elapsedTime = elapsedTime + dt
end

function OnStop()
	messageAgent:Destroy()
end