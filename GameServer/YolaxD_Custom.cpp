#include "stdafx.h"
#include "YolaxD_Custom.h"
#include "ItemManager.h"
#include "Util.h"
#include "MonsterSetBase.h"
#include "MapServerManager.h"
#include "ChaosCastle.h"
#include "Monster.h"
#include "Item.h"
#include "SocketItemType.h"
#include "ObjectManager.h"
#include "GameMain.h"
#include "Map.h"
#include "Protocol.h"
#include "Trade.h"
#include "DarkSpirit.h"
#include "Notice.h"
#include "PersonalShop.h"
#include "Warehouse.h"



YolaCustom ObjYola;

BOOL gItemSerialCheck=1;
BOOL gItemZeroSerialCheck;

YolaCustom::YolaCustom()
{
}

YolaCustom::~YolaCustom()
{
}

//Pakete 

void YolaCustom::PHeadSetB(LPBYTE lpBuf, BYTE head, int size) //PBMSG_HEAD parece ser este
{
	lpBuf[0] =0xC1;		// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
}

void YolaCustom::PHeadSubSetB(LPBYTE lpBuf, BYTE head, BYTE sub, int size) //PBMSG_HEAD2 parece ser este
{
	lpBuf[0] =0xC1;	// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
	lpBuf[3] =sub;
}

void YolaCustom::PHeadSetBE( LPBYTE lpBuf, BYTE head,int size)
{
	lpBuf[0] =0xC3;
	lpBuf[1] =size;
	lpBuf[2] =head;
}

void YolaCustom::PHeadSetW(LPBYTE lpBuf, BYTE head,  int size) 
{
	lpBuf[0] = 0xC2;	// Packets Header
	lpBuf[1]= SET_NUMBERHB(size);
	lpBuf[2]= SET_NUMBERLB(size);
	lpBuf[3]= head;
}
//Inventory
BYTE YolaCustom::gObjInventoryInsertItem(int aIndex, CItem item)
{
	int w,h,iwidth,iheight;
	BYTE blank = 0;

	if(item.GetSize((int&)iwidth,(int&)iheight)==0)
	{
	//	LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

	for(h = 0; h < 8; h++)
	{
		for( w = 0; w < 8; w++)
		{
			if(*(BYTE*)(gObj[aIndex].InventoryMap+h*8+w) == 255)
			{
				blank = gObjInventoryRectCheck(aIndex,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto NEXT1;
				}

				if(blank != 255)
				{
					if(gObjCheckSerial0ItemList(&item)!=0)
					{
				//		MsgOutput(aIndex,lMsg.Get(3354));
						return -1;
					}


					if(gObjInventorySearchSerialNumber(&gObj[aIndex],item.m_Number) == 0)
					{
						return -1;
					}

					gObj[aIndex].Inventory[blank] = item;

					gObjInventoryItemSet(aIndex,blank,gObj[aIndex].Inventory[blank].m_Index);
					return blank;
				}
			}
		}
	}
NEXT1:
	if(IS_EXTENDED_INV(aIndex,1) == TRUE)
	{
		for(h = 8; h < 12; h++)
		{
			for( w = 0; w < 8; w++)
			{
				if(*(BYTE*)(gObj[aIndex].InventoryMap+h*8+w) == 255)
				{
					blank = gObjInventoryRectCheck(aIndex,w,h,iwidth,iheight);

					if(blank == 254)
					{
						goto NEXT2;
					}

					if(blank != 255)
					{
						if(gObjCheckSerial0ItemList(&item)!=0)
						{
						//	MsgOutput(aIndex,lMsg.Get(3354));
							return -1;
						}


						if(gObjInventorySearchSerialNumber(&gObj[aIndex],item.m_Number) == 0)
						{
							return -1;
						}

						gObj[aIndex].Inventory[blank] = item;

						gObjInventoryItemSet(aIndex,blank,gObj[aIndex].Inventory[blank].m_Index);
						return blank;
					}
				}
			}
		}
	}
NEXT2:
	if(IS_EXTENDED_INV(aIndex,2) == TRUE)
	{
		for(h = 12; h < 16; h++)
		{
			for( w = 0; w < 8; w++)
			{
				if(*(BYTE*)(gObj[aIndex].InventoryMap+h*8+w) == 255)
				{
					blank = gObjInventoryRectCheck(aIndex,w,h,iwidth,iheight);

					if(blank == 254)
					{
						goto GOTO_EndFunc;
					}

					if(blank != 255)
					{
						if(gObjCheckSerial0ItemList(&item)!=0)
						{
					//		MsgOutput(aIndex,lMsg.Get(3354));
							return -1;
						}


						if(gObjInventorySearchSerialNumber(&gObj[aIndex],item.m_Number) == 0)
						{
							return -1;
						}

						gObj[aIndex].Inventory[blank] = item;

						gObjInventoryItemSet(aIndex,blank,gObj[aIndex].Inventory[blank].m_Index);
						return blank;
					}
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}

BYTE YolaCustom::gObjInventoryInsertItemPos(int aIndex, CItem item, int pos, BOOL RequestCheck) //OK
{

	if(OBJMAX_RANGE(aIndex) == 0)
	{
		return -1;
	}

	if(pos < 0 || pos > MAIN_INVENTORY_NORMAL_SIZE)	//Tested
	{
		LogAdd(LOG_BOT,"Tested MAIN_INVENTORY_NORMAL_SIZE Line 21");
		return -1;
	}

	LPOBJ lpObj;

	int useClass = -1;

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
#if(ALLBOTSSTRUC == 1)
		if(lpObj->IsBot == Type_Player)
		{
			useClass = item.IsClassBot(lpObj->Class,lpObj->ChangeUp);

			if(useClass == -1)
			{
				LogAdd(LOG_BLUE,"[Using Class Error] Error UseClass %s",useClass);
				return -1;
			}

		}
#endif
	}

	lpObj->Inventory[pos] = item;

	return pos;
}

BYTE YolaCustom::gObjTempInventoryInsertItem(LPOBJ lpObj, CItem item, BYTE * TempMap)
{
	int w,h,iwidth,iheight;
	BYTE blank = 0;

	if(item.GetSize(iwidth,iheight) == 0)
	{
		LogAdd(LOG_RED,"Test Line: %d  Index: %d",__FILE__,__LINE__,item.m_Index);
		return -1;
	}
	for(h = 0; h < 8; h++)
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(TempMap + h * 8 + w) == 255)
			{
				blank = gObjTempInventoryRectCheck(TempMap,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto GOTO_EndFunc;
				}
				if(blank != 255)
				{
					gObjTempInventoryItemBoxSet(TempMap,blank,iwidth,iheight,item.m_Index);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE YolaCustom::gObjTempInventoryInsertItem(LPOBJ lpObj, int itemType, BYTE * TempMap)
{
	int w,h,iwidth,iheight;
	BYTE blank = 0;

	
	iwidth=ItemAttribute[itemType].Width;
	iheight=ItemAttribute[itemType].Height;

	for(h = 0; h < 8; h++)
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(TempMap + h * 8 + w) == 255)
			{
				blank = gObjTempInventoryRectCheck(TempMap,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto GOTO_EndFunc;
				}
				if(blank != 255)
				{
					gObjTempInventoryItemBoxSet(TempMap,blank,iwidth,iheight,itemType);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE YolaCustom::gObjTempInventoryRectCheck(BYTE * TempMap, int sx, int sy, int width, int height)
{
		int x,y,blank = 0;

		if(sx + width > 8)
		{
			return -1;
		}
		if(sy + height > 8)
		{
			return -2;
		}
		if(sy < 8 && sy+height > 8 )
		{
			return -1;
		}

		if(sy > 7 && sy < 12 && sy+height > 12)
		{
			return -1;
		}

		if(sy > 11 && sy < 16 && sy+height > 16)
		{
			return -1;
		}

		if(sy > 16 && sy < 20 && sy+height > 20)
		{
			return -1;
		}

		if(sy > 20 && sy < 24 && sy+height > 24)
		{
			return -1;
		}

		if(sy > 24 && sy < 28 && sy+height > 28)
		{
			return -1;
		}

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			if(*(BYTE*)(TempMap + (sy+y)*8 + (sx+x))!= 255)
			{
				blank += 1;
				return -1;
			}
		}
	}
	if(blank == 0)
	{
		return sx+sy*8+INVENTORY_WEAR_SIZE;
	}
	return -1;
}


void YolaCustom::gObjInventoryItemSet(int aIndex, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if(itempos < INVENTORY_WEAR_SIZE)
	{
		return;
	}
	else if(itempos > (MAIN_INVENTORY_NORMAL_SIZE-1))
	{
		return;
	}

	if(gObj[aIndex].Inventory[itempos].GetSize((int&)width,(int &)height)==0)
	{
//		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return;
	}

	gObjInventoryItemBoxSet(aIndex,itempos,width,height,set_byte);
}

BOOL YolaCustom::ExtentCheck(int x, int y, int w, int h)
{
	if ( (x >=0) && (x<w) && (y >=0) && (y<h) )
	{
		return 1;
	}
	return 0;
}

void YolaCustom::gObjTempInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos - INVENTORY_WEAR_SIZE) % 8;
	int itemposy = (itempos - INVENTORY_WEAR_SIZE) / 8;

	for(int y = 0 ; y < yl ; y++)
	{
		for(int x = 0; x < xl; x++)
		{
			*(BYTE*)(TempMap + (itemposy +y) * 8 + (itemposx + x)) = set_byte;
		}
	}
}

BYTE YolaCustom::gObjInventoryRectCheck(int aIndex, int sx, int sy, int width, int height)
{
	int x,y;
	int blank = 0;
	int S6E2 = 1;

	int InventoryRows = 12;
	if(S6E2 == 1)
		InventoryRows = 28;

	if(sx + width > 8)
	{
		return -1;
	}

	if(sy + height > InventoryRows)
	{
		return -2;
	}

	if(S6E2 == 1 && height > 1)
	{
		if(sy < 8 && sy+height > 8 )
		{
			return -1;
		}

		if(sy > 7 && sy < 12 && sy+height > 12)
		{
			return -1;
		}

		if(sy > 11 && sy < 16 && sy+height > 16)
		{
			return -1;
		}

		if(sy > 16 && sy < 20 && sy+height > 20)
		{
			return -1;
		}

		if(sy > 20 && sy < 24 && sy+height > 24)
		{
			return -1;
		}

		if(sy > 24 && sy < 28 && sy+height > 28)
		{
			return -1;
		}
	}

	int xx,yy;

	for(y = 0; y < height; y ++)
	{
		yy = sy+y;

		for(x = 0; x < width; x++)
		{
			xx = sx + x;

			if(ExtentCheck(xx,yy,8,InventoryRows)==1)
			{
				if(*(BYTE*)(gObj[aIndex].InventoryMap+(sy+y)*8+(sx+x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				LogAdd(LOG_BOT,"error : %s %d",__FILE__,__LINE__);
				return -1;
			}
		}
	}

	if(blank == 0)
	{
		return sx+sy*8+12;
	}
	return  -1;
}

BOOL YolaCustom::IS_EXTENDED_INV(int aIndex, BYTE INVNUM)
{
	int S6E2 = 1;
	if(OBJMAX_RANGE(aIndex) == false)
		return FALSE;

	if (aIndex < OBJECT_START_USER || aIndex > MAX_OBJECT)
		return FALSE;

	if(S6E2 == 1)
	{
		if(gObj[aIndex].ExInventory >= 1 && INVNUM == 1)
			return TRUE;
		else if(gObj[aIndex].ExInventory == 2 && INVNUM == 2)
			return TRUE;
		else if(gObj[aIndex].ExInventory == 3 && INVNUM == 3)
			return TRUE;
		else if(gObj[aIndex].ExInventory == 4 && INVNUM == 4)
			return TRUE;
	}
	return FALSE;
}

void YolaCustom::gObjInventoryItemBoxSet(int aIndex, int itempos, int xl, int yl, BYTE set_byte)
{
	int S6E2 =1;
	int InventoryRows = 12;
	if(S6E2 == 1)
		InventoryRows = 28;
			
	int itemposx = (itempos - INVENTORY_WEAR_SIZE)%8;
	int itemposy = (itempos - INVENTORY_WEAR_SIZE)/8;

	int xx,yy;

	for(int y = 0; y < yl; y ++)
	{
		yy = itemposy + y;

		for(int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if((ExtentCheck(xx,yy,8,InventoryRows)==1) 
#if (PACK_EDITION>=3)
				|| (gObj[aIndex].IsBot >= Type_Pet)
#endif
				)
			{
				*(BYTE*)(gObj[aIndex].InventoryMap + (itemposy + y)*8+(itemposx + x)) = set_byte;
			}
			else
			{
			//	LogAdd("error : %s %d",__FILE__,__LINE__);
				return;
			}
		}
	}
}

//Trade

void YolaCustom::gObjTradeCancel(int aIndex)	//OK
{
	if(OBJMAX_RANGE(aIndex) == 0)
	{
		LogAdd(LOG_BLUE,"Error : index[%d] error %s %d",aIndex,__FILE__,__LINE__);
		return;
	}

	if(gObj[aIndex].Type != OBJECT_USER && gObj[aIndex].Type != OBJECT_USER  || gObj[aIndex].Connected != OBJECT_ONLINE)
	{
		return;
	}

	if(gObj[aIndex].Interface.use != 1)
	{
		return;
	}

	if(gObj[aIndex].Interface.type != 1)
	{
		return;
	}

	gObjInventoryRollback(aIndex);	
	gObj[aIndex].TargetNumber = -1;	
	gObj[aIndex].Interface.use = 0;
	gObjCharTradeClear(&gObj[aIndex]);	
	GCMoneySend(aIndex,gObj[aIndex].Money);	
	gItemManager.GCItemListSend(aIndex);	
	gItemManager.GCItemEquipmentSend(aIndex);
	//GCMagicListMultiSend(&gObj[aIndex],0);
	//gObjUseSkill.SkillChangeUse(aIndex);

	//DESACTIVADOS PORQUE MAS QUE SEGURO SON DE BOTPET ESTAS OPCIONES!

	if(gObj[aIndex].GuildNumber > 0)
	{
		//GCGuildViewportNowPaint(aIndex,gObj[aIndex].GuildName,0,0);
		gObjNotifyUpdateUnionV1(&gObj[aIndex]);
		gObjNotifyUpdateUnionV2(&gObj[aIndex]);
	}
}

void YolaCustom::gObjCharTradeClear(LPOBJ lpObj)	//OK
{
	if ( lpObj->Type != OBJECT_USER && lpObj->Type != OBJECT_USER )
	{
		return;
	}

	memset(lpObj->TradeMap,0xFF,TRADE_SIZE);

	for ( int i=0 ; i< 32 ; i++)
	{
		lpObj->Trade[i].Clear();
	}

	lpObj->TradeMoney = 0;
	lpObj->TradeOk = false;
}

//Items
BOOL YolaCustom::DeleteItemByLevel (int iIndex, DWORD itemId, BYTE ItemLevel)	//OK
{
	if(OBJMAX_RANGE(iIndex) == 0)
	{
		return false;
	}

	for ( BYTE ItemPos = INVENTORY_WEAR_SIZE ; ItemPos < MAIN_INVENTORY_NORMAL_SIZE; ItemPos ++ )
	{
		if ( gObj[iIndex].Inventory[ItemPos].m_Index == itemId )
		{
			if (  gObj[iIndex].Inventory[ItemPos].m_Level == ItemLevel )
			{
				gItemManager.InventoryDelItem(iIndex,ItemPos);
				gItemManager.GCItemDeleteSend(iIndex, ItemPos, 1);
				return 1;
			}
		}
	}
	return 0;
}

int YolaCustom::ItemGetDurability(int index, int itemLevel, int ExcellentItem, int SetItem) //LIMPIAR
{
	if ( index < 0 || index >= MAX_ITEM_TYPE )
	{
		return 0;
	}

	if ( index == GET_ITEM(14,21) && itemLevel == 3 )	// Mark Lord
	{
		itemLevel=0;
	}
	
	//Kundun Symbol, Piece of Paper, Lucky Peny, Blue Ball Invitation Part
	if ( (index == GET_ITEM(14,29)) || (index == GET_ITEM(14,100)) || (index == GET_ITEM(14,101)) || (index == GET_ITEM(14,110))  || (index == GET_ITEM(14,153)) || (index == GET_ITEM(14,154)) || (index == GET_ITEM(14,155)) || (index == GET_ITEM(14,156)))
	{
			return 1;
	}

	int dur=0;

	if ( itemLevel < 5)
	{
		dur= ItemAttribute[index].Durability + itemLevel;
	}
	else if ( itemLevel >= 5 )
	{
		if ( itemLevel == 10 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2-3;
		}
		else if (itemLevel == 11 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2-1;
		}
		else if (itemLevel == 12 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+2;
		}
		else if (itemLevel == 13 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+6;
		}
		else if (itemLevel == 14 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+11;
		}
		else if (itemLevel == 15 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+17;
		}
		else
		{
			dur=ItemAttribute[index].Durability + itemLevel*2-4;
		}
	}
	
	if ( (index < GET_ITEM(12,3) || index > GET_ITEM(12,6) ) &&
		  index != GET_ITEM(0,19) &&
		  index != GET_ITEM(4,18) &&
		  index != GET_ITEM(5,10) &&
		  index != GET_ITEM(2,13) &&
		  index != GET_ITEM(12,49) &&
		  index != GET_ITEM(13,30) )
	{
		if ( SetItem != 0 )
			dur +=20;
		else if ( ExcellentItem != 0 && index != GET_ITEM(12,50) && (index < GET_ITEM(12,36) || index > GET_ITEM(12,43) ) ) // Third Wings Fix Durability *Prevent duple if items
			dur +=15;
	}

	if ( dur > 255 )
		dur = 255;

	return dur;
}

int YolaCustom::getNumberOfExcOptions(int checksum)
{
	int optionscount=0;
	int ExcOrgArr[6];

	ExcOrgArr[0]=1;
	ExcOrgArr[1]=2;
	ExcOrgArr[2]=4;
	ExcOrgArr[3]=8;
	ExcOrgArr[4]=16;
	ExcOrgArr[5]=32;

	if (checksum>0)
	{
		for (int i=0;i<6;i++)
		{
			int and_val=checksum & ExcOrgArr[i];
			if (and_val != 0)
				optionscount+=1;
		}
	}

	return optionscount;
}

int YolaCustom::gObjCheckSerial0ItemList(class CItem* lpItem)
{


	if(gItemSerialCheck == 0)
	{
		return false;
	}
	if(gItemZeroSerialCheck == 0)
	{
		return false;
	}

	if(lpItem->m_Index == GET_ITEM(19,13)
		|| lpItem->m_Index == GET_ITEM(19,14)
		|| lpItem->m_Index == GET_ITEM(19,16)
		|| lpItem->m_Index == GET_ITEM(19,22)
		|| lpItem->m_Index == GET_ITEM(19,15)
		|| lpItem->m_Index == GET_ITEM(19,30)
		|| lpItem->m_Index == GET_ITEM(19,31)
		|| lpItem->m_Index == GET_ITEM(19,31))
	{
		if(lpItem->GetNumber() == 0)
		{
			return true;
		}
	}
	return false;
}

BYTE YolaCustom::BoxExcOptions(int maxOptions)
{
	BYTE NumberOfOptions = 0;
	BYTE RetOption = 0, TempOption = 0, deadlock = 15;
	BYTE ExcOrgArr[6];

	if (maxOptions > 0)
	{
		if (maxOptions > 6)
			NumberOfOptions=6;
		else
			NumberOfOptions=rand()%maxOptions + 1;

		//Exc Options IDs
		ExcOrgArr[0]=8;
		ExcOrgArr[1]=16;
		ExcOrgArr[2]=2;
		ExcOrgArr[3]=4;
		ExcOrgArr[4]=32;
		ExcOrgArr[5]=1;

		if (NumberOfOptions >= 6)
		{
			RetOption = ExcOrgArr[0]+ExcOrgArr[1]+ExcOrgArr[2]+ExcOrgArr[3]+ExcOrgArr[4]+ExcOrgArr[5];
			return RetOption;
		}

		if (NumberOfOptions == 5)
		{
			TempOption = ExcOrgArr[rand()%6];

			RetOption = ExcOrgArr[0]+ExcOrgArr[1]+ExcOrgArr[2]+ExcOrgArr[3]+ExcOrgArr[4]+ExcOrgArr[5]-TempOption;
			return RetOption;
		}

		if (NumberOfOptions == 4)
		{
			RetOption = ExcOrgArr[0]+ExcOrgArr[1]+ExcOrgArr[2]+ExcOrgArr[3]+ExcOrgArr[4]+ExcOrgArr[5];
			while(true)
			{		
				TempOption = ExcOrgArr[rand()%6];

				if ( (RetOption & TempOption) == TempOption )
				{
					RetOption -= TempOption;
					NumberOfOptions += 1;
				}

				deadlock -= 1;
				if ( NumberOfOptions == 6 || deadlock == 0 )
					break;
			}
			return RetOption;
		}

		if (NumberOfOptions == 3)
		{
			RetOption = 0;
			while(true)
			{		
				TempOption = ExcOrgArr[rand()%6];

				if ( (RetOption & TempOption) != TempOption )
				{
					RetOption += TempOption;
					NumberOfOptions -= 1;
				}

				deadlock -= 1;
				if ( NumberOfOptions == 0 || deadlock == 0 )
					break;
			}
			return RetOption;
		}

		if (NumberOfOptions == 2)
		{
			RetOption = 0;
			while(true)
			{		
				TempOption = ExcOrgArr[rand()%6];

				if ( (RetOption & TempOption) != TempOption )
				{
					RetOption += TempOption;
					NumberOfOptions -= 1;
				}

				deadlock -= 1;
				if ( NumberOfOptions == 0 || deadlock == 0 )
					break;
			}
			return RetOption;
		}

		if (NumberOfOptions == 1)
		{
			RetOption = ExcOrgArr[rand()%6];
			return RetOption;
		}
	}
	return RetOption;
}

//SkyEvent
BOOL YolaCustom::SetPosMonsterSkyEvent(int aIndex, int PosTableNum)	//OK ?
{
	if ( (  (aIndex<0)? FALSE: (aIndex > MAX_OBJECT-1)? FALSE: TRUE ) == FALSE )
	{
			LogAdd(LOG_RED,"error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	
	if ( (  (PosTableNum<0)? FALSE: (PosTableNum > OBJ_MAXMONSTER-1)? FALSE: TRUE ) == FALSE )
	{
		LogAdd(LOG_RED,"error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];
	lpObj->PosNum = PosTableNum;
	lpObj->X = gMonsterSetBase.m_Mp[PosTableNum].X;
	lpObj->Y = gMonsterSetBase.m_Mp[PosTableNum].Y;
	lpObj->Map = gMonsterSetBase.m_Mp[PosTableNum].Map;

	lpObj->TX = lpObj->X;
	lpObj->TY = lpObj->Y;
	lpObj->OldX = lpObj->X;
	lpObj->OldY = lpObj->Y;
	lpObj->Dir = gMonsterSetBase.m_Mp[PosTableNum].Dir;
	lpObj->StartX = lpObj->X;
	lpObj->StartY = lpObj->Y;

	if (lpObj->Class == 44 ||  
		lpObj->Class == 53 ||  
		lpObj->Class == 54 ||  
		lpObj->Class == 55 ||  
		lpObj->Class == 56 ||
		lpObj->Class == 492)
	{

	}
	else if (  lpObj->Class >= 78 &&  lpObj->Class <= 83)
	{

	}

	return true;
}

BOOL YolaCustom::gObjIsNewClass(LPOBJ lpObj)	//SkyEvent
{
	int Class = lpObj->DBClass;

	if( Class == DB_GRAND_MASTER	 ||		//SM
		Class == DB_BLADE_MASTER	 ||		//BK
		Class == DB_HIGH_ELF		 ||		//ELF
		Class == DB_DUEL_MASTER		 ||		//MG
		Class == DB_DUEL_MASTER+1	 ||		//MG
		Class == DB_LORD_EMPEROR	 ||		//DL
		Class == DB_LORD_EMPEROR+1   ||		//DL
		Class == DB_DIMENSION_MASTER ||		//SUM
		Class == DB_RAGEFIGHER_EVO   ||		//RF
		Class == DB_RAGEFIGHER_EVO+1)		//RF
	{
		return 1;
	}else
	{
		return 0;
	}
}

int YolaCustom::gObjMonsterAdd(WORD Type, BYTE Map, BYTE X, BYTE Y)
{
	if ( gMonsterSetBase.m_Count >= MAX_OBJECT )
	{
		ErrorMessageBox("Monster attribute max over %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (gMapServerManager.CheckMapServer(Map) == FALSE )
	{
		LogAdd(LOG_BLACK,"[MonsterAdd] No need to load monster for map %d [%d,%d,%d]",Map,Type,X,Y);
		return -1;
	}

	gMonsterSetBase.m_Count++;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].Dis			= 30;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].Type			= Type;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].Map			= Map;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].TY			= X;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].TX			= Y;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].X				= X;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].Y				= Y;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].Value			= 1;
	gMonsterSetBase.m_Mp[gMonsterSetBase.m_Count].Dir			= 3;

	return gMonsterSetBase.m_Count;
}

//User

int YolaCustom::gObjGetIndex(char* szId)	//OK BUSCAR SU FUNCION CORRESPONDIENTE
{
	for (int n=OBJECT_START_USER;n<MAX_OBJECT; n++)
	{
		if ( gObj[n].Connected >= OBJECT_ONLINE)
		{
			if ( gObj[n].Name[0] == *szId )
			{
				if ( strcmp(&gObj[n].Name[0] , szId) == 0)
				{
					return n;
				}
			}
		}
	}
	return -1;
}

int YolaCustom::ItemGetNumberMake(int type, int index)	//OK	
{
	int make;

	make = type*MAX_ITEM_TYPE + index;

	if ( make < 0 || make >= MAX_ITEM )
	{
		return -1;
	}

	if (ItemAttribute[make].Width < 1 ||  ItemAttribute[make].Height < 1)
	{
		return -1;
	}
	return make;
}

//BotWarper

BOOL YolaCustom::CheckMainToMove(LPOBJ lpObj)
{
	if ( BC_MAP_RANGE(lpObj->Map) != FALSE )
	{
		return FALSE;
	}

	if ( CC_MAP_RANGE(lpObj->Map) != FALSE )
	{
		return FALSE;
	}

	if ( KALIMA_MAP_RANGE(lpObj->Map) != FALSE )
	{
		return FALSE;
	}

	if ( DS_MAP_RANGE(lpObj->Map) != FALSE )
	{
		return FALSE;
	}

	if ( IT_MAP_RANGE(lpObj->Map) != FALSE )
	{
		return FALSE;
	}


	if ( DG_MAP_RANGE(lpObj->Map) != FALSE )
	{
		return FALSE;
	}

#if (PACK_EDITION>=2)
	if ( IMPERIALGUARDIAN_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}
#endif
	if ( lpObj->Map == MAP_KANTURU3 )
	{
		return FALSE;
	}else if ( lpObj->Map == MAP_RAKLION2 )
	{
		return FALSE;
	}else if ( lpObj->Map == MAP_LAND_OF_TRIALS )
	{
		return FALSE;
	}else if ( lpObj->Map == MAP_BARRACKS )
	{
		return FALSE;
	}else if ( lpObj->Map == MAP_REFUGE )
	{
		return FALSE;
	}else if ( lpObj->Map == MAP_DUEL_ARENA )
	{
		return FALSE;
	}

	return TRUE;
}
//no terminado fijarse bien donde queda los //
BOOL YolaCustom::CheckEquipmentToMove(LPOBJ lpObj, int iTargetMapNumber)
{
	if ( iTargetMapNumber == MAP_ATLANS )
	{
		if ( lpObj->Inventory[8].IsItem() != FALSE )
		{
			if ( lpObj->Inventory[8].m_Index == GET_ITEM(13,2) ) // Uniria
			{
				return FALSE;
			}

			if ( lpObj->Inventory[8].m_Index == GET_ITEM(13,3) ) // Dino
			{
				return FALSE;
			}
		}
	}

	if ( iTargetMapNumber == MAP_ICARUS )
	{
		if ( (lpObj->Inventory[8].m_Index != GET_ITEM(13,3) &&
			 lpObj->Inventory[7].m_Index != GET_ITEM(13,30) &&
			 lpObj->Inventory[7].m_Index != GET_ITEM(12,49) &&
			 lpObj->Inventory[7].m_Index != GET_ITEM(12,50) &&
			 lpObj->Inventory[8].m_Index != GET_ITEM(13,37) &&
			 (lpObj->Inventory[7].m_Index < GET_ITEM(12,0) || lpObj->Inventory[7].m_Index > GET_ITEM(12,6)) ) ||			 
#if (CRYSTAL_EDITION == 1)
			(lpObj->pInventory[7].m_Index < GET_ITEM(12,36) || (lpObj->pInventory[7].m_Index > GET_ITEM(12,40) && (lpObj->Inventory[7].m_Index < GET_ITEM(12,200) || lpObj->Inventory[7].m_Index > GET_ITEM(12,254))))  ||
#else
			(lpObj->Inventory[7].m_Index < GET_ITEM(12,36) || lpObj->Inventory[7].m_Index > GET_ITEM(12,40))  ||
#endif
			(lpObj->Inventory[7].m_Index < GET_ITEM(12,130) || lpObj->Inventory[7].m_Index > GET_ITEM(12,135))  ||
			 lpObj->Inventory[8].m_Index == GET_ITEM(13,2) ||
			 lpObj->Inventory[11].m_Index == GET_ITEM(13,10) ||
			 lpObj->Inventory[10].m_Index == GET_ITEM(13,10)  )
		{
			return FALSE;
		}

		if ( lpObj->SummonIndex >= 0 )
		{
		//	gObjMonsterCallKill(lpObj->Index);
			GCSummonLifeSend(lpObj->Index,60,0);
		}
	}

	return TRUE;
}
BOOL YolaCustom::CheckInterfaceToMove(LPOBJ lpObj)
{
	if ( lpObj->Interface.use == 1 )
	{
		return FALSE;
	}

	if ( lpObj->PShopOpen == true )
	{
		return FALSE;
	}

	return TRUE;
}

//Map
BOOL YolaCustom::gObjCheckTeleportAreaByMap(BYTE mapnumber, BYTE x, BYTE y)
{
	if(mapnumber < 0 || mapnumber >= MAX_MAP)
	{
		return false;
	}

	unsigned char attr = gMap[mapnumber].GetAttr(x,y);

	if(attr)
	{
		return false;
	}

	attr = gMap[mapnumber].GetAttr(x,y);

	if(attr & 1)
	{
		return false;
	}

	return true;
}

BOOL YolaCustom::gObjItsInSafeZone(int PlayerID)
{
	unsigned char btMapAttribute = gMap[gObj[PlayerID].Map].GetAttr(gObj[PlayerID].X,gObj[PlayerID].Y);
	if( ((btMapAttribute & 1) == 1) && 
		(DS_MAP_RANGE(PlayerID) == FALSE) && 
		(BC_MAP_RANGE(PlayerID) == FALSE) &&
		(IT_MAP_RANGE(PlayerID) == FALSE) &&
		(DG_MAP_RANGE(PlayerID) == FALSE) &&

#if (PACK_EDITION>=2)
		(IMPERIALGUARDIAN_MAP_RANGE(PlayerID) == FALSE) &&
#endif
		(CC_MAP_RANGE(PlayerID) == FALSE))//If safezone
	{
		return 1;
	}else{
		return 0;
	}
}

BOOL YolaCustom::gObjMoveHackCheck(int aIndex)
{
	if(OBJMAX_RANGE(aIndex) == 0)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type == OBJECT_USER)
	{
		gDarkSpirit[lpObj->Index].SetMode(eDarkSpiritMode::DARK_SPIRIT_MODE_NORMAL,-1);

		if ( gObj[aIndex].CloseCount >= 0 )
		{
			return false; 
		}
/*#if(AFK_SYSTEM == 1)
		if ( gObj[aIndex].MapServerMoveQuit == true || gObj[aIndex].m_bMapAntiHackMove == true )
		{
			return false;
		}
#endif*/
		if(lpObj->PShopOpen == true)
		{
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] Closing PShop...");
			gPersonalShop.CGPShopCloseRecv(lpObj->Index);
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] Please try to move again!");
			return false;
		}

		if(lpObj->Interface.use == 1 && lpObj->Interface.type == 1)
		{
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] Cancelling Trade...");
			gTrade.CGTradeCancelButtonRecv(lpObj->Index);
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] Please try to move again!");
			return false;
		}

		if(lpObj->Interface.use == 1 && lpObj->Interface.type == 6)
		{
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] Closing Warehouse...");
			gWarehouse.CGWarehouseClose(lpObj->Index);
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] Please try to move again!");
			return false;
		}

		if(lpObj->DieRegen != 0)
		{
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] DieRegen ERROR!");
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack] Please try to move again!");
			return false;
		}

		if ((lpObj->TargetNumber!=-1)&&(lpObj->Transaction==1))
		{
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack][Move] DUPE ATTEMPT: In Trade!");
			return false;
		} 
												 //Illusion Temple				//Lucky Coins					//Imperial Guardian			//Double Goer
		if (lpObj->Interface.use == 1 && (lpObj->Interface.type == 96 ||lpObj->Interface.type == 97 || lpObj->Interface.type == 98 || lpObj->Interface.type == 99) )
		{
			lpObj->Interface.use = 0;
			lpObj->Interface.type = 0;
		}

		if ( lpObj->Interface.use != 0 ) 
		{
			gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"[Anti-Hack][Move] Character in use state!");
			return false;
		}

		if(lpObj->Map == 0)
		{
			return false;
		}
	}

	return TRUE;
}

BOOL YolaCustom::gObjGetRandomFreeArea(int iMapNumber, BYTE &cX, BYTE &cY, int iSX, int iSY, int iDX, int iDY, int iLoopCount)
{
	if( iSX <= 0 ) iSX = 1;
	if( iSY <= 0 ) iSY = 1;
	if( iDX <= 0 ) iDX = 1;
	if( iDY <= 0 ) iDY = 1;
	
	if( iSX >= 256 ) iSX = 255;
	if( iSY >= 256 ) iSY = 255;
	if( iDX >= 256 ) iDX = 255;
	if( iDY >= 256 ) iDY = 255;

	if( iLoopCount <= 0 ) iLoopCount = 1;

	while( (iLoopCount--) > 0 )
	{
		cX = iSX + (rand() % (iDX-iSX));
		cY = iSY + (rand() % (iDY-iSY));

		BYTE btMapAttr = gMap[iMapNumber].GetAttr(cX, cY);

		if( btMapAttr == 0 )
			return TRUE;
	}
	return FALSE;
}

//******************************************

BOOL CItem::GetSize(int & w, int & h)
{
	w=ItemAttribute[this->m_Index].Width  ;
	h=ItemAttribute[this->m_Index].Height  ;

	return 1;
}

//new

void CNotice::MakeNoticeMsg(void * lpNotice, BYTE btType, char * szNoticeMsg)
{
	PMSG_NOTICE_SEND * pNotice = (PMSG_NOTICE_SEND *)lpNotice;
	pNotice->type  = btType;

#ifdef GS_UNICODE
	unsigned char szTempMsg[4096]={0};
	memset( pNotice->szNoticeMessage, 0, sizeof(pNotice->szNoticeMessage));
	MultiByteToWideChar( 1258, 0, (char*)szNoticeMsg,  strlen((char*)szNoticeMsg), (unsigned short*)szTempMsg, sizeof(szTempMsg) );
	memcpy(pNotice->szNoticeMessage, szTempMsg, sizeof(pNotice->szNoticeMessage));
	pNotice->szNoticeMessage[254]=0;
	pNotice->szNoticeMessage[255]=0;
	sprintf_s(pNotice->Notice, sizeof(pNotice->Notice), "%s", szNoticeMsg);
	PHeadSetB(&pNotice->PacketHeader, 0x0D, wcslen((unsigned short *)pNotice->szNoticeMessage)*2+ 0x12);
#else
	sprintf_s(pNotice->message, sizeof(pNotice->message), "%s", szNoticeMsg);
	PHeadSetB((LPBYTE)pNotice, 0x0D, strlen(pNotice->message) + sizeof(PMSG_NOTICE_SEND) - sizeof(pNotice->message) + 1 );
#endif
}

BYTE YolaCustom::gObjInventoryInsertItemBot(LPOBJ lpObj, int type, int index, int level, int iSerial, int iDur, int ItemSlot1, int ItemSlot2, int ItemSlot3, int ItemSlot4, int ItemSlot5)
{
	CItem item;
	int w,h,iwidth,iheight;
	BYTE blank = 0;

	int item_type = ItemGetNumberMake(type,index);
	item.m_Level = level;
	item.m_Durability = iDur;
	item.m_SocketOption[0] = ItemSlot1;
	item.m_SocketOption[1] = ItemSlot2;
	item.m_SocketOption[2] = ItemSlot3;
	item.m_SocketOption[3] = ItemSlot4;
	item.m_SocketOption[4] = ItemSlot5;

	item.Convert(item_type,0,0,0,0,0,0,0,0,3);

	if(item.GetSize((int &)iwidth,(int &)iheight)==0)
	{
		//LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

	for(h = 0; h < 8; h++)
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(lpObj->InventoryMap+h*8+w) == 255)
			{
				blank = gObjInventoryRectCheck(lpObj->Index,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto NEXT1;
				}

				if(blank != 255)
				{
					lpObj->Inventory[blank] = item;
					lpObj->Inventory[blank].m_Serial = iSerial;

					gObjInventoryItemSet(lpObj->Index,blank,lpObj->Inventory[blank].m_Index);
					return blank;
				}
			}
		}
	}
NEXT1:
	if(IS_EXTENDED_INV(lpObj->Index,1) == TRUE)
	{
		for(h = 8; h < 12; h++)
		{
			for(w = 0; w < 8; w++)
			{
				if(*(BYTE*)(lpObj->InventoryMap+h*8+w) == 255)
				{
					blank = gObjInventoryRectCheck(lpObj->Index,w,h,iwidth,iheight);

					if(blank == 254)
					{
						goto NEXT2;
					}

					if(blank != 255)
					{
						lpObj->Inventory[blank] = item;
						lpObj->Inventory[blank].m_Serial = iSerial;

						gObjInventoryItemSet(lpObj->Index,blank,lpObj->Inventory[blank].m_Index);
						return blank;
					}
				}
			}
		}
	}
NEXT2:
	if(IS_EXTENDED_INV(lpObj->Index,2) == TRUE)
	{
		for(h = 12; h < 16; h++)
		{
			for(w = 0; w < 8; w++)
			{
				if(*(BYTE*)(lpObj->InventoryMap+h*8+w) == 255)
				{
					blank = gObjInventoryRectCheck(lpObj->Index,w,h,iwidth,iheight);

					if(blank == 254)
					{
						goto GOTO_EndFunc;
					}

					if(blank != 255)
					{
						lpObj->Inventory[blank] = item;
						lpObj->Inventory[blank].m_Serial = iSerial;

						gObjInventoryItemSet(lpObj->Index,blank,lpObj->Inventory[blank].m_Index);
						return blank;
					}
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

struct PMSG_INVENTORYITEMMODIFY
{
	PBMSG_HEAD h;	// C1:F3:14
	BYTE subcode;	// 3
	BYTE Pos;	// 4
	BYTE ItemInfo[MAX_ITEM_INFO];	// 5

};

void YolaCustom::GCInventoryItemOneSend(int aIndex, int pos)
{
	if(OBJMAX_RANGE(aIndex) == 0)
	{
		return;
	}

	if ( !gObj[aIndex].Inventory[pos].IsItem())
		return;

	PMSG_INVENTORYITEMMODIFY pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x14, sizeof(pMsg));
	pMsg.Pos = pos;
	gItemManager.ItemByteConvert(pMsg.ItemInfo, gObj[aIndex].Inventory[pos]);

	DataSend(aIndex, (UCHAR *)&pMsg, pMsg.h.size);
}

BYTE YolaCustom::GETHEIGHT(int aIndex)
{
	int S6E2;

	if(OBJMAX_RANGE(aIndex) == false)
		return BASIC_INV_HEIGHT;

	if (aIndex < OBJECT_START_USER || aIndex > MAX_OBJECT)
		return BASIC_INV_HEIGHT;

	if(S6E2 == 1)
	{
		if(gObj[aIndex].ExInventory >= 1 && gObj[aIndex].ExInventory <= MAX_EXTENDED_INV)
			return BASIC_INV_HEIGHT + (EXTENDED_INV_HEIGHT*gObj[aIndex].ExInventory); 
	}
	return BASIC_INV_HEIGHT;
}

BYTE YolaCustom::gObjOnlyInventoryRectCheck(int aIndex, int sx, int sy, int width, int height)
{
	int x,y;
	int blank = 0;
	int InventoryRows = 12;
	int S6E2;
	InventoryRows =	GETHEIGHT(aIndex);

	if(sx + width > 8)
	{
		return -1;
	}

	if(sy + height > InventoryRows)
	{
		return -2;
	}

	if(S6E2 == 1 && height > 1)
	{
		if(sy < 8 && sy+height > 8 )
		{
			return -1;
		}

		if(sy > 7 && sy < 12 && sy+height > 12)
		{
			return -1;
		}

		if(sy > 11 && sy < 16 && sy+height > 16)
		{
			return -1;
		}

		if(sy > 16 && sy < 20 && sy+height > 20)
		{
			return -1;
		}

		if(sy > 20 && sy < 24 && sy+height > 24)
		{
			return -1;
		}
	}

	int xx,yy;
	for(y = 0; y < height; y ++)
	{

		yy = sy+y;

		for(x = 0; x < width; x++)
		{
			xx = sx + x;

			if(ExtentCheck(xx,yy,8,InventoryRows)==1)
			{
				if(*(BYTE*)(gObj[aIndex].InventoryMap+(sy+y)*8+(sx+x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				LogAdd(LOG_BOT,"error : %s %d",__FILE__,__LINE__);
				return -1;
			}
		}
	}

	if(blank == 0)
	{
		return sx+sy*8+12;
	}
	return  -1;
}
BYTE YolaCustom::gObjShopBuyInventoryInsertItem(int aIndex, CItem item)
{
	int w,h,iwidth,iheight;
	BYTE blank = 0;

	if(item.GetSize(iwidth,iheight)==0)
	{
		//LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

	for(h = 0; h < 8; h++)
	{
		for( w = 0; w < 8; w++)
		{
			if(*(BYTE*)(gObj[aIndex].InventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto NEXT1;
				}

				if(blank != 255)
				{
					gObj[aIndex].Inventory[blank] = item;

					gObj[aIndex].Inventory[blank].m_Number = 0;
					
					gObjInventoryItemSet(aIndex,blank,gObj[aIndex].Inventory[blank].m_Serial);
					return blank;
				}
			}
		}
	}
NEXT1:
	if(IS_EXTENDED_INV(aIndex,1) == TRUE)
	{	
		for(h = 8; h < 12; h++)
		{
			for( w = 0; w < 8; w++)
			{
				if(*(BYTE*)(gObj[aIndex].InventoryMap+h*8+w) == 255)
				{
					blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

					if(blank == 254)
					{
						goto NEXT2;
					}

					if(blank != 255)
					{
						gObj[aIndex].Inventory[blank] = item;

						gObj[aIndex].Inventory[blank].m_Number = 0;
						
						gObjInventoryItemSet(aIndex,blank,gObj[aIndex].Inventory[blank].m_Index);
						return blank;
					}
				}
			}
		}
	}
NEXT2:
	if(IS_EXTENDED_INV(aIndex,2) == TRUE)
	{	
		for(h = 12; h < 16; h++)
		{
			for( w = 0; w < 8; w++)
			{
				if(*(BYTE*)(gObj[aIndex].InventoryMap+h*8+w) == 255)
				{
					blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

					if(blank == 254)
					{
						goto GOTO_EndFunc;
					}

					if(blank != 255)
					{
						gObj[aIndex].Inventory[blank] = item;

						gObj[aIndex].Inventory[blank].m_Number = 0;
						
						gObjInventoryItemSet(aIndex,blank,gObj[aIndex].Inventory[blank].m_Index);
						return blank;
					}
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

