#include "cpp2lua.h"

#include <string>

void cpp2lua::dump_stack(lua_State* L)
{
	int n = lua_gettop(L);
	printf("%s", "-----lua stack-----\n");
	printf("top:%d\n", n);
	for (int i = 1; i <= n; i++)
	{
		switch (lua_type(L, i))
		{
		case LUA_TNIL:
			printf("\t%s\n", lua_typename(L, lua_type(L, i)));
			break;
		case LUA_TBOOLEAN:
			printf("\t%s    %s\n", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TLIGHTUSERDATA:
			printf("\t%s    0x%08p\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TNUMBER:
			printf("\t%s    %f\n", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("\t%s    %s\n", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
			break;
		case LUA_TTABLE:
		{
			std::string name;
			lua_pushstring(L, "__name");
			if (lua_rawget(L, i) == LUA_TSTRING)
			{
				name.assign(lua_tostring(L, -1));
				lua_remove(L, -1);
				printf("\t%s    0x%08p [%s]\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i), name.c_str());
			}
			else
			{
				lua_remove(L, -1);
				printf("\t%s    0x%08p \n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			}
		}
		break;
		case LUA_TFUNCTION:
			printf("\t%s()  0x%08p\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TUSERDATA:
			printf("\t%s    0x%08p\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TTHREAD:
			printf("\t%s\n", lua_typename(L, lua_type(L, i)));
			break;
		default:
			printf("\tunknown type\n");
		}
	}
	printf("----------------.\n");
}

void cpp2lua::dump_call_stack(lua_State* L, int n)
{
	lua_Debug ar;
	if (lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		const char* indent;
		if (n == 0)
		{
			indent = "->\t";
			printf("\t<call stack>\n");
		}
		else
		{
			indent = "\t";
		}

		if (ar.name)
			printf("%s%s() : line %d [%s : line %d]\n", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
		else
			printf("%s unknown : line %d [%s : line %d]\n", indent, ar.currentline, ar.source, ar.linedefined);

		cpp2lua::dump_call_stack(L, n + 1);
	}
}

void cpp2lua::dostring(lua_State* L, const std::string& str)
{
	dobuffer(L, str.c_str(), str.size());
}

static int on_lua_error(lua_State *L)
{
	printf("%s\n", lua_tostring(L, -1));

	cpp2lua::dump_call_stack(L, 0);

	return 0;
}

void cpp2lua::dobuffer(lua_State* L, const char* buf, size_t len)
{
	lua_pushcclosure(L, on_lua_error, 0);
	int error_index = lua_gettop(L);
	int ret = luaL_loadbuffer(L, buf, len, "cpp2lua::dobuffer");
	if (ret == LUA_OK) {
		lua_pcall(L, 0, 1, error_index);
	}
	else {
		printf("load buffer error:%d", ret);
	}

	lua_remove(L, error_index);
	lua_pop(L, 1);
}
