--affordanceCommon.lua

MessageType = {
	DoorOpened = 0,
	DoorClosed = 1,
	DoorInteracted = 2
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
