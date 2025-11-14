// =============================================
// ChatUnicode_Example.cpp
// Example implementation of Unicode-aware chat system
// Shows how to integrate Unicode support into existing chat code
//
// NOTE: This is EXAMPLE CODE for reference only!
// Do NOT add this file to your project compilation yet.
// Use these examples as a guide when updating your actual chat code.
// =============================================

#include "stdafx.h"
#include "User.h"
#include "Protocol.h"
#include "UnicodeSupport.h"
#include "Message.h"

// Forward declarations for example purposes
void GCSendChatMessage(LPOBJ lpObj, const char* message);
void GCSendServerMessage(int aIndex, const char* message);
void GCSendWhisper(LPOBJ lpSender, LPOBJ lpTarget, const char* message);
void GCNoticeSend(int aIndex, int type, const char* message);

// External variables that should exist in your actual code
extern SQLHDBC g_hOdbcConn;

// =============================================
// EXAMPLE 1: Enhanced Chat Receive Handler
// =============================================

void CGChatRecvUnicode(PMSG_CHAT_RECV* lpMsg, int aIndex)
{
	if (OBJECT_RANGE(aIndex) == 0)
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Connected < OBJECT_ONLINE)
		return;

	// ---- UNICODE ENHANCEMENTS ----

	// 1. Validate UTF-8 encoding
	if (!IsValidUTF8(lpMsg->message))
	{
		// Invalid UTF-8 - possibly a hack attempt or corrupted data
		LogAdd(LOG_RED, "[Chat][%s] Invalid UTF-8 in message", lpObj->Name);
		return;
	}

	// 2. Get character count (not byte count)
	size_t charCount = UTF8Length(lpMsg->message);

	// 3. Check if message is too long (characters, not bytes)
	if (charCount > MAX_CHAT_MESSAGE_SIZE)
	{
		// Truncate at UTF-8 character boundary
		TruncateUTF8(lpMsg->message, MAX_CHAT_MESSAGE_SIZE);
		LogAdd(LOG_BLUE, "[Chat][%s] Message truncated from %d chars", lpObj->Name, charCount);
	}

	// 4. Optional: Filter for allowed characters
	// (This example allows all valid UTF-8, adjust as needed)
	/*
	std::wstring wMessage = UTF8ToWide(lpMsg->message);
	for (size_t i = 0; i < wMessage.length(); i++)
	{
		wchar_t c = wMessage[i];
		
		// Check if character is allowed
		// Add your own validation logic here
		if (!IsAllowedChatCharacter(c))
		{
			// Replace with '?'
			wMessage[i] = L'?';
		}
	}
	std::string filteredMsg = WideToUTF8(wMessage.c_str());
	SafeCopyUTF8(lpMsg->message, sizeof(lpMsg->message), filteredMsg.c_str());
	*/

	// ---- REST OF YOUR EXISTING CHAT CODE ----
	// The message is now validated and safe to use
	// UTF-8 passes through transparently

	// Example: Log chat with UTF-8 support
	LogAdd(LOG_BLACK, "[Chat][%s][%d chars]: %s", lpObj->Name, charCount, lpMsg->message);

	// Example: Send to other players (existing code works as-is)
	// GCSendChatMessage(lpObj, lpMsg->message);
}

// =============================================
// EXAMPLE 2: Whisper Message Handler
// =============================================

void CGWhisperRecvUnicode(PMSG_CHAT_WHISPER_RECV* lpMsg, int aIndex)
{
	if (OBJECT_RANGE(aIndex) == 0)
		return;

	LPOBJ lpObj = &gObj[aIndex];

	// Validate target name (UTF-8)
	if (!IsValidUTF8(lpMsg->name))
	{
		// Invalid target name
		return;
	}

	// Validate message (UTF-8)
	if (!IsValidUTF8(lpMsg->message))
	{
		// Invalid message
		return;
	}

	// Truncate message if needed
	TruncateUTF8(lpMsg->message, MAX_CHAT_MESSAGE_SIZE);

	// Find target player (name search now works with Unicode)
	LPOBJ lpTarget = gObjFind(lpMsg->name);

	if (lpTarget == nullptr)
	{
		// Player not found - send message in UTF-8
		// Example: "Player 'João' not found"
		char errorMsg[100];
		sprintf_s(errorMsg, "Player '%s' not found", lpMsg->name);
		// GCSendServerMessage(aIndex, errorMsg);
		return;
	}

	// Send whisper (existing protocol works with UTF-8)
	// GCSendWhisper(lpObj, lpTarget, lpMsg->message);
}

// =============================================
// EXAMPLE 3: Guild Notice with Unicode
// =============================================

bool SetGuildNoticeUnicode(int guildNumber, const char* notice)
{
	// Validate UTF-8
	if (!IsValidUTF8(notice))
		return false;

	// Check length (60 bytes for Portuguese, ~20 chars for Chinese)
	size_t byteLen = strlen(notice);
	if (byteLen > 60)
	{
		// Too long
		return false;
	}

	// Update in database using Unicode ODBC
	CUnicodeODBC db;
	if (!db.Connect("localhost", "MuOnline"))
		return false;

	char query[256];
	sprintf_s(query,
		"UPDATE Guild SET G_Notice = N'%s' WHERE G_Num = %d",
		notice, guildNumber);

	bool result = db.ExecuteQuery(query);
	db.Disconnect();

	return result;
}

// =============================================
// EXAMPLE 4: Personal Shop Name with Unicode
// =============================================

bool SetPersonalShopNameUnicode(LPOBJ lpObj, const char* shopName)
{
	// Validate UTF-8
	if (!IsValidUTF8(shopName))
	{
		// GCSendServerMessage(lpObj->Index, "Invalid shop name characters");
		return false;
	}

	// Get character count
	size_t charCount = UTF8Length(shopName);

	// Validate length (3-36 characters)
	if (charCount < 3 || charCount > 36)
	{
		// GCSendServerMessage(lpObj->Index, "Shop name must be 3-36 characters");
		return false;
	}

	// Copy safely
	SafeCopyUTF8(lpObj->PShopText, sizeof(lpObj->PShopText), shopName);

	// Example: Save to database
	/*
	CUnicodeODBC db;
	db.Connect("localhost", "MuOnline");
	
	char query[256];
	sprintf_s(query,
		"UPDATE PersonalShop SET ShopName = N'%s' WHERE OwnerName = N'%s'",
		shopName, lpObj->Name);
	
	db.ExecuteQuery(query);
	db.Disconnect();
	*/

	return true;
}

// =============================================
// EXAMPLE 5: Character Name Validation
// =============================================

bool IsValidCharacterNameUnicode(const char* name)
{
	// 1. Check if valid UTF-8
	if (!IsValidUTF8(name))
		return false;

	// 2. Get character count (not byte count)
	size_t charCount = UTF8Length(name);

	// 3. Name must be 3-10 characters
	if (charCount < 3 || charCount > 10)
		return false;

	// 4. Convert to wide string for character validation
	std::wstring wName = UTF8ToWide(name);

	// 5. Check each character
	for (size_t i = 0; i < wName.length(); i++)
	{
		wchar_t c = wName[i];

		// Allow:
		// - ASCII letters (A-Z, a-z)
		// - Numbers (0-9)
		// - Latin Extended (Portuguese, Spanish, French: À-ÿ)
		// - CJK Unified Ideographs (Chinese: 0x4E00-0x9FFF)
		// - Hangul (Korean: 0xAC00-0xD7AF)
		// - Hiragana (Japanese: 0x3040-0x309F)
		// - Katakana (Japanese: 0x30A0-0x30FF)

		bool isValid = false;

		// ASCII alphanumeric
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
			isValid = true;

		// Latin Extended (Portuguese, etc.)
		else if (c >= 0xC0 && c <= 0xFF)
			isValid = true;

		// CJK (Chinese)
		else if (c >= 0x4E00 && c <= 0x9FFF)
			isValid = true;

		// Hangul (Korean)
		else if (c >= 0xAC00 && c <= 0xD7AF)
			isValid = true;

		// Hiragana (Japanese)
		else if (c >= 0x3040 && c <= 0x309F)
			isValid = true;

		// Katakana (Japanese)
		else if (c >= 0x30A0 && c <= 0x30FF)
			isValid = true;

		// Underscore (optional)
		else if (c == '_')
			isValid = true;

		if (!isValid)
		{
			return false; // Invalid character found
		}
	}

	return true;
}

// =============================================
// EXAMPLE 6: Mail System with Unicode
// =============================================

bool SendMailUnicode(const char* senderName, const char* receiverName, 
	const char* subject, const char* message)
{
	// Validate all text fields
	if (!IsValidUTF8(senderName) || !IsValidUTF8(receiverName) ||
		!IsValidUTF8(subject) || !IsValidUTF8(message))
	{
		return false;
	}

	// Check lengths
	if (UTF8Length(subject) > 60 || UTF8Length(message) > 1000)
	{
		return false;
	}

	// Insert into database
	CUnicodeODBC db;
	if (!db.Connect("localhost", "MuOnline"))
		return false;

	char query[2048];
	sprintf_s(query,
		"INSERT INTO MemoData (Name, Subject, Memo, sName) VALUES (N'%s', N'%s', N'%s', N'%s')",
		receiverName, subject, message, senderName);

	bool result = db.ExecuteQuery(query);
	db.Disconnect();

	return result;
}

// =============================================
// EXAMPLE 7: Chat Filter for Bad Words (Unicode-aware)
// =============================================

bool ContainsBadWords(const char* message, const std::vector<std::string>& badWords)
{
	// Convert message to lowercase wide string for case-insensitive comparison
	std::wstring wMessage = UTF8ToWide(message);
	
	// Convert to lowercase
	for (size_t i = 0; i < wMessage.length(); i++)
	{
		wMessage[i] = towlower(wMessage[i]);
	}

	// Check each bad word
	for (const auto& badWord : badWords)
	{
		std::wstring wBadWord = UTF8ToWide(badWord.c_str());
		
		// Convert to lowercase
		for (size_t i = 0; i < wBadWord.length(); i++)
		{
			wBadWord[i] = towlower(wBadWord[i]);
		}

		// Search for bad word in message
		if (wMessage.find(wBadWord) != std::wstring::npos)
		{
			return true; // Bad word found
		}
	}

	return false; // Clean message
}

// =============================================
// EXAMPLE 8: Format Chat Message with Colors
// =============================================

void FormatChatMessageUnicode(char* output, size_t outputSize, 
	const char* playerName, const char* message)
{
	// Example: [João]: Olá mundo!
	// Safely format with UTF-8 strings

	// Calculate required size
	size_t nameLen = strlen(playerName);
	size_t msgLen = strlen(message);
	size_t totalLen = nameLen + msgLen + 5; // "[]: " + null

	if (totalLen >= outputSize)
	{
		// Truncate message if needed
		char tempMsg[256];
		SafeCopyUTF8(tempMsg, sizeof(tempMsg), message);
		TruncateUTF8(tempMsg, outputSize - nameLen - 10);
		
		sprintf_s(output, outputSize, "[%s]: %s", playerName, tempMsg);
	}
	else
	{
		sprintf_s(output, outputSize, "[%s]: %s", playerName, message);
	}
}

// =============================================
// EXAMPLE 9: Global Announcement with Unicode
// =============================================

void SendGlobalAnnouncementUnicode(const char* message)
{
	// Validate message
	if (!IsValidUTF8(message))
		return;

	// Truncate if needed
	char safeMsg[MAX_MESSAGE_SIZE + 1];
	SafeCopyUTF8(safeMsg, sizeof(safeMsg), message);
	TruncateUTF8(safeMsg, MAX_MESSAGE_SIZE);

	// Send to all online players
	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		if (gObj[i].Connected == OBJECT_ONLINE)
		{
			// Use existing notice system - UTF-8 passes through
			// GCNoticeSend(i, 0, safeMsg);
		}
	}

	// Log announcement with UTF-8
	LogAdd(LOG_BLACK, "[Announcement]: %s", safeMsg);
}

// =============================================
// EXAMPLE 10: Helper Function - Safe String Copy
// =============================================

void SafeChatStringCopy(char* dest, size_t destSize, const char* src, const char* fieldName)
{
	if (!IsValidUTF8(src))
	{
		LogAdd(LOG_RED, "[Chat] Invalid UTF-8 in %s field", fieldName);
		dest[0] = '\0';
		return;
	}

	SafeCopyUTF8(dest, destSize, src);
	TruncateUTF8(dest, destSize - 1);

	size_t charCount = UTF8Length(dest);
	LogAdd(LOG_BLACK, "[Chat] %s: %d chars, %d bytes", fieldName, charCount, strlen(dest));
}

// =============================================
// INTEGRATION NOTES
// =============================================

/*
To integrate Unicode chat support into your existing GameServer:

1. Add validation to all incoming chat messages:
   - Use IsValidUTF8() to check encoding
   - Use UTF8Length() for character count
   - Use TruncateUTF8() for safe truncation

2. Update database queries to use Unicode:
   - Use CUnicodeODBC wrapper class
   - Prefix string literals with N'...'
   - Example: N'João' instead of 'João'

3. Character name validation:
   - Use IsValidCharacterNameUnicode()
   - Allows Portuguese, Chinese, Japanese, Korean
   - Customize character ranges as needed

4. Existing protocol structures work as-is:
   - PMSG_CHAT_RECV already uses char arrays
   - UTF-8 is transparent to existing code
   - No protocol changes needed!

5. File logging:
   - UTF-8 log files work with most viewers
   - Use UTF-8 file functions for notices
   - Save configuration files as UTF-8

6. Testing checklist:
   - Portuguese: "João fala: Ação!"
   - Spanish: "José dice: ¡Mañana!"
   - Mixed: "Test ação mixed テスト"
   - Long names: 10 characters max
   - Long messages: 60 bytes max

7. Performance notes:
   - UTF-8 validation is very fast
   - Character counting is O(n) but n is small
   - No noticeable performance impact

8. Backward compatibility:
   - All ASCII text still works
   - Existing Spanish/English unchanged
   - New languages just work!
*/
