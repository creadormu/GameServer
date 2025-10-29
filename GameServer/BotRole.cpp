// BotRole.cpp - Bot Role/State System Implementation
#include "stdafx.h"
#include "BotRole.h"
#include "Path.h"
#include "MapPath.h"


// Role configurations
const BotRoleConfig g_BotRoleConfigs[BOT_ROLE_COUNT] = 
{
    // WARRIOR (Default Combat Bot)
    {
        BOT_ROLE_WARRIOR,
        "Warrior",
        20,     // 20% time in safe zones
        80,     // 80% time hunting
        false,  // canTrade
        false,  // canAdvertise
        true,   // canHunt
        true,   // canPVP (if configured)
        true,   // prioritizeParty
        false,  // helpNewbies
        1,      // Normal chat
        {-1},   // No preferred maps (goes anywhere)
        30,     // Wander radius
        60      // Return to safe zone after 60 min
    },
    
    // MERCHANT (Trading Bot)
    {
        BOT_ROLE_MERCHANT,
        "Merchant",
        80,     // 80% time in safe zones
        20,     // 20% time hunting (to collect jewels)
        true,   // canTrade
        true,   // canAdvertise
        true,   // canHunt (limited, for jewels)
        false,  // canPVP (merchants avoid combat)
        false,  // prioritizeParty
        false,  // helpNewbies
        2,      // Chatty (advertises)
        {0, 2, 3, -1}, // Lorencia, Devias, Noria
        50,     // Larger wander radius (walk around town)
        15      // Return to safe zone after 15 min
    },
    
    // FRIENDLY (Helper/Guide Bot)
    {
        BOT_ROLE_FRIENDLY,
        "Friendly",
        90,     // 90% time in safe zones
        10,     // 10% time hunting
        false,  // canTrade
        false,  // canAdvertise
        false,  // canHunt (only defends)
        false,  // canPVP
        true,   // prioritizeParty (helps newbies)
        true,   // helpNewbies
        3,      // Very chatty
        {0, 3, -1}, // Lorencia, Noria (newbie zones)
        40,     // Medium wander radius
        10      // Return quickly to safe zone
    }
};

const BotRoleConfig* GetBotRoleConfig(BotRoleType role)
{
    if (role < 0 || role >= BOT_ROLE_COUNT)
    {
        return &g_BotRoleConfigs[BOT_ROLE_WARRIOR]; // Default
    }
    return &g_BotRoleConfigs[role];
}

const char* GetBotRoleName(BotRoleType role)
{
    const BotRoleConfig* config = GetBotRoleConfig(role);
    return config->roleName;
}
