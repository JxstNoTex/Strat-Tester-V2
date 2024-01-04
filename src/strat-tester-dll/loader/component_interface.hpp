#pragma once

class component_interface
{
public:
	virtual ~component_interface()
	{
	}

	virtual void post_start()
	{
	}

	virtual void lua_start()
	{
	}

	virtual void pre_destroy()
	{
	}

	virtual void start_hooks()
	{
	}

	virtual void destroy_hooks()
	{
	}

	virtual void Start_frontend_hooks()
	{
	}

	virtual void db_destroy_hooks2()
	{
	}
};
