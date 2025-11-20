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
	static DWORD lastLogTime[MAX_OBJECT] = { 0 };
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
			lpObj->Name, (float)timeSinceSwitch / 60000.0f, (float)cityTimeLimit / 60000.0f);
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
	// Try different areas of the city
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

		// Based on user's map editor info:
// 0x0001 = Safe zone
// 0x0002 = Character
// 0x0004 = No Move
// 0x0008 = No Ground
// 0x0010 = Water (16 decimal)
// 0x0100 = No attack (256 decimal)

// We want ONLY walkable tiles with NO restrictions
// attr == 0 means NO flags set = completely walkable
// This ensures we avoid PVP rings, walls, water, etc.
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
	
	// Update state
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

	// CRITICAL: Get gate info - this is the ONLY way to get correct hunting coordinates
	GATE_INFO gateInfo = { 0 };
	int returnX = pBotData->MapX;  // Fallback
	int returnY = pBotData->MapY;  // Fallback
	int returnMap = pBotData->Map; // Fallback
	bool usingGateInfo = false;

	if (gGate.GetInfo(pBotData->GateNumber, &gateInfo))
	{
		// SUCCESS: Use gate's exact coordinates
		returnX = gateInfo.X;
		returnY = gateInfo.Y;
		returnMap = gateInfo.Map;
		usingGateInfo = true;

		LogAdd(LOG_GREEN, "[BotCityWander] %s using Gate %d: Map=%d (%d,%d)",
			lpObj->Name, pBotData->GateNumber, returnMap, returnX, returnY);
	}
	else
	{
		// FAILED: Gate not found, use XML config as last resort
		LogAdd(LOG_RED, "[BotCityWander] ERROR: Gate %d not found! Using XML config: Map=%d (%d,%d)",
			pBotData->GateNumber, returnMap, returnX, returnY);
	}

	// Teleport to hunting position
	gObjTeleport(aIndex, returnMap, returnX, returnY);

	// Get current time for all timers
	DWORD currentTime = GetTickCount();

	// Restore bot state after teleport
	lpObj->State = OBJECT_PLAYING;
	lpObj->Teleport = 0;
	lpObj->Rest = 0;
	lpObj->DieRegen = 0;
	lpObj->RegenOk = 0;

	// Reset movement path completely
	lpObj->PathCount = 0;
	lpObj->PathCur = 0;
	lpObj->PathStartEnd = 0;
	memset(lpObj->PathX, 0, sizeof(lpObj->PathX));
	memset(lpObj->PathY, 0, sizeof(lpObj->PathY));
	memset(lpObj->PathDir, 0, sizeof(lpObj->PathDir));

	// Reset city mode
	lpObj->IsFakeInCityMode = false;
	lpObj->IsFakeCityModeStartTime = currentTime;
	lpObj->IsFakeRegen = false;

	// Reset movement timers
	lpObj->IsFakeTimeLag = 0;
	lpObj->m_OfflineMoveDelay = 0;
	lpObj->m_OfflineTimeResetMove = currentTime - 5000;
	lpObj->AttackCustomDelay = currentTime - 31000;

	// Reset combat state
	lpObj->AttackCustom = 0;
	lpObj->IsAttackState = 0;

	// Force viewport refresh
	gObjViewportListDestroy(aIndex);
	gObjViewportListCreate(aIndex);
	gObjViewportListProtocolCreate(lpObj);

	LogAdd(LOG_GREEN, "[BotCityWander] %s returned to %s at Map=%d (%d,%d)",
		lpObj->Name, usingGateInfo ? "GATE" : "CONFIG", lpObj->Map, lpObj->X, lpObj->Y);
	LogAdd(LOG_RED, "[BotCityWander] ========================================");
}
// NPC positions in Lorencia safe zone
struct NPC_POSITION {
	int x;
	int y;
	const char* name;
};

static NPC_POSITION g_NPCPositions[] = {
	{131, 136, "Potion Girl Amy"},
	{116, 141, "Hanzo the Blacksmith"},
	{123, 135, "Lumen the Barmaid"},
	{119, 111, "Pasi the Mage"},
	{115, 118, "Moss"},
	{147, 110, "Baul"},
	{132, 161, "Chaos Card"},
	{147, 146, "Mirage"},
	{125, 146, "Wandering Merchant Alex"}
};
static const int g_NPCCount = 9;

// Make bot wander in city
void BotWanderInCity(int aIndex)
{
	if (!gObjIsConnectedGP(aIndex)) return;

	LPOBJ lpObj = &gObj[aIndex];

	// Only wander every 3 seconds
	if (GetTickCount() < lpObj->m_OfflineMoveDelay + 3000) return;

	// 50% chance to walk to NPC, 50% chance to stand still or random walk
	int behavior = rand() % 100;

	if (behavior < 50) // 50% - Walk to NPC position
	{
		// Select random NPC
		int npcIndex = rand() % g_NPCCount;
		int targetX = g_NPCPositions[npcIndex].x;
		int targetY = g_NPCPositions[npcIndex].y;

		// Check if already near NPC (within 2 tiles)
		int distToNPC = (int)sqrt((float)((lpObj->X - targetX) * (lpObj->X - targetX) +
			(lpObj->Y - targetY) * (lpObj->Y - targetY)));

		if (distToNPC <= 2)
		{
			// Already near NPC, just stand still
			lpObj->m_OfflineMoveDelay = GetTickCount();
			LogAdd(LOG_BLUE, "[BotCityWander] %s standing near %s at (%d,%d)",
				lpObj->Name, g_NPCPositions[npcIndex].name, lpObj->X, lpObj->Y);
			return;
		}

		// NATURAL MOVEMENT: 50% short steps (1-2 tiles), 50% long steps (3-5 tiles)
		int moveDistance = (rand() % 100 < 50) ? (1 + rand() % 2) : (3 + rand() % 3);

		// Walk towards NPC position
		int stepX = lpObj->X;
		int stepY = lpObj->Y;

		// Move multiple steps closer (more natural)
		for (int step = 0; step < moveDistance; step++)
		{
			if (stepX < targetX) stepX++;
			else if (stepX > targetX) stepX--;

			if (stepY < targetY) stepY++;
			else if (stepY > targetY) stepY--;

			// Stop if we reached target
			if (stepX == targetX && stepY == targetY) break;
		}

		// Validate the step
		BYTE attr = gMap[lpObj->Map].GetAttr(stepX, stepY);
		bool hasNoMove = (attr & 0x04) != 0;
		bool hasWater = (attr & 0x10) != 0;
		bool hasNoGround = (attr & 0x08) != 0;

		if (!hasNoMove && !hasWater && !hasNoGround && IsInSafeZoneArea(lpObj->Map, stepX, stepY))
		{
			lpObj->m_OfflineMoveDelay = GetTickCount();

			// CRITICAL FIX: Use gObjMoveGate-style direct position update
			// This is what makes bots visible to players!
			gMap[lpObj->Map].DelStandAttr(lpObj->X, lpObj->Y);
			lpObj->X = stepX;
			lpObj->Y = stepY;
			lpObj->TX = stepX;
			lpObj->TY = stepY;
			lpObj->OldX = stepX;
			lpObj->OldY = stepY;
			lpObj->PathCount = 0;
			lpObj->PathCur = 0;
			gMap[lpObj->Map].SetStandAttr(stepX, stepY);

			// Send movement packet to all nearby players
			PMSG_MOVE_SEND pMsg;
			pMsg.header.set(PROTOCOL_CODE1, sizeof(pMsg));
			pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
			pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
			pMsg.x = (BYTE)stepX;
			pMsg.y = (BYTE)stepY;
			pMsg.dir = lpObj->Dir << 4;

			for (int n = 0; n < MAX_VIEWPORT; n++) {
				if (lpObj->VpPlayer2[n].type == OBJECT_USER) {
					if (lpObj->VpPlayer2[n].state != OBJECT_EMPTY &&
						lpObj->VpPlayer2[n].state != OBJECT_DIECMD &&
						lpObj->VpPlayer2[n].state != OBJECT_DIED) {
						DataSend(lpObj->VpPlayer2[n].index, (BYTE*)&pMsg, pMsg.header.size);
					}
				}
			}

			LogAdd(LOG_BLUE, "[BotCityWander] %s walking to %s (%d,%d) -> (%d,%d)",
				lpObj->Name, g_NPCPositions[npcIndex].name, targetX, targetY, stepX, stepY);
			return;
		}
	}
	else // 50% - Stand still or small random walk
	{
		// 30% chance to actually move, 20% just stand
		if ((rand() % 100) < 30)
		{
			// Small random walk nearby
			for (int attempt = 0; attempt < 10; attempt++)
			{
				int moveRange = 2; // Smaller range for natural city movement
				int maxRange = moveRange * 2 + 1;

				int offsetX = (GetLargeRand() % maxRange) - moveRange;
				int offsetY = (GetLargeRand() % maxRange) - moveRange;

				int newX = lpObj->X + offsetX;
				int newY = lpObj->Y + offsetY;

				if (newX < 0 || newY < 0 || newX > 255 || newY > 255) continue;

				BYTE attr = gMap[lpObj->Map].GetAttr(newX, newY);
				bool hasNoMove = (attr & 0x04) != 0;
				bool hasWater = (attr & 0x10) != 0;
				bool hasNoGround = (attr & 0x08) != 0;

				if (!hasNoMove && !hasWater && !hasNoGround && IsInSafeZoneArea(lpObj->Map, newX, newY))
				{
					lpObj->m_OfflineMoveDelay = GetTickCount();

					// CRITICAL FIX: Direct position update with packet send
					gMap[lpObj->Map].DelStandAttr(lpObj->X, lpObj->Y);
					lpObj->X = newX;
					lpObj->Y = newY;
					lpObj->TX = newX;
					lpObj->TY = newY;
					lpObj->OldX = newX;
					lpObj->OldY = newY;
					lpObj->PathCount = 0;
					lpObj->PathCur = 0;
					gMap[lpObj->Map].SetStandAttr(newX, newY);

					// Send movement packet
					PMSG_MOVE_SEND pMsg;
					pMsg.header.set(PROTOCOL_CODE1, sizeof(pMsg));
					pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
					pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
					pMsg.x = (BYTE)newX;
					pMsg.y = (BYTE)newY;
					pMsg.dir = lpObj->Dir << 4;

					for (int n = 0; n < MAX_VIEWPORT; n++) {
						if (lpObj->VpPlayer2[n].type == OBJECT_USER) {
							if (lpObj->VpPlayer2[n].state != OBJECT_EMPTY &&
								lpObj->VpPlayer2[n].state != OBJECT_DIECMD &&
								lpObj->VpPlayer2[n].state != OBJECT_DIED) {
								DataSend(lpObj->VpPlayer2[n].index, (BYTE*)&pMsg, pMsg.header.size);
							}
						}
					}

					LogAdd(LOG_BLUE, "[BotCityWander] %s random walk to (%d,%d)", lpObj->Name, newX, newY);
					return;
				}
			}
		}

		// If no movement, just update timer and stand still
		lpObj->m_OfflineMoveDelay = GetTickCount();
		LogAdd(LOG_BLUE, "[BotCityWander] %s standing still at (%d,%d)", lpObj->Name, lpObj->X, lpObj->Y);
	}
}
#endif // USE_FAKE_ONLINE
