if exist ..\release\lua.exe goto RELEASE
if exist ..\debug\lua.exe goto DEBUG

echo You need to compile either a debug or release version of lua.exe.
goto END

:RELEASE
  ..\release\lua -e"target=\"msw\"" wrap.lua
  echo Creating wrappers release
  goto END

:DEBUG
  ..\debug\lua -e"target=\"msw\"" wrap.lua
  echo Creating wrappers debug
  goto END

:END
