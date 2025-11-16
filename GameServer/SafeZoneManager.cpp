// SafeZoneManager.cpp - Safe Zone Detection & Movement
#include "stdafx.h"
#include "SafeZoneManager.h"
#include "User.h"
#include "Map.h"
#include "Util.h"
#include "Path.h"

// Define safe zones with coordinates
const SafeZoneArea g_SafeZones[] = {
    // Lorencia (main city - expanded to cover full town area)
    { 0, "Lorencia", 80, 200, 80, 140, true },

    // Devias (town area)
    { 2, "Devias", 180, 230, 0, 60, false },

    // Noria (newbie zone)  
    { 3, "Noria", 150, 200, 100, 160, true },

    // Elveland (newbie zone)
    { 51, "Elveland", 40, 90, 180, 230, true },

    // Elbeland (safe town)
    { 1, "Elbeland", 0, 255, 0, 255, false }, // Entire map is safe
};

const int g_SafeZoneCount = sizeof(g_SafeZones) / sizeof(g_SafeZones[0]);

bool IsSafeZone(int mapNumber)
{
    for (int i = 0; i < g_SafeZoneCount; i++)
    {
        if (g_SafeZones[i].mapNumber == mapNumber)
        {
            return true;
        }
    }
    return false;
}

bool IsInSafeZoneArea(int mapNumber, int x, int y)
{
    for (int i = 0; i < g_SafeZoneCount; i++)
    {
        if (g_SafeZones[i].mapNumber == mapNumber)
        {
            if (x >= g_SafeZones[i].minX && x <= g_SafeZones[i].maxX &&
                y >= g_SafeZones[i].minY && y <= g_SafeZones[i].maxY)
            {
                return true;
            }
        }
    }
    return false;
}

bool GetRandomSafeZoneCoords(int mapNumber, int* outX, int* outY)
{
    for (int i = 0; i < g_SafeZoneCount; i++)
    {
        if (g_SafeZones[i].mapNumber == mapNumber)
        {
            int rangeX = g_SafeZones[i].maxX - g_SafeZones[i].minX;
            int rangeY = g_SafeZones[i].maxY - g_SafeZones[i].minY;

            // Try up to 50 times to find a walkable spot
            for (int attempt = 0; attempt < 50; attempt++)
            {
                int testX = g_SafeZones[i].minX + (GetLargeRand() % rangeX);
                int testY = g_SafeZones[i].minY + (GetLargeRand() % rangeY);
                
                // Check map attributes: must not be wall, river, blocked, or safe zone boundary
                BYTE attr = gMap[mapNumber].GetAttr(testX, testY);
                
                // attr & 1 = wall
                // attr & 2 = object
                // attr & 4 = water/river
                // attr & 8 = safe zone
                
                // We want ONLY walkable tiles (no walls, no rivers, no blocked areas)
                if ((attr & 1) == 0 && (attr & 4) == 0)
                {
                    *outX = testX;
                    *outY = testY;
                    return true;
                }
            }
            
            // If we couldn't find a good spot after 50 tries, return false
            return false;
        }
    }
    return false;
}

bool MoveBotToRandomNearbyPoint(int aIndex, int radius)
{
    LPOBJ lpObj = &gObj[aIndex];

    if (lpObj == NULL || lpObj->Connected < OBJECT_LOGGED)
    {
        return false;
    }

    // Try multiple times to find a walkable point
    for (int attempt = 0; attempt < 10; attempt++)
    {
        // Get random offset within radius
        int offsetX = (GetLargeRand() % (radius * 2 + 1)) - radius;
        int offsetY = (GetLargeRand() % (radius * 2 + 1)) - radius;

        int targetX = lpObj->X + offsetX;
        int targetY = lpObj->Y + offsetY;

        // Clamp to map bounds
        if (targetX < 0) targetX = 0;
        if (targetY < 0) targetY = 0;
        if (targetX > 255) targetX = 255;
        if (targetY > 255) targetY = 255;

        // Check if target is walkable
        BYTE attr = gMap[lpObj->Map].GetAttr(targetX, targetY);
        if ((attr & 1) != 0) // Wall
        {
            continue; // Try again
        }

        // SIMPLE SOLUTION: Just set target coordinates
        // The bot's existing movement AI will handle pathfinding automatically!
        lpObj->TX = targetX;
        lpObj->TY = targetY;
        lpObj->MTX = targetX;
        lpObj->MTY = targetY;
        lpObj->Dir = GetPathPacketDirPos(lpObj->X, lpObj->Y);


        return true;
    }

    return false;
}

void SafeZoneWander(int aIndex)
{
    LPOBJ lpObj = &gObj[aIndex];

    if (lpObj == NULL || lpObj->Connected < OBJECT_LOGGED)
    {
        return;
    }

    // Check if bot is in safe zone
    if (!IsInSafeZoneArea(lpObj->Map, lpObj->X, lpObj->Y))
    {
        // Bot is not in safe zone, try to return to spawn point
        // You can implement return logic here if needed
        return;
    }

    // Move to random nearby point (MoveRange is used as wander radius)
    int wanderRadius = 30; // Default

    // Try to get MoveRange from bot config if available
    // This will be passed from FakeOnline bot data

    MoveBotToRandomNearbyPoint(aIndex, wanderRadius);
}
