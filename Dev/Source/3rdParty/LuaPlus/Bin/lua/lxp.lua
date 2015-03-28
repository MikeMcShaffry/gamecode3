-- pack.lua
-- support code for pack library
-- usage lua -lpack ...

local function so(x)
	local SOPATH= os.getenv"LUA_SOPATH" or "./"
	assert(loadlib(SOPATH..x,"luaopen_"..x))()
end

so"lxp"
