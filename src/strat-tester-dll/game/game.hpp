#pragma once

#include "utils/minlog.hpp"

namespace game
{
	extern uintptr_t base;
	extern MinLog minlog;
	extern std::unordered_map<game::dvarStrHash_t, std::string> dvarHashMap_s;

	void LoadDvarHashMap();

	template <typename T>
	class symbol
	{
	public:
		symbol(const uintptr_t address)
			: object_(reinterpret_cast<T*>(address))
		{
		}

		T* get() const
		{
			return object_;
		}

		operator T* () const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

	private:
		T* object_;
	};
	void show_error(const std::string& text, const std::string& title = "Error");
	size_t get_base();

	inline size_t relocate(const size_t val)
	{
		if (!val) return 0;

		const auto base = get_base();
		return base + (val - 0x140000000);
	}

	inline size_t derelocate(const size_t val)
	{
		if (!val) return 0;

		const auto base = get_base();
		return (val - base) + 0x140000000;
	}

	inline size_t derelocate(const void* val)
	{
		return derelocate(reinterpret_cast<size_t>(val));
	}

}

size_t operator"" _g(size_t val);
#include "symbols.hpp"