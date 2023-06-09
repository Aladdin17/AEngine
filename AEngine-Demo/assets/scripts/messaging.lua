-- message.lua
AgentCategory = {
	ENEMY = 0,
	PLAYER = 1,
	SUPPLIES = 2
}

MessageType = {
	POSITION = 0,
	DAMAGE = 1,
	SPOTTED = 2,
	AREA_DAMAGE = 3,
	KILLED = 4,
	PICKUP = 5
}

Position_Data = {}
function Position_Data.new(pos)
	local instance = {}
	instance.pos = pos
	return instance
end

Damage_Data = {}
function Damage_Data.new(amount)
	local instance = {}
	instance.amount = amount
	return instance
end

Spotted_Data = {}
function Spotted_Data.new(targetPos, spotterPos)
	local instance = {}
	instance.targetPos = targetPos
	instance.spotterPos = spotterPos
	return instance
end

AreaDamage_Data = {}
function AreaDamage_Data.new(amount, radius, pos)
	local instance = {}
	instance.amount = amount
	instance.radius = radius
	instance.pos = pos
	return instance
end
