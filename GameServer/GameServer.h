#pragma once

#define MAX_LOADSTRING 100

#define WM_TIMER_1000 100
#define WM_TIMER_2000 101
#define WM_TIMER_10000 102

#define WM_JOIN_SERVER_MSG_PROC (WM_USER+1)
#define WM_DATA_SERVER_MSG_PROC (WM_USER+2)

// Function declarations
INT_PTR CALLBACK CreateBotsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ConfigClassDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
bool CreateMultipleBotsAdvanced(int botCount, int startFrom, int gateNumber, int mapNumber, int mapX, int mapY,
int minLevel, int maxLevel, int selectedClass, int phamViTrain, int moveRange, int timeReturn,
int tuNhatItem, int tuDongReset, int partyMode, int pvpMode, int postKhiDie, int enabledConfigs, int botStayCity, int timeForCity);
bool ExecuteSQLFile(const char* sqlFilePath, const char* serverName, const char* databaseName, char* errorMsg, int errorMsgSize);
bool UpdateAccountsXML(bool replaceMode, char* errorMsg, int errorMsgSize);
bool TestSQLConnection(const char* serverName, std::vector<std::string>& databases, char* errorMsg, int errorMsgSize);

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance,int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK About(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK UserOnline(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK IPBanned(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

