-- Door.lua
dofile("assets/scripts/messaging.lua")

local messageAgent
local isOpen
local grid
local doorTransform
local doorPhysicsBody

local animating = false
local open = 0
local close = -89
local rotation = 0

-- Animate door
local function animate(dt)
    if animating then
        if isOpen then
            if rotation > close then
                rotation = rotation - 90 * dt
                doorPhysicsBody:SetRotation(AEMath.Vec3ToQuat(Vec3.new(0, rotation, 0)))
            else
                animating = false
            end
        else
            if rotation < open then
                rotation = rotation + 90 * dt
                doorPhysicsBody:SetRotation(AEMath.Vec3ToQuat(Vec3.new(0, rotation, 0)))
            else
                animating = false
            end
        end
    end
end

function OnStart()
	messageAgent = MessageService.CreateAgent(entity:GetTagComponent().ident)
	messageAgent:AddToCategory(AgentCategory.DOOR)
    messageAgent:RegisterMessageHandler(
        MessageType.OPENABLE,
        function(msg)
            if(AEMath.Length(doorTransform.translation - msg.payload.pos) < 25.0) then
                -- Door is being closed
                if isOpen then
                    isOpen = false
                    grid:ToggleObstacle("Door", false)
                    animating = true
                -- Door is being opened
                else
                    isOpen = true
                    grid:ToggleObstacle("Door", true)
                    animating = true
                end
            end
        end
    )
end

function OnFixedUpdate(dt)

    animate(dt)

    if grid == nil then
        grid = SceneManager.GetActiveScene():GetEntity("AI_Grid"):GetNavigationGridComponent()
        grid:AddObstacle(entity:GetTransformComponent().translation, "Door", 1)
        doorTransform = entity:GetTransformComponent()
        doorPhysicsBody = entity:GetPhysicsBody()
    end
end

function OnDestroy()
	messageAgent:Destroy()
end
