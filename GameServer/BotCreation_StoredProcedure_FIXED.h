// =============================================
// FIXED VERSION - Header File
// =============================================

#ifndef _BOT_CREATION_STORED_PROCEDURE_FIXED_H
#define _BOT_CREATION_STORED_PROCEDURE_FIXED_H

#pragma once

// =====================================================
// Initialization (call once at server startup)
// =====================================================
bool InitializeBotODBC(const char* server, const char* database, const char* user, const char* password);

// =====================================================
// Main Functions
// =====================================================

// Call stored procedure to create a single bot
bool CallCreateBotStoredProc(
    const char* accountID,
    const char* charName,
    int classCode,
    int level,
    int mapNumber,
    int mapX,
    int mapY,
    int str,
    int dex,
    int vit,
    int ene,
    int cmd,
    const char* invHex,
    const char* magicHex
);

// Batch create bots via stored procedure (fastest method)
bool CreateBotsViaBatch(
    int startFrom,
    int botCount,
    int minLevel,
    int maxLevel,
    int mapNumber,
    int mapX,
    int mapY
);

#endif // _BOT_CREATION_STORED_PROCEDURE_FIXED_H
