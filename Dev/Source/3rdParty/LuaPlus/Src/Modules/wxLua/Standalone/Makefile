# File:		Makefile
# Author:	J Winwood
# Created:	2002
# Updated:	
# Copyright: (c) 2002 Lomtick Software. All rights reserved.
#
# Makefile for wxLua (GTK version)
#

LUA = ../lua-5.0.2
WXDIR = $(shell wx-config --prefix)
WXLIBDIR = $(WXDIR)/lib

WXBASENAME = $(shell wx-config --basename)
WXLIBBASE  = $(shell wx-config --basename)
WXVERSION  = $(shell wx-config --version)
WXRELEASE  = $(shell wx-config --release)

CXXFLAGS = $(shell wx-config --cxxflags) -MMD -g
LDLIBS   = $(shell wx-config --libs)
CXX      = $(shell wx-config --cxx) 

PROGRAM = wxLua

WXSTC_LIB = $(WXLIBBASE)_stc-$(WXRELEASE)
WXFL_LIB = $(WXLIBBASE)_fl-$(WXRELEASE)
LUA_LIBS  = $(LUA)/lib/liblua.a $(LUA)/lib/liblualib.a
WXLUA_LIB = $(WXLIBBASE)_lua-$(WXRELEASE)

APPEXTRADEFS=-I$(LUA)/include -I../Library -I$(WXDIR)/contrib/include -fexceptions -DLUACALL=

#link with shared libraries
#APPEXTRALIBS=-L$(WXLIBDIR) -l$(WXLUA_LIB) -l$(WXSTC_LIB) -l$(WXFL_LIB) $(LUA_LIBS)

#link with static libraries
APPEXTRALIBS=-L$(WXLIBDIR) $(WXLIBDIR)/lib$(WXLUA_LIB).a $(WXLIBDIR)/lib$(WXSTC_LIB).a  $(WXLIBDIR)/lib$(WXFL_LIB).a  $(LUA_LIBS)

OBJECTS=wxLuaApp.o wxLuaDebugTarget.o wxLuaWrap.o

DEPFILES=$(OBJECTS:.o=.d)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

all: luawrappers $(PROGRAM)  

$(PROGRAM): $(OBJECTS) $(LUA_LIBS)
	$(CXX) -o $@ $(OBJECTS) $(LDLIBS) $(APPEXTRALIBS) 

luawrappers:
	make -f Makefile_import

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(PROGRAM) core

-include $(DEPFILES)
