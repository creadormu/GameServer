# Bot City Wandering Feature Guide

## Overview
This feature allows bots in the FakeOnline system to alternate between hunting in designated zones and wandering in safe zones (cities). Bots in city mode will walk around, chat with players, and interact socially without engaging in combat.

## New XML Attributes

Add these new attributes to your `Accounts.xml` bot configuration:

### BotStayCity
- **Type:** Integer (0 or 1)
- **Default:** 0 (disabled)
- **Description:** 
  - `0` = Disabled - Bot operates in traditional hunting mode only
  - `1` = Enabled - Bot will alternate between city and hunting modes

### TimeForCity
- **Type:** Integer (minutes)
- **Default:** 40
- **Description:** Time in minutes the bot will spend wandering in the city before returning to hunting

## XML Configuration Example

```xml
<Info Account="Bot0001" 
      Password="123456" 
      Name="André86" 
      SkillID="9" 
      SecondarySkillID="12" 
      UseBuffs_0="16" 
      UseBuffs_1="-1" 
      UseBuffs_2="-1" 
      GateNumber="614" 
      Map="0" 
      MapX="200" 
      MapY="101" 
      PhamViTrain="6" 
      MoveRange="30" 
      TimeReturn="40" 
      TuNhatItem="1" 
      TuDongReset="1" 
      PartyMode="1" 
      PVPMode="1" 
      PostKhiDie="1"
      BotStayCity="1"
      TimeForCity="30" />
```

## How It Works

### Mode Switching
1. **Initial State:** Bot starts in hunting mode at the configured gate
2. **Hunting → City:** After `TimeReturn` minutes, bot teleports to a safe zone
3. **City → Hunting:** After `TimeForCity` minutes, bot returns to hunting gate
4. **Repeat:** Cycle continues indefinitely

### City Behavior
When in city mode, bots will:
- ✅ Walk around randomly within safe zone boundaries
- ✅ Use chat and talk features (already implemented)
- ✅ Respond to player conversations
- ✅ Trade with players (if trade feature is enabled)
- ❌ NOT use combat skills (SkillID, SecondarySkillID)
- ❌ NOT use buff skills (UseBuffs)
- ❌ NOT attack monsters or players
- ❌ NOT pick up items

### Hunting Behavior
When in hunting mode, bots operate normally:
- Use combat skills
- Attack monsters/players (if PVPMode=1)
- Pick up items (if TuNhatItem=1)
- Form parties (if PartyMode=1)
- Use buffs

## Safe Zone Detection

The system uses the existing safe zone detection from `SafeZoneManager.cpp`. Current safe zones include:

| Map | Name | Coordinates | Type |
|-----|------|-------------|------|
| 0 | Lorencia | (80-180, 80-220) | Newbie Zone |
| 2 | Devias | (180-230, 0-60) | Town |
| 3 | Noria | (150-200, 100-160) | Newbie Zone |
| 51 | Elveland | (40-90, 180-230) | Newbie Zone |
| 1 | Elbeland | (0-255, 0-255) | Entire Map |

### Adding More Safe Zones
Edit `SafeZoneManager.cpp` to add more cities:

```cpp
const SafeZoneArea g_SafeZones[] = {
    // Your new city
    { MapNumber, "CityName", minX, maxX, minY, maxY, isNewbieZone },
    ...
};
```

## Technical Implementation

### New Structure Fields

**OFFEXP_DATA (FakeOnline.h)**
```cpp
int BotStayCity;      // 0=disabled, 1=enabled
int TimeForCity;      // Time in minutes to spend in city
```

**LPOBJ (User.h)**
```cpp
bool IsFakeInCityMode;         // true=city mode, false=hunting mode
DWORD IsFakeCityModeStartTime; // When current mode started
int IsFakeCitySpawnX;          // City spawn X coordinate
int IsFakeCitySpawnY;          // City spawn Y coordinate
int IsFakeCitySpawnMap;        // City spawn map number
```

### Key Functions

**CFakeOnline Class Functions:**
- `InitializeCityMode(LPOBJ lpObj)` - Initialize city mode on bot login
- `UpdateBotMode(LPOBJ lpObj, OFFEXP_DATA* pBotData)` - Check and switch modes
- `HandleCityWandering(LPOBJ lpObj, OFFEXP_DATA* pBotData)` - Wander in city
- `SwitchToCityMode(LPOBJ lpObj, OFFEXP_DATA* pBotData)` - Switch to city
- `SwitchToHuntingMode(LPOBJ lpObj, OFFEXP_DATA* pBotData)` - Switch to hunting
- `IsBotInCityMode(LPOBJ lpObj)` - Check current mode

## Configuration Examples

### Example 1: Merchant Bot (Mostly in City)
```xml
<Info Account="BotMerchant" 
      Name="TraderBot" 
      BotStayCity="1" 
      TimeForCity="50" 
      TimeReturn="10" />
```
Result: Spends 50 minutes in city, 10 minutes hunting

### Example 2: Social Bot (Balanced)
```xml
<Info Account="BotSocial" 
      Name="FriendlyBot" 
      BotStayCity="1" 
      TimeForCity="30" 
      TimeReturn="30" />
```
Result: Spends equal time in city and hunting

### Example 3: Hunter Bot with City Breaks
```xml
<Info Account="BotHunter" 
      Name="WarriorBot" 
      BotStayCity="1" 
      TimeForCity="15" 
      TimeReturn="45" />
```
Result: Mostly hunts, takes 15-minute city breaks

### Example 4: Traditional Bot (No City)
```xml
<Info Account="BotTraditional" 
      Name="OldStyleBot" 
      BotStayCity="0" />
```
Result: Never goes to city, hunts continuously

## Logging

The system provides detailed logging for debugging:

```
[CityWander][André86] City wandering initialized - starting in hunting mode
[CityWander][André86] Switched to CITY mode at map 0 (120,150) for 30 minutes
[CityWander][André86] Wandering in city to (125,148)
[CityWander][André86] Switched to HUNTING mode at gate 614 for 40 minutes
```

## Troubleshooting

### Bot doesn't go to city
- Check `BotStayCity="1"` is set
- Verify `TimeReturn` is not 0
- Check logs for initialization message

### Bot stuck in city
- Check `TimeForCity` value is reasonable
- Verify safe zone coordinates are valid
- Check if bot has valid hunting gate

### Bot doesn't wander
- Check `MoveRange` value (default: 30)
- Verify safe zone boundaries in SafeZoneManager.cpp
- Check for wall/obstacle detection issues

### Bot uses skills in city
- This should not happen - check logs
- Verify `IsFakeInCityMode` is being set
- Check safe zone detection is working

## Performance Notes

- City mode checking adds minimal overhead
- Safe zone detection is optimized with pre-defined areas
- Mode switching is event-based, not poll-based
- No additional network traffic is generated

## Compatibility

- ✅ Works with existing bot features (chat, trade, party)
- ✅ Compatible with PVPMode
- ✅ Compatible with auto-reset
- ✅ Compatible with item pickup (disabled in city)
- ✅ Maintains C++ version and structure

## Future Enhancements

Potential features to add:
- Specific city preference per bot
- Time-based city visits (e.g., only at night)
- Group city events
- City-specific chat phrases
- Shop opening in cities
- Multiple safe zones per session

---

**Created:** 2025-11-13  
**Version:** 1.0  
**Author:** AI Assistant  
**Compatibility:** GameServer v1.x+
