if not luacom then
  local SOPATH= os.getenv"LUA_SOPATH" or "./"
  local init, err1, err2 = loadlib(SOPATH.."luacom","luacom_openlib")
  assert (init, (err1 or '')..(err2 or ''))
  init()
end

return nil
