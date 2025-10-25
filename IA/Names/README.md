# Bot Name System - Multi-Language Support

## 📖 Overview

The GameServer now loads bot names from external text files, allowing you to easily switch between different languages and cultures for your bot names!

## 📁 Directory Structure

```
IA/Names/
├── Spanish/
│   ├── MaleNames.txt
│   └── FemaleNames.txt
├── English/
│   ├── MaleNames.txt
│   └── FemaleNames.txt
├── Chinese/
│   ├── MaleNames.txt
│   └── FemaleNames.txt
├── Japanese/
│   ├── MaleNames.txt
│   └── FemaleNames.txt
├── German/
│   ├── MaleNames.txt
│   └── FemaleNames.txt
└── [Your Custom Language]/
    ├── MaleNames.txt
    └── FemaleNames.txt
```

## 🚀 How to Use

### 1. **Select Language in Create Bots Dialog**
   - Open the "Create Bots" dialog in GameServer
   - Find the "Language" dropdown
   - Select your preferred language (Spanish, English, Chinese, Japanese, German, etc.)
   - The names will be automatically loaded when you create bots

### 2. **Create Your Own Language Pack**

Create a new folder inside `IA/Names/` with your language name:

```
IA/Names/Korean/
├── MaleNames.txt
└── FemaleNames.txt
```

### 3. **Edit Name Files**

Each name file is a simple text file with **one name per line**:

```txt
; Comments start with semicolon (;) or hash (#)
; One name per line

Carlos
Diego
Miguel
Juan
Pedro
Luis
```

**Important Rules:**
- One name per line
- Maximum 31 characters per name
- Lines starting with `;` or `#` are comments (ignored)
- Empty lines are ignored
- UTF-8 encoding supported (for Chinese, Japanese, etc.)

### 4. **Name Format**

Bot names are generated as: `[Name][Number]`

Examples:
- `Carlos23`
- `Maria45`
- `Hiroshi87`

The number is randomly generated between 10-99.

## 🎯 Features

✅ **Unlimited Names** - Add as many names as you want  
✅ **Easy to Edit** - Simple text files, edit with any text editor  
✅ **Multi-Language** - Full UTF-8 support for any language  
✅ **Hot Reload** - Change language without restarting the server  
✅ **Fallback** - If files are missing, uses built-in Spanish names  
✅ **Gender Support** - Separate male/female names (ELF and SUM use female names)

## 📝 Example: Creating a Portuguese Name Pack

1. Create folder: `IA/Names/Portuguese/`
2. Create `MaleNames.txt`:
```txt
; Portuguese Male Names
Joao
Pedro
Lucas
Gabriel
Rafael
Miguel
Tiago
```

3. Create `FemaleNames.txt`:
```txt
; Portuguese Female Names
Maria
Ana
Beatriz
Carolina
Juliana
Mariana
Sofia
```

4. Restart GameServer
5. Select "Portuguese" in the Create Bots dialog

## 🌍 Supported Languages (Included)

- **Spanish** - Names from Spanish-speaking countries
- **English** - Common English names
- **Chinese** - Pinyin romanization of Chinese names
- **Japanese** - Romaji romanization of Japanese names
- **German** - German names

## 🛠️ Technical Details

- **File Encoding**: UTF-8 (for special characters)
- **Max Name Length**: 31 characters (database limit)
- **Default Language**: Spanish (if no files found)
- **Configuration Path**: `IA/Names/[Language]/`
- **Auto-Detection**: Server automatically scans available languages on startup

## 🐛 Troubleshooting

### Problem: "No language folders found"
**Solution**: Make sure `IA/Names/` directory exists and contains language folders

### Problem: "Failed to load language"
**Solution**: Check that both `MaleNames.txt` and `FemaleNames.txt` exist in the language folder

### Problem: Names appear as "Bot" only
**Solution**: Name files are empty or have incorrect format. Check one name per line.

### Problem: Chinese/Japanese names show as ???
**Solution**: Save the name files with UTF-8 encoding

## 💡 Tips

1. **Mix Languages**: Create a "Mixed" folder with names from different cultures
2. **Theme Names**: Create themed name packs (Fantasy, SciFi, Medieval, etc.)
3. **Regional Names**: Create specific regional variations (Mexican, Argentinian, etc.)
4. **Test Names**: Add a few test names first, then expand your list
5. **Community Names**: Share your name packs with the community!

## 📚 Resources

- More name ideas: [behindthename.com](https://www.behindthename.com/)
- Chinese names: [chinesenames.org](https://chinesenames.org/)
- Japanese names: [jisho.org](https://jisho.org/)

## 🎉 Enjoy Your Multi-Language Bot System!

Now you can create bots with authentic names from any culture around the world!

---

**Developed by**: MU ALFA Team  
**Version**: 1.0  
**Date**: 2025-10-24
