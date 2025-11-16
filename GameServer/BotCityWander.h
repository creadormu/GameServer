// BotCityWander.h - Simple City Wandering for Bots
#pragma once

#if USE_FAKE_ONLINE == TRUE

// Check if bot should switch to city mode
bool ShouldBotGoToCity(int aIndex);

// Check if bot should return to hunting
bool ShouldBotReturnToHunting(int aIndex);

// Teleport bot to safe zone (city)
void TeleportBotToCity(int aIndex);

// Teleport bot back to hunting gate
void TeleportBotToHunting(int aIndex);

// Make bot wander in city
void BotWanderInCity(int aIndex);

#endif
