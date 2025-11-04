#include "stdafx.h"
#include "Resource.h"
#include "BloodCastle.h"
#include "CastleDeep.h"
#include "CastleSiege.h"
#include "ChaosCastle.h"
#include "Crywolf.h"
#include "CustomArena.h"
#include "CustomEventDrop.h"
#include "CustomOnlineLottery.h"
#include "CustomQuiz.h"
#include "DevilSquare.h"
#include "EventTvT.h"
#include "GameServer.h"
#include "GameMain.h"
#include "IllusionTemple.h"
#include "InvasionManager.h"
#include "JSProtocol.h"
#include "Message.h"
#include "MiniDump.h"
#include "Notice.h"
#include "Protect.h"
#include "QueueTimer.h"
#include "ServerDisplayer.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "SocketManagerUdp.h"
#include "ThemidaSDK.h"
#include "Util.h"
#include "ReiDoMU.h"
#include "IpManager.h"
#include "Log.h"
#include "CustomAttack.h"
#include "CustomStore.h"
#include "OfflineMode.h"
#include "FakeOnline.h"
#include "SkyEvent.h"
#include "SwampEvent.h"
#include "User.h"
#include "Guild.h"
#include "Path.h"
#include "ClassConfig.h"
#include "NameManager.h"
#include "PhraseManager.h"
#include "pugixml.hpp"
#include <fstream>
#include <sstream>
#include "BotCreation_StoredProcedure.h"
#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")
#include "UpdateManager.h"
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")




TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
HINSTANCE hInst;
HWND hWnd;
int Conectar = 0;

HWND hWndComboBox;



// ====================================
// ADD THESE GLOBAL VARIABLES AT THE TOP OF GameServer.cpp
// (After #includes, before WinMain)
// ====================================

HBRUSH g_hBrushBlack = NULL;
HBRUSH g_hBrushRed = NULL;
HBRUSH g_hBrushDarkGray = NULL;

// ====================================
// ADD THIS FUNCTION TO CREATE BRUSHES
// Call this in WinMain after window creation
// ====================================

void InitializeDialogBrushes()
{
	if (g_hBrushBlack == NULL)
	{
		g_hBrushBlack = CreateSolidBrush(RGB(20, 20, 20));      // Dark black
		g_hBrushRed = CreateSolidBrush(RGB(60, 0, 0));          // Dark red
		g_hBrushDarkGray = CreateSolidBrush(RGB(40, 40, 40));   // Dark gray
	}
}

void CleanupDialogBrushes()
{
	if (g_hBrushBlack) DeleteObject(g_hBrushBlack);
	if (g_hBrushRed) DeleteObject(g_hBrushRed);
	if (g_hBrushDarkGray) DeleteObject(g_hBrushDarkGray);
}



int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) // OK
{
	/*if(gProtect.ReadMainFile("..\\Data\\Hack\\keyword.enc") == 0)
	{
		MessageBox(0,"Licencia not found or invalid!","Error",MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}*/

	VM_START

	CMiniDump::Start();

	LoadString(hInstance,IDS_APP_TITLE,szTitle,MAX_LOADSTRING);
	LoadString(hInstance,IDC_GAMESERVER,szWindowClass,MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if(InitInstance(hInstance,nCmdShow) == 0)
	{
		return 0;
	}

	SetLargeRand();

	gServerInfo.ReadStartupInfo("GameServerInfo",".\\Data\\GameServerInfo - Common.dat");
	// Initialize class configuration
	g_ClassConfigManager.Initialize();
	// Initialize name manager
	g_NameManager.Initialize();
	g_PhraseManager.Initialize();
		// Initialize update manager
	gUpdateManager.Init(hWnd);


	#if(PROTECT_STATE==1)

	#if(GAMESERVER_UPDATE>=801)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S8_GAME_SERVER);
	#elif(GAMESERVER_UPDATE>=601)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S6_GAME_SERVER);
	#elif(GAMESERVER_UPDATE>=401)
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S4_GAME_SERVER);
	#else
		//gProtect.StartAuth(AUTH_SERVER_TYPE_S2_GAME_SERVER);
	#endif

	#endif

	char buff[256];

	wsprintf(buff,"[%s] %s (ON: %d) %s",GAMESERVER_VERSION,gServerInfo.m_ServerName, gObjTotalUser,GAMESERVER_CLIENT);

	SetWindowText(hWnd,buff);

	gServerDisplayer.Init(hWnd);

	WSADATA wsa;

	if(WSAStartup(MAKEWORD(2,2),&wsa) == 0)
	{
		if(gSocketManager.Start((WORD)gServerInfo.m_ServerPort) == 0)
		{
			LogAdd(LOG_RED,"Could not start GameServer");
		}
		else
		{
			GameMainInit(hWnd);

			JoinServerConnect(WM_JOIN_SERVER_MSG_PROC);

			DataServerConnect(WM_DATA_SERVER_MSG_PROC);

			gSocketManagerUdp.Connect(gServerInfo.m_ConnectServerAddress,(WORD)gServerInfo.m_ConnectServerPort);

			SetTimer(hWnd,WM_TIMER_1000,1000,0);

			SetTimer(hWnd,WM_TIMER_10000,10000,0);

			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER,100,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER_MOVE,100,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER_AI,100,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER_AI_MOVE,100,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_EVENT,100,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_VIEWPORT,1000,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_FIRST,1000,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_CLOSE,1000,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_MATH_AUTHENTICATOR,10000,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_ACCOUNT_LEVEL,60000,&QueueTimerCallback);

			gQueueTimer.CreateTimer(QUEUE_TIMER_PICK_COMMAND,6000,&QueueTimerCallback);
		}
	}
	else
	{
		LogAdd(LOG_RED,"WSAStartup() failed with error: %d",WSAGetLastError());
	}

	gServerDisplayer.PaintAllInfo();

	gServerDisplayer.PaintName();

	SetTimer(hWnd,WM_TIMER_2000,2000,0);

	HACCEL hAccelTable = LoadAccelerators(hInstance,(LPCTSTR)IDC_GAMESERVER);

	MSG msg;

	while(GetMessage(&msg,0,0,0) != 0)
	{
		if(TranslateAccelerator(msg.hwnd,hAccelTable,&msg) == 0)
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	CMiniDump::Clean();

	VM_END

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) // OK
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance,(LPCTSTR)IDI_GAMESERVER);
	wcex.hCursor = LoadCursor(0,IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = (LPCSTR)IDC_GAMESERVER;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance,(LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance,int nCmdShow) // OK
{
	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass,szTitle,WS_OVERLAPPEDWINDOW | WS_THICKFRAME,CW_USEDEFAULT,0,980,750,0,0,hInstance,0);

	if(hWnd == 0)
	{
		return 0;
	}

	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	return 1;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) // OK
{

	const char ClassName[] = "MainWindowClass";

	HWND hWndStatusBar;

	switch(message)
	{

		case WM_CREATE:

		{

            hWndStatusBar = CreateWindowEx(

            0,

            STATUSCLASSNAME,

            NULL,

            WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|CCS_BOTTOM,

            0,

            0,

            0,

            0,

            hWnd,

            (HMENU)IDC_STATUSBAR,

            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),

            NULL);

            int iStatusWidths[] = {190,270,360,450,580, -1};

            char text[256];

            SendMessage(hWndStatusBar, SB_SETPARTS, 6, (LPARAM)iStatusWidths);

			wsprintf(text, "Mu Alfa %s - Update %d ", GAMESERVER_NAME, UPDATE_GAMING);

            SendMessage(hWndStatusBar, SB_SETTEXT, 0,(LPARAM)text);

			wsprintf(text, "OffStore: %d", gObjOffStore);

            SendMessage(hWndStatusBar, SB_SETTEXT, 1,(LPARAM)text);

			wsprintf(text, "OffAttack: %d", gObjOffAttack);

            SendMessage(hWndStatusBar, SB_SETTEXT, 2,(LPARAM)text);

			wsprintf(text, "Bots Buffer: %d", gObjTotalBot);

            SendMessage(hWndStatusBar, SB_SETTEXT, 3,(LPARAM)text);

			wsprintf(text, "Monsters: %d/%d", gObjTotalMonster,MAX_OBJECT_MONSTER);

            SendMessage(hWndStatusBar, SB_SETTEXT, 4,(LPARAM)text);

			SendMessage(hWndStatusBar, SB_SETTEXT, 5,(LPARAM)NULL);

            ShowWindow(hWndStatusBar, SW_SHOW);

		}
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{

				case IDM_ONLINEUSERS:
					DialogBox(hInst,(LPCTSTR)IDD_ONLINEUSER,hWnd,(DLGPROC)UserOnline);
					break;
				case IDM_ABOUT:
					DialogBox(hInst,(LPCTSTR)IDD_ABOUTBOX,hWnd,(DLGPROC)About);
					break;
				case IDM_EXIT:
					if(MessageBox(0,"Are you sure to terminate GameServer?","Ask terminate server",MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						DestroyWindow(hWnd);
					}
					break;
				case IDM_FILE_ALLUSERLOGOUT:
					gObjAllLogOut();
					break;
				case IDM_FILE_ALLUSERDISCONNECT:
					gObjAllDisconnect();
					break;
				case IDM_FILE_1MINUTESERVERCLOSE:
					if(gCloseMsg == 0)
					{
						gCloseMsg = 1;
						gCloseMsgTime = 60;
						gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(487));
					}
					break;
				case IDM_FILE_3MINUTESERVERCLOSE:
					if(gCloseMsg == 0)
					{
						gCloseMsg = 1;
						gCloseMsgTime = 180;
						gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(488));
					}
					break;
				



				case IDM_FILE_5MINUTESERVERCLOSE:
					if(gCloseMsg == 0)
					{
						gCloseMsg = 1;
						gCloseMsgTime = 300;
						gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(489));

					}
					break;

					//==================================================================================================================================================
					//FakeOnline_EMU	

#if USE_FAKE_ONLINE == TRUE

				case ID_FAKEONLINE_RELOADDATA:
					s_FakeOnline.LoadFakeData(".\\IA\\Accounts.xml");
					break;
				case ID_FAKEONLINE_ADDFAKEONLINE:
				{
					if (gJoinServerConnection.CheckState() != 0 && gDataServerConnection.CheckState() != 0)
					{
						s_FakeOnline.RestoreFakeOnline();
						//s_FakeOnline.AccountsRestored = 2;
					}

					/*
					if (s_FakeOnline.AccountsRestored == 0)
					{
					s_FakeOnline.AccountsRestored = 1;
					}*/
				}
				break;
				case ID_FAKEONLINE_DELFAKEONLINE:
				{
					for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
					{
						if (gObjIsConnectedGP(n) != 0 && gObj[n].IsFakeOnline)
						{
							s_FakeOnline.OnAttackAlreadyConnected(&gObj[n]);

						}
					}
				}
				break;

				// In your WndProc or command handler
				case ID_FAKEONLINE_CREATEBOTS:
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_CREATEBOTS), hWnd, CreateBotsDialogProc);
					break;
				}

#endif

				//==================================================================================================================================================

				case IDM_RELOAD_RELOADCASHSHOP:
					gServerInfo.ReadCashShopInfo();
					break;
				case IDM_RELOAD_RELOADCHAOSMIX:
					gServerInfo.ReadChaosMixInfo();
					break;
				case IDM_RELOAD_RELOADCHARACTER:
					gServerInfo.ReadCharacterInfo();
					break;
				case IDM_RELOAD_RELOADCOMMAND:
					gServerInfo.ReadCommandInfo();
					break;
				case IDM_RELOAD_RELOADCOMMON:
					gServerInfo.ReadCommonInfo();
					break;
				case IDM_RELOAD_RELOADCUSTOM:
					gServerInfo.ReadCustomInfo();
					break;
				case IDM_RELOAD_RELOADEVENT:
					gServerInfo.ReadEventInfo();
					break;
				case IDM_RELOAD_RELOADEVENTITEMBAG:
					gServerInfo.ReadEventItemBagInfo();
					break;
				case IDM_RELOAD_RELOADHACK:
					gServerInfo.ReadHackInfo();
					break;
				case IDM_RELOAD_RELOADITEM:
					gServerInfo.ReadItemInfo();
					break;
				case IDM_RELOAD_RELOADMONSTER:
					gServerInfo.ReloadMonsterInfo();
					break;
				case IDM_RELOAD_RELOADMOVE:
					gServerInfo.ReadMoveInfo();
					break;
				case IDM_RELOAD_RELOADQUEST:
					gServerInfo.ReadQuestInfo();
					break;
				case IDM_RELOAD_RELOADSHOP:
					gServerInfo.ReadShopInfo();
					break;
				case IDM_RELOAD_RELOADSKILL:
					gServerInfo.ReadSkillInfo();
					break;
				case IDM_RELOAD_RELOADUTIL:
					gServerInfo.ReadUtilInfo();
					break;
				case IDM_RELOAD_RELOADBOTS: //MC bots
					gServerInfo.ReloadBotInfo(); //MC bots
					break;
				case IDM_RELOAD_RELOADALL:
					gServerInfo.ReloadAll();
					break;
				case IDM_STARTONLINELOTTERY_LORENCIA:
					gCustomOnlineLottery.StartNow(0);
					break;
				case IDM_STARTONLINELOTTERY_DEVIAS:
					gCustomOnlineLottery.StartNow(2);
					break;
				case IDM_STARTONLINELOTTERY_NORIA:
					gCustomOnlineLottery.StartNow(3);
					break;
				case IDM_STARTONLINELOTTERY_ELBELAND:
					gCustomOnlineLottery.StartNow(51);
					break;
				case IDM_STARTBC:
					gBloodCastle.StartBC();
					break;
				case IDM_STARTDS:
					gDevilSquare.StartDS();
					break;
				case IDM_STARTCC:
					gChaosCastle.StartCC();
					break;
				case IDM_STARTIT:
					gIllusionTemple.StartIT();
					break;
				case IDM_STARTQUIZ:
					gCustomQuiz.StartQuiz();
					break;
				case IDM_STARTDROP:
					gCustomEventDrop.StartDrop();
					break;
				case IDM_STARTKING:
					gReiDoMU.StartKing();
					break;
				case IDM_STARTTVT:
					gTvTEvent.StartTvT();
					break;
				case IDM_INVASION0:
					gInvasionManager.StartInvasion(0);
					break;
				case IDM_INVASION1:
					gInvasionManager.StartInvasion(1);
					break;
				case IDM_INVASION2:
					gInvasionManager.StartInvasion(2);
					break;
				case IDM_INVASION3:
					gInvasionManager.StartInvasion(3);
					break;
				case IDM_INVASION4:
					gInvasionManager.StartInvasion(4);
					break;
				case IDM_INVASION5:
					gInvasionManager.StartInvasion(5);
					break;
				case IDM_INVASION6:
					gInvasionManager.StartInvasion(6);
					break;
				case IDM_INVASION7:
					gInvasionManager.StartInvasion(7);
					break;
				case IDM_INVASION8:
					gInvasionManager.StartInvasion(8);
					break;
				case IDM_INVASION9:
					gInvasionManager.StartInvasion(9);
					break;
				case IDM_INVASION10:
					gInvasionManager.StartInvasion(10);
					break;
				case IDM_INVASION11:
					gInvasionManager.StartInvasion(11);
					break;
				case IDM_INVASION12:
					gInvasionManager.StartInvasion(12);
					break;
				case IDM_INVASION13:
					gInvasionManager.StartInvasion(13);
					break;
					case IDM_INVASION14:
					gInvasionManager.StartInvasion(14);
					break;
				case IDM_INVASION15:
					gInvasionManager.StartInvasion(15);
					break;
				case IDM_INVASION16:
					gInvasionManager.StartInvasion(16);
					break;
				case IDM_INVASION17:
					gInvasionManager.StartInvasion(17);
					break;
				case IDM_INVASION18:
					gInvasionManager.StartInvasion(18);
					break;
				case IDM_INVASION19:
					gInvasionManager.StartInvasion(19);
					break;
				case IDM_INVASION20:
					gInvasionManager.StartInvasion(20);
					break;
				case IDM_INVASION21:
					gInvasionManager.StartInvasion(21);
					break;
				case IDM_INVASION22:
					gInvasionManager.StartInvasion(22);
					break;
				case IDM_INVASION23:
					gInvasionManager.StartInvasion(23);
					break;
				case IDM_INVASION24:
					gInvasionManager.StartInvasion(24);
					break;
				case IDM_INVASION25:
					gInvasionManager.StartInvasion(25);
					break;
				case IDM_CA0:
					gCustomArena.StartCustomArena(0);
					break;
				case IDM_CA1:
					gCustomArena.StartCustomArena(1);
					break;
				case IDM_CA2:
					gCustomArena.StartCustomArena(2);
					break;
				case IDM_CA3:
					gCustomArena.StartCustomArena(3);
					break;
				case IDM_CA4:
					gCustomArena.StartCustomArena(4);
					break;
				case IDM_CA5:
					gCustomArena.StartCustomArena(5);
					break;
				case IDM_CA6:
					gCustomArena.StartCustomArena(6);
					break;
				case IDM_CA7:
					gCustomArena.StartCustomArena(7);
					break;
				case IDM_CA8:
					gCustomArena.StartCustomArena(8);
					break;
				case IDM_CA9:
					gCustomArena.StartCustomArena(9);
					break;
				case IDM_CA10:
					gCustomArena.StartCustomArena(10);
					break;
				case IDM_CA11:
					gCustomArena.StartCustomArena(11);
					break;
				case IDM_CA12:
					gCustomArena.StartCustomArena(12);
					break;
				case IDM_CA13:
					gCustomArena.StartCustomArena(13);
					break;
				case IDM_STARTCS:
					#if(GAMESERVER_TYPE==1)
					gCastleSiege.StartCS();
					#endif
					break;
				case IDM_STARTCW:
					gCrywolf.StartCW();
					break;
				case IDM_STARTLD:
					gCastleDeep.StartLD();
					break;
				case IDM_SKY_EVENT:
					gSkyEvent.StartEvent();
					break;
				case IDM_SWAMP_OF_PIECE:
					gSwampEvent.StartEvent();
					break;
				case IDM_UPDATE_CHECK:
					gUpdateManager.ManualCheckForUpdates();
					break;
				case IDM_UPDATE_CONFIG:
					gUpdateManager.ShowConfigDialog();
					break;

				default:
					return DefWindowProc(hWnd,message,wParam,lParam);
			}
			break;
		case WM_CLOSE:
			if (MessageBox(0, "Close GameServer?", "GameServer", MB_OKCANCEL) == IDOK)
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_TIMER:
			switch(wParam)
			{
				case WM_TIMER_1000:
					GJServerUserInfoSend();
					ConnectServerInfoSend();
					break;
				case WM_TIMER_2000:
					gObjCountProc();
					gServerDisplayer.Run();
					break;
				case WM_TIMER_10000:
					JoinServerReconnect(hWnd,WM_JOIN_SERVER_MSG_PROC);
					DataServerReconnect(hWnd,WM_DATA_SERVER_MSG_PROC);
					gUpdateManager.OnTimer();
					break;
			}
			break;
		case WM_JOIN_SERVER_MSG_PROC:
			JoinServerMsgProc(wParam,lParam);
			break;
		case WM_DATA_SERVER_MSG_PROC:
			DataServerMsgProc(wParam,lParam);
			break;
		//case WM_PAINT:
		////{
		////	PAINTSTRUCT ps;
		////	HDC hdc = BeginPaint(hWnd, &ps);
		////
		////	HBITMAP hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
		////	if (hBitmap)
		////	{
		////		HDC hMemDC = CreateCompatibleDC(hdc);
		////		SelectObject(hMemDC, hBitmap);
		////
		////		BITMAP bmp;
		////		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		////
		////		// Pinta el TopBar en la esquina superior izquierda
		////		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
		////
		////		DeleteDC(hMemDC);
		////		DeleteObject(hBitmap);
		////	}
		////
			//EndPaint(hWnd, &ps);
		//}
		return 0;
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd,message,wParam,lParam);
	}

	return 0;
}




// ====================================
// BOT AUTOMATION HELPER FUNCTIONS
// ====================================

// Execute SQL file using SQLCMD (SQL Server command-line tool)
bool ExecuteSQLFile(const char* sqlFilePath, const char* serverName, const char* databaseName, char* errorMsg, int errorMsgSize)
{
	// Build SQLCMD command with Windows Authentication
	char cmdLine[1024];
	sprintf_s(cmdLine, sizeof(cmdLine),
		"sqlcmd -S \"%s\" -d \"%s\" -E -i \"%s\" -o \"IA\\Generated\\SQLOutput.log\"",
		serverName, databaseName, sqlFilePath);

	LogAdd(LOG_BLUE, "[ExecuteSQL] Command: %s", cmdLine);

	// Execute SQLCMD
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		sprintf_s(errorMsg, errorMsgSize, "Failed to start SQLCMD.\nError: %d\n\nMake sure SQL Server tools are installed!", GetLastError());
		LogAdd(LOG_RED, "[ExecuteSQL] CreateProcess failed: %d", GetLastError());
		return false;
	}

	// Wait for completion (timeout: 60 seconds)
	DWORD waitResult = WaitForSingleObject(pi.hProcess, 60000);

	DWORD exitCode = 0;
	GetExitCodeProcess(pi.hProcess, &exitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (waitResult == WAIT_TIMEOUT)
	{
		sprintf_s(errorMsg, errorMsgSize, "SQL execution timeout (>60s)");
		LogAdd(LOG_RED, "[ExecuteSQL] Timeout");
		return false;
	}

	if (exitCode != 0)
	{
		sprintf_s(errorMsg, errorMsgSize, "SQLCMD failed with exit code: %d\n\nCheck IA\\Generated\\SQLOutput.log for details", exitCode);
		LogAdd(LOG_RED, "[ExecuteSQL] Exit code: %d", exitCode);
		return false;
	}

	LogAdd(LOG_GREEN, "[ExecuteSQL] Success!");
	return true;
}

// Update or Replace Accounts.xml from IA_Accounts.xml
bool UpdateAccountsXML(bool replaceMode, char* errorMsg, int errorMsgSize)
{
	using namespace pugi;

	const char* sourceFile = "IA\\Generated\\IA_Accounts.xml";
	const char* targetFile = "IA\\Accounts.xml";
	const char* backupFile = "IA\\Accounts_Backup.xml";

	// Load source file (generated bots)
	xml_document sourceDoc;
	xml_parse_result sourceResult = sourceDoc.load_file(sourceFile);

	if (!sourceResult)
	{
		sprintf_s(errorMsg, errorMsgSize, "Failed to load source file:\n%s\n\nError: %s", sourceFile, sourceResult.description());
		LogAdd(LOG_RED, "[UpdateXML] Failed to load source: %s", sourceResult.description());
		return false;
	}

	xml_node sourceRoot = sourceDoc.child("FakeOnlineData");
	if (!sourceRoot)
	{
		sprintf_s(errorMsg, errorMsgSize, "Invalid XML format in source file!");
		LogAdd(LOG_RED, "[UpdateXML] No FakeOnlineData root in source");
		return false;
	}

	if (replaceMode)
	{
		// REPLACE MODE: Create backup and copy source to target
		LogAdd(LOG_BLUE, "[UpdateXML] REPLACE mode");

		// Backup existing file if it exists
		if (GetFileAttributesA(targetFile) != INVALID_FILE_ATTRIBUTES)
		{
			if (!CopyFileA(targetFile, backupFile, FALSE))
			{
				sprintf_s(errorMsg, errorMsgSize, "Failed to create backup file!");
				LogAdd(LOG_RED, "[UpdateXML] Backup failed");
				return false;
			}
			LogAdd(LOG_GREEN, "[UpdateXML] Backup created: %s", backupFile);
		}

		// Copy source to target
		if (!CopyFileA(sourceFile, targetFile, FALSE))
		{
			sprintf_s(errorMsg, errorMsgSize, "Failed to copy file!");
			LogAdd(LOG_RED, "[UpdateXML] Copy failed");
			return false;
		}

		LogAdd(LOG_GREEN, "[UpdateXML] REPLACE completed!");
		return true;
	}
	else
	{
		// UPDATE MODE: Merge new bots into existing file
		LogAdd(LOG_BLUE, "[UpdateXML] UPDATE mode");

		xml_document targetDoc;
		xml_parse_result targetResult = targetDoc.load_file(targetFile);

		// If target doesn't exist or is invalid, create new
		if (!targetResult)
		{
			LogAdd(LOG_BLUE, "[UpdateXML] Target doesn't exist, creating new");
			if (!CopyFileA(sourceFile, targetFile, FALSE))
			{
				sprintf_s(errorMsg, errorMsgSize, "Failed to create target file!");
				return false;
			}
			return true;
		}

		xml_node targetRoot = targetDoc.child("FakeOnlineData");
		if (!targetRoot)
		{
			// Create root if missing
			targetRoot = targetDoc.append_child("FakeOnlineData");
		}

		// Backup existing file
		if (!CopyFileA(targetFile, backupFile, FALSE))
		{
			LogAdd(LOG_RED, "[UpdateXML] Warning: Backup failed");
		}

		// Build map of existing accounts for duplicate checking
		std::map<std::string, xml_node> existingAccounts;
		for (xml_node info = targetRoot.child("Info"); info; info = info.next_sibling("Info"))
		{
			const char* account = info.attribute("Account").value();
			if (account && account[0] != '\\0')
			{
				existingAccounts[account] = info;
			}
		}

		// Merge new bots
		int addedCount = 0;
		int updatedCount = 0;

		for (xml_node sourceInfo = sourceRoot.child("Info"); sourceInfo; sourceInfo = sourceInfo.next_sibling("Info"))
		{
			const char* account = sourceInfo.attribute("Account").value();
			if (!account || account[0] == '\\0')
				continue;

			auto it = existingAccounts.find(account);
			if (it != existingAccounts.end())
			{
				// Update existing
				xml_node existingNode = it->second;

				// Copy all attributes
				for (xml_attribute attr = sourceInfo.first_attribute(); attr; attr = attr.next_attribute())
				{
					existingNode.attribute(attr.name()).set_value(attr.value());
				}

				updatedCount++;
			}
			else
			{
				// Add new
				targetRoot.append_copy(sourceInfo);
				addedCount++;
			}
		}

		// Copy MSGThongBao and Config from source if they exist
		xml_node sourceMSG = sourceDoc.child("MSGThongBao");
		if (sourceMSG)
		{
			xml_node targetMSG = targetDoc.child("MSGThongBao");
			if (!targetMSG)
			{
				targetDoc.prepend_copy(sourceMSG);
			}
		}

		xml_node sourceConfig = sourceDoc.child("Config");
		if (sourceConfig)
		{
			xml_node targetConfig = targetDoc.child("Config");
			if (!targetConfig)
			{
				xml_node msgNode = targetDoc.child("MSGThongBao");
				if (msgNode)
				{
					targetDoc.insert_copy_after(sourceConfig, msgNode);
				}
				else
				{
					targetDoc.prepend_copy(sourceConfig);
				}
			}
		}

		// Save merged file
		if (!targetDoc.save_file(targetFile))
		{
			sprintf_s(errorMsg, errorMsgSize, "Failed to save merged file!");
			LogAdd(LOG_RED, "[UpdateXML] Save failed");
			return false;
		}

		LogAdd(LOG_GREEN, "[UpdateXML] UPDATE completed! Added: %d, Updated: %d", addedCount, updatedCount);

		// Store counts for message box
		sprintf_s(errorMsg, errorMsgSize, "Added: %d new bots\\nUpdated: %d existing bots", addedCount, updatedCount);
		return true;
	}
}




// ====================================
// CLASS CONFIGURATION DIALOG
// ====================================

INT_PTR CALLBACK ConfigClassDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hComboClass, hComboConfigSelect;
	static int selectedClassCode = 0;
	static int selectedConfig = 1;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		// Add config selector combo
		hComboConfigSelect = GetDlgItem(hDlg, IDC_COMBO_CONFIGSELECT);
		SendMessage(hComboConfigSelect, CB_ADDSTRING, 0, (LPARAM)"Config 1 (ClassConfig.ini)");
		SendMessage(hComboConfigSelect, CB_ADDSTRING, 0, (LPARAM)"Config 2 (ClassConfig2.ini)");
		SendMessage(hComboConfigSelect, CB_ADDSTRING, 0, (LPARAM)"Config 3 (ClassConfig3.ini)");
		SendMessage(hComboConfigSelect, CB_ADDSTRING, 0, (LPARAM)"Config 4 (ClassConfig4.ini)");
		SendMessage(hComboConfigSelect, CB_ADDSTRING, 0, (LPARAM)"Config 5 (ClassConfig5.ini)");
		SendMessage(hComboConfigSelect, CB_ADDSTRING, 0, (LPARAM)"Config 6 (ClassConfig6.ini)");
		SendMessage(hComboConfigSelect, CB_ADDSTRING, 0, (LPARAM)"Config 7 (ClassConfig7.ini)");
		SendMessage(hComboConfigSelect, CB_SETCURSEL, 0, 0);

		hComboClass = GetDlgItem(hDlg, IDC_COMBO_CLASSCONFIG);

		std::map<int, ClassHexData>& allClasses = g_ClassConfigManager.GetAllClasses();
		for (std::map<int, ClassHexData>::iterator it = allClasses.begin(); it != allClasses.end(); ++it)
		{
			ClassHexData& data = it->second;
			int idx = SendMessage(hComboClass, CB_ADDSTRING, 0, (LPARAM)data.className.c_str());
			SendMessage(hComboClass, CB_SETITEMDATA, idx, (LPARAM)data.classCode);
		}

		SendMessage(hComboClass, CB_SETCURSEL, 0, 0);

		selectedClassCode = 0;
		selectedConfig = 1;
		ClassHexData* data = g_ClassConfigManager.GetClassData(0, 1);
		if (data)
		{
			SetDlgItemText(hDlg, IDC_EDIT_CLASSINVHEX, data->inventoryHex.c_str());
			SetDlgItemText(hDlg, IDC_EDIT_CLASSMAGICHEX, data->magicListHex.c_str());
		}

		return TRUE;
	}

	case WM_COMMAND:
	{
		// Config selector changed
		if (LOWORD(wParam) == IDC_COMBO_CONFIGSELECT && HIWORD(wParam) == CBN_SELCHANGE)
		{
			selectedConfig = SendMessage(hComboConfigSelect, CB_GETCURSEL, 0, 0) + 1;

			ClassHexData* data = g_ClassConfigManager.GetClassData(selectedClassCode, selectedConfig);
			if (data)
			{
				SetDlgItemText(hDlg, IDC_EDIT_CLASSINVHEX, data->inventoryHex.c_str());
				SetDlgItemText(hDlg, IDC_EDIT_CLASSMAGICHEX, data->magicListHex.c_str());
			}
			return TRUE;
		}
		// Class changed
		else if (LOWORD(wParam) == IDC_COMBO_CLASSCONFIG && HIWORD(wParam) == CBN_SELCHANGE)
		{
			int idx = SendMessage(hComboClass, CB_GETCURSEL, 0, 0);
			selectedClassCode = (int)SendMessage(hComboClass, CB_GETITEMDATA, idx, 0);

			ClassHexData* data = g_ClassConfigManager.GetClassData(selectedClassCode, selectedConfig);
			if (data)
			{
				SetDlgItemText(hDlg, IDC_EDIT_CLASSINVHEX, data->inventoryHex.c_str());
				SetDlgItemText(hDlg, IDC_EDIT_CLASSMAGICHEX, data->magicListHex.c_str());
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BTN_SAVECLASS)
		{
			char invHex[8192] = { 0 };
			char magicHex[2048] = { 0 };

			GetDlgItemText(hDlg, IDC_EDIT_CLASSINVHEX, invHex, sizeof(invHex));
			GetDlgItemText(hDlg, IDC_EDIT_CLASSMAGICHEX, magicHex, sizeof(magicHex));

			if (strlen(invHex) < 10 || strlen(magicHex) < 10)
			{
				MessageBox(hDlg, "Please enter valid hex strings!", "Error", MB_OK | MB_ICONERROR);
				return TRUE;
			}

			g_ClassConfigManager.SetClassData(selectedClassCode, invHex, magicHex, selectedConfig);

			char msg[256];
			sprintf_s(msg, sizeof(msg), "Saved to Config %d!\n\nYou can now:\n- Configure another class\n- Switch to another config", selectedConfig);
			MessageBox(hDlg, msg, "Success", MB_OK | MB_ICONINFORMATION);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)

		{

			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}

		else if (LOWORD(wParam) == IDC_BTN_TESTCONNECTION)
		{
			// Test SQL Connection button
			char serverName[128] = ".\\SQLEXPRESS";
			GetDlgItemTextA(hDlg, IDC_EDIT_SQLSERVER, serverName, sizeof(serverName));

			// Use default if empty
			if (strlen(serverName) == 0)
				strcpy_s(serverName, ".\\SQLEXPRESS");

			EnableWindow(hDlg, FALSE);
			SetCursor(LoadCursor(NULL, IDC_WAIT));

			std::vector<std::string> databases;
			char errorMsg[512];

			if (TestSQLConnection(serverName, databases, errorMsg, sizeof(errorMsg)))
			{
				// Populate database combo
				HWND hComboDb = GetDlgItem(hDlg, IDC_COMBO_DATABASES);
				SendMessage(hComboDb, CB_RESETCONTENT, 0, 0);

				for (size_t i = 0; i < databases.size(); i++)
				{
					SendMessage(hComboDb, CB_ADDSTRING, 0, (LPARAM)databases[i].c_str());
				}

				// Auto-select MuOnline if it exists
				int muIndex = SendMessage(hComboDb, CB_FINDSTRINGEXACT, -1, (LPARAM)"MuOnline");
				if (muIndex != CB_ERR)
				{
					SendMessage(hComboDb, CB_SETCURSEL, muIndex, 0);
				}
				else if (databases.size() > 0)
				{
					SendMessage(hComboDb, CB_SETCURSEL, 0, 0);
				}

				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);

				char successMsg[512];
				sprintf_s(successMsg, sizeof(successMsg),
					"? Connection successful!\n\nServer: %s\n\nFound %d databases.\nSelect one from the dropdown.",
					serverName, databases.size());
				MessageBox(hDlg, successMsg, "SQL Connection Test", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);
				MessageBox(hDlg, errorMsg, "Connection Test Failed", MB_OK | MB_ICONERROR);
			}

			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BTN_REFRESHDB)
		{
			// Refresh database list - same as test connection
			SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_BTN_TESTCONNECTION, BN_CLICKED), (LPARAM)GetDlgItem(hDlg, IDC_BTN_TESTCONNECTION));
			return TRUE;
		}
		else if (LOWORD(wParam) == WM_CLOSE)
		{
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}



		break;
	}

		case WM_CLOSE:  // ADD THIS
			EndDialog(hDlg, IDCANCEL);
			return TRUE;

	}

	return FALSE;
}




// Redirect old function to new implementation
bool CreateMultipleBotsAdvanced(int botCount, int startFrom, int gateNumber, int mapNumber, int mapX, int mapY,
	int minLevel, int maxLevel, int selectedClass, int phamViTrain, int moveRange, int timeReturn,
	int tuNhatItem, int tuDongReset, int partyMode, int pvpMode, int postKhiDie, int enabledConfigs)
{
	LogAdd(LOG_BLACK, (char*)"[CreateBots] ===== START =====");
	LogAdd(LOG_BLACK, (char*)"[CreateBots] Count=%d, StartFrom=%d, EnabledConfigs=%d", botCount, startFrom, enabledConfigs);

	// SAFETY CHECKS
	if (botCount > 1000 || botCount < 1)
	{
		LogAdd(LOG_RED, (char*)"[CreateBots] ERROR: Invalid bot count");
		return false;
	}

	if (startFrom < 1)
	{
		LogAdd(LOG_RED, (char*)"[CreateBots] ERROR: Invalid start number");
		return false;
	}

	// Class configuration
	struct ClassConfig {
		int classCode;
		const char* className;
		int mainSkill;
		int secondarySkill;
		int buff1, buff2, buff3;
		int percentage;
		int str, dex, vit, ene, cmd;
		bool useFemaleNames;
	};

	ClassConfig classes[] = {
		{0,  "DW",  9,   12,  16,  -1,  -1, 15, 2000, 2000, 2000, 5000, 0,    false},
		{16, "DK",  44,  41,  48,  -1,  -1, 30, 5000, 4500, 5500, 4000, 0,    false},
		{32, "ELF", 24,  52,  26,  27,  28, 20, 2500, 4500, 2500, 3000, 0,    true},
		{48, "MG",  8,  55,   -1,  -1,  -1, 10, 4000, 3000, 4000, 4000, 0,    false},
		{64, "DL",  65,  61,  64,  -1,  -1, 10, 4500, 3500, 4500, 2500, 5000, false},
		{80, "SUM", 214, 215, 217, 218, -1, 10, 2000, 2000, 2000, 5000, 0,    true},
		{96, "RF",  264, 263, 266, 268, -1, 5,  4500, 4500, 4500, 2000, 0,    false}
	};

	int classCount = sizeof(classes) / sizeof(classes[0]);

	// Calculate cumulative percentages
	int cumulativePercentages[10];
	int total = 0;
	for (int i = 0; i < classCount; i++)
	{
		total += classes[i].percentage;
		cumulativePercentages[i] = total;
	}

	LogAdd(LOG_BLACK, (char*)"[CreateBots] Opening XML file...");

	// Open ONLY XML file (no SQL file!)
	FILE* xmlFile = NULL;
	errno_t err = fopen_s(&xmlFile, "IA\\Generated\\IA_Accounts.xml", "w");
	if (err != 0 || !xmlFile)
	{
		LogAdd(LOG_RED, (char*)"[CreateBots] FAILED to open XML file! Error: %d", err);
		return false;
	}
	LogAdd(LOG_GREEN, (char*)"[CreateBots] XML file opened successfully");

	// Write XML header
	fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n", xmlFile);
	fputs("<MSGThongBao IndexMesMin=\"3020\" IndexMesMax=\"3030\"/>\n\n", xmlFile);
	fputs("<Config DelayRange=\"40000\" />\n\n", xmlFile);
	fputs("<FakeOnlineData>\n", xmlFile);

	LogAdd(LOG_GREEN, (char*)"[CreateBots] Starting bot generation...");

	// Process bots
	int successCount = 0;
	int failCount = 0;

	for (int i = 0; i < botCount; i++)
	{
		int botNumber = startFrom + i;
		char account[11];
		char charName[11];

		sprintf_s(account, sizeof(account), "Bot%04d", botNumber);

		// Select class
		ClassConfig* selectedClassConfig = NULL;

		if (selectedClass == -1)
		{
			int randValue = i % 100;
			for (int j = 0; j < classCount; j++)
			{
				if (randValue < cumulativePercentages[j])
				{
					selectedClassConfig = &classes[j];
					break;
				}
			}
		}
		else
		{
			for (int j = 0; j < classCount; j++)
			{
				if (classes[j].classCode == selectedClass)
				{
					selectedClassConfig = &classes[j];
					break;
				}
			}
		}

		if (!selectedClassConfig) selectedClassConfig = &classes[0];

		// Select random config
		int configIndex = g_ClassConfigManager.GetRandomConfigIndex(enabledConfigs);

		// Verify class is configured
		if (!g_ClassConfigManager.IsClassConfigured(selectedClassConfig->classCode, configIndex))
		{
			LogAdd(LOG_RED, (char*)"[CreateBots] Class %s NOT configured in Config %d!",
				selectedClassConfig->className, configIndex);
			fclose(xmlFile);
			return false;
		}

		// Generate name
		if (selectedClassConfig->useFemaleNames)
		{
			sprintf_s(charName, sizeof(charName), "%s%d",
				g_NameManager.GetRandomFemaleName(), GetLargeRand() % 90 + 10);
		}
		else
		{
			sprintf_s(charName, sizeof(charName), "%s%d",
				g_NameManager.GetRandomMaleName(), GetLargeRand() % 90 + 10);
		}

		int level = minLevel + (GetLargeRand() % (maxLevel - minLevel + 1));
		int finalMapX = mapX + ((i % 20) - 10);
		int finalMapY = mapY + (((i / 20) % 20) - 10);

		// Write to XML (KEEP THIS!)
		fprintf(xmlFile,
			"  <Info Account=\"%s\" Password=\"123456\" Name=\"%s\" "
			"SkillID=\"%d\" SecondarySkillID=\"%d\" "
			"UseBuffs_0=\"%d\" UseBuffs_1=\"%d\" UseBuffs_2=\"%d\" "
			"GateNumber=\"%d\" Map=\"%d\" MapX=\"%d\" MapY=\"%d\" "
			"PhamViTrain=\"%d\" MoveRange=\"%d\" TimeReturn=\"%d\" "
			"TuNhatItem=\"%d\" TuDongReset=\"%d\" "
			"PartyMode=\"%d\" PVPMode=\"%d\" PostKhiDie=\"%d\" />\n",
			account, charName,
			selectedClassConfig->mainSkill, selectedClassConfig->secondarySkill,
			selectedClassConfig->buff1, selectedClassConfig->buff2, selectedClassConfig->buff3,
			gateNumber, mapNumber, finalMapX, finalMapY,
			phamViTrain, moveRange, timeReturn,
			tuNhatItem, tuDongReset,
			partyMode, pvpMode, postKhiDie
		);

		// Get hex data
		const char* invHex = g_ClassConfigManager.GetInventoryHex(selectedClassConfig->classCode, configIndex);
		const char* magicHex = g_ClassConfigManager.GetMagicListHex(selectedClassConfig->classCode, configIndex);

		if (!invHex || !magicHex)
		{
			LogAdd(LOG_RED, (char*)"[CreateBots] Bot %d: Failed to get hex data", i + 1);
			failCount++;
			continue;
		}

		// =====================================================
		// NEW: Call stored procedure instead of writing SQL file
		// =====================================================
		if (CallBotStoredProcedure(account, charName, selectedClassConfig->classCode,
			level, mapNumber, finalMapX, finalMapY,
			selectedClassConfig->str, selectedClassConfig->dex,
			selectedClassConfig->vit, selectedClassConfig->ene,
			selectedClassConfig->cmd, invHex, magicHex))
		{
			successCount++;
			if ((i + 1) % 10 == 0)
			{
				LogAdd(LOG_BLUE, (char*)"[CreateBots] Progress: %d/%d", i + 1, botCount);
			}
		}
		else
		{
			failCount++;
			LogAdd(LOG_RED, (char*)"[CreateBots] Bot %d (%s) creation failed", i + 1, account);
		}
	}

	// Write XML footer
	fputs("</FakeOnlineData>\n", xmlFile);
	fclose(xmlFile);

	LogAdd(LOG_GREEN, (char*)"[CreateBots] ===== COMPLETED =====");
	LogAdd(LOG_GREEN, (char*)"[CreateBots] Success: %d, Failed: %d", successCount, failCount);
	LogAdd(LOG_GREEN, (char*)"[CreateBots] XML file: IA\\Generated\\IA_Accounts.xml");

	return (successCount > 0);
}



// ====================================
// SIMPLIFIED BOT CREATOR DIALOG
// ====================================

INT_PTR CALLBACK CreateBotsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hComboClass, hComboPartyMode, hComboPVPMode, hComboLanguage, hComboDatabase;
	static HBRUSH hBrushBg = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		// Create custom brush for dialog background
		hBrushBg = CreateSolidBrush(RGB(25, 25, 25)); // Dark background

		// Basic config - DEFAULTS
		SetDlgItemInt(hDlg, IDC_EDIT_BOTCOUNT, 50, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_STARTFROM, 1, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_GATENUMBER, 17, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_MAP, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_MAPX, 125, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_MAPY, 125, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_MINLEVEL, 250, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_MAXLEVEL, 400, FALSE);

		// Bot behavior defaults
		SetDlgItemInt(hDlg, IDC_EDIT_PHAMVITRAIN, 6, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_MOVERANGE, 30, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT_TIMERETURN, 40, FALSE);
		CheckDlgButton(hDlg, IDC_CHECK_TUNHATITEM, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_TUDONGRESET, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_POSTKHIDIE, BST_CHECKED);

		// Config checkboxes
		CheckDlgButton(hDlg, IDC_CHECK_CONFIG1, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_CONFIG2, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_CONFIG3, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_CONFIG4, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_CONFIG5, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_CONFIG6, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECK_CONFIG7, BST_UNCHECKED);

		// Class combo
		hComboClass = GetDlgItem(hDlg, IDC_COMBO_CLASS);
		SendMessage(hComboClass, CB_ADDSTRING, 0, (LPARAM)"All Classes (Mixed)");
		SendMessage(hComboClass, CB_SETITEMDATA, 0, (LPARAM)-1);

		std::map<int, ClassHexData>& allClasses = g_ClassConfigManager.GetAllClasses();
		for (std::map<int, ClassHexData>::iterator it = allClasses.begin(); it != allClasses.end(); ++it)
		{
			ClassHexData& data = it->second;
			int idx = SendMessage(hComboClass, CB_ADDSTRING, 0, (LPARAM)data.className.c_str());
			SendMessage(hComboClass, CB_SETITEMDATA, idx, (LPARAM)data.classCode);
		}
		SendMessage(hComboClass, CB_SETCURSEL, 0, 0);

		// Party Mode combo
		hComboPartyMode = GetDlgItem(hDlg, IDC_COMBO_PARTYMODE);
		SendMessage(hComboPartyMode, CB_ADDSTRING, 0, (LPARAM)"0 - Disabled");
		SendMessage(hComboPartyMode, CB_ADDSTRING, 0, (LPARAM)"1 - Accept Party Invites");
		SendMessage(hComboPartyMode, CB_ADDSTRING, 0, (LPARAM)"2 - Send Party Invites");
		SendMessage(hComboPartyMode, CB_SETCURSEL, 1, 0);

		// PVP Mode combo
		hComboPVPMode = GetDlgItem(hDlg, IDC_COMBO_PVPMODE);
		SendMessage(hComboPVPMode, CB_ADDSTRING, 0, (LPARAM)"0 - Disabled");
		SendMessage(hComboPVPMode, CB_ADDSTRING, 0, (LPARAM)"1 - Defend Only");
		SendMessage(hComboPVPMode, CB_ADDSTRING, 0, (LPARAM)"2 - Attack All Players");
		SendMessage(hComboPVPMode, CB_SETCURSEL, 1, 0);

		// Language combo
		hComboLanguage = GetDlgItem(hDlg, IDC_COMBO_LANGUAGE);
		if (hComboLanguage)
		{
			int langCount = g_NameManager.GetAvailableLanguageCount();
			if (langCount > 0)
			{
				for (int i = 0; i < langCount; i++)
				{
					const char* langName = g_NameManager.GetLanguageName(i);
					SendMessage(hComboLanguage, CB_ADDSTRING, 0, (LPARAM)langName);
				}
				const char* currentLang = g_NameManager.GetCurrentLanguage();
				int selIdx = (int)SendMessage(hComboLanguage, CB_FINDSTRINGEXACT, -1, (LPARAM)currentLang);
				SendMessage(hComboLanguage, CB_SETCURSEL, (selIdx != CB_ERR) ? selIdx : 0, 0);
			}
			else
			{
				SendMessage(hComboLanguage, CB_ADDSTRING, 0, (LPARAM)"Default (Spanish)");
				SendMessage(hComboLanguage, CB_SETCURSEL, 0, 0);
			}
		}

		// Database combo
		hComboDatabase = GetDlgItem(hDlg, IDC_COMBO_DATABASES);
		SendMessage(hComboDatabase, CB_ADDSTRING, 0, (LPARAM)"MuOnline");
		SendMessage(hComboDatabase, CB_SETCURSEL, 0, 0);

		// Set default server
		SetDlgItemText(hDlg, IDC_EDIT_SQLSERVER, ".\\SQLEXPRESS");

		return TRUE;
	}

	case WM_CTLCOLORDLG:
		return (INT_PTR)hBrushBg;

	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(220, 220, 220)); // Light gray text
		SetBkMode(hdcStatic, TRANSPARENT);
		return (INT_PTR)hBrushBg;
	}

	case WM_CTLCOLORBTN:
	{
		HDC hdcButton = (HDC)wParam;
		SetTextColor(hdcButton, RGB(255, 255, 255));
		SetBkMode(hdcButton, TRANSPARENT);
		return (INT_PTR)CreateSolidBrush(RGB(180, 0, 0)); // Red buttons
	}

	case WM_COMMAND:
	{
		// Language changed
		if (LOWORD(wParam) == IDC_COMBO_LANGUAGE && HIWORD(wParam) == CBN_SELCHANGE)
		{
			int selIdx = (int)SendMessage(hComboLanguage, CB_GETCURSEL, 0, 0);
			if (selIdx != CB_ERR)
			{
				char langName[50];
				SendMessage(hComboLanguage, CB_GETLBTEXT, selIdx, (LPARAM)langName);

				bool namesOK = g_NameManager.LoadLanguage(langName);
				bool phrasesOK = g_PhraseManager.LoadLanguage(langName);

				if (namesOK || phrasesOK)
				{
					LogAdd(LOG_GREEN, "[Language] Changed to: %s", langName);
#if USE_FAKE_ONLINE == TRUE
					LoadBotPhrasesFromFile(g_PhraseManager.GetBotPhrasesPath());
					LoadBotKeywordResponses(g_PhraseManager.GetAnsweringPath());
#endif
				}
			}
			return TRUE;
		}
		// Configure Class Equipment
		else if (LOWORD(wParam) == IDC_BTN_CONFIGCLASS)
		{
			HWND hConfigDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CONFIGCLASS), hDlg, ConfigClassDialogProc);
			if (hConfigDlg)
			{
				ShowWindow(hConfigDlg, SW_SHOW);
			}
			return TRUE;
		}
		// Test Connection Button - FIXED VERSION
		else if (LOWORD(wParam) == IDC_BTN_TESTCONNECTION)
		{
			char serverName[128] = { 0 };
			GetDlgItemTextA(hDlg, IDC_EDIT_SQLSERVER, serverName, sizeof(serverName));

			// Trim whitespace
			int len = strlen(serverName);
			while (len > 0 && (serverName[len - 1] == ' ' || serverName[len - 1] == '\t'))
			{
				serverName[--len] = '\0';
			}

			// Use default if empty
			if (len == 0)
			{
				strcpy_s(serverName, ".\\SQLEXPRESS");
			}

			LogAdd(LOG_BLUE, "[TestConnection] Testing server: '%s'", serverName);

			EnableWindow(hDlg, FALSE);
			SetCursor(LoadCursor(NULL, IDC_WAIT));

			std::vector<std::string> databases;
			char errorMsg[512];

			if (TestSQLConnection(serverName, databases, errorMsg, sizeof(errorMsg)))
			{
				// Populate database combo
				hComboDatabase = GetDlgItem(hDlg, IDC_COMBO_DATABASES);
				SendMessage(hComboDatabase, CB_RESETCONTENT, 0, 0);

				for (size_t i = 0; i < databases.size(); i++)
				{
					SendMessage(hComboDatabase, CB_ADDSTRING, 0, (LPARAM)databases[i].c_str());
				}

				// Auto-select MuOnline if exists
				int muIndex = SendMessage(hComboDatabase, CB_FINDSTRINGEXACT, -1, (LPARAM)"MuOnline");
				if (muIndex != CB_ERR)
				{
					SendMessage(hComboDatabase, CB_SETCURSEL, muIndex, 0);
				}
				else if (databases.size() > 0)
				{
					SendMessage(hComboDatabase, CB_SETCURSEL, 0, 0);
				}

				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);

				char successMsg[512];
				sprintf_s(successMsg, sizeof(successMsg),
					"✅ Connection successful!\n\nServer: %s\nFound %d databases\n\nSelect database from dropdown.",
					serverName, (int)databases.size());
				MessageBox(hDlg, successMsg, "SQL Connection Test", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);
				MessageBox(hDlg, errorMsg, "Connection Test Failed", MB_OK | MB_ICONERROR);
			}

			return TRUE;
		}
		// Refresh Database List
		else if (LOWORD(wParam) == IDC_BTN_REFRESHDB)
		{
			SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_BTN_TESTCONNECTION, BN_CLICKED),
				(LPARAM)GetDlgItem(hDlg, IDC_BTN_TESTCONNECTION));
			return TRUE;
		}
		// CREATE BOTS Button
		else if (LOWORD(wParam) == IDC_BTN_CREATEBOTS)
		{
			BOOL bSuccess;

			// Get all parameters
			int botCount = GetDlgItemInt(hDlg, IDC_EDIT_BOTCOUNT, &bSuccess, FALSE);
			if (!bSuccess || botCount < 1 || botCount > 1000)
			{
				MessageBox(hDlg, "Bot count must be between 1-1000", "Error", MB_OK | MB_ICONERROR);
				return TRUE;
			}

			int startFrom = GetDlgItemInt(hDlg, IDC_EDIT_STARTFROM, &bSuccess, FALSE);
			if (!bSuccess || startFrom < 1)
			{
				MessageBox(hDlg, "Start From must be >= 1", "Error", MB_OK | MB_ICONERROR);
				return TRUE;
			}

			int gateNumber = GetDlgItemInt(hDlg, IDC_EDIT_GATENUMBER, &bSuccess, FALSE);
			int mapNumber = GetDlgItemInt(hDlg, IDC_EDIT_MAP, &bSuccess, FALSE);
			int mapX = GetDlgItemInt(hDlg, IDC_EDIT_MAPX, &bSuccess, FALSE);
			int mapY = GetDlgItemInt(hDlg, IDC_EDIT_MAPY, &bSuccess, FALSE);
			int minLevel = GetDlgItemInt(hDlg, IDC_EDIT_MINLEVEL, &bSuccess, FALSE);
			int maxLevel = GetDlgItemInt(hDlg, IDC_EDIT_MAXLEVEL, &bSuccess, FALSE);

			if (!bSuccess || minLevel < 1 || maxLevel > 400 || minLevel > maxLevel)
			{
				MessageBox(hDlg, "Level range: 1-400 (min <= max)", "Error", MB_OK | MB_ICONERROR);
				return TRUE;
			}

			// Bot behavior
			int phamViTrain = GetDlgItemInt(hDlg, IDC_EDIT_PHAMVITRAIN, &bSuccess, FALSE);
			int moveRange = GetDlgItemInt(hDlg, IDC_EDIT_MOVERANGE, &bSuccess, FALSE);
			int timeReturn = GetDlgItemInt(hDlg, IDC_EDIT_TIMERETURN, &bSuccess, FALSE);
			int tuNhatItem = (IsDlgButtonChecked(hDlg, IDC_CHECK_TUNHATITEM) == BST_CHECKED) ? 1 : 0;
			int tuDongReset = (IsDlgButtonChecked(hDlg, IDC_CHECK_TUDONGRESET) == BST_CHECKED) ? 1 : 0;
			int postKhiDie = (IsDlgButtonChecked(hDlg, IDC_CHECK_POSTKHIDIE) == BST_CHECKED) ? 1 : 0;

			int partyMode = SendMessage(hComboPartyMode, CB_GETCURSEL, 0, 0);
			int pvpMode = SendMessage(hComboPVPMode, CB_GETCURSEL, 0, 0);

			int idx = SendMessage(hComboClass, CB_GETCURSEL, 0, 0);
			int selectedClass = (int)SendMessage(hComboClass, CB_GETITEMDATA, idx, 0);

			// Get enabled configs
			int enabledConfigs = 0;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG1) == BST_CHECKED) enabledConfigs |= (1 << 0);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG2) == BST_CHECKED) enabledConfigs |= (1 << 1);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG3) == BST_CHECKED) enabledConfigs |= (1 << 2);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG4) == BST_CHECKED) enabledConfigs |= (1 << 3);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG5) == BST_CHECKED) enabledConfigs |= (1 << 4);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG6) == BST_CHECKED) enabledConfigs |= (1 << 5);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG7) == BST_CHECKED) enabledConfigs |= (1 << 6);

			if (enabledConfigs == 0)
			{
				MessageBox(hDlg, "Please select at least one Config!", "Error", MB_OK | MB_ICONERROR);
				return TRUE;
			}

			// Get database settings
			char serverName[128] = { 0 };
			char dbName[64] = { 0 };
			GetDlgItemTextA(hDlg, IDC_EDIT_SQLSERVER, serverName, sizeof(serverName));
			GetDlgItemTextA(hDlg, IDC_COMBO_DATABASES, dbName, sizeof(dbName));

			// Trim whitespace from server name
			int len = strlen(serverName);
			while (len > 0 && (serverName[len - 1] == ' ' || serverName[len - 1] == '\t'))
			{
				serverName[--len] = '\0';
			}

			// Use defaults if empty
			if (len == 0) strcpy_s(serverName, ".\\SQLEXPRESS");
			if (strlen(dbName) == 0) strcpy_s(dbName, "MuOnline");

			LogAdd(LOG_BLUE, "[CreateBots] Server: '%s', Database: '%s'", serverName, dbName);

			// Initialize database connection
			if (!InitializeBotODBC(serverName, dbName, "", ""))
			{
				MessageBox(hDlg,
					"❌ Failed to connect to database!\n\n"
					"Check:\n"
					"• SQL Server is running\n"
					"• Server name is correct\n"
					"• Database name is correct\n"
					"• Windows Authentication is enabled",
					"Database Error", MB_OK | MB_ICONERROR);
				return TRUE;
			}

			// Warn for large batches
			if (botCount > 100)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg),
					"⚠ Creating %d bots at once.\n\nThis may take a while...\n\nContinue?", botCount);
				if (MessageBox(hDlg, msg, "Warning", MB_YESNO | MB_ICONWARNING) != IDYES)
					return TRUE;
			}

			// Disable dialog during creation
			EnableWindow(hDlg, FALSE);
			SetCursor(LoadCursor(NULL, IDC_WAIT));

			// Call creation function
			if (CreateMultipleBotsAdvanced_StoredProc(botCount, startFrom, gateNumber, mapNumber, mapX, mapY,
				minLevel, maxLevel, selectedClass, phamViTrain, moveRange, timeReturn,
				tuNhatItem, tuDongReset, partyMode, pvpMode, postKhiDie, enabledConfigs))
			{
				int configCount = 0;
				for (int i = 0; i < 7; i++)
				{
					if (enabledConfigs & (1 << i)) configCount++;
				}

				char szMsg[1024];
				sprintf_s(szMsg, sizeof(szMsg),
					"✅ Successfully created %d bots!\n\n"
					"📋 Details:\n"
					"  • Range: Bot%04d to Bot%04d\n"
					"  • Location: %s\n"
					"  • Levels: %d-%d\n"
					"  • Configs Used: %d\n"
					"  • Database: %s\n\n"
					"📝 Files Generated:\n"
					"  • IA\\Generated\\IA_Accounts.xml\n\n"
					"═══════════════════════════════\n"
					"🎯 NEXT STEPS:\n"
					"═══════════════════════════════\n"
					"1️⃣ Click 'UPDATE ACCOUNTS.XML' button\n"
					"2️⃣ Go to GameServer menu\n"
					"3️⃣ Click 'Reload IA Data'\n"
					"4️⃣ Click 'Add Fake Online'\n\n"
					"✨ Your bots are ready to use!",
					botCount, startFrom, startFrom + botCount - 1,
					(gateNumber > 0) ? "Gate" : "Custom Coords",
					minLevel, maxLevel, configCount, dbName);

				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);
				MessageBox(hDlg, szMsg, "🎉 Bots Created!", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);
				MessageBox(hDlg,
					"❌ Failed to create bots!\n\n"
					"Check:\n"
					"• Database connection is working\n"
					"• Stored procedure exists\n"
					"• Console logs for details",
					"Error", MB_OK | MB_ICONERROR);
			}

			return TRUE;
		}
		// UPDATE ACCOUNTS.XML Button
		else if (LOWORD(wParam) == IDC_BTN_UPDATEXML)
		{
			int choice = MessageBox(hDlg,
				"Choose update mode:\n\n"
				"✅ UPDATE (Yes):\n"
				"  • Merges new bots with existing\n"
				"  • Keeps existing bots\n"
				"  • Creates backup\n\n"
				"🔄 REPLACE (No):\n"
				"  • Replaces entire file\n"
				"  • Deletes existing bots\n"
				"  • Creates backup\n\n"
				"Choose UPDATE?",
				"Update Accounts.xml", MB_YESNOCANCEL | MB_ICONQUESTION);

			if (choice == IDCANCEL)
				return TRUE;

			bool updateMode = (choice == IDYES);

			EnableWindow(hDlg, FALSE);
			SetCursor(LoadCursor(NULL, IDC_WAIT));

			char resultMsg[512];
			if (UpdateAccountsXML(updateMode == false, resultMsg, sizeof(resultMsg)))
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);

				char successMsg[1024];
				if (updateMode)
				{
					sprintf_s(successMsg, sizeof(successMsg),
						"[OK] Accounts.xml UPDATED!\n\n"
						"%s\n\n"
						"[OK] Backup: IA\\Accounts_Backup.xml\n\n"
						"═══════════════════════════════\n"
						"[OK] FINAL STEPS:\n"
						"═══════════════════════════════\n"
						"1 Menu → Reload IA Data\n"
						"2 Menu → Add Fake Online\n\n"
						"[OK] Bots ready!",
						resultMsg);
				}
				else
				{
					sprintf_s(successMsg, sizeof(successMsg),
						"[OK] Accounts.xml REPLACED!\n\n"
						"All previous bots removed.\n\n"
						"Backup: IA\\Accounts_Backup.xml\n\n"
						"═══════════════════════════════\n"
						"[OK] FINAL STEPS:\n"
						"═══════════════════════════════\n"
						"1 Menu → Reload IA Data\n"
						"2 Menu → Add Fake Online\n\n"
						"[OK] Bots ready!");
				}

				MessageBox(hDlg, successMsg, "🎉 Success!", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EnableWindow(hDlg, TRUE);
				MessageBox(hDlg, resultMsg, "❌ Update Failed", MB_OK | MB_ICONERROR);
			}

			return TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}

	case WM_DESTROY:
		if (hBrushBg)
		{
			DeleteObject(hBrushBg);
			hBrushBg = NULL;
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, IDCANCEL);
		return TRUE;
	}

	return FALSE;
}




LRESULT CALLBACK About(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam) // OK
{
	switch(message)
	{
		case WM_INITDIALOG:
			return 1;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg,LOWORD(wParam));
				return 1;
			}
			break;
	}

	return 0;
}

LRESULT CALLBACK UserOnline(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam) // OK
{
	
	switch(message)
	{
		case WM_INITDIALOG:
					

			hWndComboBox = GetDlgItem(hDlg, IDC_LIST1);

            if( !hWndComboBox )
            {
                MessageBox(hDlg,
                           "Could not create the combo box",
                           "Failed Control Creation",
                           MB_OK);
                return FALSE;
            }


			for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
			{
				if(gObj[n].Connected >= OBJECT_LOGGED && gObj[n].Type == OBJECT_USER)
				{
					    char fulltext[30]; 
						wsprintf(fulltext,"%s (%s)",gObj[n].Account,gObj[n].Name);

						int pos = SendMessage(hWndComboBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>((LPCTSTR)fulltext));
						SendMessage(hWndComboBox, LB_SETITEMDATA, pos, (LPARAM) gObj[n].Account);
				}
			}

			return 1;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg,LOWORD(wParam));
				return 1;
			}

			switch(LOWORD(wParam))
			{
			case IDC_BUTTONDC:

					int itemIndex = (int) SendMessage(hWndComboBox, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
					
				    if (itemIndex == LB_ERR)
					{
						return 0;
					}

					// Getdata
					char* s = (char*)SendMessage(hWndComboBox, LB_GETITEMDATA, itemIndex, 0);

					for(int n = OBJECT_START_USER; n < MAX_OBJECT ; n++)
					{
						if(gObj[n].Connected >= OBJECT_LOGGED && strcmp(gObj[n].Account,s) == 0 )
						{
							LPOBJ lpObj = &gObj[n];

							gObjUserKill(lpObj->Index);

							gCustomAttack.OnAttackAlreadyConnected(lpObj);

							gCustomStore.OnPShopAlreadyConnected(lpObj);

							g_OfflineMode.OnHelperpAlreadyConnected(lpObj);

							CloseClient(lpObj->Index);

							MessageBox(hDlg, "Account disconnect sucefully", "Confirm", MB_OK);
							break;
						}
					}

				return 1;
			}
			break;
	}

	return 0;
}
