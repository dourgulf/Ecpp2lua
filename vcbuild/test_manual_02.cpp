#include <string>
#include <vector>

#include "cpp2lua.h"

static std::string join_vector(const std::vector<int>& vec, const char* sep = ",")
{
	std::string result;
	for (size_t i=0; i<vec.size(); ++i)
	{
		char buf[64];
		if (i>0)
		{
			snprintf(buf, sizeof(buf), "%s%d", sep, vec[i]);
		}
		else
		{
			snprintf(buf, sizeof(buf), "%d", vec[i]);
		}
		result += buf;
	}
	return result;
}

static int join_vector_wrapper(lua_State* L)
{
	int args = lua_gettop(L);
	if (args == 1)
	{
		lua_pushvalue(L, lua_upvalueindex(1));
	}

	std::vector<int> vec;
	lua_pushnil(L);
	int tb_index = 1;
	while (lua_next(L, tb_index) != 0)
	{
		vec.emplace_back((int)lua_tointeger(L, lua_gettop(L)));
		lua_pop(L, 1);	// remain key for next loop
	}

	const char* sep = lua_tostring(L, 2);
	std::string s = join_vector(vec, sep);
	lua_pushstring(L, s.c_str());
	return 1;
}


void test_manual_01()
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	lua_pushstring(L, ",");
	lua_pushcclosure(L, join_vector_wrapper, 1);
	lua_setglobal(L, "test");

	std::string lua_code = "a = {10,9,8}; s = test(a); print(s);";
	cpp2lua::dostring(L, lua_code);

	lua_close(L);
}
