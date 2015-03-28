--Register all the event types we'll be using.
EventManager:RegisterEventType( "xyzzy" );

--Set up a listener function.
function MyFirstListener( dataBag )
	LuaStateManager:PrintDebugMessage( "Got some data!" )
	return true
end

--Now register this listener for an event type.
EventManager:AddScriptListener( "new_game", MyFirstListener )

--Listen for a script-based event.
function MyScriptBasedListener( dataBag )
	local outputString = ""

	if ( nil == dataBag ) then
		outputString = "GOT NIL EVENT DATA!"
	elseif ( nil == dataBag[ "EventValue" ] ) then
		outputString = "GOT EVENT DATA, BUT MISSING THE 'EventValue' MEMBER!"
	else
		local origValue = dataBag[ "EventValue" ]
		
		--Convert to string.
		local finalVal = tostring( origValue )
		outputString = "EVENT DATA: " .. finalVal
	end
	
	print( outputString )
end

EventManager:AddScriptListener( "xyzzy", MyScriptBasedListener )

--Now trigger some made up event.
EventManager:TriggerEvent( "xyzzy", { EventValue = "HELLO, WORLD!" } )
EventManager:RemoveScriptListener( "xyzzy", MyScriptBasedListener )

--We want to listen for new actor events.
function NewActorListener( dataBag )
	local outputString = "A new actor was created!"
	if ( nil ~= dataBag ) then
		local actorID = dataBag[ "ActorId" ]
		if ( nil ~= actorID ) then
			outputString = outputString .. "\n\tACTOR ID: " .. tostring( actorID )
			local actorInfo = ActorList[ actorID ]
			if ( nil ~= actorInfo ) then
				outputString = outputString .. "\n\tOnCreateFunc: "
				local onCreateFuncName = actorInfo[ "OnCreateFunc" ]
				if ( nil ~= onCreateFuncName ) then
					outputString = outputString .. onCreateFuncName
				else
					outputString = outputString .. "NONE"
				end
				outputString = outputString .. "\n\tOnDestroyFunc: "
				local onDestroyFuncName = actorInfo[ "OnDestroyFunc" ]
				if ( nil ~= onDestroyFuncName ) then
					outputString = outputString .. onDestroyFuncName
				else
					outputString = outputString .. "NONE"
				end
			end
		end
	end
	
	print( outputString )
end
	
EventManager:AddScriptListener( "new_actor", NewActorListener )

--Now let's rez up a glowing grid!

--These are the actor parameters.
local gridParams =
{
	ActorType = "grid",
	Color =
	{
		R = 0.4,
		G = 0.4,
		B = 0.4,
		A = 1.0,
	},
	Texture = "grid.dds",
	Squares = 100,
	OnCreateFunc = "GlowingGridOnCreate",
	--OnDestroyFunc = "GlowingGridOnDestroy",
}
--These are the functions related to this actor.
function GlowingGridOnCreate( actorID, actorDataTable )
	print( "GLOWING GRID CREATED!" )
	
	--Kick off the glowing grid.  Set global data.
	actorDataTable.GlowingGridTimer = 0.0
	
	--Begin letting *this actor* listen for the update tick.
	EventManager:AddScriptActorListener( "update_tick", GlowingGridUpdate, actorID )
	
	--Set the grid colors in our local actor data.
	actorDataTable.StartColor = {}
	actorDataTable.DestColor =
	{
		R = 0.4,
		G = 0.4,
		B = 0.4,
		A = 1.0,
	}
	
	--And our constant timer value.
	actorDataTable.kMaxTimerValue = 1.0
end

function GlowingGridOnDestroy( actorID, actorDataTable )
	print( "GLOWING GRID DESTROYED!" )
	
	--Stop listening!
	EventManager:RemoveScriptActorListener( "update_tick", GlowingGridUpdate, actorID )
end

function GlowingGridUpdate( eventData, actorData )
	local dt = eventData[ "Seconds" ]
	actorData.GlowingGridTimer = actorData.GlowingGridTimer - dt
	if ( actorData.GlowingGridTimer < 0 ) then
		--Reset the timer and pick a new dest/src color.
		actorData.GlowingGridTimer = actorData.kMaxTimerValue
		
		local newRed = math.random()
		local newGreen = math.random()
		local newBlue = math.random()
		local newAlpha = math.random()
		
		--Swap destination for current color.
		actorData.StartColor[ "R" ] = actorData.DestColor[ "R" ]
		actorData.StartColor[ "G" ] = actorData.DestColor[ "G" ]
		actorData.StartColor[ "B" ] = actorData.DestColor[ "B" ]
		actorData.StartColor[ "A" ] = actorData.DestColor[ "A" ]
		
		--Set new dest color.
		actorData.DestColor[ "R" ] = newRed
		actorData.DestColor[ "G" ] = newGreen
		actorData.DestColor[ "B" ] = newBlue
		actorData.DestColor[ "A" ] = newAlpha
	else
		--Keep moving the current color towards the destination.
		--We'll use a linear interpolation.
		local percent = 1.0 - ( actorData.GlowingGridTimer / actorData.kMaxTimerValue )
		local deltaR = actorData.DestColor[ "R" ] - actorData.StartColor[ "R" ]
		local deltaG = actorData.DestColor[ "G" ] - actorData.StartColor[ "G" ]
		local deltaB = actorData.DestColor[ "B" ] - actorData.StartColor[ "B" ]
		local deltaA = actorData.DestColor[ "A" ] - actorData.StartColor[ "A" ]
		local finalR = actorData.StartColor[ "R" ] + ( deltaR * percent )
		local finalG = actorData.StartColor[ "G" ] + ( deltaG * percent )
		local finalB = actorData.StartColor[ "B" ] + ( deltaB * percent )
		local finalA = actorData.StartColor[ "A" ] + ( deltaA * percent )
		
		local gridColorParams =
		{
			ActorID = actorData.ActorID,
		
			Color =
			{
				R = finalR,
				G = finalG,
				B = finalB,
				A = finalA,
			},
		}
		
		EventManager:TriggerEvent( "update_actor_params", gridColorParams )
	end
end

--Now that everything is in place, let's create the actor.
EventManager:TriggerEvent( "request_new_actor", gridParams )
