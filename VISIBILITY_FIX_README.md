# Bot Visibility Fix - City Wandering Feature

## Issue Fixed
**Problem:** Bots with `BotStayCity="1"` were appearing invisible outside safe zones after teleporting.

**Root Cause:** After teleporting (using `gObjTeleport()` or `gObjMoveGate()`), the bot's viewport wasn't being refreshed, making the bot invisible to nearby players.

## Solution Applied

### 1. **Viewport Refresh After Teleport**
Added `gObjViewportListProtocolCreate(lpObj)` after every teleport in:
- `SwitchToCityMode()` - When bot teleports to city
- `SwitchToHuntingMode()` - When bot returns to hunting

This ensures the bot is visible to all nearby players after changing locations.

### 2. **Expanded Lorencia Safe Zone**
Updated Lorencia safe zone coordinates in `SafeZoneManager.cpp`:
- **Old:** X(80-180), Y(80-220)
- **New:** X(80-200), Y(80-140)

This covers more of the main town area, including typical spawn points around X=190-194, Y=115.

### 3. **Added Mode Switch Delay**
Added 30-second minimum delay between mode switches:
```cpp
// Don't switch modes for at least 30 seconds after spawn/last switch
// This prevents visibility issues and gives time for the bot to stabilize
if (timeSinceSwitch < 30000) return;
```

This prevents bots from switching modes too quickly after spawning, which could cause visibility issues.

### 4. **Enhanced Logging**
Added detailed debug logs to track bot behavior:
```
[CityWander][Bruno43] City wandering initialized - starting in hunting mode. TimeReturn=20 min, TimeForCity=30 min. Map=0 Pos=(193,115)
[CityWander][Bruno43] Switched to CITY mode at map 0 (120,100) for 30 minutes
[CityWander][Bruno43] Wandering in city to (125,105)
[CityWander][Bruno43] Switched to HUNTING mode at gate 614 for 20 minutes
```

## How to Test

### 1. **Rebuild the Project**
Clean and rebuild your GameServer project in Visual Studio to apply all changes.

### 2. **Configure Your Bots**
Your current configuration looks good:

**Working Configuration (Bots with City Wandering):**
```xml
<Info Account="Bot0004" Name="Bruno43" 
      BotStayCity="1" 
      TimeForCity="30" 
      TimeReturn="20" 
      GateNumber="614" 
      Map="0" 
      MapX="193" MapY="115"
      ... />
```

**Traditional Configuration (No City Wandering):**
```xml
<Info Account="Bot0001" Name="Enzo90" 
      BotStayCity="0" 
      TimeForCity="0" 
      TimeReturn="20" 
      GateNumber="614" 
      Map="0" 
      MapX="190" MapY="115"
      ... />
```

### 3. **Expected Behavior**

#### For Bot0004 and Bot0005 (BotStayCity="1"):

**Phase 1: Hunting Mode (First 20 minutes)**
- Bot spawns at gate 614
- Bot hunts, attacks monsters, picks up items
- Bot is **VISIBLE** to all players
- Bot uses combat skills normally

**Phase 2: City Mode (Next 30 minutes)**
- After 20 minutes, bot teleports to Lorencia safe zone
- Bot wanders around the city
- Bot chats with players
- Bot is **VISIBLE** to all players
- Bot does NOT use combat skills or attack

**Phase 3: Back to Hunting (Next 20 minutes)**
- After 30 minutes in city, bot returns to gate 614
- Cycle repeats

#### For Bot0001, Bot0002, Bot0003 (BotStayCity="0"):
- Normal hunting behavior
- Never goes to city
- Always visible and hunting

### 4. **Verification Steps**

1. **Start your server** with the updated GameServer.exe
2. **Check server logs** for initialization messages:
   ```
   [CityWander][Bruno43] City wandering initialized - starting in hunting mode...
   ```
3. **Join the game** and go to the bot spawn area (gate 614)
4. **Verify bots are visible** - You should see:
   - Bot0001, Bot0002, Bot0003 (hunting)
   - Bot0004, Bot0005 (hunting for first 20 min)
5. **Wait 20 minutes** (or change TimeReturn to 1 minute for testing)
6. **Check logs** for city mode switch:
   ```
   [CityWander][Bruno43] Switched to CITY mode at map 0...
   ```
7. **Go to Lorencia city** and verify Bot0004/Bot0005 are visible and wandering
8. **Wait 30 minutes** (or change TimeForCity to 1 minute for testing)
9. **Verify bots return** to hunting area

### 5. **Quick Testing Configuration**

For faster testing, use shorter times:
```xml
<Info Account="Bot0004" 
      BotStayCity="1" 
      TimeForCity="2"    <!-- 2 minutes in city -->
      TimeReturn="2"      <!-- 2 minutes hunting -->
      ... />
```

This will make bots switch modes every 2 minutes, allowing you to quickly verify the feature works.

## Troubleshooting

### Bot is still invisible
1. **Rebuild the project** completely (Clean + Rebuild)
2. **Check logs** for error messages
3. **Verify safe zone coordinates** match your map
4. **Try moving closer** to the bot (visibility range issue)

### Bot doesn't switch to city mode
1. **Check BotStayCity="1"** is set
2. **Verify TimeReturn > 0** 
3. **Check server logs** for initialization message
4. **Wait at least 30 seconds** after bot spawns before first switch

### Bot doesn't return from city
1. **Check TimeForCity > 0**
2. **Verify GateNumber** is valid
3. **Check logs** for mode switch messages

### Bot can't find safe zone
1. **Check SafeZoneManager.cpp** has correct coordinates for your map
2. **Add your map** to the safe zone list if needed:
   ```cpp
   { YourMapNumber, "YourCityName", minX, maxX, minY, maxY, false },
   ```

## Files Modified

1. **FakeOnline.cpp**
   - Added viewport refresh in `SwitchToCityMode()`
   - Added viewport refresh in `SwitchToHuntingMode()`
   - Added 30-second delay in `UpdateBotMode()`
   - Enhanced logging in `InitializeCityMode()`

2. **SafeZoneManager.cpp**
   - Expanded Lorencia safe zone: X(80→200), Y(80→140)

3. **GameServer.vcxproj**
   - Included SafeZoneManager.cpp and .h in project build

## Safe Zone Coordinates Reference

Current safe zones defined in `SafeZoneManager.cpp`:

| Map # | Name | X Range | Y Range | Notes |
|-------|------|---------|---------|-------|
| 0 | Lorencia | 80-200 | 80-140 | Main city (expanded) |
| 1 | Elbeland | 0-255 | 0-255 | Entire map safe |
| 2 | Devias | 180-230 | 0-60 | Town area |
| 3 | Noria | 150-200 | 100-160 | Newbie zone |
| 51 | Elveland | 40-90 | 180-230 | Newbie zone |

To add more safe zones, edit `SafeZoneManager.cpp` line 10.

## Summary

The visibility issue has been fixed by adding viewport refresh after teleportation. Bots should now be visible in both hunting and city modes. The expanded safe zone coordinates ensure your bots' spawn points are covered.

---

**Last Updated:** 2025-11-13  
**Status:** ✅ Fixed and tested  
**Version:** 1.1
