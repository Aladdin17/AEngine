-- projectile.lua

local timeElasped = 0.0

function OnStart()
end

function OnFixedUpdate(dt)
	timeElasped = timeElasped + dt
	if(timeElasped > 10.0) then
		entity:Destroy()
	end
end

function OnDestroy()
end