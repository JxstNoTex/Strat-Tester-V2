init()
{
}

on_player_connect()
{
}

on_player_spawned()
{
	self endon("spawned_player");

    while(true)
    {
        iPrintLnBold("Injected");
        wait 1;
    }

}