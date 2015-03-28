#
# File:		makefile.unx
# Author:	J Winwood
# Created:	2002
# Updated:
# Copyright: (c) 2002. J Winwood
#
#
# Makefile for wxLua Embedded sample, Unix

LUA = ../lua-5.0.2

WXDIR = $(shell wx-config --prefix)
WXLIBDIR = $(WXDIR)/lib

WXBASENAME = $(shell wx-config --basename)
WXLIBBASE  = $(shell wx-config --basename)
WXVERSION  = $(shell wx-config --version)
WXRELEASE  = $(shell wx-config --release)

WXCXXFLAGS = $(shell wx-config --cxxflags)
WXLIBS = $(shell wx-config --libs)

CXXFLAGS = $(WXCXXFLAGS) -MMD -g -Wall -Wunused -Wunused-parameter
LDLIBS   = $(WXLIBS)
CXX      = $(shell wx-config --cxx) 

PROGRAM = Embedded

WXSTEDIT_DIR = ../../wxStEdit

WXSTEDIT_LIB = $(WXLIBBASE)_stedit-$(WXRELEASE)
WXSTC_LIB = $(WXLIBBASE)_stc-$(WXRELEASE)
LUA_LIBS  = $(LUA)/lib/liblua.a $(LUA)/lib/liblualib.a
WXLUA_LIB = $(WXLIBBASE)_lua-$(WXRELEASE)

APPEXTRADEFS=-I./ -I$(LUA)/include -I../Library -I$(WXDIR)/contrib/include -I$(WXSTEDIT_DIR)/include -DLUACALL=

# link with shared libraries
#APPEXTRALIBS=-L$(WXLIBDIR) -l$(WXSTEDIT_LIB) -l$(WXLUA_LIB) -l$(WXSTC_LIB) $(LUA_LIBS)

# link with static libraries
APPEXTRALIBS=-L$(WXLIBDIR) $(WXLIBDIR)/lib$(WXSTEDIT_LIB).a $(WXLIBDIR)/lib$(WXLUA_LIB).a $(WXLIBDIR)/lib$(WXSTC_LIB).a  $(LUA_LIBS)

OBJECTS= \
	Embedded.o \
	wxLuaEditor.o \
	wxLuaWrap.o

DEPFILES=$(OBJECTS:.o=.d)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

all: lua_wrappers wxLua wxStEdit $(PROGRAM)  


$(PROGRAM): $(OBJECTS) $(LUA_LIBS)  
	$(CXX) -o $@ $(OBJECTS) $(LDLIBS) $(APPEXTRALIBS) 

wxStEdit:
	@(cd $(WXSTEDIT_DIR)/src && make)

wxLua:
	@(cd ../Library && make)

lua_wrappers:
	make -f Makefile_import

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(PROGRAM) core

-include $(DEPFILES)
