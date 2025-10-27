// =============================================
// OPTIMIZED BOT CREATION USING STORED PROCEDURES
// Header File
// =============================================

#ifndef _BOT_CREATION_STORED_PROCEDURE_H
#define _BOT_CREATION_STORED_PROCEDURE_H

#pragma once

// =====================================================
// Main Functions
// =====================================================

// Optimized bot creation using stored procedure batch method
// This is 100x faster than file generation method
bool CreateMultipleBotsOptimized(
    int botCount,
    int startFrom,
    int gateNumber,
    int mapNumber,
    int mapX,
    int mapY,
    int minLevel,
    int maxLevel,
    int selectedClass,      // -1 for random distribution
    int configIndex,        // -1 for random configs
    int phamViTrain,
    int moveRange,
    int timeReturn,
    int tuNhatItem,
    int tuDongReset,
    int partyMode,
    int pvpMode,
    int postKhiDie,
    bool useFemaleName = false,
    const char* language = NULL
);

// Wrapper function that matches old signature
// Can be used as drop-in replacement for CreateMultipleBotsAdvanced
bool CreateMultipleBotsAdvanced_StoredProc(
    int botCount,
    int startFrom,
    int gateNumber,
    int mapNumber,
    int mapX,
    int mapY,
    int minLevel,
    int maxLevel,
    int selectedClass,
    int phamViTrain,
    int moveRange,
    int timeReturn,
    int tuNhatItem,
    int tuDongReset,
    int partyMode,
    int pvpMode,
    int postKhiDie,
    int enabledConfigs
);

// Create a single bot via stored procedure (for debugging/testing)
bool CreateSingleBotViaStoredProcedure(
    const char* accountID,
    const char* charName,
    int classCode,
    int configIndex,
    int level,
    int mapNumber,
    int mapX,
    int mapY
);

// =====================================================
// Utility Functions
// =====================================================

// Delete a range of bot accounts
bool DeleteBotRange(int startBotNumber, int endBotNumber);

// Reset bot names to unused status
bool ResetBotNames();

// Execute a stored procedure (internal helper)
bool ExecuteStoredProcedure(const char* procedureCall, int* resultCode = NULL);

#endif // _BOT_CREATION_STORED_PROCEDURE_H
