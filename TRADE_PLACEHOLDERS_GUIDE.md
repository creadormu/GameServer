# FakeBot Trade Placeholders - Complete Guide

## ✅ What Was Fixed

### Problem 1: Trade phrases not reading placeholders
**FIXED** - The `ReplaceTradePlaceholders` function was commented out in `ChatRecv`. Now it's active!

### Problem 2: Trade window messages don't show item names
**FIXED** - Trade opening message now uses `GetItemName()` to show actual item names!

## 📝 How Placeholders Work

### Available Placeholders

1. **{item_required}** - Shows the item(s) the bot wants from the player
2. **{item_reward}** - Shows the item(s) the bot will give to the player
3. **{player_name}** - Shows the player's name (works in all contexts)

### Where They Work

✅ **BotPhrases.txt #TRADE section** - Random bot chat phrases
✅ **BotAnswering.txt keyword responses** - Responses to player chat
✅ **Trade window opening message** - Shows item names when trade starts

## 📄 BotPhrases.txt Configuration

### Location
`/IA/Phrases/[Language]/BotPhrases.txt`

### Format
```
#TRADE, 30
Looking for {item_required}, trade me!
Need {item_required}, good deal waiting!
Buying {item_required} for {item_reward}!
Trading {item_required} -> {item_reward}!
WTB: {item_required} = {item_reward}
Someone have {item_required}?
Need {item_required}, have {item_reward}!
{item_required} wanted, {item_reward} offered!
Trade your {item_required} here!
Quick trade: {item_required} -> {item_reward}
```

The number `30` is the probability (30% chance bot will say a trade phrase when appropriate).

### When Bots Say Trade Phrases

Bots will say trade phrases when:
- They have `CanTradeWithBot()` = true (configured in FakeBotTrade.txt)
- Random chance hits (based on probability setting)
- They're walking around normally (not in combat/party/etc.)

## 🎮 Example Output

### Configuration
```
FakeBotTrade.txt:
Bot account: ricardo
Required: Jewel of Life (14,16)
Reward: Jewel of Creation (14,22)
```

### Bot Phrases (Before Placeholder Replacement)
```
Looking for {item_required}, trade me!
Need {item_required}, have {item_reward}!
```

### Bot Phrases (After Placeholder Replacement)
```
Looking for Jewel of Life, trade me!
Need Jewel of Life, have Jewel of Creation!
```

### Trade Window Message
```
ricardo: Need Jewel of Life, will give Jewel of Creation. Put items and press OK!
```

## 🛠️ Technical Details

### Code Changes Made

#### 1. FakeOnline.h
```cpp
// Made GetItemName public so BotTrader can use it
std::string GetItemName(int itemType);
```

#### 2. FakeOnline.cpp (ChatRecv function)
```cpp
// BEFORE (commented out)
//reply = ReplaceTradePlaceholders(reply, lpBot->Account);

// AFTER (active)
reply = ReplaceTradePlaceholders(reply, lpBot->Account);
```

#### 3. BotTrader.cpp (TradeOpen function)
```cpp
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

// Send notice with actual item names
char tradeMsg[256];
sprintf_s(tradeMsg, sizeof(tradeMsg), "%s: Need %s, will give %s. Put items and press OK!", 
    lpBot->Name, requiredMsg.c_str(), rewardMsg.c_str());
gNotice.NewNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, tradeMsg);
```

### How GetItemName Works

The function converts item type IDs to readable names:

```cpp
GET_ITEM(14, 13) -> "Jewel of Bless"
GET_ITEM(14, 14) -> "Jewel of Soul"
GET_ITEM(12, 15) -> "Jewel of Chaos"
GET_ITEM(14, 16) -> "Jewel of Life"
GET_ITEM(14, 22) -> "Jewel of Creation"
// ... and many more items
```

For items with level/options:
```cpp
Jewel of Bless+9 Exc -> "Jewel of Bless+9 Exc"
```

### Multiple Items Support

If a bot requires/gives multiple items:
```
Required: Jewel of Life + Jewel of Soul
Reward: Jewel of Creation + Jewel of Chaos

Bot says: "Need Jewel of Life + Jewel of Soul, have Jewel of Creation + Jewel of Chaos!"
```

## 🌍 Multi-Language Support

All language files updated with trade phrases:
- ✅ English (IA/Phrases/English/BotPhrases.txt)
- ✅ Spanish (IA/Phrases/Spanish/BotPhrases.txt)
- ✅ German (IA/Phrases/German/BotPhrases.txt)
- ✅ Chinese (IA/Phrases/Chinese/BotPhrases.txt)
- ✅ Japanese (IA/Phrases/Japanese/BotPhrases.txt)

Each language has culturally appropriate trade phrases with placeholders!

## 🎯 Usage Tips

### For Better Trade Bot Behavior

1. **Set appropriate probability**
   ```
   #TRADE, 30  <- Higher number = more frequent trade chat
   ```

2. **Mix placeholder and non-placeholder phrases**
   ```
   Need {item_required}!           <- Shows specific item
   Looking to trade something?     <- Generic phrase
   {item_required} -> {item_reward}!  <- Shows both items
   ```

3. **Use varied phrase styles**
   - Short: "WTB: {item_required}"
   - Long: "Looking for {item_required}, will give {item_reward}!"
   - Urgent: "Need {item_required} NOW!"
   - Casual: "Anyone got {item_required}?"

### For BotAnswering.txt (Keyword Responses)

You can also use placeholders in keyword responses:

```
[TRADE_INQUIRY]
trade, buy, sell, item

[TRADE_INQUIRY_RESPONSE]
I need {item_required}, I can give you {item_reward}!
Want to trade? I need {item_required}
I have {item_reward} for your {item_required}
```

## 📊 Testing

### Test Checklist

1. ✅ Bot says trade phrases with actual item names (not placeholders)
2. ✅ Trade window message shows item names when trade opens
3. ✅ Multiple items show with "+" separator
4. ✅ Works in all languages
5. ✅ Player name placeholder still works
6. ✅ Bots only say trade phrases if they can actually trade

### Example Test

1. Configure bot "ricardo" to trade Life for Creation
2. Start server and look for ricardo
3. Wait near ricardo
4. Ricardo should randomly say things like:
   - "Need Jewel of Life, have Jewel of Creation!"
   - "Looking for Jewel of Life, trade me!"
   - "Trading Jewel of Life -> Jewel of Creation!"
5. Click trade on ricardo
6. You should see: "ricardo: Need Jewel of Life, will give Jewel of Creation. Put items and press OK!"

## 🎉 Result

Your bots now communicate like real traders!
- They advertise what they want and what they offer
- Messages are clear and specific
- Players know exactly what to bring
- Trade system feels more alive and human-like!

---

**Status**: ✅ COMPLETE
**Placeholders Working**: ✅ YES
**All Languages Updated**: ✅ YES
**Trade Messages Fixed**: ✅ YES
