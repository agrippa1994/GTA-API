/**
 * SAMP 0.3z API by Slider
 * 
 * @author			Slider
 * @date			2014-05-09
 * @copyright		(c) by Slider - www.gta-api.de
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */

// includes
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <tlhelp32.h>
#include <cstdint>
#include <fstream>
#include <Lmcons.h>
#include <tchar.h>
#include <ctime>
#include "main.h"
#include "memorymanagement.h"

// settings
#define MAX_PLAYER_NAME 32

// cped
#define CPED_POINTER_ADDR 0xB6F5F0
#define CPED_HEALTH_ADDR 0x540
#define CPED_ARMOUR_ADDR 0x548

// vehicle
#define VEHICLE_POINTER_ADDR 0xBA18FC
#define VEHICLE_HEALTH_ADDR 0x4C0
#define VEHICLE_ID_ADDR 0x0
#define VEHICLE_MODEL_ID_ADDR 0xB700F0
#define VEHICLE_SIREN_STATE_ADDR 0x42D
#define VEHICLE_GASPEDAL_STATE_ADDR 0x49C
#define VEHICLE_LOCK_STATE_ADDR 0x4F8
#define VEHICLE_ENGINE_STATE_ADDR 0x428

// samp
#define SAMP_SENDCHAT_FUNC_ADDR 0x4CA0
#define SAMP_SENDCMD_FUNC_ADDR 0x7BDD0
#define SAMP_ADDCHATMSG_FUNC_ADDR 0x7AA00
#define SAMP_CHATINFO_ADDR 0x212A6C
#define SAMP_PLAYERNAME_ADDR 0x2123F7
#define SAMP_SERVERIP_ADDR 0x2121F5
#define SAMP_SERVERPORT_ADDR 0x2122F6
#define SAMP_INCHAT_POINTER_ADDR 0x212A94
#define SAMP_INCHAT_POINTER_OFF_ADDR 0x55

// gta
#define PLAYER_SKIN_ADDR 0xC1399A2
#define PLAYER_POS_X 0xB6F2E4
#define PLAYER_POS_Y 0xB6F2E8
#define PLAYER_POS_Z 0xB6F2EC
#define PLAYER_INTERIOR_ADDR 0xA4ACE8
#define PLAYER_VEHICLE_ID 0x488078
#define PLAYER_STATE_ADDR 0x530

// interface
#define INTERFACE_HEALTH_COLOR_ADDR 0xBAB22C
#define INTERFACE_MONEY_COLOR_ADDR 0xBAB230
#define INTERFACE_WANTEDLEVEL_COLOR_ADDR 0xBAB244

// random shit
#define API_VERSION "1.0.0 Alpha 9"

// namespaces
using namespace std;

// prototypes
int CheckHandles();
int GetGTAProcessID();
void ConvertHexToRGB(int, int&, int&, int&);

// variables
char ProcessName[20];
DWORD addr;
DWORD buffer;
DWORD puffer;
DWORD gtaProcessID;
DWORD sampDLL;
HANDLE gtaHandle;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SA:MP Functions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * int API_GetPlayerName(char*& name)
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 *
 * @params			name			char*&
 */
int API_GetPlayerName(char *&playername) {
	char name[MAX_PLAYER_NAME] = { "Unknown" };

	if (CheckHandles()) {
		addr = sampDLL + SAMP_PLAYERNAME_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(addr), (LPVOID)name, sizeof(name), NULL);

		memcpy(playername, name, sizeof(name));
		
		return 1;
	}

	return 0;
}

/**
 * int API_SendChat(char* text)
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 *
 * @params			text			char*
 */
int API_SendChat(char *text) {
	if (CheckHandles()) {
		int length = strlen(text) + 1;

		if (text[0] != '/') {
			PVOID cmd = VirtualAllocEx(gtaHandle, 0, length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			WriteProcessMemory(gtaHandle, cmd, text, length, 0);
 
			DWORD func = sampDLL + SAMP_SENDCHAT_FUNC_ADDR;

			HANDLE hThread = CreateRemoteThread(gtaHandle, 0, 0, (LPTHREAD_START_ROUTINE)func, cmd, 0, 0);
			DWORD dwExitCode = 0;
			
			if (hThread) {
				WaitForSingleObject(hThread, INFINITE);
				GetExitCodeThread(hThread, &dwExitCode);
			}
 
			VirtualFreeEx(gtaHandle, cmd, length, MEM_RELEASE);
			CloseHandle(hThread);

			return 1;
		}
		else {
			PVOID cmd = VirtualAllocEx(gtaHandle, 0, length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			WriteProcessMemory(gtaHandle, cmd, text, length, 0);
 
			DWORD func = sampDLL + SAMP_SENDCMD_FUNC_ADDR;

			HANDLE hThread = CreateRemoteThread(gtaHandle, 0, 0, (LPTHREAD_START_ROUTINE)func, cmd, 0, 0);
			DWORD dwExitCode = 0;
			
			if (hThread) {
				WaitForSingleObject(hThread, INFINITE);
				GetExitCodeThread(hThread, &dwExitCode);
			}
 
			VirtualFreeEx(gtaHandle, cmd, length, MEM_RELEASE);
			CloseHandle(hThread);

			return 1;
		}
	}

	return 0;
}

/**
 * int API_AddChatMessage(char* text)
 *
 * @author			Slider
 * @date			2014-05-11
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 *
 * @params			text			char*
 */
/*
int API_AddChatMessage(char *text) {
	if (CheckHandles()) {
		uint32_t	chatinfo = sampDLL + SAMP_CHATINFO_ADDR;
		uint32_t	func = sampDLL + SAMP_ADDCHATMSG_FUNC_ADDR;

		__asm mov eax, dword ptr[chatinfo]
		__asm mov ecx, dword ptr[eax]
		__asm push 0
		__asm push 0xFFFFFF
		__asm push 0
		__asm push text
		__asm push 12
		__asm call func
		*/
		/*
void addToChatWindow ( char *text, D3DCOLOR textColor, int playerID )
{
	if ( g_SAMP == NULL || g_Chat == NULL )
		return;

	if ( text == NULL )
		return;

	if ( playerID < -1 )
		playerID = -1;

	uint32_t	chatinfo = g_dwSAMP_Addr + SAMP_CHAT_INFO_OFFSET;
	uint32_t	func = g_dwSAMP_Addr + FUNC_ADDTOCHATWND;

	if ( playerID != -1 )
	{
		// getPlayerName does the needed validity checks, no need for doubles
		char *playerName = (char*)getPlayerName(playerID);
		if ( playerName == NULL )
			return;

		D3DCOLOR playerColor = samp_color_get(playerID);

		__asm mov eax, dword ptr[chatinfo]
		__asm mov ecx, dword ptr[eax]
		__asm push playerColor
		__asm push textColor
		__asm push playerName
		__asm push text
		__asm push 10
		__asm call func
//		__asm pop eax
//		__asm pop ecx
		return;
	}

	__asm mov eax, dword ptr[chatinfo]
	__asm mov ecx, dword ptr[eax]
	__asm push 0
	__asm push textColor
	__asm push 0
	__asm push text
	__asm push 12
	__asm call func
//	__asm pop eax
//	__asm pop ecx
	return;
}
*/
/*
		return 1;
	}

	return 0;
}
*/

/**
 * int API_GetServerIP()
 *
 * @author			Slider
 * @date			2014-05-30
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 *
 * @params			serverip			char*&
 */
int API_GetServerIP(char *&serverip) {
	char ip[15] = { "Unknown" };

	if (CheckHandles()) {
		addr = sampDLL + SAMP_SERVERIP_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(addr), (LPVOID)ip, sizeof(ip), NULL);

		memcpy(serverip, ip, sizeof(ip));

		return 1;
	}

	return 0;
}

/**
 * int API_GetServerPort()
 *
 * @author			Slider
 * @date			2014-05-29
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
/*
int API_GetServerPort() {
	if (CheckHandles()) {
		DWORD port;
		DWORD puffer;

		DWORD *addr = (DWORD*)sampDLL + SAMP_SERVERPORT_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(addr), &port, sizeof(puffer), NULL);

		return port;
	}

	return 0;
}
*/

/**
 * int API_IsChatOpen()
 *
 * @author			Slider
 * @date			2014-05-09
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
/*
int API_IsChatOpen() {
	if (CheckHandles()) {
		int result;

		DWORD *p = (DWORD*)SAMP_INCHAT_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		DWORD addrr = buffer + SAMP_INCHAT_POINTER_OFF_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(addrr), &result, sizeof(puffer),NULL);

		cout << "Chat: " << result << endl;

		DWORD *p = (DWORD*)sampDLL + SAMP_INCHAT_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, (LPVOID)buffer_two, sizeof(buffer_two), NULL);

		cout << "Ptr: " << p << " Buffer; " << buffer_two << endl;

		addr = buffer_two + SAMP_INCHAT_POINTER_OFF_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)addr, (LPVOID)result, sizeof(result), NULL);

		cout << "Addr: " << addr << " Result: " << result << endl;

		return 1;
	}

	return 0;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// GTA Functions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * int API_GetPlayerHealth()
 *
 * @author			Slider
 * @date			2014-05-09
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetPlayerHealth() {
	float health = 0;
	
	if (CheckHandles()) {
		// cped pointer
		DWORD puffer1;
		DWORD puffer2;
		DWORD buffer1;

		DWORD *p = (DWORD*)CPED_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer1, sizeof(puffer1),NULL);

		DWORD healthaddr = buffer1 + CPED_HEALTH_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(healthaddr), &health, sizeof(puffer2),NULL);
		
		return (int)health;
	}

	return 0;
}

/**
 * int API_GetPlayerArmour()
 *
 * @author			Slider
 * @date			2014-05-09
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetPlayerArmour() {
	float armour = 0.0;
	
	if (CheckHandles()) {
		// cped pointer
		DWORD puffer;
		DWORD buffer;

		DWORD *p = (DWORD*)CPED_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		DWORD healthaddr = buffer + CPED_ARMOUR_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(healthaddr), &armour, sizeof(puffer),NULL);
		
		return (int)armour;
	}

	return 0;
}

/**
 * float API_GetPlayerPos(float &x, float &y, float &z)
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetPlayerPos(float &x, float &y, float &z) {
	if (CheckHandles()) {
		DWORD puffer;

		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_POS_X, &x, sizeof(puffer),NULL);
		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_POS_Y, &y, sizeof(puffer),NULL);
		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_POS_Z, &z, sizeof(puffer),NULL);
		
		return 1;
	}

	return 0;
}

/**
 * int API_GetPlayerPosX()
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetPlayerPosX(float &position) {
	if (CheckHandles()) {
		DWORD puffer;

		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_POS_X, &position, sizeof(puffer),NULL);
		return 1;
	}

	return 0;
}

/**
 * int API_GetPlayerPosX()
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetPlayerPosY(float &position) {
	if (CheckHandles()) {
		DWORD puffer;

		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_POS_Y, &position, sizeof(puffer),NULL);
		return 1;
	}

	return 0;
}

/**
 * int API_GetPlayerPosZ()
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetPlayerPosZ(float &position) {
	if (CheckHandles()) {
		DWORD puffer;

		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_POS_Z, &position, sizeof(puffer),NULL);
		return 1;
	}

	return 0;
}

/**
 * int API_IsPlayerInAnyInterior()
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_IsPlayerInAnyInterior() {
	if (CheckHandles()) {
		DWORD puffer;
		int intid;

		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_INTERIOR_ADDR, &intid, sizeof(puffer), NULL);
		if (intid) {
			return 1;
		}
	}

	return 0;
}

/**
 * int API_GetInteriorID()
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetPlayerInteriorID() {
	if (CheckHandles()) {
		DWORD puffer;
		int intid;

		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_INTERIOR_ADDR, &intid, sizeof(puffer), NULL);
		return intid;
	}

	return 0;
}

/**
 * int API_GetPlayerSkin()
 *
 * @author			Slider
 * @date			2014-05-11
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
/*
int API_GetPlayerSkin() {
	if (CheckHandles()) {
		DWORD puffer;
		int skinid;

		ReadProcessMemory(gtaHandle, (DWORD*)PLAYER_SKIN_ADDR, &skinid, sizeof(puffer), NULL);
		return skinid;
	}

	return 0;
}
*/

/**
 * int API_IsPlayerInRangeOfPoint()
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_IsPlayerInRangeOfPoint(float x, float y, float z, float radius) {
	if (CheckHandles()) {
		DWORD puffer;
		float x1;
		float y1;
		float z1;

		float posX;
		float posY;
		float posZ;

		API_GetPlayerPos(x1, y1, z1);

		posX = x1 - x;
		posY = y1 - y;
		posZ = z1 - z;

		if (((posX < radius) && (posX > -radius)) && ((posY < radius) && (posY > -radius)) && ((posZ < radius) && (posZ > -radius))) {
			return 1;
		}
	}

	return 0;
}

/**
 * int API_IsPlayerDriver()
 *
 * @author			Slider
 * @date			2014-06-01
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
/*
int API_IsPlayerDriver() {
	if (CheckHandles()) {
		int state;

		DWORD *p = (DWORD*)CPED_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		DWORD healthaddr = buffer + PLAYER_STATE_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(healthaddr), &state, sizeof(puffer),NULL);
		
		if (state == 50) {
			return 1;
		}
	}

	return 0;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Vehicle Functions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * int API_GetVehicleID()
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
/*
int API_GetVehicleID() {
	if (CheckHandles()) {
		int vehicleID;
		DWORD puffer;

		ReadProcessMemory(gtaHandle, (DWORD*)(VEHICLE_ID_ADDR), &vehicleID, sizeof(puffer),NULL);
		
		if (API_IsPlayerInAnyVehicle()) {
			return vehicleID;
		}
	}

	return 0;
}
*/

/**
 * int API_GetLastVehicleID()
 *
 * @author			Slider
 * @date			2014-05-30
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
/*
int API_GetLastVehicleID() {
	if (CheckHandles()) {
		int vehicleID;
		DWORD puffer;

		ReadProcessMemory(gtaHandle, (DWORD*)(VEHICLE_ID_ADDR), &vehicleID, sizeof(puffer),NULL);
		
		return vehicleID;
	}

	return 0;
}
*/

/**
 * int API_GetVehicleHealth()
 *
 * @author			Slider
 * @date			2014-05-26
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetVehicleHealth() {
	float health = 0.0;
	
	if (CheckHandles()) {
		// cvehicle pointer
		DWORD puffer;
		DWORD buffer;

		DWORD *p = (DWORD*)VEHICLE_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		DWORD healthaddr = buffer + VEHICLE_HEALTH_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(healthaddr), &health, sizeof(puffer),NULL);

		return (int)health;
	}

	return 0;
}

/**
 * int API_IsPlayerInAnyVehicle()
 *
 * @author			Slider
 * @date			2014-05-26
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_IsPlayerInAnyVehicle() {
	float health = 0.0;
	
	if (CheckHandles()) {
		// cvehicle pointer
		DWORD puffer;
		DWORD buffer;

		DWORD *p = (DWORD*)VEHICLE_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		DWORD healthaddr = buffer + VEHICLE_HEALTH_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(healthaddr), &health, sizeof(puffer),NULL);

		if (health > 0.0) {
			return 1;
		}
	}

	return 0;
}

/**
 * int API_GetVehicleModelID()
 *
 * @author			Slider
 * @date			2014-05-30
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetVehicleModelID() {
	if (CheckHandles()) {
		int modelID;
		DWORD puffer;

		ReadProcessMemory(gtaHandle, (DWORD*)(VEHICLE_MODEL_ID_ADDR), &modelID, sizeof(puffer),NULL);
		
		if (API_IsPlayerInAnyVehicle()) {
			return modelID;
		}
	}

	return 0;
}

/**
 * int API_VehicleSirenStateChange()
 *
 * @author			Slider
 * @date			2014-05-31
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_VehicleSirenStateChange() {
	if (CheckHandles()) {
		int value;

		DWORD *p = (DWORD*)VEHICLE_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		DWORD sirenaddr = buffer + VEHICLE_SIREN_STATE_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(sirenaddr), &value, sizeof(value), NULL);

		cout << "ID: " << value << endl;
		
		// on to off
		if (value == -402643760) {
			value = -402643888;
			WriteProcessMemory(gtaHandle,(DWORD*)(sirenaddr),&value,sizeof(value),&puffer);
		}
		// of to on
		else if (value == -402643888) {
			value = -402643760;
			WriteProcessMemory(gtaHandle,(DWORD*)(sirenaddr),&value,sizeof(value),&puffer);
		}

		return 1;
	}

	return 0;
}

/**
 * int API_GetGasPedalState()
 *
 * @author			Slider
 * @date			2014-05-31
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetGasPedalState() {
	if (CheckHandles()) {
		float value;

		DWORD *p = (DWORD*)VEHICLE_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		addr = buffer + VEHICLE_GASPEDAL_STATE_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(addr), &value, sizeof(value), NULL);
		
		return (int)value;
	}

	return 0;
}

/**
 * int API_GetVehicleLockState()
 *
 * @author			Slider
 * @date			2014-05-31
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetVehicleLockState() {
	if (CheckHandles()) {
		DWORD value;

		DWORD *p = (DWORD*)VEHICLE_POINTER_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

		addr = buffer + VEHICLE_LOCK_STATE_ADDR;
		ReadProcessMemory(gtaHandle, (DWORD*)(addr), &value, sizeof(value), NULL);
		
		if (value == 2) {
			return 1;
		}
	}

	return 0;
}

/**
 * int API_GetVehicleEngineState()
 *
 * @author			Slider
 * @date			2014-06-01
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetVehicleEngineState() {
	if (CheckHandles()) {
		if (API_IsPlayerInAnyVehicle()) {
			int value;

			DWORD *p = (DWORD*)VEHICLE_POINTER_ADDR;
			ReadProcessMemory(gtaHandle, (DWORD*)p, &buffer, sizeof(puffer),NULL);

			addr = buffer + VEHICLE_ENGINE_STATE_ADDR;
			ReadProcessMemory(gtaHandle, (DWORD*)(addr), &value, sizeof(value), NULL);

			if (value == 19013656) {
				return 1;
			}
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Interface Functions
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * int API_SetInterfaceHealthColor()
 *
 * @author			Slider
 * @date			2014-05-31
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_SetInterfaceHealthColor(int color) {
	if (CheckHandles()) {
		int r, g, b;
		ConvertHexToRGB(color, r, g, b);

		byte write[] = { r, g, b, 1 };

		WriteProcessMemory(gtaHandle,(DWORD*)INTERFACE_HEALTH_COLOR_ADDR,&write,sizeof(write),&puffer);
		return 1;
	}

	return 0;
}

/**
 * int API_SetInterfaceMoneyColor()
 *
 * @author			Slider
 * @date			2014-05-31
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_SetInterfaceMoneyColor(int color) {
	if (CheckHandles()) {
		int r, g, b;
		ConvertHexToRGB(color, r, g, b);

		byte write[] = { r, g, b, 1 };
		WriteProcessMemory(gtaHandle,(DWORD*)INTERFACE_MONEY_COLOR_ADDR,&write,sizeof(write),&puffer);

		return 1;
	}

	return 0;
}

/**
 * int API_SetInterfaceWantedLevelColor()
 *
 * @author			Slider
 * @date			2014-05-31
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_SetInterfaceWantedLevelColor(int color) {
	if (CheckHandles()) {
		int r, g, b;
		ConvertHexToRGB(color, r, g, b);

		byte write[] = { r, g, b, 1 };

		WriteProcessMemory(gtaHandle,(DWORD*)INTERFACE_WANTEDLEVEL_COLOR_ADDR,&write,sizeof(write),&puffer);
		return 1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Random Shit Functions
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * int API_GetChatLine(int, char)
 *
 * @author			Slider
 * @date			2014-05-10
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
/*
int API_GetChatLine(int line) {
	fstream f;
	char path[512];
    char *username;
    DWORD usernameSize = sizeof(username);
	char content[256];
	
	if (CheckHandles()) {
		GetUserNameA(username, &usernameSize);

		cout << username;

		strcat(path, "C:\\Users\\");
		strcat(path, username);
		strcat(path, "\\Documents\\GTA San Andreas User Files\\SAMP\\chatlog.txt");

		f.open(path, ios::in);
		f.getline(content, sizeof(content));

		while (!f.eof()) {
			cout << content << endl;
		}

		f.close();
		
		return 1;
	}

	return 0;
}
*/

/**
 * int ConvertHexToRGB()
 *
 * @author			Slider
 * @date			2014-05-31
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
void ConvertHexToRGB(int hex, int &r, int &g, int &b) {
	 r = (hex & 0xFF0000) >> 16;
	 g = (hex & 0xFF00) >> 8;
	 b = hex & 0xFF;
}

/**
 * int API_GetVersion()
 *
 * @author			Slider
 * @date			2014-05-29
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetVersion(char *&version) {
	version = API_VERSION;
	return 1;
}

/**
 * int API_GetTimestamp()
 *
 * @author			Slider
 * @date			2014-05-29
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_GetTimestamp() {
	return time(0);
}

/**
 * int API_SetGTAProcessName()
 *
 * @author			Slider
 * @date			2014-06-01
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int API_SetGTAProcessName(char* name) {
	for (int i = 0; i < sizeof(ProcessName); i++) {
		ProcessName[i] = '\0';
	}
	strcat(ProcessName, name);
	//ProcessName = name; 
	return 1;
}

/**
 * int CheckHandles()
 *
 * @author			Slider
 * @date			2014-05-09
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int CheckHandles() {
	char procname[32];

	// check gta process name
	//API_SetGTAProcessName("gta_sa.exe");
	/*
	if (!strlen(ProcessName)) {
		ProcessName = "gta_sa.exe";
	}
	*/

	//cout << "Prozess: " << ProcessName << endl;

	// copy the ProcessName into procname
	//memcpy(ProcessName, procname, sizeof(procname));

	// check gta_sa.exe process
	gtaProcessID = GetGTAProcessID();
	if (!gtaProcessID) return 0;

	// check gta handle
	gtaHandle = OpenProcess(STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE |  0xFFF, FALSE, GetGTAProcessID());
	if (!gtaHandle) return 0;

	// attach to samp.dll
	sampDLL = GetModuleBaseAddress(L"gta_sa.exe", L"samp.dll");
	if (!sampDLL) return 0;

	return 1;
}

/**
 * int GetGTAProcessID()
 *
 * @author			Slider
 * @date			2014-05-09
 * @license			General Public License <https://www.gnu.org/licenses/gpl>
 */
int GetGTAProcessID() {
	/*
	char *procname;
	memcpy(procname, ProcessName, sizeof(ProcessName));
	*/
	DWORD processID = FindProcessID(L"gta_sa.exe");
	//cout << "ProzessID " << processID << " und der Name: " << ProcessName << endl;
	return processID;
}