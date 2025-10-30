# 🧪 Guía Rápida de Pruebas - Fixes Aplicados

## ✅ 3 Problemas Corregidos

---

## Fix 1: Carpetas Se Crean Correctamente

### **Antes:**
```
Init() → Crea carpetas con path por defecto
  ↓
LoadConfig() → Cambia m_TempDirectory
  ↓
CheckForUpdates() → Intenta usar nueva ruta
  ↓
ERROR: Carpeta no existe
```

### **Ahora:**
```
Init() → LoadConfig() PRIMERO
  ↓
m_TempDirectory = ".\Update\Temp" (o custom)
  ↓
Crea carpetas con path correcto
  ↓
CheckForUpdates() → Funciona ✅
```

### **Prueba:**
```bash
1. Delete Update/ folder
2. Start GameServer.exe
3. Update → Check for Updates
4. Resultado: "You have the latest version of Mu Alfa!" ✅
   (No más "Failed to create file")
```

---

## Fix 2: Auto-Download Pregunta Para Aplicar

### **Antes:**
```
Auto-Download ON
  ↓
Encuentra update
  ↓
Descarga automáticamente
  ↓
¡Se detiene! ❌
  ↓
/Update/Temp/ lleno de archivos
```

### **Ahora:**
```
Auto-Download ON
  ↓
Encuentra update → Notificación
  ↓
Descarga automáticamente con progreso
  ↓
PREGUNTA: "Apply update now?" ✅
  ↓
[YES] → Aplica + Limpia
[NO]  → Warning + Log
```

### **Prueba:**
```bash
1. Config Updates → Auto-Download ON
2. Sube update al servidor
3. Restart GameServer.exe
4. Espera notificación: "Update available!"
5. Espera descarga automática (barra progreso)
6. Verifica pregunta: "Apply update now?" ✅
7. Click YES
8. Verifica: Update/ folder eliminado ✅
```

---

## Fix 3: Mensajes Claros y Útiles

### **Antes:**
- "Update Check Failed" (sin detalles)
- "Failed to create file..." (confuso)

### **Ahora:**

**Cuando No Hay Update:**
```
"You have the latest version of Mu Alfa!

Current Version: 1.0.0

No update needed."
```

**Cuando Hay Error de Red:**
```
"Failed to check for updates.

Please check:
• Your internet connection
• Update server URL is correct
• Update server is online

Check the console for details."
```

**Cuando Postpones Update:**
```
"Update downloaded but not applied.

You can apply it later, but remember to delete
the Update folder manually if you cancel."
```

### **Prueba:**
```bash
Test A: Sin Update
1. Check for Updates
2. Verifica: "You have the latest version" ✅

Test B: Error de Red
1. Apaga internet
2. Check for Updates
3. Verifica: Mensaje con tips útiles ✅

Test C: Postponer
1. Download update → YES
2. Apply → NO
3. Verifica: Warning sobre limpiar ✅
```

---

## 🎯 Test Completo

### Preparación:
```
1. Borra Update/ folder
2. Config Updates:
   - Enabled: ON
   - Auto-Check: ON
   - Auto-Download: OFF (para control)
   - URL: https://yoursite.com/updates
```

### Test 1: Check Manual Sin Update
```
1. Update → Check for Updates
2. ✅ Carpetas creadas
3. ✅ Manifest descargado
4. ✅ Mensaje: "You have the latest version of Mu Alfa!"
5. ✅ No errors en console
```

### Test 2: Check Manual Con Update
```
1. Sube update_manifest.txt con version nueva
2. Update → Check for Updates
3. ✅ Mensaje detallado con info
4. ✅ Download? → YES
5. ✅ Barra de progreso aparece
6. ✅ Progreso se actualiza
7. ✅ Apply? → YES
8. ✅ Update aplicado
9. ✅ Update/ folder eliminado
```

### Test 3: Auto-Download
```
1. Config Updates → Auto-Download ON
2. Sube update al servidor
3. Restart GameServer
4. ✅ Notificación: "Update available!"
5. ✅ Descarga automática inicia
6. ✅ Barra progreso visible
7. ✅ PREGUNTA: "Apply now?" (← FIX)
8. ✅ YES → Aplica y limpia
```

### Test 4: Postponer Update
```
1. Check for Updates → Update disponible
2. Download? → YES
3. Descarga completa
4. Apply? → NO
5. ✅ Warning: "remember to delete Update folder"
6. ✅ Log: "User postponed update"
7. Verifica: Update/Temp/ tiene archivos
8. Limpia manualmente: Delete Update/
```

### Test 5: Error de Red
```
1. Desconecta internet
2. Check for Updates
3. ✅ Mensaje útil con tips
4. ✅ Console muestra error detallado
5. Reconecta internet
6. Check again → ✅ Funciona
```

---

## 📊 Checklist de Verificación

### Carpetas:
- [ ] Update/ se crea al iniciar
- [ ] Update/Temp/ se crea al iniciar
- [ ] No errors "Failed to create file"
- [ ] Carpetas se crean con path correcto de config

### Auto-Download:
- [ ] Descarga automáticamente cuando hay update
- [ ] Muestra barra de progreso
- [ ] PREGUNTA para aplicar después de descargar
- [ ] Limpia archivos si aplicas
- [ ] Avisa si no aplicas

### Mensajes:
- [ ] "You have latest version" cuando no hay update
- [ ] Mensaje con tips cuando hay error de red
- [ ] Warning cuando postpones update
- [ ] Mensajes en español e inglés claros

### Manual Check:
- [ ] Funciona sin update disponible
- [ ] Funciona con update disponible
- [ ] Descarga con progreso
- [ ] Pregunta para aplicar
- [ ] Limpia después de aplicar

---

## 🐛 Si Algo Falla

### Error: "Failed to create file"
**Causa:** Carpetas no se crearon  
**Fix:** Verifica que Init() llama LoadConfig() ANTES de CreateDirectoryA  
**Status:** ✅ CORREGIDO

### Error: Auto-download no pregunta para aplicar
**Causa:** Faltaba código después de DownloadUpdate()  
**Fix:** Agregado MessageBox en CheckForUpdates()  
**Status:** ✅ CORREGIDO

### Error: Mensaje confuso sin update
**Causa:** CheckForUpdates() devolvía false, mostraba error genérico  
**Fix:** Ahora distingue entre "no update" y "error de red"  
**Status:** ✅ CORREGIDO

### Update/Temp/ no se limpia
**Causa:** Usuario cancela aplicación  
**Fix:** Ahora muestra warning para limpiar manualmente  
**Alternative:** Aplicar el update para limpieza automática  
**Status:** ✅ CORREGIDO (con warning)

---

## 🎉 Resultado Esperado

### Flujo Normal (Sin Update):
```
Check for Updates
  ↓
Carpetas OK
  ↓
Descarga manifest OK
  ↓
Compara versions: igual
  ↓
Mensaje: "You have latest version!" ✅
```

### Flujo Normal (Con Update, Manual):
```
Check for Updates
  ↓
Update disponible
  ↓
Mensaje detallado → Download? YES
  ↓
Barra progreso: 0% → 100%
  ↓
Apply? YES
  ↓
Update aplicado
  ↓
Update/ folder eliminado ✅
```

### Flujo Normal (Con Update, Auto-Download):
```
GameServer inicia
  ↓
Auto-check encuentra update
  ↓
Notificación
  ↓
Auto-download inicia
  ↓
Barra progreso
  ↓
PREGUNTA: "Apply now?" ✅
  ↓
YES → Aplicado y limpio ✅
```

---

## 💡 Tips

1. **Siempre aplica el update** después de descargar para que se limpie automáticamente
2. **Si cancelas**, recuerda eliminar `Update/` folder manualmente
3. **Verifica console logs** para detalles de errores
4. **Prueba con internet OFF** para ver mensajes de error
5. **Config Updates** abre diálogo visual (no necesitas editar INI)

---

## 📝 Resumen de Cambios en Código

### UpdateManager.cpp

**Línea 37-45:** Init() ahora crea carpetas DESPUÉS de LoadConfig
```cpp
// Load configuration FIRST
LoadConfig(".\\Data\\UpdateConfig.ini");

// Create directories AFTER
CreateDirectoryA(".\\Update", NULL);
CreateDirectoryA(m_TempDirectory, NULL);
```

**Línea 152-167:** Auto-download ahora pregunta para aplicar
```cpp
if (m_AutoDownload) {
    if (DownloadUpdate()) {
        // ← NUEVO
        int result = MessageBoxA(..., "Apply update now?", ...);
        if (result == IDYES) {
            ApplyUpdate();
        }
    }
}
```

**Línea 789-857:** ManualCheckForUpdates con mejor manejo
```cpp
if (!CheckForUpdates()) {
    // Error de red
    return;
}

if (!m_UpdateAvailable) {
    // No update - mensaje positivo
    return;
}

// Update available - mostrar diálogo
```

---

¡Todo listo para probar! 🚀
