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
	// Try different areas of the city
	for (int attempt = 0; attempt < 100 && !foundGoodSpot; attempt++)
	{
		// Search in different city areas
		int baseX = 120 + (rand() % 40); // 120-160
		int baseY = 120 + (rand() % 20); // 120-140
		
		BYTE attr = gMap[cityMap].GetAttr(baseX, baseY);
		
		// We want ONLY attribute 0 or 8 (empty or safe zone marker)
		// NO walls (1), NO objects (2), NO water (4)
		if (attr == 0 || attr == 8)
		{
			// Verify it's in safe zone
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
	
	// Save old position for map attribute cleanup
	int oldMap = lpObj->Map;
	int oldX = lpObj->X;
	int oldY = lpObj->Y;
	
	// Remove bot from old map position
	if (oldMap >= 0 && oldMap < MAX_MAP)
	{
		gMap[oldMap].DelStandAttr(oldX, oldY);
	}
	
	// Clear viewport from old position
	gObjViewportListDestroy(aIndex);
	
	// Move bot to city (direct coordinate change like hunting movement)
	lpObj->Map = cityMap;
	lpObj->X = cityX;
	lpObj->Y = cityY;
	lpObj->TX = cityX;
	lpObj->TY = cityY;
	lpObj->OldX = cityX;
	lpObj->OldY = cityY;
	lpObj->MTX = cityX;
	lpObj->MTY = cityY;
	
	// Update state
	lpObj->IsFakeInCityMode = true;
	lpObj->IsFakeCityModeStartTime = GetTickCount();
	lpObj->IsFakeCitySpawnX = cityX;
	lpObj->IsFakeCitySpawnY = cityY;
	lpObj->IsFakeCitySpawnMap = cityMap;
	
	// Add bot to new map position
	gMap[cityMap].SetStandAttr(cityX, cityY);
	
	// CRITICAL: Recreate viewport and send character info
	gObjViewportListCreate(aIndex);
	gObjViewportListProtocolCreate(lpObj);
	
	// Send character position update packet (like when bot first spawns)
	PMSG_MOVE_SEND pMsgMove;
	pMsgMove.header.set(PROTOCOL_CODE1, sizeof(pMsgMove));
	pMsgMove.index[0] = SET_NUMBERHB(aIndex);
	pMsgMove.index[1] = SET_NUMBERLB(aIndex);
	pMsgMove.x = cityX;
	pMsgMove.y = cityY;
	pMsgMove.dir = lpObj->Dir << 4;
	
	// Send to all players in viewport
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if (lpObj->VpPlayer2[n].state != OBJECT_EMPTY && 
				lpObj->VpPlayer2[n].state != OBJECT_DIECMD && 
				lpObj->VpPlayer2[n].state != OBJECT_DIED)
			{
				DataSend(lpObj->VpPlayer2[n].index, (BYTE*)&pMsgMove, pMsgMove.header.size);
			}
		}
	}
	
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
	
	// Save old city position
	int oldMap = lpObj->Map;
	int oldX = lpObj->X;
	int oldY = lpObj->Y;
	
	// Remove from old map
	if (oldMap >= 0 && oldMap < MAX_MAP)
	{
		gMap[oldMap].DelStandAttr(oldX, oldY);
	}
	
	// Clear viewport
	gObjViewportListDestroy(aIndex);
	
	// Update state
	lpObj->IsFakeInCityMode = false;
	lpObj->IsFakeCityModeStartTime = GetTickCount();
	
	// Use gate system to teleport back
	gObjMoveGate(aIndex, pBotData->GateNumber);
	
	// CRITICAL: Recreate viewport for visibility
	gObjViewportListCreate(aIndex);
	gObjViewportListProtocolCreate(lpObj);
	
	LogAdd(LOG_BLUE, "[BotCityWander] %s returned to HUNTING at gate %d [OldPos=%d,%d,%d]", 
		lpObj->Name, pBotData->GateNumber, oldMap, oldX, oldY);
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
