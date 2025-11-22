# Envolvente Convexa (Tarea 10)

**Descripción:**
- Este proyecto contiene una implementación visual en C++ de la envolvente convexa
  (Convex Hull) usando el algoritmo Monotone Chain. Está pensada como tarea práctica
  para la asignatura de Programación 3 y demostración gráfica con Allegro 5.

**Qué hace el programa:**
- Permite al usuario añadir puntos con el ratón y calcula en tiempo real la envolvente
  convexa del conjunto de puntos introducidos.
- Dibuja los puntos, las aristas de la envolvente y un relleno semitransparente para
  visualizar el área convexa mínima que contiene todos los puntos.

**Controles:**
- Clic izquierdo: añadir un punto en la posición del cursor.
- `R`: generar 30 puntos aleatorios dentro de la ventana.
- `C`: limpiar todos los puntos.
- `ESC`: cerrar la aplicación.

**Archivo principal:**
- `EnvolventeConvexaAllegro.cpp` — código fuente con la implementación del algoritmo
  Monotone Chain y la parte gráfica con Allegro 5. El archivo está comentado en español.

**Cómo compilar (ejemplo en Windows con MinGW/MSYS2):**

1. Abrir la consola de MSYS2/MinGW donde Allegro 5 esté disponible.
2. Situarse en la carpeta `Tarea10`:

```bash
cd "c:/Users/jg745/Downloads/VSCODE/Programacion3_IECSA/Tarea10"
```

3. Compilar con g++ enlazando las librerías de Allegro (ajusta según tu instalación):

```bash
g++ EnvolventeConvexaAllegro.cpp -o EnvolventeConvexaAllegro.exe \
  -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf
```

- Si tu instalación de Allegro exige otras librerías (por ejemplo `-lallegro_image`,
  `-lallegro_audio`), añádelas a la línea de enlace.
- En Visual Studio Code puedes usar la tarea preconfigurada del workspace
  (por ejemplo `Compilar C++ con Allegro`).

**Ejecutar:**

```bash
./EnvolventeConvexaAllegro.exe
```

**Notas técnicas:**
- Algoritmo: Monotone Chain (O(n log n) por ordenación). El cálculo se re-ejecuta
  cada vez que se añade un punto.
- Relleno de la envolvente: se aproxima con un fan de triángulos desde el primer
  vértice para obtener un efecto de área semitransparente.
- El programa intenta cargar la fuente TTF `C:\Windows\Fonts\arial.ttf` para
  mostrar instrucciones; si no está disponible, el programa sigue funcionando sin texto.

**Estructura del directorio (relevante):**
- `Tarea10/EnvolventeConvexaAllegro.cpp` — código fuente.
- `Tarea10/README.md` — este archivo.

**Licencia / Créditos:**
- Código original creado como ejercicio de clase. Si lo usas o lo subes a GitHub,
  añade un archivo `LICENSE` con la licencia que prefieras.

Si quieres, puedo añadir una captura de pantalla al README o generar un GIF corto
de la ejecución para que la página del repositorio quede más atractiva. ¿Te interesa eso?
