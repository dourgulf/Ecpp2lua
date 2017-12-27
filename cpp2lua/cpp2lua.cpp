#include "cpp2lua.h"

#include <string>

void cpp2lua::dump_stack(lua_State* L)
{
	int n = lua_gettop(L);
	printf("%s", "-----lua stack-----");
	printf("top:%d", n);
	for (int i = 1; i <= n; i++)
	{
		switch (lua_type(L, i))
		{
			switch (lua_type(L, i))
			{
			case LUA_TNIL:
				printf("\t%s", lua_typename(L, lua_type(L, i)));
				break;
			case LUA_TBOOLEAN:
				printf("\t%s    %s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TLIGHTUSERDATA:
				printf("\t%s    0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TNUMBER:
				printf("\t%s    %f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				printf("\t%s    %s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
				break;
			case LUA_TTABLE:
			{
				std::string name;
				lua_pushstring(L, "__name");
				if (lua_rawget(L, i) == LUA_TSTRING)
				{
					name.assign(lua_tostring(L, -1));
					lua_remove(L, -1);
					printf("\t%s    0x%08p [%s]", lua_typename(L, lua_type(L, i)), lua_topointer(L, i), name.c_str());
				}
				else
				{
					lua_remove(L, -1);
					printf("\t%s    0x%08p ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				}
			}
			break;
			case LUA_TFUNCTION:
				printf("\t%s()  0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TUSERDATA:
				printf("\t%s    0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TTHREAD:
				printf("\t%s", lua_typename(L, lua_type(L, i)));
				break;
			}
		}
	}
	printf("----------------.");
}
