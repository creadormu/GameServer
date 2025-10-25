// SafeZoneManager.h - Safe Zone Detection & Movement
#pragma once

// Safe zone detection
bool IsSafeZone(int mapNumber);
bool IsInSafeZoneArea(int mapNumber, int x, int y);

// Safe zone wandering for bots
void SafeZoneWander(int aIndex);
bool MoveBotToRandomNearbyPoint(int aIndex, int radius);

// Get random safe zone coordinates
bool GetRandomSafeZoneCoords(int mapNumber, int* outX, int* outY);

// Safe zone definitions
struct SafeZoneArea
{
    int mapNumber;
    const char* mapName;
    int minX, maxX;
    int minY, maxY;
    bool isNewbieZone;
};

extern const SafeZoneArea g_SafeZones[];
extern const int g_SafeZoneCount;
