-- modify these to change the behaviour of the agaent
local turnStateRotationDegreesPerSecond = 60.0
local moveStateRotationDegreesPerSecond = 120.0
local min_speed = 40.0  -- Minimum speed
local max_speed = 80.0
local deceleration_distance = 10.0  -- Start decelerating when closer than this distance
local acceleration_distance = 20.0  -- Start accelerating when farther than this distance
local moveRotateFlag = false

local initialForward = Vec3.new(0.0, 0.0, 1.0)

----------------------------------------------------------------------------------------------------
-- internal state variables
local stateTimer
local turnDir
local turnTime
local wanderTime
local atDestination
local movingTo
local gridUpdateFlag = false

local flag = false

-- book states
local checked = false
local wasBookedThere = false
local bookPosition
local bookHomePosition = Vec3.new(573, 8, 52.5)
local holdingBook = false
local book

--testing
local atLocationA = true

local grid
local waypoints
local currentWaypoint

local messageAgent
local bookMoved = false
local bookHome

local State = {
	LAST = -1,
	IDLE = 0,
	WANDER = 1,
    MOVE = 2,
	GETBOOK = 3,
	CHECK = 4
}

function TraverseAStar(dt)
	if waypoints:Size() > 0 then

		-- Has the grid changed should we try get the waypoints again?
		if gridUpdateFlag then
			waypoints = grid:GetWaypoints(entity:GetTransformComponent().translation, movingTo, true)
			gridUpdateFlag = false
		end

		local direction = Vec3.new(waypoints[currentWaypoint], waypoints[currentWaypoint + 1], waypoints[currentWaypoint + 2]) - entity:GetTransformComponent().translation
		local distance = math.sqrt(direction.x * direction.x + direction.z * direction.z)
		local speed = entity:GetPlayerControllerComponent():GetSpeed()

		-- Deceleration
		if distance < deceleration_distance then
			local required_deceleration = (speed ^ 2) / (2 * distance)
			newSpeed = speed - required_deceleration * dt
			if newSpeed < min_speed then
				newSpeed = min_speed
			end
		end

		-- Acceleration
		if distance > acceleration_distance then
			local acceleration_factor = 5.0
			newSpeed = speed + acceleration_factor * dt

			if newSpeed > max_speed then
				newSpeed = max_speed
			end
		end

		entity:GetPlayerControllerComponent():SetSpeed(newSpeed)

		local normCurrentForward = AEMath.Normalize(AEMath.RotateVec(initialForward, entity:GetTransformComponent().orientation))
		local normDirection = AEMath.Normalize(direction)
		local dot = AEMath.Dot(normCurrentForward, normDirection)
		local cross = AEMath.Cross(normCurrentForward, normDirection)

		if dot < 0.99 then
			if cross.y < 0 then
				entity:RotateLocal(math.rad(-moveStateRotationDegreesPerSecond) * dt, Vec3.new(0.0, 1.0, 0.0))
			else
				entity:RotateLocal(math.rad(moveStateRotationDegreesPerSecond) * dt, Vec3.new(0.0, 1.0, 0.0))
			end
		end

		if distance < 0.5 then
			currentWaypoint = currentWaypoint + 3
			moveRotateFlag = true
			if currentWaypoint >= waypoints:Size() then
				atDestination = true
			end
		end

		entity:GetPlayerControllerComponent():Move(direction)
	else
		print("No waypoints found")
		atDestination = true
	end
end

----------------------------------------------------------------------------------------------------
local fsm = FSM.new({
	FSMState.new("idle",
		{ State.WANDER, State.MOVE, State.CHECK },

		-- on update
		function(dt)
			stateTimer = stateTimer + dt

            local whatToDo = math.random(0, 3)

			-- if something switch to wander
			if (whatToDo == 0) then
				return State.GCHECK
			end

			-- if something switch to turn
			if (whatToDo == 1) then
				return State.CHECK
			end

            -- if something switch to move
            if (whatToDo == 2) then
                return State.CHECK
            end

			return State.IDLE
		end,

		-- on enter
		function()
			print(entity:GetTagComponent().tag .. " is entering idle state")
			entity:GetAnimationComponent():SetAnimation("NPC.gltf/walk")
			stateTimer = 0.0
		end
	),

	FSMState.new("wander",
		{ State.IDLE, State.MOVE, State.CHECK },

		-- on update
		function(dt)

            if atDestination then

				local whatToDo = math.random(0, 3)
				
                -- if something switch to turn
                if (whatToDo == 0) then
                    --return State.CHECK
                end

                -- if something switch to idle
                if (whatToDo == 1) then
                    return State.IDLE
                end

                -- if somethign switch to move
                if (whatToDo == 2) then
                    return State.MOVE
                end
            else
				TraverseAStar(dt)
			end

			return State.WANDER
		end,

		-- on enter
		function()
			local wanderInX = math.random(20, 300)
			local wanderInZ = math.random(0, 60)
			local pos = entity:GetTransformComponent().translation
			atDestination = false
			currentWaypoint = 1
			moveRotateFlag = false
			movingTo = Vec3.new(pos.x + wanderInX, 0.0, pos.z + wanderInZ)
			waypoints = grid:GetWaypoints(pos, movingTo, false)
			print(entity:GetTagComponent().tag .. " is entering wander state")
			entity:GetAnimationComponent():SetAnimation("NPC.gltf/walk")
		end
	),

	FSMState.new("move",
		{ State.IDLE, State.WANDER, State.CHECK },
		-- on update
		function(dt)

			if atDestination then

				local whatToDo = math.random(0, 3)

				-- if something switch to turn
				if (whatToDo == 0) then
					return State.TURN
				end

				-- if something switch to idle
				if (whatToDo == 1) then
					return State.IDLE
				end

				-- if somethign switch to wander
				if (whatToDo == 2) then
					return State.WANDER
				end
				
			else
				TraverseAStar(dt)
			end

			return State.MOVE
		end,

		-- on enter
		function()
			print(entity:GetTagComponent().tag .. " is entering move state")
			moveRotateFlag = false
			atDestination = false
			currentWaypoint = 1
			if(atLocationA) then
				movingTo = Vec3.new(543.5, 0.0, 41.5)
				waypoints = grid:GetWaypoints(entity:GetTransformComponent().translation, movingTo, true)
				atLocationA = false
			else
				movingTo = Vec3.new(7.5, 0.0, 0.0)
				waypoints = grid:GetWaypoints(entity:GetTransformComponent().translation, movingTo, true)
				atLocationA = true
			end

			entity:GetAnimationComponent():SetAnimation("NPC.gltf/walk")
		end
	),

	FSMState.new("getbook",
		{ State.IDLE, State.WANDER, State.MOVE },
		-- on update
		function(dt)

			if atDestination then
				
				if(flag == false) then
					movingTo = bookHomePosition
					waypoints = grid:GetWaypoints(entity:GetTransformComponent().translation, movingTo, true)
					moveRotateFlag = false
					atDestination = false
					currentWaypoint = 1
					flag = true
					holdingBook = true
				else
					book:GetPhysicsBody():SetTranslation(bookHomePosition)
					book:GetPhysicsBody():SetLinearMomentum(Vec3.new(0, 0, 0))
					book:GetPhysicsBody():SetAngularMomentum(Vec3.new(0, 0, 0))
					holdingBook = false

					local whatToDo = math.random(0, 3)

					-- if something switch to turn
					if (whatToDo == 0) then
						return State.TURN
					end

					-- if something switch to idle
					if (whatToDo == 1) then
						return State.IDLE
					end

					-- if somethign switch to wander
					if (whatToDo == 2) then
						return State.WANDER
					end
				end			
			else
				TraverseAStar(dt)

				if holdingBook then
					book:GetPhysicsBody():SetTranslation(entity:GetPlayerControllerComponent():GetUnitDirection() * 5.0)
				end
			end

			return State.GETBOOK
		end,

		-- on enter
		function()
			print(entity:GetTagComponent().tag .. " is retrieving the book")
			moveRotateFlag = false
			atDestination = false
			currentWaypoint = 1
			flag = false
			holdingBook = false
			movingTo = bookPosition
			waypoints = grid:GetWaypoints(entity:GetTransformComponent().translation, movingTo, true)
			entity:GetAnimationComponent():SetAnimation("NPC.gltf/walk")
		end
	),

	FSMState.new("check",
		{ State.IDLE, State.WANDER, State.MOVE, State.GETBOOK },

		-- on update
		function(dt)
			if(atDestination) then

				-- Request book position
				if(flag == false) then
					messageAgent:SendMessageToCategory(
						AgentCategory.BOOK,
						MessageType.HOME,
						{}
					)

					flag = true
				end

				-- Position was received
				if checked then

					-- If book is home
					if wasBookedThere then

						-- Make teacher happy

						local whatToDo = math.random(0, 3)

						-- if something switch to turn
						if (whatToDo == 0) then
							return State.WANDER
						end
	
						-- if something switch to idle
						if (whatToDo == 1) then
							return State.MOVE
						end
	
						-- if somethign switch to wander
						if (whatToDo == 2) then
							return State.IDLE
						end

					-- If book is not home
					else
						-- Make teacher angry
						-- move teacher to book
						return State.GETBOOK
					end
				end
			else
				TraverseAStar(dt)
			end

			return State.CHECK
		end,

		-- on enter
		function()
			print(entity:GetTagComponent().tag .. " is checking on the book")
			wasBookedThere = false
			checked = false
			moveRotateFlag = false
			atDestination = false
			currentWaypoint = 1
			flag = false
			movingTo = bookHomePosition
			waypoints = grid:GetWaypoints(entity:GetTransformComponent().translation, movingTo, true)
			entity:GetAnimationComponent():SetAnimation("NPC.gltf/walk")
		end
	)},

	-- initial state
	State.IDLE
);

----------------------------------------------------------------------------------------------------
function OnStart()
	messageAgent = MessageService.CreateAgent(entity:GetTagComponent().ident)
	messageAgent:AddToCategory(AgentCategory.TEACHER)
	messageAgent:RegisterMessageHandler(
		MessageType.HOME,
		function(msg)
			checked = true
			wasBookedThere = msg.payload.isHome
			bookPosition = msg.payload.pos
		end
	)

	messageAgent:RegisterMessageHandler(
		MessageType.GRIDUPDATE,
		function (msg)
			gridUpdateFlag = true
		end
	)

	fsm:Init()
end

function OnFixedUpdate(dt)

	if grid == nil then
		grid = SceneManager.GetActiveScene():GetEntity("AI_Grid"):GetNavigationGridComponent()
	end

	if book == nil then
		book = SceneManager.GetActiveScene():GetEntity("Book")
	end

	fsm:OnUpdate(dt)
end