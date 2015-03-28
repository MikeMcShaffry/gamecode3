--Performs our initialization.

--Override the print function.
function print( printStuff )
	LuaStateManager:PrintDebugMessage( printStuff )
end

print("Hello, world!")