-- patrol.lua
--[[
author:
	Christien Alden (34119981)

purpose:
	Provides a basic patrol behaviour to entities; they will start in a wander state until they
	see the player, at which point they will seek the player. If the player gets too far away,
	they will return to the idle state for a short period of time before returning to the wander
	state.
--]]

-- load the messaging 'header'
dofile("assets/scripts/messaging.lua")

-- modify these to change the behaviour of the agaent
local idleTime = 10.0
local rotationDegreesPerSecond = 60.0
local seekDistanceStart = 80.0
local seekDistanceStop = 200.0
local attackRange = 10.0
local attackDamage = 10.0
local viewConeAngleDegrees = 60.0
local radioRange = 150.0
local health = 15
local resistance = 0.50

----------------------------------------------------------------------------------------------------
-- internal state variables
local messageAgent
local stateTimer
local turnDir
local turnTime
local wanderTime
local targetPosition
local entityTag
local trackPosition
local messageCooldown = 0.0
local animDuration = 0.0
local animTimer = 0.0

local State = {
	LAST = -1,
	SEEK = 0,
	WANDER = 1,
	TURN = 2,
	TRACK = 3,
	ATTACK = 4,
	DEATH = 5
}

----------------------------------------------------------------------------------------------------
local function GetVectorToPlayer()
	if (targetPosition == nil) then
		return nil
	end

	return targetPosition - entity:GetTransformComponent().translation
end

local function DistanceBetweenTwoVectors(v1, v2)
	return AEMath.Length(v1 - v2)
end

local function CalculateAngleBetweenVectors(v1, v2)
	local dot = AEMath.Dot(v1, v2)
	return math.acos(dot / (AEMath.Length(v1) * AEMath.Length(v2)))
end

local function PlayerDetected()
	-- calculate distance and angle to player
	local targetVec = GetVectorToPlayer()
	local targetDist = AEMath.Length(targetVec)
	--local actualAngle = CalculateAngleBetweenVectors(targetVec, -entity:GetTransformComponent():LocalZ())
	--local maxAngle = math.rad(viewConeAngleDegrees) / 2.0

	-- if within range and within view cone, return true
	if (targetDist <= seekDistanceStart) then
		return true
	end

	return false
end

local function RadioTeammates()
	-- send a spotted message to all enemies
	messageAgent:SendMessageToCategory(
		AgentCategory.ENEMY,
		MessageType.SPOTTED,
		Spotted_Data.new(Vec3.new(targetPosition), Vec3.new(entity:GetTransformComponent().translation))
	)
end

----------------------------------------------------------------------------------------------------
local fsm = FSM.new({
	FSMState.new("seek",
		{ State.WANDER, State.ATTACK, State.DEATH },

		-- on update
		function(dt)
			stateTimer = stateTimer + dt

			-- if the target is far enough, switch to idle state
			local targetVec = GetVectorToPlayer()
			if (AEMath.Length(targetVec) >= seekDistanceStop) then
				return State.WANDER
			end

			-- apply damage if player is close enough to attack
			if (AEMath.Length(targetVec) <= attackRange) then
				return State.ATTACK
			end

			-- send a spotted message to all enemies approx. every 5 seconds
			if (messageCooldown >= 10.0) then
				RadioTeammates()
				messageCooldown = 0.0
			end

			-- continue seeking
			entity:GetTransformComponent():LookAt(targetPosition)
			entity:GetPlayerControllerComponent():Move(AEMath.Normalize(targetPosition - entity:GetTransformComponent().translation))
			return State.SEEK
		end,

		-- on enter
		function()
			entity:GetAnimationComponent():SetAnimation("walk.dae")
			stateTimer = 0.0
		end
	),

	FSMState.new("wander",
		{ State.SEEK, State.TURN, State.DEATH },

		-- on update
		function(dt)
			stateTimer = stateTimer + dt

			-- if the player is detected, alert enemies and switch to seek state
			if (PlayerDetected()) then
				RadioTeammates()
				return State.SEEK
			end

			-- rotate 90 degrees every 5 seconds
			if (stateTimer > wanderTime) then
				return State.TURN
			end

			-- wander a little
			local direction = AEMath.RotateVec(Vec3.new(0.0, 0.0, -1.0), entity:GetTransformComponent().orientation)
			entity:GetPlayerControllerComponent():Move(direction)

			return State.WANDER
		end,

		-- on enter
		function()
			entity:GetAnimationComponent():SetAnimation("walk.dae")
			stateTimer = 0.0
			wanderTime = math.random(1, 5)
		end
	),

	FSMState.new("turn",
		{ State.LAST, State.DEATH },

		-- on update
		function(dt)
			stateTimer = stateTimer + dt
			if (stateTimer >= turnTime) then
				return State.LAST
			end

			if (turnDir > 0.5) then
				entity:RotateLocal(math.rad(rotationDegreesPerSecond) * dt, Vec3.new(0.0, 1.0, 0.0))
			else
				entity:RotateLocal(math.rad(-rotationDegreesPerSecond) * dt, Vec3.new(0.0, 1.0, 0.0))
			end

			return State.TURN
		end,

		-- on enter
		function()
			entity:GetAnimationComponent():SetAnimation("walk.dae")
			stateTimer = 0.0
			turnDir = math.random(0, 1)
			turnTime = math.random(1, 5)
		end
	),

	FSMState.new("track",
		{ State.WANDER, State.SEEK, State.DEATH },

		function(dt)
			stateTimer = stateTimer + dt

			if (PlayerDetected()) then
				RadioTeammates()
				return State.SEEK
			end

			-- if the player has not been seen, continue to the last position known
			local positionVec = trackPosition - entity:GetTransformComponent().translation
			if (AEMath.Length(positionVec) <= 5.0) then
				return State.WANDER
			end

			entity:GetTransformComponent():LookAt(trackPosition)
			entity:GetPlayerControllerComponent():Move(AEMath.Normalize(positionVec))
			return State.TRACK
		end,

		-- on enter
		function()
			stateTimer = 0.0
		end
	),

	FSMState.new("attack",
		{ State.SEEK, State.DEATH },

		-- on update
		function(dt)
			stateTimer = stateTimer + dt

			-- once the animation has played, apply damage
			if (stateTimer >= animDuration) then
				return State.SEEK
			end

			return State.ATTACK
		end,

		function()
			entity:GetAnimationComponent():SetAnimation("attack.dae")
			-- hack to fix timing issue
			animDuration = (entity:GetAnimationComponent():GetDuration() / 3.0) * 0.95
			stateTimer = 0.0
		end,

		function()
			-- apply damage when leaving state
			messageAgent:SendMessageToCategory(
				AgentCategory.PLAYER,
				MessageType.DAMAGE,
				Damage_Data.new(attackDamage)
			)
		end
	),

	FSMState.new("death",
		{ },

		function(dt)
			stateTimer = stateTimer + dt
			-- play animation then die
			if (stateTimer >= animDuration) then
				-- destroy
				messageAgent:Destroy()
				entity:Destroy()
				return State.DEATH
			end

			return State.DEATH
		end,

		function()
			print("entered death state")
			entity:GetAnimationComponent():SetAnimation("death.dae")
			-- hack to fix timing issue
			animDuration = (entity:GetAnimationComponent():GetDuration() / 3.0) * 0.95
			print(animDuration)
			stateTimer = 0.0
		end
	)},

	-- initial state
	State.WANDER
);

----------------------------------------------------------------------------------------------------
function OnStart()
	entityTag = entity:GetTagComponent().tag

	fsm:Init()
	messageAgent = MessageService.CreateAgent(entity:GetTagComponent().ident)
	messageAgent:AddToCategory(AgentCategory.ENEMY)
	messageAgent:RegisterMessageHandler(
		MessageType.POSITION,
		function(msg)
			targetPosition = msg.payload.pos
		end
	)

	messageAgent:RegisterMessageHandler(
		MessageType.SPOTTED,
		function(msg)
			print("received spotted player")
			-- drop the message if the enemy is seeking or idle
			local currentState = fsm:GetCurrentState()
			if ((currentState == State.SEEK) or (currentState == State.ATTACK) or (currentState == State.DEATH)) then
				return
			end

			-- drop the message if the sender is too far away, trying to mimic a radio range
			if (DistanceBetweenTwoVectors(msg.payload.spotterPos, entity:GetTransformComponent().translation) > radioRange) then
				return
			end

			-- record the position and switch to track state
			trackPosition = msg.payload.targetPos
			fsm:GoToState(State.TRACK)
		end
	)

	messageAgent:RegisterMessageHandler(
		MessageType.AREA_DAMAGE,
		function(msg)
			if (DistanceBetweenTwoVectors(msg.payload.pos, entity:GetTransformComponent().translation) <= msg.payload.radius) then
				health = health - (msg.payload.amount * resistance)
			end

			-- if enough damage has been taken
			if (health <= 0) then
				-- go to state as lon
				fsm:GoToState(State.DEATH)

				messageAgent:SendMessageToAgent(
					msg.sender,
					MessageType.KILLED,
					{}
				)

				-- alert teammates to power up
				messageAgent:SendMessageToCategory(
					AgentCategory.ENEMY,
					MessageType.POWER_UP,
					PowerUp_Data.new(1.25)
				)

				messageAgent:Destroy()
				return
			end

			-- go into seek state if not already in it
			if (fsm:GetCurrentState() ~= State.ATTACK) then
				fsm:GoToState(State.SEEK)
			end
		end
	)

	messageAgent:RegisterMessageHandler(
		MessageType.POWER_UP,
		function(msg)
			resistance = resistance * msg.payload.factor
		end
	)

	messageAgent:RegisterMessageHandler(
		MessageType.KILLED,
		function(msg)
			-- set position to a huge value, bit of a hack
			targetPosition = Vec3.new(10000.0, 10000.0, 10000.0)
			fsm:GoToState(State.WANDER)
		end
	)
end

function OnFixedUpdate(dt)
	messageCooldown = messageCooldown + dt
	fsm:OnUpdate(dt)
end

function OnDestroy()
	messageAgent:Destroy()
end
