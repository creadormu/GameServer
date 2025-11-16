# Bot Invisibility Fix - COMPLETE SOLUTION

## Problem Identified

Bots with `BotStayCity="1"` were becoming invisible because:
1. **Immediate Teleportation**: Bots were switching to city mode IMMEDIATELY after spawning (within milliseconds), before their viewport was properly established
2. **Viewport Desynchronization**: Teleporting before full connection caused the game client to lose track of the bot's position
3. **No Stabilization Period**: No delay between bot spawn and first mode switch

## Complete Solution Applied

### 1. **Triple-Layer Stabilization System**

#### Layer 1: Connection Check (5 seconds)
```cpp
// Only update mode if bot is fully connected and stable
if (lpObj->ConnectTickCount != 0 && (GetTickCount() - lpObj->ConnectTickCount) >= 5000)
```
- Bot must be connected for at least 5 seconds before ANY mode updates

#### Layer 2: Initialization Check
```cpp
// Safety check: if IsFakeCityModeStartTime is 0, bot wasn't properly initialized
if (lpObj->IsFakeCityModeStartTime == 0) return;
```
- Prevents mode switching if city wandering wasn't properly initialized

#### Layer 3: Mode Switch Delay (60 seconds)
```cpp
// Don't switch modes for at least 60 seconds after spawn/last switch
if (timeSinceSwitch < 60000) return;
```
- Bot must stay in current mode for minimum 60 seconds before switching

### 2. **Improved Movement System**

Changed from direct coordinate setting to using `FakeAnimationMove()`:
```cpp
// Use existing FakeOnline movement (more reliable)
FakeAnimationMove(lpObj->Index, tpx, tpy, false);
```

This ensures:
- Proper pathfinding
- Wall/obstacle detection
- Viewport synchronization
- Smooth movement animations

### 3. **Enhanced Viewport Refresh**

Added viewport refresh after every teleport:
```cpp
gObjTeleport(lpObj->Index, cityMap, citySpawnX, citySpawnY);
// Refresh viewport to make bot visible
gObjViewportListProtocolCreate(lpObj);
```

## Expected Behavior After Fix

### **Bot0004 (Bruno43)** - With `BotStayCity="1"` and `TimeReturn="2"`, `TimeForCity="2"`:

**Timeline:**

| Time | State | Location | Visible? | Actions |
|------|-------|----------|----------|---------|
| 00:00 | SPAWN | Gate 614 (193,115) | ✅ YES | Spawning |
| 00:05 | HUNTING | Gate 614 area | ✅ YES | Combat, item pickup, moving |
| 02:00 | SWITCH | → Safe Zone | ✅ YES | Teleporting to city |
| 02:05 | CITY | Lorencia (~177,131) | ✅ YES | Wandering, chatting |
| 04:00 | SWITCH | → Gate 614 | ✅ YES | Returning to hunting |
| 04:05 | HUNTING | Gate 614 area | ✅ YES | Combat resumes |

### **Bot0001, Bot0002, Bot0003** - With `BotStayCity="0"`:

| Time | State | Location | Visible? | Actions |
|------|-------|----------|----------|---------|
| 00:00+ | HUNTING | Gate 614 area | ✅ YES | Normal hunting forever |

## Important Timing Notes

### Minimum Time Settings

Due to the stabilization periods, use these MINIMUM values:

```xml
<!-- MINIMUM for testing -->
<Info BotStayCity="1" 
      TimeForCity="2"    <!-- Minimum 2 minutes (120 sec) -->
      TimeReturn="2"      <!-- Minimum 2 minutes (120 sec) -->
      ... />
```

Why?
- 5 sec: Connection stabilization
- 60 sec: Minimum mode time
- = **65 seconds minimum** per mode
- Recommend: **2 minutes** (120 sec) for safe testing

### Recommended Production Settings

```xml
<!-- RECOMMENDED for production -->
<Info BotStayCity="1" 
      TimeForCity="20"    <!-- 20 minutes in city -->
      TimeReturn="40"      <!-- 40 minutes hunting -->
      ... />
```

## Updated Safe Zone Coordinates

Lorencia safe zone expanded to cover more spawn areas:
```
Map 0 (Lorencia): X(80-200), Y(80-140)
```

Your bot spawn points (190-194, 115) are now INSIDE the safe zone boundary.

## What You'll See in Logs

### Correct Logs (Fixed):
```
01:04:39 [FakeOnline]  [TK: Bot0004 NV: Bruno43][Cls:0] Online at Map:0 X:193 Y:115 Gate:614
01:04:39 [CityWander][Bruno43] City wandering initialized - starting in hunting mode. TimeReturn=2 min, TimeForCity=2 min. Map=0 Pos=(193,115)
... (60 seconds of hunting) ...
01:05:39 [CityWander][Bruno43] Switched to CITY mode at map 0 (177,131) for 2 minutes
... (bot visible in city, wandering) ...
01:07:39 [CityWander][Bruno43] Switched to HUNTING mode at gate 614 for 2 minutes
... (bot visible at gate, hunting) ...
```

### Your Previous Logs (Broken):
```
01:04:39 [FakeOnline]  [TK: Bot0004 NV: Bruno43][Cls:0] Online at Map:0 X:193 Y:115 Gate:614
01:04:39 [CityWander][Bruno43] Switched to CITY mode at map 0 (177,131) for 2 minutes
```
❌ Missing initialization log
❌ Switched immediately (no delay)

## Testing Steps

### 1. **Rebuild Project**
```
Build → Clean Solution
Build → Rebuild Solution
```

### 2. **Update Your Accounts.xml**

Keep Bot0001-Bot0003 as controls (no city mode):
```xml
<Info Account="Bot0001" ... BotStayCity="0" TimeForCity="0" TimeReturn="20" />
<Info Account="Bot0002" ... BotStayCity="0" TimeForCity="0" TimeReturn="20" />
<Info Account="Bot0003" ... BotStayCity="0" TimeForCity="0" TimeReturn="20" />
```

Test bots with city mode (2 min cycles for fast testing):
```xml
<Info Account="Bot0004" ... BotStayCity="1" TimeForCity="2" TimeReturn="2" />
<Info Account="Bot0005" ... BotStayCity="1" TimeForCity="2" TimeReturn="2" />
```

### 3. **Start Server and Verify**

**Check Initialization (within first 5 seconds):**
```
[CityWander][Bruno43] City wandering initialized - starting in hunting mode. TimeReturn=2 min...
[CityWander][Paulo20] City wandering initialized - starting in hunting mode. TimeReturn=2 min...
```
✅ If you see these messages → City mode is properly initialized
❌ If missing → Bot won't switch modes (will hunt normally)

**Check First Mode Switch (after ~65 seconds):**
```
[CityWander][Bruno43] Switched to CITY mode at map 0 (177,131) for 2 minutes
```
✅ Should happen ~60-70 seconds after spawn
❌ If immediate (< 10 sec) → Fix not applied

### 4. **In-Game Verification**

**Minute 0-1: At Gate 614**
- Go to gate 614 area
- All 5 bots should be visible
- Bot0001-Bot0003 hunting
- Bot0004-Bot0005 hunting (waiting for mode switch)

**Minute 1-2: Check City (Lorencia)**
- After ~65 seconds, go to Lorencia city center
- Bot0004 and Bot0005 should teleport there
- Check coordinates around (150-180, 100-135)
- Both bots should be **VISIBLE** and walking around

**Minute 3-4: Return to Gate 614**
- After 2 minutes in city, bots return to hunting
- Go back to gate 614
- All bots should be **VISIBLE** again

## Troubleshooting

### Bot Still Invisible

**Check 1: Initialization Log Missing?**
```
grep "City wandering initialized" logfile.txt
```
- If missing → `InitializeCityMode()` not being called
- Check: `RestoreFakeOnline()` includes the call

**Check 2: Mode Switch Too Fast?**
```
grep "Switched to CITY mode" logfile.txt
```
- Compare timestamps with spawn time
- Should be minimum 60+ seconds apart
- If < 10 seconds → Delays not working

**Check 3: Viewport Not Refreshing?**
- Check for `gObjViewportListProtocolCreate` calls
- Should happen after every teleport

**Check 4: Wrong Coordinates?**
- Your bots spawn at X=190-194, Y=115
- Safe zone is X=80-200, Y=80-140
- ✅ Bots ARE in safe zone
- If they spawn outside, they'll try to return constantly

### Bot Doesn't Switch Modes

**Cause 1: TimeReturn/TimeForCity Too Low**
```xml
<!-- BAD: Too short -->
<Info BotStayCity="1" TimeForCity="1" TimeReturn="1" />

<!-- GOOD: At least 2 minutes -->
<Info BotStayCity="1" TimeForCity="2" TimeReturn="2" />
```

**Cause 2: BotStayCity Not Set**
```xml
<!-- BAD: Feature disabled -->
<Info BotStayCity="0" ... />

<!-- GOOD: Feature enabled -->
<Info BotStayCity="1" ... />
```

**Cause 3: ConnectTickCount Not Initialized**
- Bot needs `lpObj->ConnectTickCount` set during spawn
- This should happen automatically in `gObjAdd()` or `RestoreFakeOnline()`

## Comparing with Guard NPC (Monster 247)

You mentioned Guards can move freely. Here's why:

| Feature | Guard NPC | Fake Bot (Before Fix) | Fake Bot (After Fix) |
|---------|-----------|----------------------|----------------------|
| Type | OBJECT_MONSTER | OBJECT_USER | OBJECT_USER |
| Movement | AI pathfinding | Teleport | Teleport + Viewport sync |
| Visibility | Always synced | Lost on teleport | Maintained |
| Safe Zone | Respects boundaries | Ignored | Respects boundaries |

Guards work because they're NPCs with AI pathfinding. Fake bots are fake players, so they need special handling (viewport refresh) after teleporting.

## Files Modified

1. **FakeOnline.cpp**
   - `InitializeCityMode()` - Added safety check for disabled bots
   - `UpdateBotMode()` - Added 60-second delay and initialization check
   - `QuayLaiToaDoGoc()` - Added 5-second connection check
   - `HandleCityWandering()` - Changed to use FakeAnimationMove
   - `SwitchToCityMode()` - Added viewport refresh after teleport
   - `SwitchToHuntingMode()` - Added viewport refresh after teleport

2. **SafeZoneManager.cpp**
   - Expanded Lorencia safe zone to X(80-200), Y(80-140)

## Summary

### What Was Wrong
- Bots teleported IMMEDIATELY after spawning
- No viewport synchronization after teleport
- No stabilization period for connection

### What Was Fixed
- ✅ 5-second connection stabilization
- ✅ 60-second minimum mode duration
- ✅ Proper viewport refresh after teleports
- ✅ Better movement using FakeAnimationMove
- ✅ Expanded safe zone coverage

### Result
Bots should now be **100% VISIBLE** in both hunting and city modes!

---

**Last Updated:** 2025-11-13  
**Status:** ✅ Complete - Ready for Testing  
**Version:** 1.2 - Full Invisibility Fix
