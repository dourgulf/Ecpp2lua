打算写一个系列文章，讲述C++和lua如何协同工作。
从中可以看到C++1z的一系列的新的语言功能是怎么使得这个协同工作更加“有味道”的。

这是一个系列文章，一步一步引入C++1z有趣的东西。
文章有一个对应的[GitHub项目](https://github.com/dourgulf/Ecpp2lua.git)

# 第一节：原始的协同方式

### 首先，我们看看如何手工编写代码导出C++函数到LUA？

1. 假设我们有这样的一个C++的函数：

```c++
std::string join_vector(const std::vector<int>& vec, const char* sep = ",")
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
```

这个函数将一个整数数组连接成一个字符串，sep参数是分隔符，默认参数是逗号（,）。

2. 首先需要用一个标准的函数封装它：

```C++
int join_vector_wrapper(lua_State* L)
{
	// 从lua的栈获得参数
	// 调用join_vector计算结果
	// 把结果入栈
	return 1;
}
```

3. 然后，给它关联一个名字，设置到lua全局的名字空间

```c++
void test_manual(lua_State* L)
{
  	// 默认参数，加入到闭包的upvalue
	lua_pushstring(L, ",");
	lua_pushcclosure(L, join_vector_wrapper, 1);
	lua_setglobal(L, "test");
}
```

4. 最后写个测试main函数，执行一段lua脚本，跑起来看看。
```
int main(void)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	test_manual(L);

	std::string lua_code = "a = {10,9,8}; s = test(a); print(s);";
	cpp2lua::dostring(L, lua_code);


	lua_close(L);
	return 0;
}
```

现在回头看看`join_vector_wrapper`的具体实现：

```C++
int join_vector_wrapper(lua_State* L)
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
```

包装函数首先判断一下lua传进来的参数个数，如果只有1个参数，则意味着，没有传递sep参数。
这时候，我们就可以从闭包的upvalue中获取默认参数，并把默认参数入栈。
经过这样的处理之后，不管lua代码以后没有传sep参数，后续的处理都可以当时参数完整的情况了。

接下来是声明一个std::vector，遍历lua的table，复制到数组；并且，获得sep参数。
最后调用join_vector，然后把结果入栈。

再补充`cpp2lua::dostring`的实现如下：
```C++
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
```

### 考虑到table在lua中是如此的重要，我们建立一个辅助的类来遍历lua的table。
```C++
		struct stack_object
		{
			lua_State* m_L;
			int m_stack_pos; // absolution position
			int to_integer();
		};

		class table_iterator
		{
		public:
			table_iterator(const stack_object& table);
			bool has_next();
			void next();
			stack_object key();
			stack_object value();
		};
```
>这个类的具体实现可以阅读cpp2lua_stack_helper.h

使用这个辅助的类之后`join_vector_wrapper`变成这样了：
```C++
int join_vector_wrapper(lua_State* L)
{
	int args = lua_gettop(L);
	if (args == 1)
	{
		lua_pushvalue(L, lua_upvalueindex(1));
	}

	using namespace cpp2lua::helper;
	table_iterator table(stack_object(L, 1));
	std::vector<int> vec;
	while (table.has_next())
	{
		vec.emplace_back(table.value().to_integer());
		table.next();
	}

	const char* sep = lua_tostring(L, 2);
	std::string s = join_vector(vec, sep);
	lua_pushstring(L, s.c_str());
	return 1;
}
```

以上，就是手写导出一个C++函数到lua的全过程，包括执行一段测试代码验证我们的导出结果。
> 完整代码在test_manual_01.cpp

