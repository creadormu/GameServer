#include "stdafx.h"
#include "Log.h"
#include "GameMain.h"
#include "User.h"
#include "DSProtocol.h"
#include "BotTrader.h"
#include "Util.h"
#include "Item.h"
#include "ReadScript.h"
#include "SetItemOption.h"
#include "Monster.h"
#include "Trade.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "MuunSystem.h"
#include "GensSystem.h"
#include "FakeOnline.h"
#include <string>
#include <algorithm>

ObjBotTrader BotTrader;

int ObjBotTrader::MixNumber(int index)
{
	for(int i=0;i<this->MixCount;i++)
	{
		if(this->Mix[i].Index == index)
			return i;
	}
	return -1;
}

void ObjBotTrader::Read(char * FilePath)
{
	if(this->Enabled == true)
	{
		for(int botNum=0;botNum<MAX_BOTTRADER;botNum++)
		{
			if(this->bot[botNum].Enabled == true)
			{
				int bIndex = this->bot[botNum].index;
				if(gObjIsConnected(bIndex) == TRUE)
				{
					gObjDel(bIndex);
				}
			}
		}
	}

	memset(&this->bot,0,sizeof(this->bot));
	this->Enabled = FALSE;
	int Token;
	SMDFile = fopen(FilePath, "r");
	this->MixCount = 0;

	if ( SMDFile == NULL )
	{
		ErrorMessageBox("BotTrader data load error %s", FilePath);
		return;
	}

	while ( true )
	{
		int iType = GetToken();
		
		if ( iType == 1 )
		{
			while(true)
			{
				Token = GetToken();
				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				int BotNum = TokenNumber;
				if(BotNum < 0 || BotNum > MAX_BOTTRADER-1)
				{
					ErrorMessageBox("BotTrader error: %d out of range!", BotNum);
					return;
				}

				Token = GetToken();
				this->bot[BotNum].Class = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].Enabled = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].ChangeColorName = TokenNumber;
				
				Token = GetToken();
				strncpy(this->bot[BotNum].Name,TokenString,sizeof(this->bot[BotNum].Name));

				Token = GetToken();
				this->bot[BotNum].Map = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].X = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].Y = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].Dir = TokenNumber;

				this->bot[BotNum].MixesCount = 0;

			}
		}
		iType = GetToken();
		iType = TokenNumber;
		if ( iType == 2 )
		{
			while(true)
			{
				Token = GetToken();
				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				int BotNum = TokenNumber;
				if(BotNum < 0 || BotNum > MAX_BOTTRADER-1)
				{
					ErrorMessageBox("BotTrader error: %d doesnt exist", BotNum);
					return;
				}
				
				Token = GetToken();
				this->bot[BotNum].OnlyVip = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].ActiveGensFamily = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].ReqContribution = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].GensFamily = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].Money = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].PCPoint = TokenNumber;
			}
		}		
		iType = GetToken();
		iType = TokenNumber;
		if ( iType == 3 )
		{
			while(true)
			{
				Token = GetToken();
				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				int BotNum = TokenNumber;
				if(BotNum < 0 || BotNum > MAX_BOTTRADER-1)
				{
					ErrorMessageBox("BotTrader error: BotTraderIndex: %d doesnt exist", BotNum);
					return;
				}
				
				Token = GetToken();
				int Slot = TokenNumber;
				
				if(Slot < 0 || Slot > 8)
				{	
					ErrorMessageBox("BotTrader error: Min Slot 0 ; Max Slot 8");
					return;
				}

				Token = GetToken();
				int iType = TokenNumber;

				Token = GetToken();
				int iIndex = TokenNumber;

				this->bot[BotNum].body[Slot].num = GET_ITEM(iType,iIndex);

				Token = GetToken();
				this->bot[BotNum].body[Slot].level = TokenNumber;

				Token = GetToken();
				this->bot[BotNum].body[Slot].opt = TokenNumber;

				this->bot[BotNum].body[Slot].Enabled = true;
			}
		}
		iType = GetToken();
		iType = TokenNumber;		
		if ( iType == 4 )
		{
			while(true)
			{
				Token = GetToken();
				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}
				this->Mix[this->MixCount].Index = TokenNumber;

				Token = GetToken();
				strcpy(this->Mix[this->MixCount].Name,TokenString);

				Token = GetToken();
				this->Mix[this->MixCount].SuccessRate = TokenNumber;

				this->MixCount++;
			}
		}		
		iType = GetToken();
		iType = TokenNumber;
		if ( iType == 5 )
		{
			while(true)
			{
				Token = GetToken();
				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}
				int mIndex = TokenNumber;

				int Num = this->MixNumber(mIndex);

				if (Num  == -1)
				{
					ErrorMessageBox("BotTrader data error on Need Items (INDEX:%d)", mIndex);
					return;
				}

				Token = GetToken();
				int iType = TokenNumber;

				Token = GetToken();
				int iIndex = TokenNumber;

				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].Type = GET_ITEM(iType,iIndex);

				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].LevelMin = 0;

				Token = GetToken();
				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].LevelMax = TokenNumber;

				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].OptionMin = 0;

				Token = GetToken();
				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].OptionMax = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].Luck = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].Skill = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].Exc = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Need[this->Mix[Num].i_NeedCount].Dur = TokenNumber;

				this->Mix[Num].i_NeedCount++;
			}
		}		
		iType = GetToken();
		iType = TokenNumber;
		if ( iType == 6 )
		{
			while(true)
			{
				Token = GetToken();
				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}
				int mIndex = TokenNumber;

				int Num = this->MixNumber(mIndex);

				if (Num  == -1)
				{
					ErrorMessageBox("TradeMix data error on Success Items (INDEX:%d)", mIndex);
					return;
				}

				Token = GetToken();
				int iType = TokenNumber;

				Token = GetToken();
				int iIndex = TokenNumber;

				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].Type = GET_ITEM(iType,iIndex);

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].LevelMin = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].LevelMax = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].OptionMin = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].OptionMax = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].Luck = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].Skill = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].Exc = TokenNumber;

				Token = GetToken();
				this->Mix[Num].i_Success[this->Mix[Num].i_SuccessCount].Dur = TokenNumber;

				this->Mix[Num].i_SuccessCount++;
			}
		}		
		iType = GetToken();
		iType = TokenNumber;		
		if ( iType == 7 )
		{
			int Count = 0;
			while(true)
			{
				Token = GetToken();
				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				int BotNum = TokenNumber;
				if(BotNum < 0 || BotNum > MAX_BOTTRADER-1)
				{
					ErrorMessageBox("BotTrader error: %d doesnt exist", BotNum);
					return;
				}
				
				Token = GetToken();
				int mIndex = TokenNumber;

				int Num = this->MixNumber(mIndex);

				if (Num  == -1)
				{
					LogAdd(LOG_BOT,"TradeMix data error on Success Items (INDEX:%d)", mIndex);
					return;
				}
				this->bot[BotNum].MixesIndex[this->bot[BotNum].MixesCount] = Num;

				this->bot[BotNum].MixesCount++;
				this->Enabled = true;
			}
		}		
		break;
	}			
	fclose(SMDFile);
}

void ObjBotTrader::MakeBot()
{
	if(this->Enabled == true)
	{
		for(int botNum=0;botNum<MAX_BOTTRADER;botNum++)
		{
			if(this->bot[botNum].Enabled == true)
			{
				int result = gObjAddSummon();

				if(result >= 0)
				{
					this->bot[botNum].index = result;
					gObj[result].PosNum = (WORD)-1;
					gObj[result].X = this->bot[botNum].X;
					gObj[result].Y = this->bot[botNum].Y;
					gObj[result].OldX = this->bot[botNum].X;
					gObj[result].OldY = this->bot[botNum].Y;
					gObj[result].TX = this->bot[botNum].X;
					gObj[result].TY = this->bot[botNum].Y;
					gObj[result].MTX = this->bot[botNum].X;
					gObj[result].MTY = this->bot[botNum].Y;
					gObj[result].Dir = this->bot[botNum].Dir;
					gObj[result].Map = this->bot[botNum].Map;
					gObj[result].Live = TRUE;
					gObj[result].PathCount = 0;
					gObj[result].IsBot = Type_Trader;
					gObjSetBots(result,this->bot[botNum].Class);

					gObj[result].ChangeUp = this->bot[botNum].Class & 0x07;
					gObj[result].Class = this->bot[botNum].Class;
					gObj[result].Level = 400;
					gObj[result].Life = 1;
					gObj[result].MaxLife = 2;
					gObj[result].Mana = 1;
					gObj[result].MaxMana = 2;
					gObj[result].Experience = 0;
					gObj[result].DBClass = this->bot[botNum].Class;
					gObj[result].PKLevel = this->bot[botNum].ChangeColorName;
					gObj[result].GensFamily = this->bot[botNum].GensFamily;
					
					#if(GAMESERVER_UPDATE>=802)
					gObj[result].LoadMuunInventory = 0;
					#endif

					gObj[result].Inventory = new CItem[MAIN_INVENTORY_NORMAL_SIZE];					
					gObj[result].Inventory1 = new CItem[MAIN_INVENTORY_NORMAL_SIZE];
					gObj[result].InventoryMap1 = new BYTE[MAIN_INVENTORY_NORMAL_SIZE];
					gObj[result].InventoryMap = new BYTE[MAIN_INVENTORY_NORMAL_SIZE];

					#if(GAMESERVER_UPDATE>=802)
					gObj[result].MuunInventory = new CItem[MAIN_INVENTORY_NORMAL_SIZE+ (MAX_EXTENDED_INV*EXTENDED_INV_SIZE) + PSHOP_SIZE]; //+ (MAX_EXTENDED_INV*EXTENDED_INV_SIZE) + PSHOP_SIZE
					gObj[result].MuunInventoryMap = new BYTE[MAIN_INVENTORY_NORMAL_SIZE+ (MAX_EXTENDED_INV*EXTENDED_INV_SIZE) + PSHOP_SIZE]; // + (MAX_EXTENDED_INV*EXTENDED_INV_SIZE) + PSHOP_SIZE
					#endif

					for (int i=0;i<MAIN_INVENTORY_NORMAL_SIZE;i++)
					{
						gObj[result].Inventory[i].Clear();
						gObj[result].Inventory1[i].Clear();
					}

					memset(&gObj[result].InventoryMap[0], (BYTE)-1, MAIN_INVENTORY_NORMAL_SIZE);
					memset(&gObj[result].InventoryMap1[0], (BYTE)-1, MAIN_INVENTORY_NORMAL_SIZE);

					#if(GAMESERVER_UPDATE>=802)
					memset(&gObj[result].MuunInventoryMap[0], (BYTE)-1, MUUN_INVENTORY_SIZE);
					#endif

					strncpy(gObj[result].Name,this->bot[botNum].Name,sizeof(gObj[result].Name));


					for(int i=0;i<9;i++)
					{
						if(this->bot[botNum].body[i].num >= 0 && this->bot[botNum].body[i].Enabled == true)
						{
							CItem item;
							item.m_Level = this->bot[botNum].body[i].level;
							item.m_Option1 = 0;
							item.m_Option2 = 1;
							item.m_Option3 = this->bot[botNum].body[i].opt;	
							item.m_Durability = 255.0f;
							item.m_JewelOfHarmonyOption = 0;
							item.m_ItemOptionEx = 0;
							item.m_SocketOption[0] = 0;
							item.m_SocketOption[1] = 0;
							item.m_SocketOption[2] = 0;
							item.m_SocketOption[3] = 0;
							item.m_SocketOption[4] = 0;
							item.Convert(this->bot[botNum].body[i].num,item.m_Option1,item.m_Option2,item.m_Option3,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus);
							
							gObj[result].Inventory[i].m_Option1 = item.m_Option1;
							gObj[result].Inventory[i].m_Option2 = item.m_Option2;
							gObj[result].Inventory[i].m_Option3 = item.m_Option3;
							gObj[result].Inventory[i].m_JewelOfHarmonyOption = item.m_JewelOfHarmonyOption;
							gObj[result].Inventory[i].m_ItemOptionEx = item.m_ItemOptionEx;

							item.m_Serial = 0;						
							gObjInventoryInsertItemPos(gObj[result].Index,item,i,0);
						}
					}
					
					gObj[result].Inventory1 = gObj[result].Inventory;
					gObj[result].InventoryMap1 = gObj[result].InventoryMap;

					#if(GAMESERVER_UPDATE>=802)
					gObj[result].MuunInventory = gObj[result].MuunInventory;
					gObj[result].MuunInventoryMap = gObj[result].MuunInventoryMap;
					#endif

					gObjectManager.CharacterMakePreviewCharSet(result);

					gObj[result].AttackType = 0;
					gObj[result].BotSkillAttack = 0;					
					gObj[result].Attribute = 100;
					gObj[result].TargetNumber = (WORD)-1;
					gObj[result].ActionState.Emotion = 0;
					gObj[result].ActionState.Attack = 0;
					gObj[result].ActionState.EmotionCount = 0;
					gObj[result].PathCount = 0;
					gObj[result].BotPower = 0;
					gObj[result].BotDefense = this->bot[botNum].OnlyVip;
					gObj[result].BotLife = 1;
					gObj[result].BotMaxLife = 1;
					gObj[result].BotLvlUpDefense = 1;
					gObj[result].BotLvlUpPower = 1;
					gObj[result].BotLvlUpLife = 1;
					gObj[result].BotLvlUpMana = 1;
					gObj[result].BotLvlUpExp = 1;
					gObj[result].BotLvlUpMaxLevel = 1;
					gObj[result].MoveRange = 1;
					gObj[result].BotFollowMe = 0;
					gObj[result].NextExperience = gLevelExperience[gObj[result].Level];

					GCSummonLifeSend(gObj[result].SummonIndex,gObj[result].MaxLife,gObj[result].Life);
	
				}
			}
		}
	}
}

BYTE ObjBotTrader::gObjInventoryInsertItemPos(int aIndex, CItem item, int pos, BOOL RequestCheck) //OK
{

	if(OBJECT_RANGE(aIndex) == 0)
	{
		return -1;
	}

	LPOBJ lpObj;

	int useClass = 0;

	lpObj = &gObj[aIndex];

	if(lpObj->Inventory[pos].IsItem() == 1)
	{
		return -1;
	}

	if(item.IsItem() == 0)
	{
		return -1;
	}

	if(pos < 12)
	{
		if(lpObj->IsBot == 0)
		{
			useClass = item.IsClassBot((char)lpObj->Class,lpObj->ChangeUp);

			if(useClass == 0)
			{
				LogAdd(LOG_BLUE,"[Using Class Error] Error UseClass %d",useClass);
				return -1;
			}

		}
	}

	lpObj->Inventory[pos] = item;

	return pos;
}

BOOL ObjBotTrader::IsInTrade(int aIndex)
{
	int number = this->GetBotIndex(gObj[aIndex].TargetNumber);
	if(number != -1)
		return 1;
	else
		return 0;
}

int ObjBotTrader::GetBotIndex(int aIndex)
{
	for(int i=0;i<MAX_BOTTRADER;i++)
	{
		if(this->bot[i].Enabled == true)
		{
			if(this->bot[i].index == aIndex)
				return i;
		}
	}
	return -1;
}

void ObjBotTrader::TradeCancel(int aIndex)
{
	gTrade.ResetTrade(aIndex);	
	gTrade.GCTradeResultSend(aIndex,3);
}

void ObjBotTrader::TradeOk(int aIndex)
{
	int MixNum=-1;
	int MixItem=-1;

	int number = this->GetBotIndex(gObj[aIndex].TargetNumber);

	if(number == -1)
	{			
		gObj[aIndex].Interface.state = 0;
		gObj[aIndex].Interface.type = 0;
		gObj[aIndex].TargetShopNumber = -1;
		gObj[aIndex].Transaction = 0;

		return;
	}
	this->TradeMix(aIndex,number);
}

BOOL ObjBotTrader::TradeOpen(int index, int nindex)
{
	if (gObjIsConnected(index) == 0 || gObjIsConnected(nindex) == 0)
		return 0;

	LPOBJ lpObj = &gObj[index];
	LPOBJ lpBot = &gObj[nindex];

	// ================================
	// SOPORTE PARA FAKEONLINE
	// ================================
	if (lpBot->IsFakeOnlineBot)
	{
		LogAdd(LOG_RED, "[FakeBotTrade] Trade solicitado por %s hacia bot FAKE %s", lpObj->Name, lpBot->Name);

		if (lpObj->Interface.use > 0)
			return 0;

		// Get trade configuration to check if bot can trade
		std::string acc = lpBot->Account;
		std::transform(acc.begin(), acc.end(), acc.begin(), ::toupper);
		auto it = s_FakeOnline.m_TradeData.find(acc);
		
		if (it == s_FakeOnline.m_TradeData.end()) {
			gNotice.NewNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, "Este bot no puede hacer trade.");
			return 0;
		}

		const auto& config = it->second;

		// FIXED: Check if bot has required items BEFORE accepting trade
		if (!s_FakeOnline.BotHasRewardItems(lpBot, config.rewardItems)) {
			gNotice.NewNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, "El bot no tiene los items necesarios en este momento.");
			LogAdd(LOG_RED, "[FakeBotTrade] Bot %s cannot trade - missing items", lpBot->Name);
			return 0;
		}

		for (int n = 0; n < TRADE_SIZE; n++)
			lpObj->Trade[n].Clear();

		memset(lpObj->TradeMap, (BYTE)-1, TRADE_SIZE);

		gObjInventoryTransaction(lpObj->Index);
		gTrade.GCTradeResponseSendBOT(true, lpObj->Index, lpBot->Name, 400, 0);
		gTrade.GCTradeOkButtonSend(lpObj->Index, 1);

		lpObj->Interface.state = 1;
		lpObj->Interface.use = 1;
		lpObj->Interface.type = 1;
		lpObj->TradeMoney = 0;
		lpObj->TargetNumber = lpBot->Index;
		lpObj->Transaction = 1;

		//Congelar tambi�n el bot para que no camine o ataque mientras est� en trade
		lpBot->Interface.state = 1;
		lpBot->Interface.use = 1;
		lpBot->Interface.type = 1;
		lpBot->TargetNumber = lpObj->Index;
		lpBot->Transaction = 1;

		// FIXED: Move bot items to trade window and show them to player
		// This happens when trade opens, NOT when player presses OK
		if (!s_FakeOnline.MoveBotRewardItemsToTrade(lpBot, config.rewardItems)) {
			gNotice.NewNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, "Error preparando items del bot.");
			// Clean up the trade state
			lpObj->Interface.use = 0;
			lpObj->Interface.type = 0;
			lpObj->Interface.state = 0;
			lpObj->TargetNumber = -1;
			lpBot->Interface.use = 0;
			lpBot->Interface.type = 0;
			lpBot->Interface.state = 0;
			lpBot->TargetNumber = -1;
			return 0;
		}

		// Now show bot items to player (using proper visibility function)
		s_FakeOnline.SendBotTradeItemsToPlayer(lpObj->Index, lpBot);

		// Build required items message with item names
		std::string requiredMsg = "";
		for (size_t i = 0; i < config.requiredItems.size(); i++) {
			if (i > 0) requiredMsg += " + ";
			requiredMsg += s_FakeOnline.GetItemName(config.requiredItems[i].Type);
		}

		// Build reward items message with item names
		std::string rewardMsg = "";
		for (size_t i = 0; i < config.rewardItems.size(); i++) {
			if (i > 0) rewardMsg += " + ";
			rewardMsg += s_FakeOnline.GetItemName(config.rewardItems[i].Type);
		}

		// Send notice to player with actual item names
		char tradeMsg[256];
		sprintf_s(tradeMsg, sizeof(tradeMsg), "%s: Need %s, will give %s. Put items and press OK!", 
			lpBot->Name, requiredMsg.c_str(), rewardMsg.c_str());
		gNotice.NewNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, tradeMsg);
		
		LogAdd(LOG_GREEN, "[FakeBotTrade] Trade abierto con %s (FAKE) - Bot items shown", lpBot->Name);
		return 1;
	}

	// ================================
	// SOPORTE PARA BOTTRADER.INI (original)
	// ================================
	int number = this->GetBotIndex(nindex);
	if (number == -1)
		return 0;

	if (this->bot[number].OnlyVip != 0 && lpObj->AccountLevel == 0)
	{
		gNotice.NewNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, "%s : Im Only work for Account Vips!", this->bot[number].Name);
		LogAdd(LOG_BOT, "[Warning] Not Open [Bot: %s] Name: %s  AccountLevel Free", this->bot[number].Name, gObj[index].Account);
		return 1;
	}

	if (this->bot[number].ActiveGensFamily == 1)
	{
		if (this->bot[number].GensFamily != GENS_FAMILY_VARNERT && gObj[index].GensFamily == GENS_FAMILY_NONE)
		{
			gNotice.NewNoticeSend(gObj[index].Index, 0, 0, 0, 0, 0, "%s : You Need be GensFamily Vanert", this->bot[number].Name);
			return 1;
		}
		if (this->bot[number].GensFamily != GENS_FAMILY_DUPRIAN && gObj[index].GensFamily == GENS_FAMILY_NONE)
		{
			gNotice.NewNoticeSend(gObj[index].Index, 0, 0, 0, 0, 0, "%s : You Need be GensFamily Duprian", this->bot[number].Name);
			return 1;
		}
		if (this->bot[number].GensFamily == GENS_FAMILY_VARNERT && gObj[index].GensFamily == GENS_FAMILY_DUPRIAN)
		{
			gNotice.NewNoticeSend(gObj[index].Index, 0, 0, 0, 0, 0, "%s : You Need be GensFamily Duprian", this->bot[number].Name);
			return 1;
		}
		if (this->bot[number].GensFamily == GENS_FAMILY_DUPRIAN && gObj[index].GensFamily == GENS_FAMILY_VARNERT)
		{
			gNotice.NewNoticeSend(gObj[index].Index, 0, 0, 0, 0, 0, "%s : You Need be GensFamily Vanert", this->bot[number].Name);
			return 1;
		}
		if (this->bot[number].ReqContribution > gObj[index].GensContribution)
		{
			gNotice.NewNoticeSend(gObj[index].Index, 0, 0, 0, 0, 0, "%s : Insuficent GensContribution : %d", this->bot[number].Name, this->bot[number].ReqContribution);
			LogAdd(LOG_RED, "[%s] Account: %s - Insuficent GensContribution : %d", this->bot[number].Name, gObj[index].Account, this->bot[number].ReqContribution);
			return 1;
		}
	}

	if (this->Enabled == TRUE)
	{
		if (lpObj->Interface.use > 0)
		{
			return 0;
		}
		else
		{
			for (int n = 0; n < TRADE_SIZE; n++)
			{
				lpObj->Trade[n].Clear();
			}
			memset(lpObj->TradeMap, (BYTE)-1, TRADE_SIZE);

			gObjInventoryTransaction(lpObj->Index);
			gTrade.GCTradeResponseSendBOT(true, lpObj->Index, lpBot->Name, 400, 0);
			gTrade.GCTradeOkButtonSend(lpObj->Index, 1);
			lpObj->Interface.state = 1;
			lpObj->Interface.use = 1;
			lpObj->Interface.type = 1;
			lpObj->TradeMoney = 0;
			lpObj->TargetNumber = lpBot->Index;
			lpObj->Transaction = 1;

			LogAdd(LOG_BOT, "[BotTrader] (%s)(%s) OPEN", gObj[index].Account, gObj[index].Name);
			gNotice.NewNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, "%s : I'm Ready: %s", gObj[nindex].Name, gObj[index].Name);

			if (this->bot[number].PCPoint > 0)
			{
				gNotice.NewNoticeSend(gObj[index].Index, 0, 0, 0, 0, 0, "%s: I Need %d PCPoint!", this->bot[number].Name, this->bot[number].PCPoint);
			}
			if (this->bot[number].Money > 0)
			{
				gNotice.NewNoticeSend(gObj[index].Index, 0, 0, 0, 0, 0, "%s: I Need %d Money!", this->bot[number].Name, this->bot[number].Money);
			}
		}
	}
	return 1;
}

int ObjBotTrader::ItemCount(int aIndex)
{
	int Count=0;
	for(int n = 0; n < TRADE_SIZE; n++)
	{
		if(gObj[aIndex].Trade[n].IsItem() == 1)
		{
			Count++;
		}
	}
	return Count;
}

bool ObjBotTrader::SearchSameIDNumber(int mIndex, int To)
{
	if(To == 0)
		return true;
	for(int in=0;in<To;in++)
	{
		if(this->Mix[mIndex].i_Need[in].Type == this->Mix[mIndex].i_Need[To].Type)
		{
			return false;
		}
	}
	return true;
}

BYTE ObjBotTrader::TradeMix(int aIndex,int BotNum)
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return 0;
	}

	int mIndex = -1;
	
	for(int i=0;i<this->bot[BotNum].MixesCount;i++)
	{
		mIndex = this->bot[BotNum].MixesIndex[i];
		if(this->Mix[mIndex].i_NeedCount == this->ItemCount(aIndex))
		{
			int found=0;
			for(int in=0;in<this->Mix[mIndex].i_NeedCount;in++)
			{
				if(this->SearchSameIDNumber(mIndex,in) == true)
				{
					for(int n = 0; n < TRADE_SIZE; n++)
					{
						if(gObj[aIndex].Trade[n].IsItem() == 1)
						{
							if(this->Mix[mIndex].i_Need[in].Type == gObj[aIndex].Trade[n].m_Index)
							{
								if(gObj[aIndex].Trade[n].m_Level >= this->Mix[mIndex].i_Need[in].LevelMax && 
									gObj[aIndex].Trade[n].m_Option2 >= this->Mix[mIndex].i_Need[in].Luck && 
									gObj[aIndex].Trade[n].m_Option3 >= this->Mix[mIndex].i_Need[in].OptionMax && 
									gObj[aIndex].Trade[n].m_Option1 >= this->Mix[mIndex].i_Need[in].Skill && 
									gObj[aIndex].Trade[n].m_NewOption >= this->Mix[mIndex].i_Need[in].Exc && 
									gObj[aIndex].Trade[n].m_Durability >= this->Mix[mIndex].i_Need[in].Dur)
								{
									found++;
								}
							}
						}
					}
				}
			}
			if(found == this->Mix[mIndex].i_NeedCount)
			{
				goto OK;
			}
		}
	}
	gNotice.NewNoticeSend(aIndex,0,0,0,0,0,"%s : I dont need those items!",this->bot[BotNum].Name);
	goto Cancel;
OK:
	if(mIndex >= 0)
	{
		if(this->bot[BotNum].PCPoint > 0)
		{
			if((gObj[aIndex].PCPoint - this->bot[BotNum].PCPoint) < 0)
			{
				gNotice.NewNoticeSend(aIndex,0,0,0,0,0,"%s : Insuficent PCPoints! %d ",this->bot[BotNum].Name,this->bot[BotNum].PCPoint);
				LogAdd(LOG_RED,"[%s] - (%s) Insuficent PCPoints",this->bot[BotNum].Name,gObj[aIndex].Name);
				goto Cancel;		
			}
		}
		if((gObj[aIndex].TradeMoney - this->bot[BotNum].Money) < 0)
		{
			gNotice.NewNoticeSend(aIndex,0,0,0,0,0,"%s : Insuficent Money! %d ",this->bot[BotNum].Name,this->bot[BotNum].Money);
			LogAdd(LOG_RED,"[%s] - (%s) Insuficent Money",this->bot[BotNum].Name,gObj[aIndex].Name);
			goto Cancel;		
		}
		
		srand(static_cast<int>(time(NULL)));
		int random = rand()%100;

		if(random <= this->Mix[mIndex].SuccessRate)
		{
			srand(static_cast<int>(time(NULL)));
			int rItem = rand()%this->Mix[mIndex].i_SuccessCount;
			int Level=0;
			int Opt=0;
			int Exc=0;
			int Luck=0;
			int Skill=0;

			if(this->Mix[mIndex].i_Success[rItem].LevelMax == this->Mix[mIndex].i_Success[rItem].LevelMin)
			{
				Level=this->Mix[mIndex].i_Success[rItem].LevelMax;
			}else
			{
				srand(static_cast<int>(time(NULL)));
				Level = rand()%(this->Mix[mIndex].i_Success[rItem].LevelMax - this->Mix[mIndex].i_Success[rItem].LevelMin) + this->Mix[mIndex].i_Success[rItem].LevelMin;
			}
			if(this->Mix[mIndex].i_Success[rItem].OptionMax == this->Mix[mIndex].i_Success[rItem].OptionMin)
			{
				Opt=this->Mix[mIndex].i_Success[rItem].OptionMax;
			}else
			{
				srand(static_cast<int>(time(NULL)));
				Opt = rand()%(this->Mix[mIndex].i_Success[rItem].OptionMax - this->Mix[mIndex].i_Success[rItem].OptionMin) + this->Mix[mIndex].i_Success[rItem].OptionMin;
			}
			if(this->Mix[mIndex].i_Success[rItem].Exc > 0)
			{ 
				Exc= gSetItemOption.GetSetItemMaxOptionCount(this->Mix[mIndex].i_Success[rItem].Exc);
			}
			if(this->Mix[mIndex].i_Success[rItem].Luck == 0)
			{
				Luck = 0;
			}else
			{
				srand(static_cast<int>(time(NULL)));
				BYTE lRnd = rand()%100;
				if(lRnd <= this->Mix[mIndex].i_Success[rItem].Luck)
				{
					Luck=1;
				}else
				{
					Luck=0;
				}
			}
			if(this->Mix[mIndex].i_Success[rItem].Skill == 0)
			{
				Skill = 0;
			}else
			{
				srand(static_cast<int>(time(NULL)));
				BYTE sRnd = rand()%100;
				if(sRnd <= this->Mix[mIndex].i_Success[rItem].Skill)
				{
					Skill=1;
				}else
				{
					Skill=0;
				}
			}
			LPOBJ lpObj = &gObj[aIndex];
			BYTE TempInventoryMap[MAIN_INVENTORY_NORMAL_SIZE];
			memcpy(TempInventoryMap,lpObj->InventoryMap,MAIN_INVENTORY_NORMAL_SIZE);	//64

			if (gItemManager.CheckItemInventorySpace(lpObj, 4, 4) == 0)
			{
				gNotice.NewNoticeSend(gObj[aIndex].Index,0,0,0,0,0,"%s: Insuficent space in inventory!",this->bot[BotNum].Name);
				LogAdd(LOG_BOT,"[BotTrader](%s)(%s) Doesnt have space on inventory",gObj[aIndex].Account,gObj[aIndex].Name);
				goto Cancel;
			}
			else
			{
				gNotice.NewNoticeSend(gObj[aIndex].Index,0,0,0,0,0,"%s : Combination Succeded!",this->bot[BotNum].Name);
				GDCreateItemSend(lpObj->Index,235,0,0,(this->Mix[mIndex].i_Success[rItem].Type),Level,0,Skill,Luck,Opt,-1,Exc,0,0,0,0,0xFE,0);
			}
		} else {
			gNotice.NewNoticeSend(aIndex,0,0,0,0,0,"%s : Combination Failed!",this->bot[BotNum].Name);

				LogAdd(LOG_RED,"[BotTrader](%s)(%s) %s Fail [%d/%d]",
				gObj[aIndex].Account,gObj[aIndex].Name,
				this->Mix[mIndex].Name,
				random,this->Mix[mIndex].SuccessRate);
		}

		if(this->bot[BotNum].PCPoint > 0)
		{
			gObj[aIndex].PCPoint -= this->bot[BotNum].PCPoint;
			gNotice.NewNoticeSend(aIndex,0,0,0,0,0,"-%d PCPoint: %d",this->bot[BotNum].PCPoint,gObj[aIndex].PCPoint); 
		}

		gObj[aIndex].Money -= this->bot[BotNum].Money;

		gObjInventoryCommit(aIndex);
		gObjectManager.CharacterMakePreviewCharSet(aIndex);
		GDCharacterInfoSaveSend(aIndex);
		gObj[aIndex].TargetNumber = -1;
		gObj[aIndex].Interface.use = 0;
		gObj[aIndex].TradeOk = 0;
		gObj[aIndex].TradeMoney = 0;
		GCMoneySend(aIndex,gObj[aIndex].Money);
		gTrade.GCTradeResultSend(aIndex,1);
		gItemManager.GCItemListSend(aIndex);
		
		for(int n = 0; n < TRADE_SIZE; n++)
		{
			gObj[aIndex].Trade[n].Clear();
		}
		gObjNotifyUpdateUnionV1(&gObj[aIndex]);
		gObjNotifyUpdateUnionV2(&gObj[aIndex]);

		LogAdd(LOG_BOT,"[BotTrader](%s)(%s) %s Completed [%d/%d]",
			gObj[aIndex].Account,gObj[aIndex].Name,
			this->Mix[mIndex].Name,
			random, this->Mix[mIndex].SuccessRate
		);
		return 1;
	}

Cancel:
	this->TradeCancel(aIndex);
	return 0;
}

