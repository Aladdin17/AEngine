--affordanceCommon.lua

--Affordance stuff
MessageType = {
	DoorOpened = 0,
	DoorClosed = 1,
	DoorInteracted = 2,
	ButtonIdle = 3,
	ButtonPressed = 4,
	ButtonReleased = 5,
	ButtonInteracted = 6,
	BookSafe = 7,
	BookPickedUp = 8,
	BookDropped = 9,
	BookThrown = 10,
	BookLost = 11
}

DoorData = {}
function DoorData.new()
	local instance = {}
	return instance
end

DoorInteractedData = {}
function DoorInteractedData.new()
	local instance = {}
	return instance
end

ButtonData = {}
function ButtonData.new()
	local instance = {}
	return instance
end

ButtonInteractedData = {}
function ButtonInteractedData.new()
	local instance = {}
	return instance
end

BookData = {}
function BookData.new()
	local instance = {}
	return instance
end

BookInteractedData = {}
function BookInteractedData.new()
	local instance = {}
	return instance
end