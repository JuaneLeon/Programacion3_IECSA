**Cubo — Salida por un lado / aparición gradual por el otro (wrap-around)**

Visual en C++ con Allegro5: un cubo 3D (aristas iguales) que se mueve y rota automáticamente,
sale por un extremo de la ventana y aparece gradualmente por el extremo opuesto.

![Demo del programa](./demo.gif)

Descripción
- Movimiento automático: el cubo se desplaza con velocidades en X/Y (`vx`, `vy`).
- Rotación automática: gira continuamente en el plano de la pantalla (rotación sobre el eje Z).
- Wrap-around gradual: cuando el cubo se acerca a un borde y comienza a salir, la copia que
  aparece por el lado opuesto se dibuja con un alpha proporcional a cuánto de su caja proyectada
  queda dentro de la ventana; de esta forma la desaparición/aparición es suave y continua.
- No hay controles por teclado: el movimiento es totalmente automático (el programa no espera
  pulsaciones para mover o rotar).

Comportamiento técnico
- Se proyectan los vértices del cubo y se calcula su bounding box proyectada.
- Para producir un fundido suave se dibujan hasta 9 copias desplazadas por múltiplos de la
  ventana (vecindario -1..1 en X/Y). Cada copia recibe un alpha igual a la fracción de su
  bounding box proyectada que está dentro de la ventana (producto de fracciones X e Y).
- Para el wrap-around la posición (`tx`, `ty`) se ajusta por múltiplos de `WIDTH`/`HEIGHT`.
  La implementación usa bucles para soportar velocidades altas y evitar saltos bruscos.

Controles
- Ninguno (movimiento y rotación automáticos).  Si quieres, puedo volver a habilitar `ESC`
  para salir o `R` para resetear — dime si lo prefieres.

Compilación (MSYS2 / MinGW en Windows)
Usa el compilador MinGW que tengas instalado (ej. `C:/msys64/mingw64/bin/g++.exe`).

```bash
C:/msys64/mingw64/bin/g++.exe \
  C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboSalePantalla\*.cpp \
  -o C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboSalePantalla\SalePantalla.exe \
  -lallegro -lallegro_main -lallegro_image -lallegro_font -lallegro_ttf -lallegro_primitives \
  -lallegro_audio -lallegro_acodec -lglew32 -lopengl32 -lglu32

start C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboSalePantalla\SalePantalla.exe
```

Notas y sugerencias
- Ajuste de alpha: actualmente la fracción es lineal; si quieres un fundido más suave puedo aplicar
  una curva de easing (por ejemplo sqrt o una función sigmoide) para la alpha.
- Aparición sincronizada: si prefieres que la aparición no sea continua sino que haya un pequeño
  retardo/pausa antes de reaparecer, puedo añadir ese comportamiento.
- Visual: puedo colorear las aristas o rellenar caras visibles para mejorar la lectura 3D.

Archivos
- `SalePantalla.cpp` : código fuente principal (movimiento y wrap gradual).

Si quieres que marque esta documentación como finalizada en la TODO list, lo hago ahora y cierro
la tarea; o si quieres más cambios (retardo, easing, reactivar teclas), dime cuál prefieres.

**Cubo — Rotación y Rebote Automático**

Visual interactivo en C++ usando Allegro5 donde un cubo 3D (aristas iguales) se mueve y
rota automáticamente, rebotando al llegar a los bordes de la ventana.

![Demo del programa](./demo.gif)

Descripción
- El cubo mantiene proporciones iguales en todos sus lados.
- Movimiento automático: el cubo se desplaza con velocidades en X/Y (`vx`, `vy`).
- Rotación automática: gira continuamente en el plano de la pantalla (rotación sobre el eje Z).
- Rebote en bordes: cuando cualquiera de sus vértices proyectados sobre la pantalla
  cruza un borde, el cubo invierte la componente de velocidad correspondiente (rebote) y
  se corrige la posición para mantenerse visible.

Controles
- `ESC` : salir del programa
- `R`   : resetear posición (centro), rotación y velocidades a los valores por defecto

Requisitos
- Allegro5 con los addons: primitives y font (recomendado MSYS2/MinGW en Windows).

Compilación (MSYS2 / MinGW en Windows)
Usa el compilador MinGW que tengas instalado (ej. `C:/msys64/mingw64/bin/g++.exe`).

```bash
C:/msys64/mingw64/bin/g++.exe \
  C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboRotacionRebote\*.cpp \
  -o C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboRotacionRebote\RotacionRebote.exe \
  -lallegro -lallegro_main -lallegro_image -lallegro_font -lallegro_ttf -lallegro_primitives \
  -lallegro_audio -lallegro_acodec -lglew32 -lopengl32 -lglu32

start C:\Users\<tu_usuario>\Downloads\VSCODE\Programacion3_IECSA\CuboRotacionRebote\RotacionRebote.exe
```

Notas de implementación
- Proyección: ortográfica con un pequeño desplazamiento según la coordenada Z
  para dar sensación de profundidad sin distorsionar proporciones.
- Colisión/rebote: la detección se hace sobre las coordenadas proyectadas de los vértices
  del cubo; al rebotar se invierte `vx` o `vy` y se ajusta `tx`/`ty` para mantenerlo visible.
- Rotación: se realiza alrededor del eje Z (giro en el plano X-Y de la pantalla).

Archivos relevantes
- `RotacionRebote.cpp` : código fuente principal con la lógica automática y rebote.

Ideas opcionales que puedo añadir
- Hacer que la rotación también invierta su sentido al rebotar.
- Inicializar `vx`, `vy` y `rotVel` con valores aleatorios.
- Añadir relleno y coloreado de caras visibles para mejorar la apariencia 3D.
- Generar y añadir un `demo.gif` al directorio para mostrar el programa en el README de GitHub.

Si quieres que aplique alguna de las ideas opcionales, dime cuál y lo implemento.
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
