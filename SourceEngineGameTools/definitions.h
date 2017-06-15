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

//----------------------------------
//----------------------------------
//--------------L4D2-------------
//----------------------------------
//----------------------------------
#define IClientModePointer			0x837E78	// (IClientMode*)(client.dll + 0x837E78)

//---------------
//----PLAYERS----
//---------------

#define l4d2_ellisbody (Stride == 32 && NumVertices == 4822 && primCount == 8088) 
#define l4d2_ellishead (Stride == 32 && ( (NumVertices == 2037 && primCount == 3832) || (NumVertices == 2661 && primCount == 4872) )) 
#define l4d2_ellis (l4d2_ellisbody || l4d2_ellishead )

#define l4d2_coatchbody (Stride == 32 && NumVertices == 5710 && primCount == 9262) 
#define l4d2_coatchead	(Stride == 32 && ( (NumVertices == 1819 && primCount == 3375) || (NumVertices == 2387 && primCount == 4527) ))
#define l4d2_coatch (l4d2_coatchbody || l4d2_coatchead )

#define l4d2_rachellebody (Stride == 32 && NumVertices == 6443 && primCount == 10138) 
#define l4d2_rachellehead (Stride == 32 && ( (NumVertices == 1647 && primCount == 3069) || (NumVertices == 3947 && primCount == 6910) ))
#define l4d2_rachelle (l4d2_rachellebody || l4d2_rachellehead )

#define l4d2_nickbody (Stride == 32 && ( (NumVertices == 7957 && primCount == 11314) || (NumVertices == 7913 && primCount == 12247) )) 
#define l4d2_nickhead (Stride == 32 && ( (NumVertices == 1680 && primCount == 3133) || (NumVertices == 2384 && primCount == 4540) )) 
#define l4d2_nick (l4d2_nickbody || l4d2_nickhead)

#define l4d2_survivor (l4d2_ellis || l4d2_coatch || l4d2_rachelle || l4d2_nick)

#define l4d2_pistolhands (Stride == 32 && NumVertices == 10318 && primCount == 9340) 
#define l4d2_hands (Stride == 32 && NumVertices == 4262 && primCount == 6468) 


//---------------
//----WEAPONS---
//--------------


//GUNS
#define l4d2_p220 (Stride == 32 && NumVertices == 3406 && primCount == 2883)
#define l4d2_glock (Stride == 32 && NumVertices == 961 && primCount == 835) 
#define l4d2_deagle (Stride == 32 && NumVertices == 2530 && primCount == 1962) 

#define l4d2_guns (l4d2_p220 || l4d2_glock || l4d2_deagle)

//SHOTGUNS
#define l4d2_spas12 (Stride == 32 && NumVertices == 5320 && primCount == 5277) 
#define l4d2_shotgunchrome (Stride == 32 && NumVertices == 3525 && primCount == 3418) 
#define l4d2_autoshotgun (Stride == 32 && NumVertices == 2942 && primCount == 2224)  
#define l4d2_pumpshotgun (Stride == 32 && NumVertices == 2507 && primCount == 2362) 

#define l4d2_shotguns (l4d2_spas12 || l4d2_shotgunchrome || l4d2_autoshotgun || l4d2_pumpshotgun)

//SMG
#define l4d2_uzi (Stride == 32 && NumVertices == 2318 && primCount == 1875)
#define l4d2_uzisilent (Stride == 32 && NumVertices == 5283 && primCount == 5221)
#define l4d2_mp5 (Stride == 32 && NumVertices == 502 && primCount == 410) 

#define l4d2_smg (l4d2_uzi || l4d2_uzisilent || l4d2_mp5)

//MELEE
#define l4d2_axe (Stride == 32 && NumVertices == 1594 && primCount == 2638) //hache
#define l4d2_crowbar (Stride == 32 && NumVertices == 1061 && primCount == 1510) //barredefer
#define l4d2_baseballbat (Stride == 32 && NumVertices == 641 && primCount == 1132)
#define l4d2_chainsaw (Stride == 32 && NumVertices == 4880 && primCount == 4287) //tronconneuse
#define l4d2_katana (Stride == 32 && NumVertices == 950 && primCount == 1179) 
#define l4d2_cricketbat (Stride == 32 && NumVertices == 1354 && primCount == 2103) 

#define l4d2_melee (l4d2_axe || l4d2_crowbar || l4d2_baseballbat || l4d2_chainsaw || l4d2_katana || l4d2_cricketbat)

//RIFLES
#define l4d2_m4 (Stride == 32 && NumVertices == 2562 && primCount == 2018) 
#define l4d2_scar (Stride == 32 && NumVertices == 4878 && primCount == 3765) 
#define l4d2_ak47 (Stride == 32 && NumVertices == 3574 && primCount == 2817)
#define l4d2_m60 (Stride == 32 && NumVertices == 6366 && primCount == 4785) 
#define l4d2_sig552 (Stride == 32 && NumVertices == 573 && primCount == 417)

#define l4d2_rifles (l4d2_m4 || l4d2_scar || l4d2_ak47 || l4d2_m60 || l4d2_sig552)

//SNIPERS
#define l4d2_awp (Stride == 32 && NumVertices == 681 && primCount == 523) 
#define l4d2_scout (Stride == 32 && NumVertices == 606 && primCount == 477) 
#define l4d2_militarysniper (Stride == 32 && NumVertices == 4278 && primCount == 4059)
#define l4d2_huntingrifle (Stride == 32 && NumVertices == 2514 && primCount == 2069) 

#define l4d2_snipers (l4d2_awp || l4d2_scout || l4d2_militarysniper || l4d2_huntingrifle)

//GRENADELAUNCHER
#define l4d2_grenadelauncher (NumVertices == 2575 && primCount == 2265) 

#define l4d2_weapons (l4d2_guns || l4d2_shotguns || l4d2_smg || l4d2_melee || l4d2_rifles || l4d2_snipers || l4d2_grenadelauncher)

//---------------
//----STUFF------
//---------------

#define l4d2_medickit (Stride == 32 && NumVertices == 883 && primCount == 902) 
#define l4d2_adrenaline (Stride == 32 && NumVertices == 2035 && primCount == 1842) 
#define l4d2_defibrillator (Stride == 32 && NumVertices == 4901 && primCount == 5885) 
#define l4d2_painpills (Stride == 32 && NumVertices == 240 && primCount == 282) //(cachet)
#define l4d2_gascan (Stride == 32 && NumVertices == 855 && primCount == 952) //gericane
#define l4d2_propanetank (Stride == 32 && NumVertices == 1105 && primCount == 1092) //bombone de prop
#define l4d2_oxygentank (Stride == 32 && NumVertices == 1779 && primCount == 1742) //bombone d'oxy
#define l4d2_fireworkbox (Stride == 32 && NumVertices == 222 && primCount == 334)
#define l4d2_pipebomb (Stride == 32 && NumVertices == 2375 && primCount == 1956) 
#define l4d2_bilebomb (Stride == 32 && ( (NumVertices == 98 && primCount == 192) || (NumVertices == 1526 && primCount == 1491) )) 
#define l4d2_molotov (Stride == 32 && ( (NumVertices == 442 && primCount == 576) || (NumVertices == 200 && primCount == 296) )) 
#define l4d2_cola (Stride == 32 && ( (NumVertices == 1353 && primCount == 1337) || (NumVertices == 930 && primCount == 1446) ))
#define l4d2_gnome (Stride == 32 && NumVertices == 1648 && primCount == 2088)
#define l4d2_explosifbullet (Stride == 32 && NumVertices == 815 && primCount == 736) //explosif bullet box
#define l4d2_incendiarybullet (Stride == 32 && NumVertices == 821 && primCount == 736) //incendiary bullet box

#define l4d2_stuff (l4d2_medickit || l4d2_adrenaline || l4d2_defibrillator || l4d2_painpills || l4d2_gascan || l4d2_propanetank || l4d2_oxygentank || l4d2_fireworkbox || l4d2_pipebomb || l4d2_bilebomb \
		|| l4d2_molotov || l4d2_cola || l4d2_gnome || l4d2_explosifbullet || l4d2_incendiarybullet)


//---------------
//----ZOMBIE-----
//---------------

//INFECTED
#define l4d2_zombie1body (Stride == 32 && NumVertices == 2035 && primCount == 3038)
#define l4d2_zombie1head (Stride == 32 && NumVertices == 834 && primCount == 1289) 
#define l4d2_zombie2body (Stride == 32 && NumVertices == 1673 && primCount == 2608) 
#define l4d2_zombie2head (Stride == 32 && NumVertices == 854 && primCount == 1389) 
#define l4d2_zombie3body (Stride == 32 && NumVertices == 1732 && primCount == 2581)

//(NOT WORKING) too many zombies models, too lazy to log all of them :p

#define l4d2_randomzombies (l4d2_zombie1body || l4d2_zombie1head || l4d2_zombie2body || l4d2_zombie2head || l4d2_zombie3body)

//SPECIAL ZOMBIE
#define l4d2_tank (Stride == 32 && NumVertices == 3600 && primCount == 5812) 
#define l4d2_smoker (Stride == 32 && NumVertices == 6571 && primCount == 11361) 
#define l4d2_hunter (Stride == 32 && NumVertices == 6896 && primCount == 11701) 
#define l4d2_boomer (Stride == 32 && NumVertices == 3248 && primCount == 5257) 
#define l4d2_charger (Stride == 32 && NumVertices == 3522 && primCount == 6071) 
#define l4d2_jockey (Stride == 32 && ( (NumVertices == 6340 && primCount == 10382) || (NumVertices == 6340 && primCount == 504) ))
#define l4d2_witch (Stride == 32 && ( (NumVertices == 3362 && primCount == 5099) || (NumVertices == 488 && primCount == 566) ))
#define l4d2_spitter (Stride == 32 && ( (NumVertices == 4811 && primCount == 5621) || (NumVertices == 345 && primCount == 534) || (NumVertices == 4811 && primCount == 2381) ))

#define l4d2_specialzombies (l4d2_tank || l4d2_smoker || l4d2_hunter || l4d2_boomer || l4d2_charger || l4d2_jockey || l4d2_witch || l4d2_spitter)

#define l4d2_zombies (l4d2_randomzombies || l4d2_specialzombies)
