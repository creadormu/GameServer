// BotRole.h - Bot Role/State System
#pragma once
#include "MapPath.h"

// Bot Role Types
enum BotRoleType
{
    BOT_ROLE_WARRIOR = 0,   // Default: Combat, hunting, leveling
    BOT_ROLE_MERCHANT = 1,  // Trading, safe zones, advertising
    BOT_ROLE_FRIENDLY = 2,  // Helper, guide, social
    BOT_ROLE_COUNT = 3
};

// Bot behavior configuration per role
struct BotRoleConfig
{
    BotRoleType role;
    const char* roleName;
    
    // Zone preferences
    int safeZoneTimePercent;    // % of time in safe zones (0-100)
    int huntingTimePercent;     // % of time hunting (0-100)
    
    // Behavior flags
    bool canTrade;              // Can initiate trades
    bool canAdvertise;          // Sends trade advertisements
    bool canHunt;               // Actively hunts monsters
    bool canPVP;                // Participates in PVP
    bool prioritizeParty;       // Seeks parties actively
    bool helpNewbies;           // Responds to help requests more
    
    // Chat behavior
    int chatProbabilityMultiplier;  // 1.0 = normal, 2.0 = chatty, 0.5 = quiet
    
    // Movement behavior
    int preferredMaps[10];      // Preferred map numbers (-1 = end of list)
    int wanderRadius;           // How far to wander (tiles)
    int returnToSafeZoneMinutes; // Minutes before returning to safe zone
};

// Get role configuration
const BotRoleConfig* GetBotRoleConfig(BotRoleType role);

// Get role name for display
const char* GetBotRoleName(BotRoleType role);

// Default configurations
extern const BotRoleConfig g_BotRoleConfigs[BOT_ROLE_COUNT];
