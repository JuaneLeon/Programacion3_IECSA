# ⏰ Reloj Analógico (Allegro)

Este proyecto contiene una demo en C++ que dibuja un **reloj analógico** en una ventana usando Allegro 5 y sus addons. El código principal es `RelojAnaligico.cpp`.

---

## ✨ Vista rápida

- Tamaño de la ventana: 800×600
- Centro del reloj: (400, 300)
- Radio del reloj: 250 px
- FPS objetivo: ~60 (usa `ALLEGRO_TIMER` con 1/60)
- Fuente: `al_create_builtin_font()` (fuente interna de Allegro)

---

## ¿Qué hace el programa?

- Inicializa Allegro y los addons: primitives, font y ttf.
- Crea una ventana y un `ALLEGRO_TIMER` que actualiza la pantalla ~60 veces por segundo.
- Obtiene la hora local del sistema (hora, minuto, segundo) y calcula los ángulos de las manecillas:
	- Hora: (horas % 12 + minutos/60.0) * 30 grados
	- Minuto: (minutos + segundos/60.0) * 6 grados
	- Segundo: segundos * 6 grados
- Dibuja la cara del reloj, marcas de horas (con diferente grosor para 12/3/6/9), contorno, manecillas y números (1–12).
- Espera al evento `ALLEGRO_EVENT_DISPLAY_CLOSE` para cerrar la aplicación.

---

## Dependencias

- Allegro 5
- Addon: `allegro_primitives`
- Addon: `allegro_font`
- Addon: `allegro_ttf`

Instala Allegro y los paquetes correspondientes en tu sistema (en MSYS2/MinGW suelen ser `mingw-w64-x86_64-allegro` y paquetes relacionados).

---

## Cómo compilar

Ejemplo genérico (si Allegro está en el PATH y las bibliotecas están instaladas):

```bash
g++ RelojAnaligico.cpp -o RelojAnaligico.exe -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf
```

Ejemplo usando MSYS2/MinGW64 (ajusta la ruta al compilador si es necesario):

```bash
C:/msys64/mingw64/bin/g++.exe RelojAnaligico.cpp -o RelojAnaligico.exe -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf
```

Si obtienes errores de enlace relacionados con `main`, añade `-lallegro_main` al final de la línea de enlace.

---

## Cómo ejecutar

Desde una terminal en la carpeta del proyecto:

```bash
./RelojAnaligico.exe    # en MSYS2 / Linux con compatibilidad
# o en PowerShell / CMD de Windows:
.\RelojAnaligico.exe
```

La ventana mostrará un fondo rosa, la cara del reloj blanca, marcadores y manecillas que indican la hora actual del sistema.

---

## Controles

- Cierra la ventana con el botón de cerrar (la aplicación escucha `ALLEGRO_EVENT_DISPLAY_CLOSE`).

---

## Detalles de implementación importantes

- Usa `ALLEGRO_TIMER` para actualizar la animación a ~60 FPS en lugar de depender sólo de `al_rest`.
- Los números del reloj se dibujan con `al_draw_text` usando la fuente integrada (`al_create_builtin_font()`). Si prefieres una fuente TTF, carga una con `al_load_ttf_font()`.
- Para los ángulos se utiliza una constante `pi = 3.1416` y conversiones de grados a radianes.

---

## Sugerencias de mejoras

1. Suavizar la animación de la manecilla de los segundos (interpolación entre ticks) o usar sub-segundos si deseas transición continua.
2. Reemplazar la fuente integrada por una TTF para mejor aspecto (usar `al_load_ttf_font("miFuente.ttf", tamaño, 0)`).
3. Añadir opciones de configuración: tamaño de ventana, color de fondo, estilo de marcadores.
4. Añadir atajos de teclado: por ejemplo `ESC` para cerrar, `F` para alternar fullscreen, `+/-` para cambiar velocidad de simulación.
5. Soporte HiDPI y reescalado al cambiar el tamaño de la ventana.

---

## ¿Quieres que haga esto por ti?

- Puedo añadir `ESC` para cerrar, soporte para fullscreen o atajos ahora mismo.
- Puedo generar una tarea de VSCode (tasks.json) que compile con tu toolchain de MSYS2/MinGW.

Indica qué prefieres y lo implemento.

---

Archivo fuente: `RelojAnaligico.cpp`

