## Bot Answers System - Multi-Language Keyword Responses

## 📖 Overview

The Answering system allows bots to respond to keywords in chat. When players say specific words, bots will reply with context-appropriate responses!

## 📁 Directory Structure

```
IA/Answers/
├── Spanish/
│   └── Answering.txt
├── English/
│   └── Answering.txt
├── Chinese/
│   └── Answering.txt
├── Japanese/
│   └── Answering.txt
├── German/
│   └── Answering.txt
└── [Your Custom Language]/
    └── Answering.txt
```

## 🎯 How It Works

### **Keyword Triggers** → **Bot Responses**

1. Player says something in chat
2. Bot checks if message contains any keywords
3. Bot responds with a random response from that category
4. Creates natural conversation!

## 📝 File Format

```txt
; Comments start with semicolon

#CATEGORY_NAME
keyword1
keyword2
keyword3
>>>
response1
response2
response3

#ANOTHER_CATEGORY
more keywords
>>>
more responses
```

### **>>>** separator
The `>>>` line separates keywords (above) from responses (below)

## 💬 Example Categories

### **#GREETING** - Greet players
**Keywords**: hello, hi, hey, greetings  
**Responses**: "Hello! How are you?", "Hey! What's up?"

**In action:**
```
Player: "hello everyone"
Bot: "Hello! How are you?"
```

### **#PARTY** - Party requests
**Keywords**: party, pt, group, team  
**Responses**: "Sure, send me an invite!", "Yes, let's go!"

**In action:**
```
Player: "anyone want party?"
Bot: "Sure, send me an invite!"
```

### **#TRADE** - Trading
**Keywords**: trade, sell, buy, exchange  
**Responses**: "What are you looking for?", "Check my inventory"

**In action:**
```
Player: "selling items"
Bot: "What do you offer?"
```

### **#HELP** - Help requests
**Keywords**: help, assist, aid  
**Responses**: "How can I help?", "Tell me what you need"

**In action:**
```
Player: "I need help"
Bot: "How can I help?"
```

### **#GOODBYE** - Farewells
**Keywords**: goodbye, bye, see you, later  
**Responses**: "See you later!", "Bye! Take care"

**In action:**
```
Player: "goodbye everyone"
Bot: "See you later!"
```

### **#THANKS** - Thank you
**Keywords**: thanks, thx, ty, thank you  
**Responses**: "You're welcome!", "My pleasure!"

**In action:**
```
Player: "thanks for the help"
Bot: "You're welcome!"
```

### **#LEVEL** - Level questions
**Keywords**: level, lvl  
**Responses**: "I'm level {level}", "Leveling up"

**In action:**
```
Player: "what level are you?"
Bot: "I'm level 350"
```

### **#LOCATION** - Location questions
**Keywords**: where, location, map  
**Responses**: "I'm at {map}", "Around here"

**In action:**
```
Player: "where are you?"
Bot: "I'm at Lorencia"
```

## 🎨 Customization Examples

### **1. Friendly Bot**
```txt
#GREETING
hi
hello
>>>
Hi friend! 😊
Hello! Great to see you!
Hey there buddy!
How's your day going?

#HELP
help
>>>
I'm always here to help!
What can I do for you?
Let me assist you!
```

### **2. Professional Merchant Bot**
```txt
#GREETING
hello
>>>
Welcome to my shop
Greetings, customer
How may I serve you?

#TRADE
trade
buy
sell
>>>
Here are my items
Best prices guaranteed
What interests you today?
Premium quality items

#GOODBYE
bye
>>>
Thank you for your business
Come again soon
Pleasure doing business
```

### **3. Warrior Bot**
```txt
#GREETING
hello
>>>
Greetings warrior
Ready for battle?
Let's fight!

#PARTY
party
>>>
Form up! Let's raid!
Warriors unite!
Battle calls!

#PVP
fight
duel
>>>
Bring it on!
I accept your challenge!
Let's test our skills!
```

### **4. Helper Bot**
```txt
#HELP
help
lost
where
>>>
Need directions?
I can show you around
What do you need to know?
Ask me anything!

#LOCATION
map
where
>>>
I know these lands well
Follow me if needed
This is {map}
```

## 💡 Advanced Tips

### **Multiple Keywords Per Category**
```txt
#SALUDO
hola
hey
buenas
hi
hello
>>>
¡Hola! ¿Qué tal?
Hey! What's up?
Buenas!
```

### **Case-Insensitive Matching**
Keywords match regardless of case:
- "HELLO" = "hello" = "Hello"

### **Partial Word Matching**
If keyword is "hello":
- "hello there" ✅ Matches
- "hellooo" ✅ Matches
- "say hello to" ✅ Matches

### **Multiple Categories Can Match**
Bot checks ALL categories and can respond to multiple:
```txt
Player: "hello, want to party?"
Bot might respond to #GREETING or #PARTY (or both!)
```

### **Random Response Selection**
Bot picks ONE random response from matching category:
```txt
#GREETING
hi
>>>
Hello!
Hi there!
Hey!
What's up?

; Bot will say ONE of these randomly
```

## 🌍 Language-Specific Features

### **Spanish**
```txt
#SALUDO
hola
buenas
hey
>>>
¡Hola! ¿Cómo estás?
Buenas! ¿Qué tal?
Hey! ¿Qué hay?
```

### **English**
```txt
#GREETING
hello
hi
hey
sup
>>>
Hello! How are you?
Hi there!
Hey! What's going on?
```

### **Chinese**
```txt
#GREETING
你好
嗨
>>>
你好！你好吗？
嗨！怎么样？
```

### **Japanese**
```txt
#GREETING
konnichiwa
ohayo
>>>
Konnichiwa! Genki?
Ohayo gozaimasu!
```

## 🔧 Placeholders (If Supported)

Some variables can be used in responses:

### **{player_name}** - Player who triggered
```txt
#GREETING
hello
>>>
Hello {player_name}!
Nice to see you, {player_name}!
```

### **{level}** - Bot's level
```txt
#LEVEL
level
>>>
I'm level {level}
Currently {level}
```

### **{map}** - Current map
```txt
#LOCATION
where
>>>
I'm at {map}
Currently in {map}
```

### **{class}** - Bot's class
```txt
#CLASS
what class
>>>
I'm a {class}
{class} warrior
```

## 🛠️ Troubleshooting

### **Bot not responding?**
✓ Check file exists: `IA/Answers/[Language]/Answering.txt`  
✓ Check `>>>` separator is present  
✓ Check keywords don't have typos  
✓ Check player's message contains the keyword  
✓ Check console for `[PhraseManager]` errors

### **Wrong language responses?**
✓ Check language selected in dropdown  
✓ Check console: `[Language] Changed to: [Language]`  
✓ Restart or reload IA

### **Bot responding to everything?**
✓ Keywords are too generic ("a", "the", etc.)  
✓ Use more specific keywords  
✓ Add cooldowns if available

### **Bot not responding to specific words?**
✓ Add the word to keywords list  
✓ Check spelling matches exactly  
✓ Test with simpler keywords first

## 📊 Testing In-Game

### **Basic Test:**
1. Create bots with your language
2. Check console for: `[PhraseManager] Found Answering.txt`
3. Say "hello" near a bot
4. Bot should respond from #GREETING category

### **Advanced Test:**
1. Test each category: greeting, party, trade, etc.
2. Try different keyword variations
3. Check if responses are random
4. Test with multiple bots (should all respond differently)

## 🎯 Best Practices

### ✅ DO:
- Use common, natural keywords
- Add multiple keyword variations
- Write varied, natural responses
- Test thoroughly before deploying
- Use appropriate language for audience

### ❌ DON'T:
- Use single-letter keywords ("a", "I")
- Use offensive language
- Make responses too long (spam)
- Use too many placeholders
- Forget the `>>>` separator

## 🎨 Creative Categories

### **Roleplay Responses**
```txt
#ROLEPLAY_WARRIOR
warrior
knight
battle
>>>
*raises sword*
For honor and glory!
*battle stance*
```

### **Funny Responses**
```txt
#JOKES
joke
funny
lol
>>>
😄 Want to hear a joke?
I'm not just a bot, I'm THE bot!
*tells joke in {language}*
```

### **Quest Helper**
```txt
#QUEST
quest
mission
task
>>>
Need quest help?
I know all the quests here
What quest are you on?
```

### **Market Info**
```txt
#PRICES
price
cost
how much
>>>
Prices vary daily
Check the market
I can help with pricing
```

## 📚 Integration with Phrases

The Answer system works together with BotPhrases:
- **BotPhrases** = Bots speak randomly on their own
- **Answering** = Bots respond to player chat

Both use the same language selection!

## 🎉 Make Your Bots Come Alive!

With proper answers, your bots will feel like real players!

---

**Related:** See `IA/Phrases/README.md` for bot phrases system

**Developed by**: MU ALFA Team  
**Version**: 1.0  
**Date**: 2025-10-24
