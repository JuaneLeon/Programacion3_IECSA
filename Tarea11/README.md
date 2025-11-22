# Manipulación de bits (XOR) — Tarea11

Este programa muestra una encriptación/descencriptación sencilla usando la operación XOR (bit a bit) sobre una cadena de caracteres. Está pensado para hacer visible, paso a paso, cómo cambia cada byte del mensaje.

**Contenido:**
- `ManipulacionBits.cpp`: código fuente en C++ que encripta/descencripta el arreglo `texto` usando una clave de un byte.

**Cómo funciona (visual):**
- Se toma el arreglo `texto[] = "hola"` y la `clave_m = 45` (0x2D).
- Para cada byte del mensaje se aplica `byte ^ clave` y se muestra:
  - índice del byte, valor antes (hex y char), valor después (hex y char)
  - estado parcial del mensaje en bytes después de procesar ese byte
- Al final se muestra el mensaje encriptado y luego se repite el proceso para volver al original (XOR es reversible).

**Compilación (Windows / msys2 - como en el entorno del curso):**
```bash
# usando mingw (ruta según tu instalación)
c:/msys64/mingw64/bin/g++.exe "Tarea11/ManipulacionBits.cpp" -o "Tarea11/ManipulacionBits.exe"
```

**Compilación (g++ en PATH):**
```bash
g++ "Tarea11/ManipulacionBits.cpp" -o "Tarea11/ManipulacionBits.exe"
```

**Ejecución:**
```bash
# Desde bash/terminal
Tarea11/ManipulacionBits.exe
```

Al final el programa mostrará `Presiona Enter para salir...` y esperará a que presiones Enter, para que la ventana no se cierre automáticamente.

**Salida de ejemplo (simulada):**
```
Mensaje original: hola
En bytes: 0x68 0x6F 0x6C 0x61 
  i=0: antes=0x68 ('h') -> despues=0x45 ('E')
  estado parcial en bytes: 0x45 0x6F 0x6C 0x61 
  i=1: antes=0x6F ('o') -> despues=0x42 ('B')
  estado parcial en bytes: 0x45 0x42 0x6C 0x61 
  i=2: antes=0x6C ('l') -> despues=0x41 ('A')
  estado parcial en bytes: 0x45 0x42 0x41 0x61 
  i=3: antes=0x61 ('a') -> despues=0x4C ('L')
  estado parcial en bytes: 0x45 0x42 0x41 0x4C 
Encriptado: EBAL
Encriptado en bytes: 0x45 0x42 0x41 0x4C 
  i=0: antes=0x45 ('E') -> despues=0x68 ('h')
  estado parcial en bytes: 0x68 0x42 0x41 0x4C 
  i=1: antes=0x42 ('B') -> despues=0x6F ('o')
  estado parcial en bytes: 0x68 0x6F 0x41 0x4C 
  i=2: antes=0x41 ('A') -> despues=0x6C ('l')
  estado parcial en bytes: 0x68 0x6F 0x6C 0x4C 
  i=3: antes=0x4C ('L') -> despues=0x61 ('a')
  estado parcial en bytes: 0x68 0x6F 0x6C 0x61 
Desencriptado: hola
Desencriptado en bytes: 0x68 0x6F 0x6C 0x61 
Presiona Enter para salir...
```

**Descripción breve de funciones clave en `ManipulacionBits.cpp`:**
- `encripta(char *mensaje, char clave, int tam)`: aplica XOR byte por byte y muestra información parcial tras cada operación.
- `desencripta(char *mensaje, char clave, int tam)`: llama a `encripta` (XOR con la misma clave invierte la operación).
- `muestra_mensaje(char *mensaje, int tam)`: imprime los bytes del buffer en formato `0xHH`.
- `imprime_como_string_seguro(char *mensaje, int tam)`: imprime los `tam` caracteres; los no imprimibles se muestran como `.` para evitar problemas con bytes nulos.

**Consejos para el README en GitHub (visual):**
- Puedes copiar el bloque de "Salida de ejemplo" en la sección principal para que se vea cuando alguien abra el repo.
- Si quieres una vista aún más visual, añade una captura de pantalla del terminal tras ejecutar el programa y súbela junto al README.

Si quieres que yo también cree un `README.md` en la raíz del repo con enlaces a cada ejercicio, lo agrego.

---
Archivo generado automáticamente para acompañar `ManipulacionBits.cpp`.
