# Bot Phrases System - Multi-Language Support

## 📖 Overview

The GameServer now loads bot phrases from external text files, allowing you to customize what your bots say in different languages and situations!

## 📁 Directory Structure

```
IA/Phrases/
├── Spanish/
│   └── BotPhrases.txt
├── English/
│   └── BotPhrases.txt
├── Chinese/
│   └── BotPhrases.txt
├── Japanese/
│   └── BotPhrases.txt
├── German/
│   └── BotPhrases.txt
└── [Your Custom Language]/
    └── BotPhrases.txt
```

## 🎯 Phrase Categories

### **#GENERAL** - General random phrases
Bots say these randomly while playing
- Example: "Good day everyone", "Training hard"
- Probability: Set after category (e.g., `#GENERAL, 5` = 5% chance)

### **#NEAR_REAL_PLAYER** - When near real players
Bots say these when a real player is nearby
- Example: "Hello friend!", "Nice to see you"
- Higher probability to feel more interactive

### **#IN_PARTY** - When in a party
Bots say these when they're in a party
- Example: "Excellent party!", "Let's go team!"
- Creates team spirit

### **#PVP** - During PvP combat
Bots say these during active PvP fights
- Example: "Good fight!", "You're strong!"
- Makes PvP more immersive

### **#TRADE** - When trading
Bots say these when trade mode is active
- Example: "What do you need?", "Check what I have"
- Makes bot traders feel real

### **#MORNING** - Morning hours (6:00-11:59)
Bots say these during morning game time
- Example: "Good morning!", "Perfect morning for training"

### **#AFTERNOON** - Afternoon hours (12:00-17:59)
Bots say these during afternoon game time
- Example: "Good afternoon", "Productive afternoon"

### **#NIGHT** - Night hours (18:00-5:59)
Bots say these during night game time
- Example: "Good evening", "Night of adventures"

### **#MAP_SPECIFIC** - Specific map phrases
Bots say these when on specific maps
- Add `, MAP_NUMBER` after the category
- Example: `#MAP_SPECIFIC, 0` for Lorencia

## 📝 File Format

### Basic Structure:
```txt
; Comment line (ignored)

#CATEGORY, probability
phrase line 1
phrase line 2
phrase line 3

#ANOTHER_CATEGORY, probability
more phrases here
```

### Example:
```txt
; Spanish bot phrases
; Lines starting with ; are comments

#GENERAL, 5
Buen día a todos
Hola comunidad
¿Alguien por aquí?

#NEAR_REAL_PLAYER, 15
Hola amigo!
¿Qué tal?
Saludos!
```

## 🎨 Customization Tips

### 1. **Adjust Probabilities**
Higher number = more frequent
```txt
#GENERAL, 3        → Rare (3% chance)
#NEAR_PLAYER, 20   → Common (20% chance)
#PVP, 30           → Very common (30% chance)
```

### 2. **Add Personality**
Create unique bot personalities:
```txt
; Friendly bot
#GENERAL, 10
I love helping people!
Friends make the game fun!

; Serious warrior bot
#GENERAL, 10
Training is life
Focus on victory
```

### 3. **Regional Variations**
Create Spanish-Mexico, Spanish-Argentina, etc.
```txt
IA/Phrases/Spanish-Mexico/BotPhrases.txt
IA/Phrases/Spanish-Argentina/BotPhrases.txt
```

### 4. **Event-Based Phrases**
Create seasonal phrase sets:
```txt
IA/Phrases/Christmas/BotPhrases.txt
IA/Phrases/Halloween/BotPhrases.txt
```

## 🌍 Supported Languages (Included)

- **Spanish** - Complete set with 40+ phrases
- **English** - Complete set with 40+ phrases  
- **Chinese** - Template with key phrases
- **Japanese** - Romaji template
- **German** - Complete German phrases

## 🚀 How to Use

### **Option 1: Use Language Dropdown**
1. Open "Create Multiple Bots" dialog
2. Select language from "Bot Names Language" dropdown
3. Both names AND phrases change automatically!
4. Create bots and they'll speak that language

### **Option 2: Edit Files Manually**
1. Navigate to `IA/Phrases/[Language]/`
2. Edit `BotPhrases.txt` with your favorite text editor
3. Save the file
4. Restart GameServer (or reload IA)
5. Bots will use new phrases

## 📚 Advanced Features

### **Placeholders** (if supported in your code):
- `{player_name}` - Player's name
- `{level}` - Bot's level
- `{map}` - Current map name
- `{class}` - Bot's class

Example:
```txt
#GENERAL, 5
I'm level {level} now!
{map} is a great place
```

### **Map-Specific Phrases**:
```txt
#MAP_SPECIFIC, 0
Lorencia is my home
Beautiful Lorencia!

#MAP_SPECIFIC, 2
Devias has tough monsters
Training in Devias
```

### **Class-Specific** (if enabled):
```txt
#CLASS_SPECIFIC, 0    ; Dark Wizard
Magic is my power
Casting spells all day

#CLASS_SPECIFIC, 16   ; Dark Knight
Sword and shield!
Warrior's way
```

## 💡 Creative Ideas

### **Role-Playing Bots**
```txt
; Knight bot
#GENERAL, 10
Honor above all
Protecting the weak
Justice prevails

; Merchant bot
#GENERAL, 10
Best prices here!
Looking for deals?
Business is good
```

### **Helpful Bots**
```txt
#GENERAL, 10
Need help? Just ask
I know good spots
Happy to assist
```

### **Silent Bots**
```txt
; Keep all categories but with low probability
#GENERAL, 1
...
Focused.

#NEAR_PLAYER, 2
*nods*
```

### **Chatty Bots**
```txt
; High probability on all categories
#GENERAL, 25
#NEAR_PLAYER, 40
#IN_PARTY, 50
```

## 🛠️ Troubleshooting

### **Bots not speaking?**
- Check file exists: `IA/Phrases/[Language]/BotPhrases.txt`
- Check file format (one phrase per line)
- Check probability values (not 0)
- Check console for errors: `[PhraseManager]` messages

### **Wrong language phrases?**
- Check language selected in dropdown
- Check console: `[Language] Changed to: [Language]`
- Restart server if needed

### **UTF-8 characters not showing?**
- Save file as UTF-8 encoding
- Use Notepad++ or VS Code
- Don't use Windows Notepad (uses ANSI)

### **Phrases too frequent/rare?**
- Adjust probability numbers in category headers
- Higher = more frequent
- Lower = more rare

## 📊 Testing

### **Test phrases in-game:**
1. Create bots with specific language
2. Watch console for phrase loading messages
3. Watch game chat for bot phrases
4. Near a bot to trigger #NEAR_PLAYER phrases
5. Party with bot to trigger #IN_PARTY phrases
6. Attack bot to trigger #PVP phrases

### **Log messages to watch:**
```
[PhraseManager] Loaded language: English (Phrases: OK, Answers: OK)
[Language] Changed to: Spanish (Names: OK, Phrases: OK)
[Language] Bot phrases and answers reloaded
```

## 🎉 Enjoy Your Multi-Language Bots!

Your bots can now speak any language you want!

---

**Related:** See `IA/Answers/README.md` for keyword response system

**Developed by**: MU ALFA Team  
**Version**: 1.0  
**Date**: 2025-10-24
