#pragma once

#include "User.h" 
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "BotTrader.h"

#if USE_FAKE_ONLINE == TRUE

// ... (struct OFFEXP_DATA y BotActivePVPCombatState sin cambios) ...
struct OFFEXP_DATA
{
	char Account[11];
	char Password[11];
	char Name[11];
	WORD SkillID;
	WORD UseBuffs[3];
	int TuNhatItem;
	int TuDongReset;
	int GateNumber;
	int Map;
	int MapX;
	int MapY;
	int PhamViTrain;
	int TimeReturn;
	int PostKhiDie;
	int PVPMode; 
	int PartyMode;
	int MoveRange;
	int MainAttackSkillID;
	int SecondaryAttackSkillID;
	// City wandering attributes
	int BotStayCity;      // 0=disabled, 1=enabled - bot will wander in safe zones
	int TimeForCity;      // Time in minutes to spend in city before returning to hunting
};


struct FAKEBOT_TRADE_ITEM {
	std::string tradeName;
	int successRate;
	std::vector<MixesItems> requiredItems;
	std::vector<MixesItems> rewardItems;
};


struct BotActivePVPCombatState
{
	bool isInActiveCombat;
	DWORD lastPVPActionTick;      
	bool saidInitialPVPPhrase; 

	BotActivePVPCombatState() : isInActiveCombat(false), lastPVPActionTick(0), saidInitialPVPPhrase(false) {}
};


class CFakeOnline
{
public:
	CFakeOnline();
	~CFakeOnline(); 

	void RestoreFakeOnline();
	void AttemptRandomBotComment(int aIndex);
	OFFEXP_DATA* GetOffExpInfo(LPOBJ lpObj);
	OFFEXP_DATA* GetOffExpInfoByAccount(LPOBJ lpObj);
	void LoadFakeData(char* path);
	void CheckAutoReset(LPOBJ lpObj); // Add this declaration
	bool CanTradeWithBot(const LPOBJ lpBot);
	void FakeAttackProc(LPOBJ lpObj);
	void DarkWizardFakeAttack(LPOBJ lpObj);
	void DKFakeAttack(LPOBJ lpObj);
	void ELFFakeAttack(LPOBJ lpObj);
	void MGFakeAttack(LPOBJ lpObj);
	void DLFakeAttack(LPOBJ lpObj);
	void SUMFakeAttack(LPOBJ lpObj);
	void RFFakeAttack(LPOBJ lpObj);
	void Attack(int UserIndex);
	void OnAttackAlreadyConnected(LPOBJ lpObj);
	int NhatItem(int aIndex);
	void QuayLaiToaDoGoc(int aIndex);
	void PostChatMSG(LPOBJ lpObj);
	// City wandering functions
	void InitializeCityMode(LPOBJ lpObj);
	void UpdateBotMode(LPOBJ lpObj, OFFEXP_DATA* pBotData);
	void HandleCityWandering(LPOBJ lpObj, OFFEXP_DATA* pBotData);
	void SwitchToHuntingMode(LPOBJ lpObj, OFFEXP_DATA* pBotData);
	void SwitchToCityMode(LPOBJ lpObj, OFFEXP_DATA* pBotData);
	bool IsBotInCityMode(LPOBJ lpObj);
	int	IndexMsgMin; 
	int	IndexMsgMax; 
	void SuDungMauMana(int aIndex);
	void TuDongBuffSkill(int aIndex);
	void TuDongDanhSkill(int aIndex); 
	bool GetTargetPlayer(LPOBJ lpObj, int SkillNumber, int* MonsterIndex); 
	bool GetTargetMonster(LPOBJ lpObj, int SkillNumber, int* MonsterIndex);
	void SendSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber);
	void SendMultiSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber);
	void SendDurationSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber);
	void SendRFSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber);
	void GuiYCParty(int aIndex, int bIndex);
	void ChatRecv(LPOBJ lpSender, const char* message);
	void LoadFakeBotTradeConfig(const char* path);
	bool CanStartTradeWithBot(int playerIndex, LPOBJ lpBot);
	void SendBotTradeItemsToPlayer(int playerIndex, LPOBJ lpBot);
	bool HandleFakeBotTrade(int playerIndex, LPOBJ lpBot);
	void TradeCancel(int aIndex);
	int CountTradeItems(int aIndex);
	bool IsUniqueItemType(std::vector<MixesItems>& needList, int currentIndex);
	void InitTradeDisplay(int playerIndex, LPOBJ lpBot);
	void DebugTradePackets(int playerIndex);
	void StoreBotTradeItems(LPOBJ lpBot, int playerIndex);
	void GCTradeResponseSend(int aIndex, BYTE response, const char* name, int level, int guildNumber);
	bool BotHasRewardItems(LPOBJ lpBot, const std::vector<MixesItems>& rewardItems);
	bool MoveBotRewardItemsToTrade(LPOBJ lpBot, const std::vector<MixesItems>& rewardItems);
	bool InitializeBotTrade(int playerIndex, LPOBJ lpBot);
	int GetJewelBankCount(LPOBJ lpBot, int jewelType);
	bool DecreaseBotJewelBank(LPOBJ lpBot, int jewelType, int count);
	std::string GetItemName(int itemType);

public:
	std::map<std::string, FAKEBOT_TRADE_ITEM> m_TradeData;
	std::map<std::string, OFFEXP_DATA> m_Data;
	int AccountsRestored;
	DWORD TimeFakeLogIn;
    std::map<int, BotActivePVPCombatState> m_botPVPCombatStates; 
	//void CheckAutoReset(LPOBJ lpObj); // Asegúrate que esta línea exista

private:
	std::string ReplaceTradePlaceholders(const std::string& phrase, const std::string& botAccount);
	DWORD m_dwLastCommentTick[MAX_OBJECT];
	DWORD m_dwLastPlayerNearbyCommentTick[MAX_OBJECT]; // Nuevo cooldown
	DWORD m_dwLastLocalChatTick[MAX_OBJECT]; // <-- AÑADE ESTA LÍNEA
    CRITICAL_SECTION m_BotDataMutex;

};
   
// Funciones globales y variables externas
void LoadBotKeywordResponses(const char* filename);
void LoadBotPhrasesFromFile(const char* filename); 
std::string GetRandomBotPhrase(int currentMap, bool realPlayerNearby, bool inParty, bool inActivePVPCombat); 

extern std::vector<std::string> g_BotPhrasesGeneral;
extern std::vector<std::string> g_BotPhrasesNear;
extern std::vector<std::string> g_BotPhrasesInParty; 
extern std::vector<std::string> g_BotPhrasesPVP;
extern std::vector<std::string> g_BotPhrasesTrade;
// NUEVO: Frases según hora del día
extern std::vector<std::string> g_BotPhrasesMorning;
extern std::vector<std::string> g_BotPhrasesAfternoon;
extern std::vector<std::string> g_BotPhrasesNight;
extern std::map<int, std::vector<std::string>> g_BotPhrasesMapSpecific;


// NUEVO: Variables globales para probabilidades configurables
extern int g_ProbGeneral;
extern int g_ProbNearRealPlayer;
extern int g_ProbInParty;
extern int g_ProbPVP;
extern int g_ProbTrade;
// NUEVO: Probabilidades para hora del día
extern int g_ProbMorning;
extern int g_ProbAfternoon;
extern int g_ProbNight;
extern int g_ProbMapSpecificBase; // Probabilidad si solo aplica estar en un mapa con frases específicas

extern CFakeOnline s_FakeOnline;

#endif // USE_FAKE_ONLINE == TRUE