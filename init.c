
void main()
{

	//INIT WEATHER BEFORE ECONOMY INIT------------------------
	Weather weather = g_Game.GetWeather();

    weather.MissionWeather(false);    // false = use weather controller from Weather.c

    weather.GetOvercast().Set( Math.RandomFloatInclusive(0.4, 0.6), 1, 0);
    weather.GetRain().Set( 0, 0, 1);
    weather.GetFog().Set( Math.RandomFloatInclusive(0.05, 0.1), 1, 0);


	Hive ce = CreateHive();
	if ( ce )
		ce.InitOffline();


	int year = 2018; // Year to Set
	int month = 6; //Month to Set
	int day = 21; //Day to Set
	int hour = 4; // Hour to Set
	int minute = 30; // Minute to Set
	GetGame().GetWorld().SetDate(year, month, day, hour, minute);
}

class CustomMission: MissionServer
{	
	void SetRandomHealth(EntityAI itemEnt)
	{
		int rndHlt = Math.RandomInt(40,100);
		itemEnt.SetHealth("","",rndHlt);
	}

	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt;
		playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");//Creates random player
		Class.CastTo(m_player, playerEnt);
		
		GetGame().SelectPlayer(identity, m_player);
		
		return m_player;
	}
	
	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		player.RemoveAllItems();

		EntityAI itemTop;
		EntityAI primary;
		EntityAI itemEnt;
		ItemBase itemBs;
		EntityAI axe;
		EntityAI rags;
		float rand;
		int testint = 0;
		switch (Math.RandomInt(0, 9)) { 
		case 0:
			// Soldier
			player.GetInventory().CreateInInventory("TTSKOPants");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("TShirt_Green");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("CombatBoots_Black");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("CombatKnife");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("BallisticHelmet_UN");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 1:
			// Paramedic
			player.GetInventory().CreateInInventory("ParamedicPants_Green");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("ParamedicJacket_Green");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("SalineBagIV");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("BandageDressing");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 2:
			// Office worker
			player.GetInventory().CreateInInventory("SlacksPants_Black");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("WoolCoat_Black");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("AthleticShoes_Black");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Paper");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("SodaCan_Cola");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("ThinFramesGlasses");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 3:
			// Biker
			player.GetInventory().CreateInInventory("Jeans_Black");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("RidersJacket_Black");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("MotoHelmet_Red");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("HikingBootsLow_Black");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Matchbox");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Pipe");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 4:
			// Hiker
			player.GetInventory().CreateInInventory("HikingJacket_Red");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("CargoPants_Blue");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("HikingBootsLow_Blue");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("MountainBag_Blue");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Compass");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("WaterBottle");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 5:
			// Cop
			player.GetInventory().CreateInInventory("PoliceJacket");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("PolicePants");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("CombatBoots_Grey");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Flashlight");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Battery9V");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("PersonalRadio");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Apple");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 6:
			// Lumberjack
			player.GetInventory().CreateInInventory("Shirt_RedCheck");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Jeans_Blue");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("WoodAxe");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("WorkingBoots_Brown");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Ushanka_Green");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 7:
			// Doctor/Surgeon
			player.GetInventory().CreateInInventory("MedicalScrubsPants_Blue");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("LabCoat");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("WorkingBoots_Yellow");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("MedicalScrubsHat_Blue");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("Epinephrine");itemBs = ItemBase.Cast(itemEnt);
			// player.GetInventory().CreateInInventory("MedicalScrubsShirt_Blue");itemBs = ItemBase.Cast(itemEnt);
			// player.GetInventory().CreateInInventory("FirstAidKit");itemBs = ItemBase.Cast(itemEnt);
			break;
		case 8:
			// Fireman
			player.GetInventory().CreateInInventory("FirefighterJacket_Beige");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("FirefightersPants_Beige");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("FirefightersHelmet_White");itemBs = ItemBase.Cast(itemEnt);
			player.GetInventory().CreateInInventory("WorkingBoots_Yellow");itemBs = ItemBase.Cast(itemEnt);
			// player.GetInventory().CreateInInventory("FirefighterAxe");itemBs = ItemBase.Cast(itemEnt);
			break;
		}
		//Give basic loadout:
		// primary = BasicLoadoutClass(player);
		int testWeaponInt = 3;
		switch (Math.RandomInt(0, 4)) {
			case 0: 
				primary = assaultClass(player);itemBs = ItemBase.Cast(primary);
				break;
			case 1: 
				primary = sniperClass(player);itemBs = ItemBase.Cast(primary);
				break;
			case 2: 
				primary = smgClass(player);itemBs = ItemBase.Cast(primary);
				break;
			case 3: 
				primary = BasicLoadoutClass(player);itemBs = ItemBase.Cast(primary);
				break;
			// case else:
				// primary = BasicLoadoutClass(player);itemBs = ItemBase.Cast(primary);
				// break;
		}

	//   Give universal gear
		switch (Math.RandomInt(0,3)) {
			case 0: 
				itemEnt = player.GetInventory().CreateInInventory("Torch");itemBs = ItemBase.Cast(itemEnt);
				break;
			case 1: 
				itemEnt = player.GetInventory().CreateInInventory("Battery9V");itemBs = ItemBase.Cast(itemEnt);
				break;
			case 2:
				itemEnt = player.GetInventory().CreateInInventory("BatteryCharger");itemBs = ItemBase.Cast(itemEnt);
				break;
		}
		axe = player.GetInventory().CreateInInventory("FirefighterAxe");
		itemEnt = player.GetInventory().CreateInInventory("Compass");itemBs = ItemBase.Cast(itemEnt);
		itemEnt = player.GetInventory().CreateInInventory("ChernarusMap");itemBs = ItemBase.Cast(itemEnt);
		rags = player.GetInventory().CreateInInventory("Rag");itemBs = ItemBase.Cast(rags);
		itemBs.SetQuantity(4);

		player.LocalTakeEntityToHands(primary);
		player.SetQuickBarEntityShortcut(primary, 0, true);
		player.SetQuickBarEntityShortcut(rags, 2, true);
		player.SetQuickBarEntityShortcut(axe, 3, true);
		
		itemTop = player.FindAttachmentBySlotName("Body");
		
		if ( itemTop )
		{
			itemEnt = itemTop.GetInventory().CreateInInventory("Rag");
			if ( Class.CastTo(itemBs, itemEnt ) )
				itemBs.SetQuantity(4);

			SetRandomHealth(itemEnt);
			
			itemEnt = itemTop.GetInventory().CreateInInventory("RoadFlare");
			SetRandomHealth(itemEnt);
		
			itemEnt = itemTop.GetInventory().CreateInInventory("StoneKnife");
			SetRandomHealth(itemEnt);
		}

		rand = Math.RandomFloatInclusive(0.0, 1.0);
		if ( rand < 0.25 )
			itemEnt = player.GetInventory().CreateInInventory("SodaCan_Cola");
		else if ( rand > 0.75 )
			itemEnt = player.GetInventory().CreateInInventory("SodaCan_Spite");
		else
			itemEnt = player.GetInventory().CreateInInventory("SodaCan_Pipsi");
		
		SetRandomHealth(itemEnt);

		rand = Math.RandomFloatInclusive(0.0, 1.0);
		if ( rand < 0.35 )
			itemEnt = player.GetInventory().CreateInInventory("Apple");
		else if ( rand > 0.65 )
			itemEnt = player.GetInventory().CreateInInventory("Pear");
		else
			itemEnt = player.GetInventory().CreateInInventory("Plum");
		
		SetRandomHealth(itemEnt);
		
	//		SetRandomHealth(itemEnt);
	//		player.GetInventory().CreateInInventory("HuntingKnife");
	}
	void addMags(PlayerBase player, string mag_type, int count)
	{
		if (count < 1)
			return;

		EntityAI mag;

		for (int i = 0; i < count; i++) {
			mag = player.GetInventory().CreateInInventory(mag_type);
			ItemBase itemBs = ItemBase.Cast(mag);
		}

		player.SetQuickBarEntityShortcut(mag, 1, true);
	}
	EntityAI BasicLoadoutClass(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("M4A1");
		gun.GetInventory().CreateAttachment("M4_RISHndgrd_Black");
		gun.GetInventory().CreateAttachment("M4_MPBttstck_Black");
		gun.GetInventory().CreateAttachment("ACOGOptic");
		addMags(player, "Mag_STANAG_30Rnd", 3);

		return gun;
	}
	EntityAI assaultClass(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("M4A1");
		gun.GetInventory().CreateAttachment("M4_RISHndgrd_Black");
		gun.GetInventory().CreateAttachment("M4_MPBttstck_Black");
		gun.GetInventory().CreateAttachment("ACOGOptic");
		addMags(player, "Mag_STANAG_30Rnd", 3);

		return gun;
	}

	EntityAI sniperClass(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("SVD");
		gun.GetInventory().CreateAttachment("PSO1Optic");
		addMags(player, "Mag_SVD_10Rnd", 3);

		return gun;
	}

	EntityAI smgClass(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("UMP45");
		gun.GetInventory().CreateAttachment("PistolSuppressor");
		addMags(player, "Mag_UMP_25Rnd", 3);

		return gun;
	}
};
  
Mission CreateCustomMission(string path)
{
	return new CustomMission();
}