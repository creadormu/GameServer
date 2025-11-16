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
#include <cmath>

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
	DWORD currentTick = GetTickCount();
	DWORD timeSinceSwitch = currentTick - lpObj->IsFakeCityModeStartTime;
	DWORD cityTimeLimit = pBotData->TimeForCity * 60 * 1000;
	
	// Log timer check every 10 seconds for debugging
	static DWORD lastLogTime[MAX_OBJECT] = {0};
	if (currentTick - lastLogTime[aIndex] > 10000)
	{
		float minutesElapsed = (float)timeSinceSwitch / 60000.0f;
		float minutesLimit = (float)cityTimeLimit / 60000.0f;
		LogAdd(LOG_BLUE, "[BotCityWander][Timer] %s in CITY: %.1f/%.1f min (TimeForCity=%d)", 
			lpObj->Name, minutesElapsed, minutesLimit, pBotData->TimeForCity);
		lastLogTime[aIndex] = currentTick;
	}
	
	// Time to go back to hunting?
	bool shouldReturn = (timeSinceSwitch >= cityTimeLimit);
	if (shouldReturn)
	{
		LogAdd(LOG_RED, "[BotCityWander] %s TIME TO RETURN! Elapsed=%.1f min, Limit=%.1f min", 
			lpObj->Name, (float)timeSinceSwitch/60000.0f, (float)cityTimeLimit/60000.0f);
	}
	
	return shouldReturn;
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
	// Based on user's map editor:
	// 0x0001 = Safe zone
	// 0x0002 = Character (can stand)
	// 0x0004 = No Move (AVOID!)
	// 0x0008 = No Ground (AVOID!)
	// 0x0010 = Water (AVOID!)
	// 0x0100 = No attack (PVP ring - AVOID!)
	
	// We want: Safe zone (0x0001) OR Character (0x0002), but NOT No Move (0x0004)
	for (int attempt = 0; attempt < 100 && !foundGoodSpot; attempt++)
	{
		// Search in main city square area, AVOIDING PVP ring (140-155)
		int baseX = 120 + (rand() % 18); // 120-138 (stops before PVP ring at 140)
		int baseY = 120 + (rand() % 15); // 120-135
		
		// Double-check: make sure we're NOT in PVP ring area
		if (baseX >= 140 && baseX <= 155 && baseY >= 120 && baseY <= 135)
		{
			continue; // Skip PVP ring coordinates
		}
		
		BYTE attr = gMap[cityMap].GetAttr(baseX, baseY);
		
		// Accept tiles with ONLY safe zone (0x0001) or character (0x0002) flags
		// REJECT tiles with No Move (0x0004), No Ground (0x0008), Water (0x0010), or No Attack (0x0100)
		bool hasNoMove = (attr & 0x04) != 0;  // No Move flag
		bool hasWater = (attr & 0x10) != 0;   // Water flag
		bool hasNoGround = (attr & 0x08) != 0; // No Ground flag
		bool hasNoAttack = (attr & 0x100) != 0; // No Attack (PVP) flag
		
		// Tile must NOT have any blocking flags
		if (!hasNoMove && !hasWater && !hasNoGround && !hasNoAttack)
		{
			// Verify it's in safe zone using our safe zone manager
			if (IsInSafeZoneArea(cityMap, baseX, baseY))
			{
				cityX = baseX;
				cityY = baseY;
				foundGoodSpot = true;
				LogAdd(LOG_GREEN, "[BotCityWander] %s found walkable city spot at (%d,%d) attribute=0x%02X", 
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
	
	LogAdd(LOG_RED, "[BotCityWander] ========== %s RETURNING TO HUNTING ==========", lpObj->Name);
	LogAdd(LOG_RED, "[BotCityWander] Current position: Map=%d (%d,%d)", oldMap, oldX, oldY);
	LogAdd(LOG_RED, "[BotCityWander] Target hunting coords: Map=%d (%d,%d)", 
		pBotData->Map, pBotData->MapX, pBotData->MapY);
	
	// Add LARGE random offset so bot spawns far from home position
	// This triggers the "return to corner" logic (requires distance >= MoveRange+5)
	// MoveRange is usually 30, so we need offset of ~40-50 to ensure movement
	int offsetX = (rand() % 61) - 30; // -30 to +30
	int offsetY = (rand() % 61) - 30;
	
	// Ensure minimum distance of 35 tiles from home to trigger movement
	int distance = (int)sqrt((float)(offsetX * offsetX + offsetY * offsetY));
	if (distance < 35)
	{
		// Force larger offset if too close
		offsetX = 40;
		offsetY = 0;
	}
	
	int returnX = pBotData->MapX + offsetX;
	int returnY = pBotData->MapY + offsetY;
	
	LogAdd(LOG_BLUE, "[BotCityWander] %s teleporting to (%d,%d) [offset: %+d,%+d, dist:%d]", 
		lpObj->Name, returnX, returnY, offsetX, offsetY, distance);
	
	// Teleport directly to hunting coordinates (NOT gate, since bot is not at gate!)
	gObjTeleport(aIndex, pBotData->Map, returnX, returnY);
	
	// Get current time for all timers
	DWORD currentTime = GetTickCount();
	
	// Restore bot state after teleport (gObjTeleport sets OBJECT_DELCMD temporarily)
	lpObj->State = OBJECT_PLAYING;
	lpObj->Teleport = 0;
	lpObj->Rest = 0;
	lpObj->DieRegen = 0;
	lpObj->PathCount = 0;
	
	// CRITICAL: Reset city mode AFTER teleport
	lpObj->IsFakeInCityMode = false;
	lpObj->IsFakeCityModeStartTime = currentTime;
	
	// CRITICAL FIX: Set IsFakeRegen = FALSE first to trigger movement initialization!
	// Bot will detect it's far from monsters and start wandering
	lpObj->IsFakeRegen = false;
	
	// CRITICAL FIX: Reset ALL movement timers so bot can move again!
	lpObj->m_OfflineMoveDelay = currentTime;
	lpObj->m_OfflineTimeResetMove = currentTime;
	lpObj->IsFakeTimeLag = currentTime;
	lpObj->AttackCustomDelay = currentTime;
	
	LogAdd(LOG_GREEN, "[BotCityWander] %s successfully returned to HUNTING at Map=%d (%d,%d)", 
		lpObj->Name, lpObj->Map, lpObj->X, lpObj->Y);
	LogAdd(LOG_RED, "[BotCityWander] ========================================");
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
		
		// Check map attributes - avoid No Move, Water, No Ground
		BYTE attr = gMap[lpObj->Map].GetAttr(newX, newY);
		
		// Only walk on tiles WITHOUT blocking flags
		bool hasNoMove = (attr & 0x04) != 0;
		bool hasWater = (attr & 0x10) != 0;
		bool hasNoGround = (attr & 0x08) != 0;
		
		if (!hasNoMove && !hasWater && !hasNoGround)
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
