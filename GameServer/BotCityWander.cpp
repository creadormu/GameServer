// BotCityWander.cpp - Simple City Wandering Implementation
#include "stdafx.h"
#include "BotCityWander.h"
#include "FakeOnline.h"
#include "User.h"
#include "Map.h"
#include "Util.h"
#include "Gate.h"
#include "Log.h"
#include "Move.h"
#include "Viewport.h"
#include "SafeZoneManager.h"

#if USE_FAKE_ONLINE == TRUE

extern CFakeOnline s_FakeOnline;

// Check if bot should switch to city mode
bool ShouldBotGoToCity(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return false;
	
	LPOBJ lpObj = &gObj[aIndex];
	if (!lpObj->IsFakeOnline) return false;
	
	// Get bot config
	OFFEXP_DATA* pBotData = s_FakeOnline.GetOffExpInfo(lpObj);
	if (!pBotData || pBotData->BotStayCity != 1) return false;
	
	// Already in city mode
	if (lpObj->IsFakeInCityMode) return false;
	
	// Check if enough time has passed in hunting mode
	DWORD timeSinceSwitch = GetTickCount() - lpObj->IsFakeCityModeStartTime;
	DWORD huntingTimeLimit = pBotData->TimeReturn * 60 * 1000;
	
	// Must wait at least 60 seconds after spawn/last switch
	if (timeSinceSwitch < 60000) return false;
	
	// Time to go to city?
	return (timeSinceSwitch >= huntingTimeLimit);
}

// Check if bot should return to hunting
bool ShouldBotReturnToHunting(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return false;
	
	LPOBJ lpObj = &gObj[aIndex];
	if (!lpObj->IsFakeOnline) return false;
	
	// Get bot config
	OFFEXP_DATA* pBotData = s_FakeOnline.GetOffExpInfo(lpObj);
	if (!pBotData || pBotData->BotStayCity != 1) return false;
	
	// Not in city mode
	if (!lpObj->IsFakeInCityMode) return false;
	
	// Check if enough time has passed in city mode
	DWORD timeSinceSwitch = GetTickCount() - lpObj->IsFakeCityModeStartTime;
	DWORD cityTimeLimit = pBotData->TimeForCity * 60 * 1000;
	
	// Time to go back to hunting?
	return (timeSinceSwitch >= cityTimeLimit);
}

// Teleport bot to safe zone (city)
void TeleportBotToCity(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return;
	
	LPOBJ lpObj = &gObj[aIndex];
	
	// Find safe zone coordinates
	int cityX = 0, cityY = 0;
	if (!GetRandomSafeZoneCoords(0, &cityX, &cityY)) // Lorencia
	{
		cityX = 130; cityY = 125; // Default Lorencia center
	}
	
	// Teleport to city
	gObjTeleport(aIndex, 0, cityX, cityY);
	
	// Update state
	lpObj->IsFakeInCityMode = true;
	lpObj->IsFakeCityModeStartTime = GetTickCount();
	lpObj->IsFakeCitySpawnX = cityX;
	lpObj->IsFakeCitySpawnY = cityY;
	lpObj->IsFakeCitySpawnMap = 0;
	
	// Refresh viewport
	gObjViewportListProtocolCreate(lpObj);
	
	LogAdd(LOG_BLUE, "[BotCityWander] %s teleported to CITY at (%d,%d)", lpObj->Name, cityX, cityY);
}

// Teleport bot back to hunting gate
void TeleportBotToHunting(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return;
	
	LPOBJ lpObj = &gObj[aIndex];
	OFFEXP_DATA* pBotData = s_FakeOnline.GetOffExpInfo(lpObj);
	if (!pBotData) return;
	
	// Update state first
	lpObj->IsFakeInCityMode = false;
	lpObj->IsFakeCityModeStartTime = GetTickCount();
	
	// Teleport to hunting gate
	gObjMoveGate(aIndex, pBotData->GateNumber);
	
	// Refresh viewport
	gObjViewportListProtocolCreate(lpObj);
	
	LogAdd(LOG_BLUE, "[BotCityWander] %s returned to HUNTING at gate %d", lpObj->Name, pBotData->GateNumber);
}

// Make bot wander in city
void BotWanderInCity(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return;
	
	LPOBJ lpObj = &gObj[aIndex];
	
	// Only wander every 3 seconds
	if (GetTickCount() < lpObj->m_OfflineMoveDelay + 3000) return;
	
	// Simple random walk
	int newX = lpObj->X + (rand() % 7) - 3; // -3 to +3
	int newY = lpObj->Y + (rand() % 7) - 3;
	
	// Clamp to map bounds
	if (newX < 0) newX = 0;
	if (newY < 0) newY = 0;
	if (newX > 255) newX = 255;
	if (newY > 255) newY = 255;
	
	// Check if walkable
	BYTE attr = gMap[lpObj->Map].GetAttr(newX, newY);
	if ((attr & 1) == 0) // Not a wall
	{
		lpObj->TX = newX;
		lpObj->TY = newY;
		lpObj->m_OfflineMoveDelay = GetTickCount();
	}
}

#endif // USE_FAKE_ONLINE
