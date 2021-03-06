#include "../lua/callback.h"
#include "../lua/jassbind.h"
#include <base/warcraft3/jass/trampoline_function.h>
#include <Windows.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	uintptr_t jass_read(jassbind* lj, jass::variable_type vt, int idx);

	int safe_pcall (lua_State *pState, int nargs, int nresults)
	{
		int error = lua_pcall(pState, nargs, nresults, 0);
		switch (error)
		{
		case LUA_OK:
			break;
		case LUA_ERRRUN:
		case LUA_ERRMEM:
		case LUA_ERRERR:
			printf("Error(%d): %s\n", error, lua_tostring(pState, -1));
			lua_pop(pState, 1);
			break;
		default:
			printf("Error(%d)\n", error);
			break;
		}

		return error;
	}

	callback::callback()
		: ref_(0)
	{ }

	callback::callback(lua::state* ls, uint32_t index)
		: ref_(0)
	{ 
		ls->pushvalue(index);
		ref_ = luaL_ref(ls->self(), LUA_REGISTRYINDEX);
	}

	callback::callback(uint32_t ref)
		: ref_(ref)
	{ }

	bool callback::call_pre(lua::state* ls) const
	{
		ls->rawgeti(LUA_REGISTRYINDEX, ref_);
		if (!ls->isfunction(-1))
		{
			printf("callback::call() attempt to call (not a function)\n");
			ls->pop(1);
			return false;
		}

		return true;
	}

	uintptr_t callback::call(lua::state* ls, size_t param_size, jass::variable_type result_vt) const
	{
		if (safe_pcall(ls->self(), param_size, (result_vt != jass::TYPE_NOTHING) ? 1: 0) != LUA_OK)
		{
			return 0;
		}
		
		if (result_vt == jass::TYPE_NOTHING)
		{
			return 0;
		}

		uintptr_t ret = jass_read((jassbind*)ls, result_vt, -1);
		ls->pop(1);
		return ret;
	}

	uint32_t __fastcall jass_callback(uint32_t ls, uint32_t param)
	{
		callback that(param);

		if (!that.call_pre((lua::state*)ls))
		{
			return 0;
		} 
		return that.call((lua::state*)ls, 0, jass::TYPE_BOOLEAN);
	}

	uint32_t cfunction_to_code(lua::state* ls, uint32_t index)
	{
		ls->pushvalue(index);
		return jass::trampoline_create(jass_callback, (uintptr_t)ls->self(), (uintptr_t)luaL_ref(ls->self(), LUA_REGISTRYINDEX));
	}
}}}
