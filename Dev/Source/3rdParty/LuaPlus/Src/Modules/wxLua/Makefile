LUA = lua-5.0.2
WXLUA_DIRS = Import Library Standalone Embedded

all:
	@for d in $(LUA); do (cd $$d && $(MAKE) ); done
	@for d in $(WXLUA_DIRS); do (cd $$d && $(MAKE)); done

clean:
	@for d in $(LUA); do (cd $$d && $(MAKE) clean); done
	@for d in $(WXLUA_DIRS); do (cd $$d && $(MAKE) clean); done

rebuild: clean all

