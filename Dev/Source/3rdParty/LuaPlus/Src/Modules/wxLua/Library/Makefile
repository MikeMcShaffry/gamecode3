# File:		Makefile
# Author:	J Winwood
# Created:	2002
# Updated:	
# Copyright: (c) 2002 Lomtick Software. All rights reserved.
#
# Makefile for wxLua library, GTK version

LUA = ../lua-5.0.2

WXDIR      = $(shell wx-config --prefix)
WXBASENAME = $(shell wx-config --basename)
WXLIBBASE  = $(shell wx-config --basename)
WXVERSION  = $(shell wx-config --version)
WXRELEASE  = $(shell wx-config --release)

CXXFLAGS = $(shell wx-config --cxxflags) -MMD -g -Wall
LDLIBS   = $(shell wx-config --libs)
CXX      = $(shell wx-config --cxx) 

APPEXTRADEFS=-I$(LUA)/include -I$(WXDIR)/contrib/include -fexceptions -DLUACALL= 

LIBVERSION_CURRENT=1
LIBVERSION_REVISION=5
LIBVERSION_AGE=0

TARGET_LIBNAME = lib$(WXLIBBASE)_lua-$(WXRELEASE)

TARGET_LIB_LOCAL  = $(TARGET_LIBNAME)
TARGET_LIB_STATIC = $(WXDIR)/lib/$(TARGET_LIBNAME).a
TARGET_LIB_SHARED = $(WXDIR)/lib/$(TARGET_LIBNAME).so.$(LIBVERSION_CURRENT).$(LIBVERSION_REVISION).$(LIBVERSION_AGE)
TARGET_LIB_LINK1  = $(TARGET_LIBNAME).so.$(LIBVERSION_CURRENT)
TARGET_LIB_LINK2  = $(TARGET_LIBNAME).so

RM        = rm -f
AR        = ar
AROPTIONS = rcu
RANLIB    = ranlib
LN_S      = ln -s

HEADERS = \
	wxLuaCallback.h \
	wxLuaDebuggerService.h \
	wxLuaDebug.h \
	wxLuaDebugIO.h \
	wxLuaDebugServer.h \
	wxLua.h \
	wxLuaHtmlWindow.h \
	wxLuaInternals.h \
	wxLuaInterpreter.h \
	wxLuaLibrary.h \
	wxLuaPrinting.h \
	wxLuaSocket.h \
	wxLuaSplitTree.h \
	wxLuaStackTree.h

SOURCES = \
	wxLuaDebug.cpp \
	wxLuaDebuggerService.cpp \
	wxLuaDebugIO.cpp \
	wxLuaDebugServer.cpp \
	wxLuaHtmlWindow.cpp \
	wxLuaInternals.cpp \
	wxLuaInterpreter.cpp \
	wxLuaLibrary.cpp \
	wxLuaPrinting.cpp \
	wxLuaSocket.cpp \
	wxLuaSplitTree.cpp \
	wxLuaStackTree.cpp

OBJECTS=$(SOURCES:.cpp=.o)
DEPFILES=$(OBJECTS:.o=.d)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS)-o $@ $<

all:   $(TARGET_LIB_STATIC) $(TARGET_LIB_SHARED)

$(TARGET_LIB_STATIC) : $(OBJECTS)   
	@$(RM) $(TARGET_LIB_LOCAL).a
	$(AR) $(AROPTIONS) $(TARGET_LIB_LOCAL).a $(OBJECTS)
	$(RANLIB) $(TARGET_LIB_LOCAL).a
	cp $(TARGET_LIB_LOCAL).a $(TARGET_LIB_STATIC)

$(TARGET_LIB_SHARED): $(OBJECTS)
	$(CXX) -shared -o $(TARGET_LIB_LOCAL).so $(OBJECTS)
	cp $(TARGET_LIB_LOCAL).so $(TARGET_LIB_SHARED)
	cd $(WXDIR)/lib \
                && $(RM) $(TARGET_LIB_LINK1) $(TARGET_LIB_LINK2) \
                && $(LN_S) $(TARGET_LIB_SHARED) $(TARGET_LIB_LINK1) \
                && $(LN_S) $(TARGET_LIB_SHARED) $(TARGET_LIB_LINK2)

clean:
	rm -f core $(OBJECTS) $(DEPFILES) \
          $(TARGET_LIB_LOCAL).a \
          $(TARGET_LIB_LOCAL).so \
	      $(TARGET_LIB_STATIC) $(TARGET_LIB_SHARED) \
          $(WXDIR)/lib/$(TARGET_LIB_LINK1) \
          $(WXDIR)/lib/$(TARGET_LIB_LINK2) \

-include $(DEPFILES)
