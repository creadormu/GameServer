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
#include "Protocol.h"

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
	OFFEXP_DATA* pBotData = s_FakeOnline.GetOffExpInfo(lpObj);
	if (!pBotData) return;
	
	int cityMap = 0; // Lorencia
	int cityX = 135;
	int cityY = 125;
	bool foundGoodSpot = false;
	
	// Search for a truly walkable spot in Lorencia safe zone
	// AVOID the PVP ring area (coordinates 140-155)
	// Search in the main city square instead (120-138 X, 120-135 Y)
	for (int attempt = 0; attempt < 100 && !foundGoodSpot; attempt++)
	{
		// Search in main city square area, AVOIDING PVP ring (140-155)
		int baseX = 120 + (rand() % 18); // 120-138 (stops before PVP ring at 140)
		int baseY = 120 + (rand() % 15); // 120-135
		
		BYTE attr = gMap[cityMap].GetAttr(baseX, baseY);
		
		// We want ONLY walkable tiles with NO restrictions
		// attr == 0 means NO flags set = completely walkable
		// This ensures we avoid PVP rings, walls, water, etc.
		if (attr == 0)
		{
			// Double-check: make sure we're NOT in PVP ring area
			if (baseX >= 140 && baseX <= 155 && baseY >= 120 && baseY <= 135)
			{
				continue; // Skip PVP ring coordinates
			}
			
			// Verify it's in safe zone using our safe zone manager
			if (IsInSafeZoneArea(cityMap, baseX, baseY))
			{
				cityX = baseX;
				cityY = baseY;
				foundGoodSpot = true;
				LogAdd(LOG_GREEN, "[BotCityWander] %s found walkable city spot at (%d,%d) attribute=%d", 
					lpObj->Name, cityX, cityY, attr);
				break;
			}
		}
	}
	
	if (!foundGoodSpot)
	{
		LogAdd(LOG_RED, "[BotCityWander] %s couldn't find walkable spot! Using default.", lpObj->Name);
	}
	
	// Save old position for logging
	int oldMap = lpObj->Map;
	int oldX = lpObj->X;
	int oldY = lpObj->Y;
	
	// Use the game's built-in teleport function (same as real players)
	// This handles ALL visibility, viewport, and packet sending automatically
	gObjTeleport(aIndex, cityMap, cityX, cityY);
	
	// CRITICAL: gObjTeleport sets State=OBJECT_DELCMD, we need to restore it immediately
	lpObj->State = OBJECT_PLAYING;
	lpObj->Teleport = 0;
	lpObj->Rest = 0;
	lpObj->DieRegen = 0;
	
	// Update bot city mode state AFTER teleport
	lpObj->IsFakeInCityMode = true;
	lpObj->IsFakeCityModeStartTime = GetTickCount();
	lpObj->IsFakeCitySpawnX = cityX;
	lpObj->IsFakeCitySpawnY = cityY;
	lpObj->IsFakeCitySpawnMap = cityMap;
	
	LogAdd(LOG_BLUE, "[BotCityWander] %s teleported to CITY at (%d,%d) [OldPos=%d,%d,%d]", 
		lpObj->Name, cityX, cityY, oldMap, oldX, oldY);
}

// Teleport bot back to hunting gate
void TeleportBotToHunting(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return;
	
	LPOBJ lpObj = &gObj[aIndex];
	OFFEXP_DATA* pBotData = s_FakeOnline.GetOffExpInfo(lpObj);
	if (!pBotData) return;
	
	// Save old city position for logging
	int oldMap = lpObj->Map;
	int oldX = lpObj->X;
	int oldY = lpObj->Y;
	
	// Update state BEFORE teleport
	lpObj->IsFakeInCityMode = false;
	lpObj->IsFakeCityModeStartTime = GetTickCount();
	
	LogAdd(LOG_BLUE, "[BotCityWander] %s returning to HUNTING at gate %d [CityPos=%d,%d,%d]", 
		lpObj->Name, pBotData->GateNumber, oldMap, oldX, oldY);
	
	// Use game's built-in gate movement (handles all visibility automatically)
	// Let the game do its thing - don't interfere with viewport or state
	gObjMoveGate(aIndex, pBotData->GateNumber);
}

// Make bot wander in city
void BotWanderInCity(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return;
	
	LPOBJ lpObj = &gObj[aIndex];
	
	// Only wander every 3 seconds
	if (GetTickCount() < lpObj->m_OfflineMoveDelay + 3000) return;
	
	// Try to find a walkable spot nearby (similar to hunting movement)
	for (int attempt = 0; attempt < 10; attempt++)
	{
		// Random walk in a small area (like city wandering)
		int moveRange = 3;
		int maxRange = moveRange * 2 + 1;
		
		int offsetX = (GetLargeRand() % maxRange) - moveRange; // -3 to +3
		int offsetY = (GetLargeRand() % maxRange) - moveRange;
		
		int newX = lpObj->X + offsetX;
		int newY = lpObj->Y + offsetY;
		
		// Clamp to map bounds
		if (newX < 0 || newY < 0 || newX > 255 || newY > 255) continue;
		
		// Check map attributes (ONLY accept attribute=0, completely clean)
		BYTE attr = gMap[lpObj->Map].GetAttr(newX, newY);
		
		// Only walk on completely clean tiles (no flags set)
		if (attr == 0)
		{
			// Verify still in safe zone
			if (IsInSafeZoneArea(lpObj->Map, newX, newY))
			{
				// Use FakeAnimationMove for proper visible movement (same as hunting)
				lpObj->m_OfflineMoveDelay = GetTickCount();
				FakeAnimationMove(aIndex, newX, newY, false);
				return;
			}
		}
	}
	
	// If we couldn't find a good spot, don't move this cycle
	// Bot will try again in 3 seconds
}

#endif // USE_FAKE_ONLINE
