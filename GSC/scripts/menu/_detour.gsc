
detour zm_magicbox<scripts\zm\_zm_magicbox.gsc>::treasure_chest_ChooseWeightedRandomWeapon( player )
{				
    keys = array::randomize(getarraykeys(level.zombie_weapons));
	if(isdefined(level.customrandomweaponweights))
	{
		keys = player [[level.customrandomweaponweights]](keys);
	}
	/#
		forced_weapon_name = getdvarstring("");
		forced_weapon = getweapon(forced_weapon_name);
		if(forced_weapon_name != "" && isdefined(level.zombie_weapons[forced_weapon]))
		{
			arrayinsert(keys, forced_weapon, 0);
		}
	#/
	//pap_triggers = zm_pap_util::get_triggers();
    pap_triggers = [[@zm_pap_util<scripts\zm\_zm_pack_a_punch_util.gsc>::get_triggers ]]();
	for(i = 0; i < keys.size; i++)
	{
		if( [[@zm_magicbox<scripts\zm\_zm_magicbox.gsc>::treasure_chest_canplayerreceiveweapon ]](player, keys[i], pap_triggers) )
		{
            return getweapon("pistol_burst");
			//return keys[i];
		}
	}
	return getweapon("pistol_burst");
} 