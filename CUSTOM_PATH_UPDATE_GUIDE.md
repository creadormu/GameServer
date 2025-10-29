# 🎯 Actualizar Archivos en Rutas Personalizadas

## ✅ Problema Resuelto!

Ahora puedes actualizar archivos en **cualquier ruta**, no solo en `GameServer\Data\`!

---

## 🔧 Nueva Característica: TargetPath

He agregado un nuevo campo **`TargetPath`** al sistema de actualizaciones.

---

## 📝 Cómo Usarlo

### Ejemplo 1: Actualizar Item.txt en MuServer/Data/Item/

**En tu `update_manifest.txt`:**

```ini
[Update]
Version=1.0.3
DownloadUrl=https://creadormu.com/mualfa/updates/Item_1.0.3.txt
FileName=Item.txt
FileHash=
FileSize=515809
FileType=1
TargetPath=..\Data\Item\Item.txt
Description=Updated items database
Required=1
```

**Resultado:** El archivo se guardará en `MuServer\Data\Item\Item.txt` ✅

---

### Ejemplo 2: Actualizar Monster.txt

```ini
[Update]
Version=1.0.4
DownloadUrl=https://creadormu.com/mualfa/updates/Monster_1.0.4.txt
FileName=Monster.txt
FileSize=245600
FileType=1
TargetPath=..\Data\Monster\Monster.txt
Description=Monster balance changes
Required=0
```

**Resultado:** El archivo se guardará en `MuServer\Data\Monster\Monster.txt` ✅

---

### Ejemplo 3: Actualizar Archivo en Carpeta Común

```ini
[Update]
Version=1.0.5
DownloadUrl=https://creadormu.com/mualfa/updates/CommonServer.cfg
FileName=CommonServer.cfg
FileSize=12400
FileType=2
TargetPath=..\CommonServer.cfg
Description=Updated common server config
Required=0
```

**Resultado:** El archivo se guardará en `MuServer\CommonServer.cfg` ✅

---

## 🎯 Entendiendo las Rutas

### Tu Estructura de Carpetas:

```
MuServer/
├── GameServer/
│   ├── GameServer.exe          ← Aquí se ejecuta el update
│   └── Data/
│       └── UpdateConfig.ini
├── Data/
│   ├── Item/
│   │   └── Item.txt            ← Quieres actualizar este
│   └── Monster/
│       └── Monster.txt
└── ConnectServer/
```

### GameServer.exe está en: `MuServer\GameServer\`
### Quieres actualizar: `MuServer\Data\Item\Item.txt`

**Solución:** Usa ruta relativa `..` para subir un nivel:

```
..              = MuServer (sube 1 nivel desde GameServer)
..\Data         = MuServer\Data
..\Data\Item    = MuServer\Data\Item
..\Data\Item\Item.txt = MuServer\Data\Item\Item.txt ✅
```

---

## 📋 Ejemplos de Rutas

| Objetivo | TargetPath |
|----------|------------|
| `MuServer\Data\Item\Item.txt` | `..\Data\Item\Item.txt` |
| `MuServer\Data\Monster\Monster.txt` | `..\Data\Monster\Monster.txt` |
| `MuServer\Data\Skill\Skill.txt` | `..\Data\Skill\Skill.txt` |
| `MuServer\CommonServer.cfg` | `..\CommonServer.cfg` |
| `MuServer\ConnectServer\Data\ServerList.txt` | `..\ConnectServer\Data\ServerList.txt` |
| `GameServer\Data\Local\Item.txt` | `.\Data\Local\Item.txt` |

**Regla Simple:**
- `..` = Sube un nivel
- `.` = Carpeta actual
- `\` = Separador de carpetas

---

## 🔄 Compatibilidad con Sistema Anterior

### Si NO especificas TargetPath:

```ini
[Update]
FileName=Item.txt
FileType=1
# NO tiene TargetPath
```

**Por defecto irá a:** `GameServer\Data\Item.txt` (comportamiento original)

### Si especificas TargetPath:

```ini
[Update]
FileName=Item.txt
FileType=1
TargetPath=..\Data\Item\Item.txt
```

**Irá a:** La ruta que especifiques ✅

---

## 🎓 Casos de Uso Comunes

### Caso 1: Actualizar Todos los Item.txt de Múltiples GameServers

```
MuServer/
├── GameServer1/
│   └── GameServer.exe
├── GameServer2/
│   └── GameServer.exe
└── Data/
    └── Item/
        └── Item.txt  ← Archivo compartido
```

**Manifest para ambos:**
```ini
TargetPath=..\Data\Item\Item.txt
```

✅ Ambos GameServers actualizarán el mismo archivo compartido!

---

### Caso 2: Actualizar Config del ConnectServer

```
MuServer/
├── GameServer/
│   └── GameServer.exe
└── ConnectServer/
    └── ConnectServer.cfg
```

**Manifest:**
```ini
TargetPath=..\ConnectServer\ConnectServer.cfg
```

✅ Actualiza el ConnectServer.cfg desde el GameServer!

---

### Caso 3: Actualizar Archivo Local del GameServer

```
MuServer/
└── GameServer/
    └── Data/
        └── Local/
            └── Settings.ini
```

**Manifest:**
```ini
TargetPath=.\Data\Local\Settings.ini
```

✅ Actualiza archivo en la carpeta del GameServer!

---

## 🛡️ Características de Seguridad

### 1. Creación Automática de Carpetas
Si la carpeta no existe, **se crea automáticamente**:
```
MuServer\Data\Item\ no existe
→ Sistema crea la carpeta automáticamente
→ Copia el archivo
```

### 2. Backup Automático
Se crea backup del archivo anterior:
```
Item.txt → Item.txt.backup
```

### 3. Verificación de Permisos
Si no tiene permisos de escritura:
```
→ Muestra error
→ No aplica la actualización
→ Archivo original intacto
```

---

## 📊 Ejemplos Completos de update_manifest.txt

### Ejemplo 1: Actualizar Item.txt

```ini
[Update]
Version=1.0.3
DownloadUrl=https://creadormu.com/mualfa/updates/Item_1.0.3.txt
FileName=Item.txt
FileHash=
FileSize=515809
FileType=1
TargetPath=..\Data\Item\Item.txt
Description=New items: Wings Lvl 4, Ancient Sets, Jewels
Required=1
```

### Ejemplo 2: Actualizar Monster.txt

```ini
[Update]
Version=1.0.4
DownloadUrl=https://creadormu.com/mualfa/updates/Monster_1.0.4.txt
FileName=Monster.txt
FileHash=
FileSize=245600
FileType=1
TargetPath=..\Data\Monster\Monster.txt
Description=Monster HP/Damage rebalance, new boss spawns
Required=0
```

### Ejemplo 3: Actualizar GameServer.exe

```ini
[Update]
Version=1.0.5
DownloadUrl=https://creadormu.com/mualfa/updates/GameServer_1.0.5.exe
FileName=GameServer.exe
FileHash=
FileSize=2458900
FileType=0
Description=Bug fixes and performance improvements
Required=1
```

**Nota:** Para GameServer.exe NO uses TargetPath, el sistema lo maneja automáticamente.

---

## 🎯 Formato del Manifest - Completo

```ini
[Update]
Version=X.X.X               # Versión del update (requerido)
DownloadUrl=http://...      # URL de descarga (requerido)
FileName=archivo.txt        # Nombre del archivo (requerido)
FileHash=                   # Hash MD5/SHA256 (opcional)
FileSize=12345              # Tamaño en bytes (requerido)
FileType=0/1/2              # 0=EXE, 1=Data, 2=Config (requerido)
TargetPath=..\ruta\file.txt # Ruta de destino (opcional, solo para FileType=1 o 2)
Description=Descripción     # Descripción del update (opcional)
Required=0/1                # 0=Opcional, 1=Requerido (opcional)
```

---

## ⚠️ Notas Importantes

### 1. FileType y TargetPath

| FileType | Usa TargetPath? | Comportamiento |
|----------|----------------|----------------|
| 0 (EXE) | ❌ No | Siempre actualiza GameServer.exe |
| 1 (Data) | ✅ Sí | Usa TargetPath o `.\\Data\\FileName` por defecto |
| 2 (Config) | ✅ Sí | Usa TargetPath o `.\\Data\\FileName` por defecto |

### 2. Separadores de Ruta
Usa `\` (backslash) para rutas en Windows:
```
✅ Correcto: ..\Data\Item\Item.txt
❌ Incorrecto: ../Data/Item/Item.txt
```

### 3. Rutas Absolutas vs Relativas
```
✅ Recomendado: ..\Data\Item\Item.txt (relativa)
❌ No recomendado: C:\MuServer\Data\Item\Item.txt (absoluta)
```

Las rutas relativas funcionan en cualquier instalación!

---

## 🧪 Testing

### Prueba 1: Archivo en Carpeta Padre
```ini
TargetPath=..\test.txt
```
Resultado: `MuServer\test.txt`

### Prueba 2: Archivo en Subcarpeta
```ini
TargetPath=.\Data\Test\test.txt
```
Resultado: `GameServer\Data\Test\test.txt`

### Prueba 3: Múltiples Niveles
```ini
TargetPath=..\..\Backups\test.txt
```
Resultado: Dos niveles arriba + `Backups\test.txt`

---

## ✅ Checklist de Actualización

Antes de publicar un update:

- [ ] Subí el archivo a mi web hosting
- [ ] El DownloadUrl apunta al archivo correcto
- [ ] El FileSize coincide con el tamaño real del archivo
- [ ] El TargetPath usa la ruta correcta (con `..` si es necesario)
- [ ] Probé la ruta en un servidor de prueba
- [ ] El FileType es correcto (0=EXE, 1=Data, 2=Config)
- [ ] La versión es mayor que la actual
- [ ] La descripción explica los cambios

---

## 🎉 Beneficios

✅ Actualiza archivos en **cualquier carpeta**  
✅ Actualiza archivos **compartidos** entre múltiples GameServers  
✅ Actualiza archivos de **otros componentes** (ConnectServer, etc.)  
✅ Creación **automática de carpetas**  
✅ **Backup automático** antes de actualizar  
✅ **Compatible** con el sistema anterior  

---

**¡Ahora puedes actualizar archivos en cualquier parte de tu MuServer!** 🚀
