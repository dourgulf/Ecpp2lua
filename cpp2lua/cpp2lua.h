#pragma once
#ifndef CPP2LUA_H__
#define CPP2LUA_H__

#include "lua.hpp"

#include <string>

namespace cpp2lua
{
	// debug helpers
	void dump_stack(lua_State* L);
	void dump_call_stack(lua_State* L, int n);

	// do a lua code
	void dostring(lua_State* L, const std::string& str);
	void dobuffer(lua_State* L, const char* buf, size_t len);
};

#endif
