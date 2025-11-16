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
	
	// Known safe walkable coordinates in Lorencia (main square, shop areas)
	// These are guaranteed to be in safe zone and walkable
	const int safeCoords[][2] = {
		{130, 125}, // Center of Lorencia
		{135, 130}, // Near shops
		{140, 128}, // Main square
		{132, 120}, // Safe walkable area
		{138, 135}, // Market area
		{125, 127}, // Safe zone
		{145, 130}, // Safe walkable
		{128, 135}, // Near NPCs
		{142, 125}, // Town center
		{136, 122}  // Safe area
	};
	
	// Pick a random safe coordinate
	int coordIndex = rand() % 10;
	int cityX = safeCoords[coordIndex][0];
	int cityY = safeCoords[coordIndex][1];
	int cityMap = 0; // Lorencia
	
	// Double-check it's walkable
	BYTE attr = gMap[cityMap].GetAttr(cityX, cityY);
	if ((attr & 1) != 0 || (attr & 4) != 0) // Wall or river
	{
		LogAdd(LOG_RED, "[BotCityWander] %s coordinate (%d,%d) has bad attribute %d! Using fallback.", 
			lpObj->Name, cityX, cityY, attr);
		// Fallback to guaranteed safe spot
		cityX = 130;
		cityY = 125;
	}
	
	// Log final coordinates and their attributes
	attr = gMap[cityMap].GetAttr(cityX, cityY);
	LogAdd(LOG_GREEN, "[BotCityWander] %s final city coords: (%d,%d) attribute=%d (1=wall,2=obj,4=water,8=safe)", 
		lpObj->Name, cityX, cityY, attr);
	
	// Teleport to city
	gObjTeleport(aIndex, cityMap, cityX, cityY);
	
	// Update state
	lpObj->IsFakeInCityMode = true;
	lpObj->IsFakeCityModeStartTime = GetTickCount();
	lpObj->IsFakeCitySpawnX = cityX;
	lpObj->IsFakeCitySpawnY = cityY;
	lpObj->IsFakeCitySpawnMap = cityMap;
	
	// CRITICAL: Refresh viewport to make bot visible
	gObjViewportListCreate(aIndex);
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
	
	// CRITICAL: Refresh viewport to make bot visible
	gObjViewportListCreate(aIndex);
	gObjViewportListProtocolCreate(lpObj);
	
	LogAdd(LOG_BLUE, "[BotCityWander] %s returned to HUNTING at gate %d", lpObj->Name, pBotData->GateNumber);
}

// Make bot wander in city
void BotWanderInCity(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return;
	
	LPOBJ lpObj = &gObj[aIndex];
	
	// Only wander every 4 seconds
	if (GetTickCount() < lpObj->m_OfflineMoveDelay + 4000) return;
	
	// Try to find a walkable spot nearby
	for (int attempt = 0; attempt < 15; attempt++)
	{
		// Random walk in a small area (3-5 tiles)
		int newX = lpObj->X + (rand() % 7) - 3; // -3 to +3
		int newY = lpObj->Y + (rand() % 7) - 3;
		
		// Clamp to map bounds
		if (newX < 0) newX = 0;
		if (newY < 0) newY = 0;
		if (newX > 255) newX = 255;
		if (newY > 255) newY = 255;
		
		// Check map attributes
		BYTE attr = gMap[lpObj->Map].GetAttr(newX, newY);
		
		// Must be walkable: no walls, no rivers, no blocked areas
		if ((attr & 1) == 0 && (attr & 4) == 0)
		{
			// Check if still in safe zone
			if (IsInSafeZoneArea(lpObj->Map, newX, newY))
			{
				lpObj->TX = newX;
				lpObj->TY = newY;
				lpObj->MTX = newX;
				lpObj->MTY = newY;
				lpObj->m_OfflineMoveDelay = GetTickCount();
				return;
			}
		}
	}
	
	// If we couldn't find a good spot, return to city spawn point
	if (lpObj->IsFakeCitySpawnX > 0 && lpObj->IsFakeCitySpawnY > 0)
	{
		lpObj->TX = lpObj->IsFakeCitySpawnX;
		lpObj->TY = lpObj->IsFakeCitySpawnY;
	}
}

#endif // USE_FAKE_ONLINE
