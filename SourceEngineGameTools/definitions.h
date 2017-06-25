#pragma once
typedef void* (*CreateInterfaceFn)(const char *Name, int *ReturnCode);

#define COLORCODE(r, g, b, a) ((DWORD)((((r)&0xff) << 24) | (((g)&0xff) << 16) | (((b)&0xff) << 8) | ((a)&0xff)))
#define RED(COLORCODE) ((int)(COLORCODE >> 24))
#define BLUE(COLORCODE) ((int)(COLORCODE >> 8) & 0xFF)
#define GREEN(COLORCODE) ((int)(COLORCODE >> 16) & 0xFF)
#define ALPHA(COLORCODE) ((int)COLORCODE & 0xFF)

#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)   
#define IN_SPEED					(1 << 17)	
#define IN_WALK						(1 << 18)	
#define IN_ZOOM						(1 << 19)	
#define IN_WEAPON1					(1 << 20)	
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)
#define IN_GRENADE1					(1 << 23)
#define IN_GRENADE2					(1 << 24)	
#define	IN_ATTACK3					(1 << 25)

#define	FL_ONGROUND					(1 << 0)	
#define FL_DUCKING					(1 << 1)	
#define	FL_WATERJUMP				(1 << 2)	
#define FL_ONTRAIN					(1 << 3)
#define FL_INRAIN					(1 << 4)	
#define FL_FROZEN					(1 << 5)
#define FL_ATCONTROLS				(1 << 6)
#define	FL_CLIENT					(1 << 7)	
#define FL_FAKECLIENT				(1 << 8)	
#define	FL_INWATER					(1 << 9)										
#define	FL_FLY						(1 << 10)	
#define	FL_SWIM						(1 << 11)
#define	FL_CONVEYOR					(1 << 12)
#define	FL_NPC						(1 << 13)
#define	FL_GODMODE					(1 << 14)
#define	FL_NOTARGET					(1 << 15)
#define	FL_AIMTARGET				(1 << 16)	
#define	FL_PARTIALGROUND			(1 << 17)	
#define FL_STATICPROP				(1 << 18)
#define FL_GRAPHED					(1 << 19) 
#define FL_GRENADE					(1 << 20)
#define FL_STEPMOVEMENT				(1 << 21)	
#define FL_DONTTOUCH				(1 << 22)
#define FL_BASEVELOCITY				(1 << 23)
#define FL_WORLDBRUSH				(1 << 24)	
#define FL_OBJECT					(1 << 25) 
#define FL_KILLME					(1 << 26)
#define FL_ONFIRE					(1 << 27)	
#define FL_DISSOLVING				(1 << 28)
#define FL_TRANSRAGDOLL				(1 << 29)
#define FL_UNBLOCKABLE_BY_PLAYER	(1 << 30) 
#define PLAYER_FLAG_BITS			10
#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)

#define	CONTENTS_EMPTY			0		/**< No contents. */
#define	CONTENTS_SOLID			0x1		/**< an eye is never valid in a solid . */
#define	CONTENTS_WINDOW			0x2		/**< translucent, but not watery (glass). */
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		/**< alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't. */
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_MIST			0x40
#define CONTENTS_OPAQUE			0x80		/**< things that cannot be seen through (may be non-solid though). */
#define	LAST_VISIBLE_CONTENTS	0x80
#define ALL_VISIBLE_CONTENTS 	(LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))
#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED5		0x200
#define CONTENTS_UNUSED6		0x4000
#define CONTENTS_TEAM1			0x800		/**< per team contents used to differentiate collisions. */
#define CONTENTS_TEAM2			0x1000		/**< between players and objects on different teams. */
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000		/**< ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW. */
#define CONTENTS_MOVEABLE		0x4000		/**< hits entities which are MOVETYPE_PUSH (doors, plats, etc) */
#define	CONTENTS_AREAPORTAL		0x8000		/**< remaining contents are non-visible, and don't eat brushes. */
#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

/**
* @section currents can be added to any other contents, and may be mixed
*/
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

/**
* @endsection
*/

#define	CONTENTS_ORIGIN			0x1000000	/**< removed before bsp-ing an entity. */
#define	CONTENTS_MONSTER		0x2000000	/**< should never be on a brush, only in game. */
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	/**< brushes to be added after vis leafs. */
#define	CONTENTS_TRANSLUCENT	0x10000000	/**< auto set if any surface has trans. */
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	/**< use accurate hitboxes on trace. */

/**
* @section Trace masks.
*/
#define	MASK_ALL				(0xFFFFFFFF)
#define	MASK_SOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) 			/**< everything that is normally solid */
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) 	/**< everything that blocks player movement */
#define	MASK_NPCSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) /**< blocks npc movement */
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME) 							/**< water physics in these contents */
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE) 							/**< everything that blocks line of sight for AI, lighting, etc */
#define MASK_OPAQUE_AND_NPCS	(MASK_OPAQUE|CONTENTS_MONSTER)										/**< everything that blocks line of sight for AI, lighting, etc, but with monsters added. */
#define	MASK_VISIBLE			(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE) 								/**< everything that blocks line of sight for players */
#define MASK_VISIBLE_AND_NPCS	(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE) 							/**< everything that blocks line of sight for players, but with monsters added. */
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX) 	/**< bullets see these as solid */
#define MASK_SHOT_HULL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE) 	/**< non-raycasted weapons see this as solid (includes grates) */
#define MASK_SHOT_PORTAL		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW) 							/**< hits solids (not grates) and passes through everything else */
#define MASK_SOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE) 					/**< everything normally solid, except monsters (world+brush only) */
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE) 			/**< everything normally solid for player movement, except monsters (world+brush only) */
#define MASK_NPCSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE) 			/**< everything normally solid for npc movement, except monsters (world+brush only) */
#define MASK_NPCWORLDSTATIC		(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE) 					/**< just the world, used for route rebuilding */
#define MASK_SPLITAREAPORTAL	(CONTENTS_WATER|CONTENTS_SLIME) 									/**< These are things that can split areaportals */

/**
* @endsection
*/

//----------------------------------
//----------------------------------
//--------------L4D2-------------
//----------------------------------
//----------------------------------
#define IClientModePointer		0x7A5380		// (IClientMode*)(client.dll + 0x794148)

// ConVar
#define sv_cheats				0x670758		// engine.dll
#define sv_pure					0x64307C		// engine.dll
#define sv_consistency			0x670998		// engine.dll
#define r_drawothermodels		0x6F85E8		// client.dll
#define cl_drawshadowtexture	0x7759B0		// client.dll
#define mat_fullbright			0xFE3F0			// materialsystem.dll
#define mp_gamemode				0x79679C		// client.dll - char*

// cbaseentity offset
#define m_iCrosshairsId			0x19D8
#define m_iLastCrosshairsId		0x19E8

// 获取本地玩家
#define GetLocalClient()			Interfaces.ClientEntList->GetClientEntity(Interfaces.Engine->GetLocalPlayer())

// 倒地
#define IsFallDown(_e)				(_e->GetNetProp<int>("m_isIncapacitated", "DT_TerrorPlayer") > 0)

// 挂边
#define IsHangingFromLedge(_e)		(_e->GetNetProp<int>("m_isHangingFromLedge", "DT_TerrorPlayer") > 0)

// 被舌头拉
#define IsVictimSmoker(_e)			(_e->GetNetProp<int>("m_tongueOwner", "DT_TerrorPlayer") > 0)

// 被猴骑
#define IsVictimJockey(_e)			(_e->GetNetProp<int>("m_jockeyAttacker", "DT_TerrorPlayer") > 0)

// 被猎人扑
#define IsVictimHunter(_e)			(_e->GetNetProp<int>("m_pounceAttacker", "DT_TerrorPlayer") > 0)

// 被牛抓住锤地板
#define IsVictimCharger(_e)			(_e->GetNetProp<int>("m_pummelAttacker", "DT_TerrorPlayer") > 0)

// 被控
#define IsControlled(_e)			(IsVictimSmoker(_e) || IsVictimJockey(_e) || IsVictimHunter(_e) || IsVictimCharger(_e))

// 无法移动(倒地挂边)
#define IsIncapacitated(_e)			(IsFallDown(_e) || IsHangingFromLedge(_e))

// 是否需要队友救援
#define IsNeedRescue(_e)			(IsIncapacitated(_e) || IsControlled(_e))

// 获取当前服务器时间
#define GetServerTime()				(Interfaces.ClientEntList->GetClientEntity(Interfaces.Engine->GetLocalPlayer())->GetTickBase() * Interfaces.Globals->interval_per_tick)

// 输出偏移地址
#define printo(_s,_n)		std::cout.setf(std::ios::hex|std::ios::uppercase);std::cout<<XorStr(_s)<<XorStr(" = 0x")<<(DWORD)_n<<std::endl;std::cout.unsetf(std::ios::hex|std::ios::uppercase)

enum WeaponID_t
{
	Weapon_Pistol = 1,				// 小手枪(包括双持) 手枪
	Weapon_ShotgunPump = 3,			// 木喷 半自动
	Weapon_ShotgunAuto = 4,			// 连喷 连点加快射速
	Weapon_SniperHunting = 6,		// 猎枪 半自动
	Weapon_ShotgunChrome = 8,		// 铁喷 半自动
	Weapon_SniperMilitary = 10,		// 连狙 半自动
	Weapon_ShotgunSpas = 11,		// 高级连喷 连点加快射速
	Weapon_Melee = 19,				// 近战武器
	Weapon_PistolMagnum = 32,		// 马格南 手枪
	Weapon_SniperAWP = 35,			// 大鸟 半自动
	Weapon_SniperScout = 36			// 鸟狙 半自动
};

enum EntityType_t
{
	ET_INVALID = -1,
	ET_CTERRORPLAYER = 231,
	ET_SURVIVORBOT = 274,
	ET_BOOMER = 0,
	ET_INFECTED = 263,
	ET_TANK = 275,
	ET_WITCH = 276,
	ET_JOCKEY = 264,
	ET_SPITTER = 271,
	ET_CHARGER = 99,
	ET_HUNTER = 262,
	ET_SMOKER = 269
};

enum ZombieClass_t
{
	ZC_SMOKER = 1,
	ZC_BOOMER = 2,
	ZC_HUNTER = 3,
	ZC_SPITTER = 4,
	ZC_JOCKEY = 5,
	ZC_CHARGER = 6,
	ZC_WITCH = 7,
	ZC_TANK = 8,
	ZC_SURVIVORBOT = 9
};

enum SurvivorBone_t
{
	BONE_SURVIVOR_BAD = -1,
	BONE_ROCHELLE_HEAD = 16,
	BONE_COACH_HEAD = 17,
	BONE_ELLIS_HEAD = 14,
	BONE_NICK_HEAD = 14,
	BONE_FRANCIS_HEAD = 14,
	BONE_ZOEY_HEAD = 14,
	BONE_LOUIS_HEAD = 14,
	BONE_BILL_HEAD = 14
};

enum ZombieBone_t
{
	BONE_ZOMBIE_BAD = -1,
	BONE_NECK = 14,
	BONE_JOCKEY_HEAD = 7,
	BONE_CHARGER_HEAD = 16,
	BONE_INFECTED_HEAD = 29,
	BONE_TANK_CHEST = 12,
	BONE_BOOMER_CHEST = 11,
	BONE_SPITTER_CHEST = 3,
	BONE_JOCKEY_CHEST = 4,
	BONE_CHARGER_CHEST = 4,
	BONE_HUNTER_CHEST = 12
};

enum entityGender_t
{
	G_MALE = 1,
	G_FEMALE,
	G_BILL,
	G_ZOEY,
	G_FRANCIS,
	G_LOUIS,
	G_NICK,
	G_ROCHELLE,
	G_COACH,
	G_ELLIS
};

//---------------
//----PLAYERS----
//---------------

#define l4d2_ellisbody(_s,_n,_p)	(_s == 32 && _n == 4822 && _p == 8088) 
#define l4d2_ellishead(_s,_n,_p)	(_s == 32 && ((_n == 2037 && _p == 3832) || (_n == 2661 && _p == 4872))) 
#define l4d2_ellis(_s,_n,_p)		(l4d2_ellisbody || l4d2_ellishead)

#define l4d2_coatchbody(_s,_n,_p)	(_s == 32 && _n == 5710 && _p == 9262) 
#define l4d2_coatchead(_s,_n,_p)	(_s == 32 && ( (_n == 1819 && _p == 3375) || (_n == 2387 && _p == 4527) ))
#define l4d2_coatch(_s,_n,_p)		(l4d2_coatchbody || l4d2_coatchead )

#define l4d2_rachellebody(_s,_n,_p)	(_s == 32 && _n == 6443 && _p == 10138) 
#define l4d2_rachellehead(_s,_n,_p)	(_s == 32 && ( (_n == 1647 && _p == 3069) || (_n == 3947 && _p == 6910) ))
#define l4d2_rachelle(_s,_n,_p)		(l4d2_rachellebody || l4d2_rachellehead )

#define l4d2_nickbody(_s,_n,_p)		(_s == 32 && ( (_n == 7957 && _p == 11314) || (_n == 7913 && _p == 12247) )) 
#define l4d2_nickhead(_s,_n,_p)		(_s == 32 && ( (_n == 1680 && _p == 3133) || (_n == 2384 && _p == 4540) )) 
#define l4d2_nick(_s,_n,_p)			(l4d2_nickbody || l4d2_nickhead)

#define l4d2_survivor(_s,_n,_p)		(l4d2_ellis(_s,_n,_p) || l4d2_coatch(_s,_n,_p) || l4d2_rachelle(_s,_n,_p) || l4d2_nick(_s,_n,_p))

#define l4d2_pistolhands (_s == 32 && _n == 10318 && _p == 9340) 
#define l4d2_hands (_s == 32 && _n == 4262 && _p == 6468) 


//---------------
//----WEAPONS---
//--------------


//GUNS
#define l4d2_p220(_s,_n,_p)			(_s == 32 && _n == 3406 && _p == 2883)
#define l4d2_glock(_s,_n,_p)		(_s == 32 && _n == 961 && _p == 835) 
#define l4d2_deagle(_s,_n,_p)		(_s == 32 && _n == 2530 && _p == 1962) 

#define l4d2_guns(_s,_n,_p)			(l4d2_p220(_s,_n,_p) || l4d2_glock(_s,_n,_p) || l4d2_deagle(_s,_n,_p))

//SHOTGUNS
#define l4d2_spas12(_s,_n,_p)		(_s == 32 && _n == 5320 && _p == 5277) 
#define l4d2_chrome(_s,_n,_p)		(_s == 32 && _n == 3525 && _p == 3418) 
#define l4d2_autoshotgun(_s,_n,_p)	(_s == 32 && _n == 2942 && _p == 2224)  
#define l4d2_pumpshotgun(_s,_n,_p)	(_s == 32 && _n == 2507 && _p == 2362) 

#define l4d2_shotguns(_s,_n,_p)		(l4d2_spas12(_s,_n,_p) || l4d2_chrome(_s,_n,_p) || l4d2_autoshotgun(_s,_n,_p) || l4d2_pumpshotgun(_s,_n,_p))

//SMG
#define l4d2_uzi(_s,_n,_p)			(_s == 32 && _n == 2318 && _p == 1875)
#define l4d2_uzisilent(_s,_n,_p)	(_s == 32 && _n == 5283 && _p == 5221)
#define l4d2_mp5(_s,_n,_p)			(_s == 32 && _n == 502 && _p == 410) 

#define l4d2_smg(_s,_n,_p)			(l4d2_uzi(_s,_n,_p) || l4d2_uzisilent(_s,_n,_p) || l4d2_mp5(_s,_n,_p))

//MELEE
#define l4d2_axe(_s,_n,_p)			(_s == 32 && _n == 1594 && _p == 2638) //hache
#define l4d2_crowbar(_s,_n,_p)		(_s == 32 && _n == 1061 && _p == 1510) //barredefer
#define l4d2_baseballbat(_s,_n,_p)	(_s == 32 && _n == 641 && _p == 1132)
#define l4d2_chainsaw(_s,_n,_p)		(_s == 32 && _n == 4880 && _p == 4287) //tronconneuse
#define l4d2_katana(_s,_n,_p)		(_s == 32 && _n == 950 && _p == 1179) 
#define l4d2_cricketbat(_s,_n,_p)	(_s == 32 && _n == 1354 && _p == 2103) 

#define l4d2_melee(_s,_n,_p)		(l4d2_axe(_s,_n,_p) || l4d2_crowbar(_s,_n,_p) || l4d2_baseballbat(_s,_n,_p) || l4d2_chainsaw(_s,_n,_p) || l4d2_katana(_s,_n,_p) || l4d2_cricketbat(_s,_n,_p))

//RIFLES
#define l4d2_m4(_s,_n,_p)			(_s == 32 && _n == 2562 && _p == 2018) 
#define l4d2_scar(_s,_n,_p)			(_s == 32 && _n == 4878 && _p == 3765) 
#define l4d2_ak47(_s,_n,_p)			(_s == 32 && _n == 3574 && _p == 2817)
#define l4d2_m60(_s,_n,_p)			(_s == 32 && _n == 6366 && _p == 4785) 
#define l4d2_sig552(_s,_n,_p)		(_s == 32 && _n == 573 && _p == 417)

#define l4d2_rifles(_s,_n,_p) (l4d2_m4(_s,_n,_p) || l4d2_scar(_s,_n,_p) || l4d2_ak47(_s,_n,_p) || l4d2_m60(_s,_n,_p) || l4d2_sig552(_s,_n,_p))

//SNIPERS
#define l4d2_awp(_s,_n,_p)			(_s == 32 && _n == 681 && _p == 523) 
#define l4d2_scout(_s,_n,_p)		(_s == 32 && _n == 606 && _p == 477) 
#define l4d2_military(_s,_n,_p)		(_s == 32 && _n == 4278 && _p == 4059)
#define l4d2_huntingrifle(_s,_n,_p)	(_s == 32 && _n == 2514 && _p == 2069) 

#define l4d2_snipers(_s,_n,_p)		(l4d2_awp(_s,_n,_p) || l4d2_scout(_s,_n,_p) || l4d2_militarysniper(_s,_n,_p) || l4d2_huntingrifle(_s,_n,_p))

//GRENADELAUNCHER
#define l4d2_grenade(_s,_n,_p)		(_n == 2575 && _p == 2265) 

#define l4d2_weapons(_s,_n,_p)		(l4d2_guns(_s,_n,_p) || l4d2_shotguns(_s,_n,_p) || l4d2_smg(_s,_n,_p) || l4d2_melee(_s,_n,_p) || l4d2_rifles(_s,_n,_p) || l4d2_snipers(_s,_n,_p) || l4d2_grenade(_s,_n,_p))

//---------------
//----STUFF------
//---------------

#define l4d2_medickit(_s,_n,_p)		(_s == 32 && _n == 883 && _p == 902) 
#define l4d2_adrenaline(_s,_n,_p)	(_s == 32 && _n == 2035 && _p == 1842) 
#define l4d2_defib(_s,_n,_p)		(_s == 32 && _n == 4901 && _p == 5885) 
#define l4d2_painpills(_s,_n,_p)	(_s == 32 && _n == 240 && _p == 282) //(cachet)
#define l4d2_gascan(_s,_n,_p)		(_s == 32 && _n == 855 && _p == 952) //gericane
#define l4d2_propanetank(_s,_n,_p)	(_s == 32 && _n == 1105 && _p == 1092) //bombone de prop
#define l4d2_oxygentank(_s,_n,_p)	(_s == 32 && _n == 1779 && _p == 1742) //bombone d'oxy
#define l4d2_fireworkbox(_s,_n,_p)	(_s == 32 && _n == 222 && _p == 334)
#define l4d2_pipebomb(_s,_n,_p)		(_s == 32 && _n == 2375 && _p == 1956) 
#define l4d2_bilebomb(_s,_n,_p)		(_s == 32 && ( (_n == 98 && _p == 192) || (_n == 1526 && _p == 1491) )) 
#define l4d2_molotov(_s,_n,_p)		(_s == 32 && ( (_n == 442 && _p == 576) || (_n == 200 && _p == 296) )) 
#define l4d2_cola(_s,_n,_p)			(_s == 32 && ( (_n == 1353 && _p == 1337) || (_n == 930 && _p == 1446) ))
#define l4d2_gnome(_s,_n,_p)		(_s == 32 && _n == 1648 && _p == 2088)
#define l4d2_explosif(_s,_n,_p)		(_s == 32 && _n == 815 && _p == 736) //explosif bullet box
#define l4d2_incendiary(_s,_n,_p)	(_s == 32 && _n == 821 && _p == 736) //incendiary bullet box

#define l4d2_stuff(_s,_n,_p)		(l4d2_medickit(_s,_n,_p) || l4d2_adrenaline(_s,_n,_p) || l4d2_defib(_s,_n,_p) || l4d2_painpills(_s,_n,_p) || l4d2_gascan(_s,_n,_p) || l4d2_propanetank(_s,_n,_p) || l4d2_oxygentank(_s,_n,_p) || l4d2_fireworkbox(_s,_n,_p) || l4d2_pipebomb(_s,_n,_p) || l4d2_bilebomb(_s,_n,_p) || l4d2_molotov(_s,_n,_p) || l4d2_cola(_s,_n,_p) || l4d2_gnome(_s,_n,_p) || l4d2_explosif(_s,_n,_p) || l4d2_incendiary(_s,_n,_p))


//---------------
//----ZOMBIE-----
//---------------

//INFECTED
#define l4d2_zombie1body(_s,_n,_p)	(_s == 32 && _n == 2035 && _p == 3038)
#define l4d2_zombie1head(_s,_n,_p)	(_s == 32 && _n == 834 && _p == 1289) 
#define l4d2_zombie2body(_s,_n,_p)	(_s == 32 && _n == 1673 && _p == 2608) 
#define l4d2_zombie2head(_s,_n,_p)	(_s == 32 && _n == 854 && _p == 1389) 
#define l4d2_zombie3body(_s,_n,_p)	(_s == 32 && _n == 1732 && _p == 2581)

//(NOT WORKING) too many zombies models, too lazy to log all of them :p

#define l4d2_common(_s,_n,_p)		(l4d2_zombie1body(_s,_n,_p) || l4d2_zombie1head(_s,_n,_p) || l4d2_zombie2body(_s,_n,_p) || l4d2_zombie2head(_s,_n,_p) || l4d2_zombie3body(_s,_n,_p))

//SPECIAL ZOMBIE
#define l4d2_tank(_s,_n,_p)			(_s == 32 && _n == 3600 && _p == 5812) 
#define l4d2_smoker(_s,_n,_p)		(_s == 32 && _n == 6571 && _p == 11361) 
#define l4d2_hunter(_s,_n,_p)		(_s == 32 && _n == 6896 && _p == 11701) 
#define l4d2_boomer(_s,_n,_p)		(_s == 32 && _n == 3248 && _p == 5257) 
#define l4d2_charger(_s,_n,_p)		(_s == 32 && _n == 3522 && _p == 6071) 
#define l4d2_jockey(_s,_n,_p)		(_s == 32 && ( (_n == 6340 && _p == 10382) || (_n == 6340 && _p == 504) ))
#define l4d2_witch(_s,_n,_p)		(_s == 32 && ( (_n == 3362 && _p == 5099) || (_n == 488 && _p == 566) ))
#define l4d2_spitter(_s,_n,_p)		(_s == 32 && ( (_n == 4811 && _p == 5621) || (_n == 345 && _p == 534) || (_n == 4811 && _p == 2381) ))

#define l4d2_special(_s,_n,_p)		(l4d2_tank(_s,_n,_p) || l4d2_smoker(_s,_n,_p) || l4d2_hunter(_s,_n,_p) || l4d2_boomer(_s,_n,_p) || l4d2_charger(_s,_n,_p) || l4d2_jockey(_s,_n,_p) || l4d2_witch(_s,_n,_p) || l4d2_spitter(_s,_n,_p))

#define l4d2_zombies(_s,_n,_p)		(l4d2_common(_s,_n,_p) || l4d2_special(_s,_n,_p))
