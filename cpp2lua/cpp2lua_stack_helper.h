#pragma once

#ifndef CPP2LUA_STACK_HELPER_H__
#define CPP2LUA_STACK_HELPER_H__

#include "lua.hpp"
#include <assert.h>
#include <string>

namespace cpp2lua
{
	namespace helper 
	{
		struct stack_object
		{
			lua_State* m_L;
			int m_stack_pos; // absolution position
			stack_object(lua_State* L, int pos)
				:m_L(L), m_stack_pos(pos)
			{
				assert(m_stack_pos != 0);
			}
			
			// create object for the top of stack
			static stack_object from_top(lua_State* L) 
			{
				return stack_object(L, lua_gettop(L));
			}

			bool is_valid() const
			{
#ifdef _DEBUG
				assert(m_stack_pos != 0);
#endif
				return m_stack_pos != 0;
			}

			int get_type() const
			{
				if (is_valid())
				{
					return lua_type(m_L, m_stack_pos);
				}
				else
				{
					return LUA_TNONE;
				}
			}

			bool is_number() const 
			{ 
				return get_type() == LUA_TNUMBER; 
			}
			
			bool is_string() const 
			{ 
				return is_valid() && lua_isstring(m_L, m_stack_pos) == 1;
			}

			bool is_integer() const 
			{ 
				return is_valid() && lua_isinteger(m_L, m_stack_pos) == 1; 
			}

			bool is_boolean() const 
			{ 
				return is_valid() && lua_isboolean(m_L, m_stack_pos); 
			}

			bool is_table()
			{
				return is_valid()
					&& lua_istable(m_L, m_stack_pos);
			}

			int to_int(int dft=0)
			{
				if (is_integer())
				{
					return (int)lua_tointeger(m_L, m_stack_pos);
				}
				return dft;
			}

			std::string to_string(const char* dft="")
			{
				if (is_string())
				{
					return lua_tostring(m_L, m_stack_pos);
				}
				return dft;
			}
		};

		// reference a table, and iterator
		class table_iterator
		{
		public:
			table_iterator(const stack_object& table)
				: m_table(table), m_has_next(false)
			{
				lua_pushnil(table.m_L);
				m_key_index = lua_gettop(table.m_L);
				do_lua_next();
			}

			bool has_next()
			{
				return m_has_next;
			}

			void next()
			{
				if (m_has_next)
				{
					lua_pop(m_table.m_L, 1);
					do_lua_next();
				}
			}

			stack_object key() 
			{
				stack_object(m_table.m_L, m_key_index);
			}

			stack_object value()
			{
				return stack_object(m_table.m_L, m_key_index + 1);
			}

		private:
			void do_lua_next()
			{
				m_has_next = (lua_next(m_table.m_L, m_table.m_stack_pos) != 0);
			}

		private:
			bool m_has_next;
			int m_key_index = 0;
			stack_object m_table;
		};
	}
}
#endif
