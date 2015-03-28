if exist ..\..\..\..\Bin\LuaPlus.exe goto RELEASE
if exist ..\..\..\..\Bin\LuaPlusD.exe goto DEBUG

echo You need to compile either a debug or release version of lua.exe.
goto END

:RELEASE
  ..\..\..\..\Bin\LuaPlus -e"target=\"msw\"" wrap.lua
  echo Creating wrappers release
  goto END

:DEBUG
  ..\..\..\..\Bin\LuaPlusD -e"target=\"msw\"" wrap.lua
  echo Creating wrappers debug
  goto END

:END
