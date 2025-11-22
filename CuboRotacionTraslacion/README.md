**Cubo Rotación y Traslación**

Visual pequeño para mostrar un cubo 3D (aristas iguales) controlable con teclado.

**Demo**
- GIF / imagen de ejemplo: (añade `demo.gif` o `screenshot.png` en este directorio para que se muestre)

![Demo del programa](./demo.gif)

**Descripción**
- Programa en C++ usando Allegro5 que dibuja un cubo proporcionado (todos los lados iguales),
  permite desplazarlo con las flechas y rotarlo fluidamente en el plano (giro sobre el eje Z).
- El cubo mantiene proporciones iguales en todos sus lados durante traslaciones y rotaciones.

**Controles**
- Flechas: mover el cubo (arriba / abajo / izquierda / derecha)
- A / D : rotar antihorario / horario (mantener tecla para rotación continua)
- R     : resetear posición y rotación
- ESC   : salir

**Requisitos**
- Allegro5 instalado (MSYS2/MinGW recomendado en Windows). Añadir los addons: primitives, font, image, ttf, audio (opcional).

**Compilación (MSYS2 / MinGW en Windows)**
Usa el compilador MinGW que tienes en `C:/msys64/mingw64/bin/g++.exe` (o el que tengas en tu PATH).

Ejemplo con las mismas dependencias que usa la tarea de VS Code:

```bash
C:/msys64/mingw64/bin/g++.exe \ 
  C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboRotacionTraslacion\*.cpp \
  -o C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboRotacionTraslacion\RotacionTraslacion.exe \
  -lallegro -lallegro_main -lallegro_image -lallegro_font -lallegro_ttf -lallegro_primitives \
  -lallegro_audio -lallegro_acodec -lglew32 -lopengl32 -lglu32

start C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboRotacionTraslacion\RotacionTraslacion.exe
```

Si tienes `pkg-config` configurado (MSYS2), también puedes compilar así:

```bash
g++ -std=c++11 RotacionTraslacion.cpp -o RotacionTraslacion.exe `pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_font-5`
```

**Notas de implementación**
- Rotación: se realiza alrededor del eje Z para que el cubo gire en el plano de la pantalla.
- Traslación: se aplica en píxeles en pantalla (tx, ty) sin cambiar la escala por eje — por tanto las aristas permanecen iguales.
- La proyección es ortográfica simple con un pequeño desplazamiento según la componente Z para dar sensación de profundidad.

**Archivos relevantes**
- `RotacionTraslacion.cpp` : código fuente principal.

**Sugerencias para el repositorio GitHub**
- Añade una captura (`screenshot.png`) o un GIF (`demo.gif`) en este directorio; GitHub mostrará la imagen en la página del repositorio.
- Si quieres compartir binarios, sube `RotacionTraslacion.exe` solo si lo consideras necesario (ten en cuenta el .gitignore y el tamaño del repo).

**Licencia**
- Añade la licencia que prefieras (por defecto no se incluye). Si quieres, te agrego una `LICENSE` (por ejemplo MIT).

---

Si quieres, puedo:
- Generar un GIF demo automáticamente (requiere que ejecutes el binario y capture pantalla) o darte instrucciones para grabarlo.
- Añadir un `Makefile`/`build.bat` para compilar más fácil.
