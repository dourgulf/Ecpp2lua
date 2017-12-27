#include "cpp2lua.h"

#include <string>

void cpp2lua::dump_stack(lua_State* L)
{
	int n = lua_gettop(L);
	cpp2lua::print_error(L, "%s", "-----lua stack-----");
	cpp2lua::print_error(L, "top:%d", n);
	for (int i = 1; i <= n; i++)
	{
		switch (lua_type(L, i))
		{
			switch (lua_type(L, i))
			{
			case LUA_TNIL:
				print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
				break;
			case LUA_TBOOLEAN:
				print_error(L, "\t%s    %s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TLIGHTUSERDATA:
				print_error(L, "\t%s    0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TNUMBER:
				print_error(L, "\t%s    %f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				print_error(L, "\t%s    %s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
				break;
			case LUA_TTABLE:
			{
				std::string name;
				lua_pushstring(L, "__name");
				if (lua_rawget(L, i) == LUA_TSTRING)
				{
					name.assign(lua_tostring(L, -1));
					lua_remove(L, -1);
					print_error(L, "\t%s    0x%08p [%s]", lua_typename(L, lua_type(L, i)), lua_topointer(L, i), name.c_str());
				}
				else
				{
					lua_remove(L, -1);
					print_error(L, "\t%s    0x%08p ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				}
			}
			break;
			case LUA_TFUNCTION:
				print_error(L, "\t%s()  0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TUSERDATA:
				print_error(L, "\t%s    0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TTHREAD:
				print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
				break;
			}
		}
	}
	cpp2lua::print_error(L, "----------------.");
}

void cpp2lua::print_error(lua_State *L, const char* fmt, ...)
{
	char text[4096];

	va_list args;
	va_start(args, fmt);
	vsnprintf(text, sizeof(text), fmt, args);
	va_end(args);

	if (lua_getglobal(L, "_ALERT") == LUA_TFUNCTION)
	{
		lua_pushstring(L, text);
		lua_call(L, 1, 0);
	}
	else
	{
		printf("%s\n", text);
		lua_pop(L, 1);
	}
}
