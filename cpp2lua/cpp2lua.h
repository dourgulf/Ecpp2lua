#pragma once
#ifndef CPP2LUA_H__
#define CPP2LUA_H__

#include "lua.hpp"
#include "cpp2lua_stack_helper.h"

#include <string>

namespace cpp2lua
{
	// debug helpers
	void dump_stack(lua_State* L);
	void dump_call_stack(lua_State* L, int n);

	// do a lua code
	void dostring(lua_State* L, const std::string& str);
	void dobuffer(lua_State* L, const char* buf, size_t len);

	template<typename Func, typename... DefaultArgs>
	void def(lua_State* L, const char* name, Func&& func, DefaultArgs&&... defaultArgs);
};

#endif
