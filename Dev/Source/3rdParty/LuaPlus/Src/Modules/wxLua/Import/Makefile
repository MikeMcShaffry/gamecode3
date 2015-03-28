#
# File:		makefile.unx
# Author:	J Winwood
# Created:	2002
# Updated:	
# Copyright: (c) 2002. J Winwood
#
#
# Makefile for wxLua library, Unix

LUA=../lua-5.0.2

include wrappers.lst

# this is just a dummy file for testing making the wrappers
all: 
# wxLuaWrap.cpp

wxLuaWrap.hpp: wxLuaWrap.lua wxLuaWrap.i Overrides.hpp $(WRAPPERS)
	$(LUA)/bin/lua wxLuaWrap.lua

clean:
	rm -f wxLuaWrap.cpp
	

