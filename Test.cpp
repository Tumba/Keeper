
#define _CRT_SECURE_NO_WARNINGS

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
#include <cstring>
#include "Invoke.h"
#include "math.h"
#include <iostream>

#include <fstream>
#include "vehiclespeedlist.var"
#define INVALID_VEHICLE_ID						(0xFFFF)
#define MAX_VEHICLES 2000
#define MAX_PLAYERS 300
#define COLOR_RED 		0xAA3333AA
#define PLAYER_STATE_NONE						(0)
#define PLAYER_STATE_ONFOOT						(1)
#define PLAYER_STATE_DRIVER						(2)
#define PLAYER_STATE_PASSENGER					(3)
#define PLAYER_STATE_EXIT_VEHICLE				(4) 
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		(5) 
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	(6) 
#define PLAYER_STATE_WASTED						(7)
#define PLAYER_STATE_SPAWNED					(8)
#define PLAYER_STATE_SPECTATING					(9)


#define AC_GUN          F[0]
#define AC_FLY          F[1]
#define AC_AIR          F[2]
#define AC_SPEED        F[3]
#define AC_JETPACK      F[4]
#define AC_DEATHFLOOD   F[5]
#define AC_STATEFLOOD   F[6]
#define AC_RCON         F[7]
#define AC_REMOVE       F[8]


std::string admin[30];

unsigned long int Money[MAX_PLAYERS];
bool IsConnected[MAX_PLAYERS];
float Health[MAX_PLAYERS];
float Armour[MAX_PLAYERS];

int ToKick[MAX_PLAYERS];
int ToBan[MAX_PLAYERS];
int EnterCar[MAX_PLAYERS];

int PlayerWeapon[MAX_PLAYERS][47];
int PlayerAmmo[MAX_PLAYERS][47];
int InteriorChange[MAX_PLAYERS];
int RemoveCheck[MAX_PLAYERS];
bool DeathTimes[MAX_PLAYERS];
int GunNoCheck[MAX_PLAYERS];
int SaveCord[MAX_VEHICLES];
int TOTALCARS;

//anti lag
int afk[MAX_PLAYERS];
int tick[MAX_PLAYERS];
//

int AirFix[MAX_PLAYERS];

int F[10];

struct airbreak
{
	float posx;
	float posy;
	float posz;
	float posa;
};

airbreak PlayerAir[MAX_PLAYERS];

airbreak Vehicle[MAX_VEHICLES];

typedef void (*logprintf_t)(char* format, ...);

logprintf_t logprintf;
extern void *pAMXFunctions;

bool IsAdmin(int playerid);//прототип


cell AMX_NATIVE_CALL Invoke_GetAddresses(AMX* amx, cell* params)
{
    return g_Invoke->getAddresses();
}

cell AMX_NATIVE_CALL _GiveMoney(AMX* amx, cell* params)
{
	Money[params[1]] += params[2];
	g_Invoke->callNative(&PAWN::GivePlayerMoney,params[1],params[2]);
	return 1;
}

cell AMX_NATIVE_CALL _GetMoney(AMX* amx, cell* params)
{
	return Money[params[1]];
}

cell AMX_NATIVE_CALL _ResetMoney(AMX* amx, cell* params)
{
	g_Invoke->callNative(&PAWN::ResetPlayerMoney,params[1]);
	Money[params[1]] = 0;
 
    return 1;
}
cell AMX_NATIVE_CALL _SetPlayerHealth(AMX* amx, cell* params)
{

	Health[params[1]] = amx_ctof(params[2]);

	g_Invoke->callNative(&PAWN::SetPlayerHealth,params[1],Health[params[1]]);
	return 1;
}

cell AMX_NATIVE_CALL _GetPlayerHealth(AMX* amx, cell*params)
{
	
    
	  
	//Health[params[1]] = amx_ctof(Health[params[1]]);				
	return Health[params[1]];

}

cell AMX_NATIVE_CALL _SetPlayerArmour(AMX* amx, cell* params)
{

	Armour[params[1]] = amx_ctof(params[2]);
	g_Invoke->callNative(&PAWN::SetPlayerArmour,params[1],Armour[params[1]]);
	return 1;
}

cell AMX_NATIVE_CALL _GetPlayerArmour(AMX* amx, cell*params)
{
	
    
	  
					 return amx_ftoc(Armour[params[1]]);
}

cell AMX_NATIVE_CALL _GivePlayerWeapon(AMX* amx, cell*params)
{
	GunNoCheck[params[1]] = 5;
	PlayerWeapon[params[1]][params[2]] = 1;
	PlayerAmmo[params[1]][params[2]] += params[3];
	g_Invoke->callNative(&PAWN::GivePlayerWeapon,params[1],params[2],params[3]);
	return 1;
}

cell AMX_NATIVE_CALL _ResetPlayerWeapons(AMX* amx, cell*params)
{
	GunNoCheck[params[1]] = 5;
	g_Invoke->callNative(&PAWN::ResetPlayerWeapons,params[1]);
	for(int i =0;i <47;i++)
	{
		 PlayerWeapon[params[1]][i] = 0;
		 PlayerAmmo[params[1]][i] = 0;
	}
	return 1;
}
bool IsVehicleInPoint(int vehicleid,float r,float x,float y,float z)
{
	float xx[3];
	g_Invoke->callNative(&PAWN::GetVehiclePos,vehicleid,&xx[0],&xx[1],&xx[2]);
	if(xx[0] - x < r && xx[0] - x > -r && xx[1] - y < r && xx[1] - y > -r && xx[2] - z < r && xx[2] - z > -r)return true;
	return false;
}

bool IsPlayerInPoint(int playerid,float r,float x,float y,float z)
{
	float xx[3];
	g_Invoke->callNative(&PAWN::GetPlayerPos,playerid,&xx[0],&xx[1],&xx[2]);
	if(xx[0] - x < r && xx[0] - x > -r && xx[1] - y < r && xx[1] - y > -r && xx[2] - z < r && xx[2] - z > -r)return true;
	return false;
}
cell AMX_NATIVE_CALL _SetPlayerPos(AMX* amx, cell*params)
{
	InteriorChange[params[1]] = 4;
	g_Invoke->callNative(&PAWN::SetPlayerPos,params[1],amx_ctof(params[2]),amx_ctof(params[3]),amx_ctof(params[4]));
	PlayerAir[params[1]].posx = amx_ctof(params[2]);
	PlayerAir[params[1]].posy = amx_ctof(params[3]);
	PlayerAir[params[1]].posz = amx_ctof(params[4]);
	return 1;
}
cell AMX_NATIVE_CALL _SetPlayerPosFindZ(AMX* amx, cell*params) 
{
	InteriorChange[params[1]] = 4;
	g_Invoke->callNative(&PAWN::SetPlayerPosFindZ,params[1],amx_ctof(params[2]),amx_ctof(params[3]),amx_ctof(params[4]));
	PlayerAir[params[1]].posx = amx_ctof(params[2]);
	PlayerAir[params[1]].posy = amx_ctof(params[3]);
	PlayerAir[params[1]].posz = amx_ctof(params[4]);
	return 1;
}

cell AMX_NATIVE_CALL _PutPlayerInVehicle(AMX* amx, cell*params)
{
	InteriorChange[params[1]] = 4;
	EnterCar[params[1]] = params[2];
	g_Invoke->callNative(&PAWN::PutPlayerInVehicle,params[1],params[2],params[3]);
	float x[3];
	g_Invoke->callNative(&PAWN::GetVehiclePos,params[2],&x[0],&x[1],&x[2]);
	PlayerAir[params[1]].posx = amx_ctof(x[0]);
	PlayerAir[params[1]].posy = amx_ctof(x[1]);
	PlayerAir[params[1]].posz = amx_ctof(x[2]);
	return 1;
}

cell AMX_NATIVE_CALL _RemovePlayerFromVehicle(AMX* amx, cell*params)
{
	g_Invoke->callNative(&PAWN::RemovePlayerFromVehicle,params[1]);
	RemoveCheck[params[1]] = 8;
	return 1;
}
void AdminMes(int playerid,char mess[])
{
	char mes[128];
	sprintf(mes,"ID: %d подозревается в использовании читов{00BFFF}({00BFFF}%s)",playerid,mess);
	for(int i = 0; i < MAX_PLAYERS;i++)
	{
		if(!IsConnected[i]) continue;
		if(IsAdmin(i))
		{
			g_Invoke->callNative(&PAWN::SendClientMessage,i,COLOR_RED,mes);

		}
	}

}

void SafeKick(int playerid,char mess[],int ac)
{
	char mes[128];
	if(ToKick[playerid] || ToBan[playerid] != 0) return;
	if(ac == 0) return;
	if(ac == 1)
	{
		ToKick[playerid] = 1;
		sprintf(mes,"{FFFFFF}Вы были кикнуты по подозрению в читерстве << {00BFFF}Причина{FFFFFF} %s >>",mess);
	}
	else if(ac == 2)
	{
		ToBan[playerid] = 1;
		sprintf(mes,"{FFFFFF}Вы были забанены по подозрению в читерстве << {00BFFF}Причина{FFFFFF} %s >>",mess);
	}
	else if(ac == 3)
	{
		AdminMes(playerid,mess);
		return;
	}
	g_Invoke->callNative(&PAWN::ShowPlayerDialog,playerid,0,0,"Античит",mes,"ОК","");
	
}

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerConnnect(AMX* amx, cell* params)
{
    Money[params[1]] = 0;
    IsConnected[params[1]] = true;
	ToKick[params[1]] = 0;
	InteriorChange[params[1]] = 0;
	RemoveCheck[params[1]] = 0;
	AirFix[params[1]] = 0;
	DeathTimes[params[1]] = false;
	GunNoCheck[params[1]] = 0;
	ToBan[params[1]] = 0;
	Health[params[1]] = 100.0;
	EnterCar[params[1]] = -1;
	afk[params[1]] = 0;
	g_Invoke->callNative(&PAWN::SetPlayerHealth,params[1],Health[params[1]]);
	for(int i =0;i <47;i++)
	{
		 PlayerWeapon[params[1]][i] = 0;
		 PlayerAmmo[params[1]][i] = 0;
	}
    return 1;
}

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerDisconnect(AMX* amx, cell* params)
{
	 IsConnected[params[1]] = false;
	 return 1;
}

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerDeath(AMX* amx, cell* params)
{
        InteriorChange[params[1]] = 4;

	if(DeathTimes[params[1]] == true) SafeKick(params[1],"Fake Kill",AC_DEATHFLOOD);
	DeathTimes[params[1]] = true;
	return 1;
}

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerSpawn(AMX* amx, cell* params)
{

	InteriorChange[params[1]] = 4;
	return 1;
}

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerStateChange(AMX* amx, cell* params)
{
	//if(params[3] == PLAYER_STATE_DRIVER) SaveCord[EnterCar[params[1]]] = 10;
	if(params[2] == PLAYER_STATE_DRIVER || params[2] == PLAYER_STATE_PASSENGER)
	{
		int id = g_Invoke->callNative(&PAWN::GetPlayerVehicleID,params[1]);
		if(id != EnterCar[params[1]]) SafeKick(params[1],"Телепорт в авто",1);
	}
	if((params[2] == 2 && params[3] == 3) || (params[2] == 3 && params[3] == 2)) SafeKick(params[1]," Флуд сменой мест",AC_STATEFLOOD);
	InteriorChange[params[1]] = 4;
        GunNoCheck[params[1]] = 4;
	return 1;
}

/*cell AMX_NATIVE_CALL _CreatePickup(AMX* amx, cell* params)
{
	int id = g_Invoke->callNative(&PAWN::CreatePickup,params[1],params[2],amx_ctof(params[3]),amx_ctof(params[4]),amx_ctof(params[5]),params[6]
	Pickup[id].x = amx_ctof(params[3]);
	Pickup[id].y = amx_ctof(params[4]);
	Pickup[id].z = amx_ctof(params[5]);
	return id;
}

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerPickup(AMX* amx, cell* params)
{
	if(!IsPlayerInPoint(params[1],10.0,Pickup[params[2]].x,Pickup[params[2]].y,Pickup[params[2]].z)
	
	) return 1;
	return 1;
}*/

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerEnterVehicle(AMX* amx, cell* params)
{
        GunNoCheck[params[1]] = 4;
	EnterCar[params[1]] = params[2];
	return 1;
}
cell AMX_NATIVE_CALL _CreateVehicle(AMX* amx, cell* params)
{
	int id = g_Invoke->callNative(&PAWN::CreateVehicle,params[1],amx_ctof(params[2]),amx_ctof(params[3]),amx_ctof(params[4]),amx_ctof(params[5]),params[6],params[7],params[8]);
	TOTALCARS ++;
	Vehicle[id].posx = amx_ctof(params[2]);
	Vehicle[id].posy = amx_ctof(params[3]);
	Vehicle[id].posz = amx_ctof(params[4]);
	Vehicle[id].posa = amx_ctof(params[5]);
	return id;
}

cell AMX_NATIVE_CALL _AddStaticVehicle (AMX* amx, cell* params)
{
	g_Invoke->callNative(&PAWN::AddStaticVehicle,params[1],amx_ctof(params[2]),amx_ctof(params[3]),amx_ctof(params[4]),amx_ctof(params[5]),params[6],params[7]);
	TOTALCARS ++;
	Vehicle[TOTALCARS].posx = amx_ctof(params[2]);
	Vehicle[TOTALCARS].posy = amx_ctof(params[3]);
	Vehicle[TOTALCARS].posz = amx_ctof(params[4]);
	Vehicle[TOTALCARS].posa = amx_ctof(params[5]);
	return 1;
}

cell AMX_NATIVE_CALL _AddStaticVehicleEx (AMX* amx, cell* params)
{
	int id = g_Invoke->callNative(&PAWN::AddStaticVehicleEx,params[1],amx_ctof(params[2]),amx_ctof(params[3]),amx_ctof(params[4]),amx_ctof(params[5]),params[6],params[7],params[8]);
	TOTALCARS ++;
	Vehicle[id].posx = amx_ctof(params[2]);
	Vehicle[id].posy = amx_ctof(params[3]);
	Vehicle[id].posz = amx_ctof(params[4]);
	Vehicle[id].posa = amx_ctof(params[5]);
	return id;
}

cell AMX_NATIVE_CALL _DestroyVehicle (AMX* amx, cell* params)
{
	if(g_Invoke->callNative(&PAWN::DestroyVehicle,params[1]))
	{
		TOTALCARS --;
		return 1;
	}	
	return 0;

}

cell AMX_NATIVE_CALL _SetVehiclePos (AMX* amx, cell* params)
{
	for(int i = 0;i < MAX_PLAYERS;i++)
	{
		if(g_Invoke->callNative(&PAWN::GetPlayerVehicleID,i) == params[1])
		{
			InteriorChange[i] = 4;
			PlayerAir[i].posx = amx_ctof(params[2]);
	        PlayerAir[i].posy = amx_ctof(params[3]);
	        PlayerAir[i].posz = amx_ctof(params[4]);


		}
	}
	g_Invoke->callNative(&PAWN::SetVehiclePos,params[1],amx_ctof(params[2]),amx_ctof(params[3]),amx_ctof(params[4]));
	Vehicle[params[1]].posx = amx_ctof(params[2]);
	Vehicle[params[1]].posy = amx_ctof(params[3]);
	Vehicle[params[1]].posz = amx_ctof(params[4]);
	return 1;
}

cell AMX_NATIVE_CALL _SetVehicleToRespawn (AMX* amx, cell* params)
{
	SaveCord[params[1]] = 2;
	g_Invoke->callNative(&PAWN::SetVehicleToRespawn,params[1]);
	return 1;
}

cell AMX_NATIVE_CALL _OnUnoccupiedVehicleUpdate(AMX* amx, cell* params)
{
	int model = g_Invoke->callNative(&PAWN::GetVehicleModel,params[1]);
	if(model == 435 || model == 450 || model ==  584|| model == 591|| model == 606|| model == 608|| model == 610|| model == 611) goto update;
	if(SaveCord[params[1]] > 0) return 1;
	if(!IsVehicleInPoint(params[1],1.0,Vehicle[params[1]].posx,Vehicle[params[1]].posy,Vehicle[params[1]].posz))
	{

			g_Invoke->callNative(&PAWN::SetVehiclePos,params[1],Vehicle[params[1]].posx,Vehicle[params[1]].posy,Vehicle[params[1]].posz);
			g_Invoke->callNative(&PAWN::SetVehicleZAngle,params[1],Vehicle[params[1]].posa);
			return 1;

		
	}

	
    update:

	float x[3];float z;
	g_Invoke->callNative(&PAWN::GetVehicleZAngle,params[1],&z);
	g_Invoke->callNative(&PAWN::GetVehiclePos,params[1],&x[0],&x[1],&x[2]);
	Vehicle[params[1]].posx = x[0];
	Vehicle[params[1]].posy = x[1];
	Vehicle[params[1]].posz = x[2];
	Vehicle[params[1]].posa = z;
	return 1;
}

cell AMX_NATIVE_CALL TEST_Hook_OnRconLoginAttempt(AMX* amx, cell* params)
{
	if(params[3] == false)
	{
		char pip[16];
		char* text = NULL;
        amx_StrParam(amx, params[1], text);
		for(int i = 0;i < MAX_PLAYERS;i++)
		{
			if(!IsConnected[i]) continue;
			if(ToKick[i] != 0) continue;
			g_Invoke->callNative(&PAWN::GetPlayerIp,i,pip,16);
			if(strcmp(text,pip) == 0)
			{
				SafeKick(i,"Взлом rcon",AC_RCON);
				break;
			}
		}
		
	}
	return 1;
}

cell AMX_NATIVE_CALL TEST_Hook_OnPlayerUpdate(AMX* amx, cell* params)
{
	tick[params[1]] = g_Invoke->callNative(&PAWN::GetTickCount);
	return 1;
}
void CheatConfig(int playerid)
{
	char mes[800];
    sprintf(mes,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",\
		F[0] != 0 ?(F[0] != 2 ? (F[0] != 3 ? "Античит на оружие\t\t\t\t{D71515}|Выключить|(Кик)":"Античит на оружие\t\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на оружие\t\t\t\t{D71515}|Выключить|(Бан)") : "Античит на оружие\t\t\t\t{15D72B}|Включить|",\
		F[1] != 0 ? (F[1] != 2 ?(F[1] != 3 ? "Античит на Fly Hack\t\t\t\t{D71515}|Выключить|(Кик)": "Античит на Fly Hack\t\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на Fly Hack\t\t\t\t{D71515}|Выключить|(Бан)") : "Античит на Fly Hack\t\t\t\t{15D72B}|Включить|",\
		F[2] != 0 ? (F[2] != 2 ?(F[2] != 3 ? "Античит на Air Brake\t\t\t\t{D71515}|Выключить|(Кик)": "Античит на Air Brake\t\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на Air Brake\t\t\t\t{D71515}|Выключить|(Бан)") : "Античит на Air Brake\t\t\t\t{15D72B}|Включить|",\
		F[3] != 0 ? (F[3] != 2 ?(F[3] != 3 ? "Античит на Speed Hack\t\t\t\t{D71515}|Выключить|(Кик)": "Античит на Speed Hack\t\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на Speed Hack\t\t\t\t{D71515}|Выключить|(Бан)") : "Античит на Speed Hack\t\t\t\t{15D72B}|Включить|",\
		F[4] != 0 ? (F[4] != 2 ?(F[4] != 3 ? "Античит на JetPack\t\t\t\t{D71515}|Выключить|(Кик)": "Античит на JetPack\t\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на JetPack\t\t\t\t{D71515}|Выключить|(Бан)") : "Античит на JetPack\t\t\t\t{15D72B}|Включить|",\
		F[5] != 0 ? (F[5] != 2 ?(F[5] != 3 ? "Античит на Fake Kill\t\t\t\t{D71515}|Выключить|(Кик)": "Античит на Fake Kill\t\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на Fake Kill\t\t\t\t{D71515}|Выключить|(Бан)") : "Античит на Fake Kill\t\t\t\t{15D72B}|Включить|",\
		F[6] != 0 ? (F[6] != 2 ?(F[6] != 3 ? "Античит на Флуд сменой мест\t\t\t{D71515}|Выключить|(Кик)": "Античит на Флуд сменой мест\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на Флуд сменой мест\t\t\t{D71515}|Выключить|(Бан)") : "Античит на Флуд сменой мест\t\t\t{15D72B}|Включить|",\
		F[7] != 0 ? (F[7] != 2 ?(F[7] != 3 ? "Античит на Взлом RCON\t\t\t{D71515}|Выключить|(Кик)": "Античит на Взлом RCON\t\t\t{D71515}|Выключить|(Сообщ админу)") : "Античит на Взлом RCON\t\t\t{D71515}|Выключить|(Бан)") : "Античит на Взлом RCON\t\t\t{15D72B}|Включить|",\
		F[8] != 0 ? (F[8] != 2 ?(F[8] != 3 ? "Античит на NOP RemovePlayerFromVehicle\t{D71515}|Выключить|(Кик)": "Античит на NOP RemovePlayerFromVehicle\t{D71515}|Выключить|(Сообщ админу)") : "Античит на NOP RemovePlayerFromVehicle\t{D71515}|Выключить|(Бан)") : "Античит на NOP RemovePlayerFromVehicle\t{15D72B}|Включить|");
		
    

	g_Invoke->callNative(&PAWN::ShowPlayerDialog,playerid,7230,2,"Keeper AntiCheat",mes,"Ок","Отмена");

		
		

}
int list;
cell AMX_NATIVE_CALL TEST_Hook_OnDialogResponse(AMX* amx, cell* params)
{
	if(params[2] == 7230 && g_Invoke->callNative(&PAWN::IsPlayerAdmin,params[1]) && params[3] == 1)
	{
		if(F[params[4]] == 0)
		{
			list = params[4];
			g_Invoke->callNative(&PAWN::ShowPlayerDialog,params[1],7231,2,"Keeper AntiCheat - наказание","Кик\nБан\nСообщение rcon администратору","ОК","Назад");
			return 1;
		}
		else
		{
			F[params[4]] = 0;
			std::fstream out;
			out.open("keeper/config.cfg",std::fstream::out);
			out << F[0] << std::endl << F[1] << std::endl << F[2] << std::endl << F[3] << std::endl << F[4] << std::endl << F[5] << std::endl << F[6] << std::endl << F[7] << std::endl << F[8] <<std::endl;
			out.close();
			CheatConfig(params[1]);
			return 1;
		}
	}
	else if(params[2] == 7231 && g_Invoke->callNative(&PAWN::IsPlayerAdmin,params[1]))
	{
		if(params[3] == 1)
		{
			F[list] = params[4] + 1;
			std::fstream out;
			out.open("keeper/config.cfg",std::fstream::out);
			out << F[0] << std::endl << F[1] << std::endl << F[2] << std::endl << F[3] << std::endl << F[4] << std::endl << F[5] << std::endl << F[6] << std::endl << F[7] << std::endl << F[8] <<std::endl;
			out.close();
			CheatConfig(params[1]);
			return 1;
		}
		else
		{
			CheatConfig(params[1]);
			return 1;
		}
	}
	return 1;
}


cell AMX_NATIVE_CALL TEST_Hook_OnPlayerCommandText(AMX* amx, cell* params)
{
	char* text = NULL;
    amx_StrParam(amx, params[2], text);
	if(strcmp(text,"!keeper") == 0 && g_Invoke->callNative(&PAWN::IsPlayerAdmin,params[1]))
	{
		CheatConfig(params[1]);
		return 0;
	}
	return 0;
}
bool IsAPlane(int carid)
{
	int a = g_Invoke->callNative(&PAWN::GetVehicleModel,carid);
	if(a = 592 || a == 577 || a == 511 || a == 512 || a == 593 || a == 520 || a == 553\
		|| a == 476 || a == 519 || a == 460 || a == 513 || a == 548 || a == 417\
		|| a == 487 || a == 488 || a == 497 || a == 563 || a == 447 || a == 469 || a == 425)return true;
	return false;
}

float GetVehicleSpeed(int playerid)
{
	float Coord[4];
	g_Invoke->callNative(&PAWN::GetVehicleVelocity,playerid, &Coord[0], &Coord[1], &Coord[2]);
	Coord[3] = sqrt(Coord[0] * Coord[0] + Coord[1] * Coord[1] + Coord[2] * Coord[2]) * 213;
    return Coord[3];
}
cell AMX_NATIVE_CALL TEST_Hook_OnPlayerExitVehicle(AMX* amx, cell* params)
{
	if(GetVehicleSpeed(params[2]) > 2.0) SaveCord[EnterCar[params[1]]] = 10;
	else
	{
		 float x[3];float z;
		 g_Invoke->callNative(&PAWN::GetVehiclePos,params[2],&x[0],&x[1],&x[2]);
		 g_Invoke->callNative(&PAWN::GetVehicleZAngle,params[2],&z);
		 Vehicle[params[2]].posx = x[0];
		 Vehicle[params[2]].posy = x[1];
		 Vehicle[params[2]].posz = x[2];
		 Vehicle[params[2]].posa = z;
	}
	EnterCar[params[1]] = -1;
	if(IsAPlane(params[2]))
       { 
            GunNoCheck[params[1]] = 4;
            PlayerWeapon[params[1]][46] = 1;
            PlayerAmmo[params[1]][46] = 1;
       }
	return 1;
}

cell AMX_NATIVE_CALL TEST_Hook_OnVehicleSpawn (AMX* amx, cell* params)
{
	SaveCord[params[1]] = 3;
	return 1;
}
PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{

    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}


PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{

    g_Invoke = new Invoke;


	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];
	
	std::fstream in;
 
	in.open("keeper/config.cfg",std::fstream::in);
 
	in >> F[0] >> F[1] >> F[2] >> F[3] >> F[4] >> F[5] >> F[6] >> F[7] >> F[8];

	in.close();

	in.open("keeper/admins.cfg",std::fstream::in);
	for(int i = 0;i < 30;i++)
	{
		in >> admin[i];
	}
	in.close();
    logprintf("========= Keeper Anticheat загружен =========");
	logprintf("*         Версия: 0.8a         *");
	logprintf("******* Автор: Tumba *******");
	logprintf("==============================================");
	
	
    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    logprintf("* Keeper Anticheat успешно выгружен!");
}


AMX_NATIVE_INFO PluginNatives[] =
{
    {"Invoke_GetAddresses", Invoke_GetAddresses},
    {"TEST_Hook_OnPlayerConnnect", TEST_Hook_OnPlayerConnnect},
	{"TEST_Hook_OnPlayerDisconnect",TEST_Hook_OnPlayerDisconnect},
	{"_ResetMoney",_ResetMoney},
	{"_GiveMoney",_GiveMoney},
	{"_GetMoney",_GetMoney},
	{"_SetPlayerHealth",_SetPlayerHealth},
	{"_GetPlayerHealth",_GetPlayerHealth},
	{"_SetPlayerArmour",_SetPlayerArmour},
	{"_GetPlayerArmour",_GetPlayerArmour},
	{"_GivePlayerWeapon",_GivePlayerWeapon},
	{"_ResetPlayerWeapons",_ResetPlayerWeapons},
	{"_SetPlayerPos",_SetPlayerPos},
	{"_PutPlayerInVehicle",_PutPlayerInVehicle},
	{"_RemovePlayerFromVehicle",_RemovePlayerFromVehicle},
	{"TEST_Hook_OnPlayerDeath",TEST_Hook_OnPlayerDeath},
	{"TEST_Hook_OnPlayerSpawn",TEST_Hook_OnPlayerSpawn},
	{"TEST_Hook_OnPlayerStateChange",TEST_Hook_OnPlayerStateChange},
	{"TEST_Hook_OnRconLoginAttempt",TEST_Hook_OnRconLoginAttempt},
	{"TEST_Hook_OnPlayerCommandText",TEST_Hook_OnPlayerCommandText},
	{"TEST_Hook_OnPlayerExitVehicle",TEST_Hook_OnPlayerExitVehicle},
	{"TEST_Hook_OnDialogResponse",TEST_Hook_OnDialogResponse},
	{"_SetPlayerPosFindZ",_SetPlayerPosFindZ},
	{"TEST_Hook_OnPlayerEnterVehicle",TEST_Hook_OnPlayerEnterVehicle},
	{"_OnUnoccupiedVehicleUpdate",_OnUnoccupiedVehicleUpdate},
	{"_CreateVehicle",_CreateVehicle},
	{"_AddStaticVehicle",_AddStaticVehicle},
	{"_AddStaticVehicleEx",_AddStaticVehicleEx},
	{"_SetVehiclePos",_SetVehiclePos},
	{"_DestroyVehicle",_DestroyVehicle},
	{"_SetVehicleToRespawn",_SetVehicleToRespawn},
	{"TEST_Hook_OnVehicleSpawn",TEST_Hook_OnVehicleSpawn},
	{"TEST_Hook_OnPlayerUpdate",TEST_Hook_OnPlayerUpdate},
    {0, 0}
};


PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{

    g_Invoke->amx_list.push_back(amx);
    return amx_Register(amx, PluginNatives, -1);
}


PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{

    for(std::list<AMX *>::iterator i = g_Invoke->amx_list.begin(); i != g_Invoke->amx_list.end(); ++i)
    {
        if(*i == amx)
        {
            g_Invoke->amx_list.erase(i);
            break;
        }
    }
    return AMX_ERR_NONE;
}


void CheckHealth(int playerid)
{
	float
		health;
	g_Invoke->callNative(&PAWN::GetPlayerHealth,playerid,&health);
	if(health < Health[playerid])
	{
		Health[playerid] = health;
		g_Invoke->callNative(&PAWN::SetPlayerHealth,playerid,Health[playerid]);
	}
	else if (health > Health[playerid])
	{
		g_Invoke->callNative(&PAWN::SetPlayerHealth,playerid,Health[playerid]);
	}
}

void CheckArmour(int playerid)
{
	float armour;
	g_Invoke->callNative(&PAWN::GetPlayerArmour,playerid,&armour);
	if(armour < Armour[playerid])
	{
		Armour[playerid] = armour;
		g_Invoke->callNative(&PAWN::SetPlayerArmour,playerid,Armour[playerid]);
	}
	else if (armour > Armour[playerid])
	{
		g_Invoke->callNative(&PAWN::SetPlayerArmour,playerid,Armour[playerid]);
	}
}


float GetPlayerSpeed(int playerid)
{
    float Coord[4];
	if(g_Invoke->callNative(&PAWN::GetPlayerState,playerid) == PLAYER_STATE_DRIVER || g_Invoke->callNative(&PAWN::GetPlayerState,playerid) == PLAYER_STATE_PASSENGER)
	{
		int veh = g_Invoke->callNative(&PAWN::GetPlayerVehicleID,playerid);
		g_Invoke->callNative(&PAWN::GetVehicleVelocity,veh, &Coord[0], &Coord[1], &Coord[2]);
	}
	else g_Invoke->callNative(&PAWN::GetPlayerVelocity,playerid, &Coord[0], &Coord[1], &Coord[2]);
    Coord[3] = sqrt(Coord[0] * Coord[0] + Coord[1] * Coord[1] + Coord[2] * Coord[2]) * 213;
    return Coord[3];
}



bool IsAdmin(int playerid)
{
	char pName[32];
	g_Invoke->callNative(&PAWN::GetPlayerName,playerid,pName,32);
	for(int i = 0;i <30;i++)
	{
		if( pName == admin[i]) return 1;

	}
	return 0;


}

void FlyCheck(int playerid)
{
	  char AnimLib[30], AnimName[30];
	  int a = g_Invoke->callNative(&PAWN::GetPlayerAnimationIndex,playerid);
	  g_Invoke->callNative(&PAWN::GetAnimationName,a, AnimLib, sizeof(AnimLib), AnimName, sizeof(AnimName));
	  if(GetPlayerSpeed(playerid) > 60.0 && strcmp(AnimLib, "SWIM") == 0 && strcmp(AnimName, "SWIM_CRAWL") == 0)
	  {
		  SafeKick(playerid,"Fly Hack>>",AC_FLY);
   
      }
	  
}



void CheckGun(int playerid)
{
	int weapons[13] = {0};
	int ammos[13] = {0};
	bool cheat = false;
	for (int i = 0; i < 13; i++)
    {
	      g_Invoke->callNative(&PAWN::GetPlayerWeaponData,playerid, i, &weapons[i], &ammos[i]);
		  if(weapons[i] != 0)
		  {
			 
			   for(int q = 0;q < 47;q++)
		       {

			       if(PlayerWeapon[playerid][q] == 1 && q == weapons[i])
				   {
					   cheat = false;
					   break;
				   }
				   cheat = true;
				   
		       }
			   if(cheat == true)
			   {
				   SafeKick(playerid,"Gun Cheat",AC_GUN);
				   return;
			   }
		  }
				  
    }	 
	
}


void CheckAmmo(int playerid)
{

	int weapon = g_Invoke->callNative(&PAWN::GetPlayerWeapon,playerid);
	int ammo = g_Invoke->callNative(&PAWN::GetPlayerAmmo,playerid);
	if(weapon != 0 && weapon != 46)
	{
		 if(ammo < PlayerAmmo[playerid][weapon])
		 {
			 PlayerAmmo[playerid][weapon] = ammo;
		 }
		 if(ammo > PlayerAmmo[playerid][weapon])
		 {
			 SafeKick(playerid,"Ammo Cheat",AC_GUN);
		 }
		
	}
}



void AirCheck(int playerid)
{
	int a = g_Invoke->callNative(&PAWN::GetPlayerAnimationIndex,playerid);
	if(AirFix[playerid] > 0) AirFix[playerid] --;
	if(g_Invoke->callNative(&PAWN::GetPlayerSurfingVehicleID,playerid) != INVALID_VEHICLE_ID) goto update;
        if(IsPlayerInPoint(playerid,10.0,617.5318,-1.9900,1000.6396)|| IsPlayerInPoint(playerid,10.0,615.2845,-124.2390,997.6873)\
        || IsPlayerInPoint(playerid,10.0,617.5356,-1.9900,1000.7245) || IsPlayerInPoint(playerid,10.0,616.7833,-74.8150,997.7633)) goto update;
	if(InteriorChange[playerid] > 0)
	{
			InteriorChange[playerid] --;
                        goto update;
			return;

	}
	if(IsAdmin(playerid)) goto update;
	if(afk[playerid] > 0 ) goto update;
	if(!IsPlayerInPoint(playerid,400.0,PlayerAir[playerid].posx,PlayerAir[playerid].posy,PlayerAir[playerid].posz))
	{
		
		SafeKick(playerid,"Teleport",AC_AIR);
		return;
	}
	if( a== 958 || a == 959 || a == 961 || a == 962 || a == 965 || a == 971 || a == 1126 || a == 1130 || a == 1132 || a ==1134 || a == 1156 || a == 1208) goto update;

	if(!IsPlayerInPoint(playerid,30.0,PlayerAir[playerid].posx,PlayerAir[playerid].posy,PlayerAir[playerid].posz))
	{
		if(AirFix[playerid] == 0)
		{
			AirFix[playerid] = 10;
			goto update;
		}
		
		SafeKick(playerid,"Air Break",AC_AIR);
		return;
	}
	update:
	float cord[3];
	g_Invoke->callNative(&PAWN::GetPlayerPos,playerid,&cord[0],&cord[1],&cord[2]);
	PlayerAir[playerid].posx = cord[0];
	PlayerAir[playerid].posy = cord[1];
	PlayerAir[playerid].posz = cord[2];
}

void CarAir(int playerid)
{
	if(InteriorChange[playerid] > 0)
	{
			InteriorChange[playerid] --;
                        goto update;
			return;

	}
	if(IsAdmin(playerid)) goto update;
        if(IsPlayerInPoint(playerid,10.0,617.5318,-1.9900,1000.6396)|| IsPlayerInPoint(playerid,10.0,615.2845,-124.2390,997.6873)\
        || IsPlayerInPoint(playerid,10.0,617.5356,-1.9900,1000.7245) || IsPlayerInPoint(playerid,10.0,616.7833,-74.8150,997.7633)) goto update;
	if(afk[playerid] > 0 ) goto update;
	if(!IsPlayerInPoint(playerid,10.0,PlayerAir[playerid].posx,PlayerAir[playerid].posy,PlayerAir[playerid].posz) &&  g_Invoke->callNative(&PAWN::GetPlayerState,playerid) == PLAYER_STATE_DRIVER && GetPlayerSpeed(playerid) < 10.0f )
	{
			SafeKick(playerid,"Air Break",AC_AIR);
			return;
		
	}
        else goto update;

	update:{
	float cord[3];
	g_Invoke->callNative(&PAWN::GetPlayerPos,playerid,&cord[0],&cord[1],&cord[2]);
	PlayerAir[playerid].posx = cord[0];
	PlayerAir[playerid].posy = cord[1];
	PlayerAir[playerid].posz = cord[2];}
}

void CheckSpeed(int playerid)
{
        if(InteriorChange[playerid] != 0) return;
	float s[3];
	int vid = g_Invoke->callNative(&PAWN::GetPlayerVehicleID,playerid);
	int model = g_Invoke->callNative(&PAWN::GetVehicleModel,vid);
	g_Invoke->callNative(&PAWN::GetVehicleVelocity,vid,&s[0],&s[1],&s[2]);
	s[0] *= s[0];
	s[1] *= s[1];
	if(s[2] < -0.023f)
	{
		 if(sqrt(s[0]+s[1])*174.0f > (MaxVehicleSpeed[model]+40.00f))
		 {
				SafeKick(playerid,"Speed Hack",AC_SPEED);
				return;
		 }
	}
	else
	{
		s[2] *= s[2];
		if(sqrt(s[0]+s[1]+s[2])*174.0f > (MaxVehicleSpeed[model]+40.00f))
		{
				SafeKick(playerid,"Speed Hack",AC_SPEED);
				return;
		}
	}					
}

void CheckLag(int playerid)
{
	if(g_Invoke->callNative(&PAWN::GetTickCount) - tick[playerid] >=1000) afk[playerid] = 3;
	else if(afk[playerid] > 0) afk[playerid] --;

}


PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
     
	static int timer = 0;
	static int time = 0;
	timer ++;
	time ++;
/*	for(int i = 0;i < MAX_PLAYERS;i++)
	{ 
		if(!IsConnected[i]) continue;
		if(ToKick[i] == 0) continue;
		g_Invoke->callNative(&PAWN::ResetPlayerWeapons,i);
	}*/
	if(time == 200)
	{
		  for(int i = 0;i < MAX_VEHICLES;i++)
		  {
			  if(SaveCord[i] > 0)
			  {
				  SaveCord[i] --;
				  if(SaveCord[i] == 0)
				  {
					  float x[3];float z;
					  g_Invoke->callNative(&PAWN::GetVehiclePos,i,&x[0],&x[1],&x[2]);
					  g_Invoke->callNative(&PAWN::GetVehicleZAngle,i,&z);
					  Vehicle[i].posx = x[0];
					  Vehicle[i].posy = x[1];
					  Vehicle[i].posz = x[2];
					  Vehicle[i].posa = z;
					  
				  }

			  }
		  }
		  for(int i = 0;i < MAX_PLAYERS;i++)
		  {
			  if(!IsConnected[i]) continue;
			  
			  if(ToKick[i] > 0)
			  {
			      ToKick[i] -= 1;
				  if(ToKick[i] == 0) g_Invoke->callNative(&PAWN::Kick,i);
				  continue;
			  }
			  if(ToBan[i] > 0)
			  {
			      ToBan[i] -= 1;
				  if(ToBan[i] == 0) g_Invoke->callNative(&PAWN::Ban,i);
				  continue;
			  }
			  CheckLag(i);
			  if(g_Invoke->callNative(&PAWN::GetPlayerState,i) == PLAYER_STATE_ONFOOT) AirCheck(i);
			  if(GunNoCheck[i] <= 0 && g_Invoke->callNative(&PAWN::GetPlayerState,i) == PLAYER_STATE_ONFOOT)
			  {
				   CheckGun(i); 
			       CheckAmmo(i);
			  }
			  else  GunNoCheck[i] --;
			//  CheckHealth(i);
			//  CheckArmour(i);
			  if(g_Invoke->callNative(&PAWN::GetPlayerSpecialAction,i) == 2)
			  {
				  SafeKick(i,"JetPack",AC_JETPACK);
				  continue;
			  }
			  if(RemoveCheck[i] != 0)
			  {
				  if(g_Invoke->callNative(&PAWN::GetPlayerState,i) == PLAYER_STATE_ONFOOT)
				  {
					  RemoveCheck[i] = 0;
					  continue;
				  }
				  RemoveCheck[i] --;
				  if(RemoveCheck[i] == 0)
				  {
					   if(g_Invoke->callNative(&PAWN::GetPlayerState,i) == PLAYER_STATE_DRIVER)
					   {
						   SafeKick(i,"Анти RemovePlayerFromVehicle",AC_REMOVE);
					       continue;
					   }
				  }
				 
			  }
		  }
		  time = 0;
	}
	if(timer == 20)
	{
		  for(int i = 0;i < MAX_PLAYERS;i++)
		  {
			  if(!IsConnected[i]) continue;
			  DeathTimes[i] = false;
			  if(ToKick[i] > 0) continue;
			  if(g_Invoke->callNative(&PAWN::GetPlayerState,i) == PLAYER_STATE_DRIVER && !IsAdmin(i)) CheckSpeed(i);
			  if(!IsAdmin(i)) FlyCheck(i);
			  if(g_Invoke->callNative(&PAWN::GetPlayerState,i) == PLAYER_STATE_DRIVER || g_Invoke->callNative(&PAWN::GetPlayerState,i) == PLAYER_STATE_PASSENGER) CarAir(i);

		  }
		  timer = 0;
	}
  
}


