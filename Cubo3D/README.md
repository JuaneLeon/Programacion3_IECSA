# Cubo 3D (CuboNormal)

Este proyecto es una pequeña demo en C++ que dibuja un cubo en perspectiva (wireframe) usando **Allegro 5** y su addon de primitivos. El código fuente principal es `CuboNormal.cpp`.

---

## ¿Qué hace?

- Define un conjunto de vértices 3D y las aristas que conectan esos vértices para formar un cubo.
- Proyecta las coordenadas 3D a 2D con una proyección simple (escala y factores de profundidad) y dibuja las aristas con colores distintos.
- Crea una ventana con Allegro y espera a que el usuario la cierre.

El programa incluye funciones de rotación (`rotateX`, `rotateY`, `rotateZ`) aunque la versión actual únicamente dibuja el cubo estático (no rota automáticamente en el bucle principal).

---

## Dependencias

- Allegro 5
- Allegro Primitives addon (`allegro_primitives`)

En sistemas Windows con MinGW/MSYS2 se recomienda tener instalados los paquetes de Allegro para poder enlazar las bibliotecas.

---

## Cómo compilar (ejemplo)

Ejemplo genérico (posible en Linux/MinGW si las bibliotecas están en el PATH):

```bash
g++ CuboNormal.cpp -o CuboNormal.exe -lallegro -lallegro_primitives
```

Ejemplo usando la toolchain de MSYS2/MinGW64 (ruta y opciones pueden variar según tu instalación):

```bash
# Usando el compilador mingw-w64
C:/msys64/mingw64/bin/g++.exe CuboNormal.cpp -o CuboNormal.exe -lallegro -lallegro_primitives
```

Si tu código requiere más addons (por ejemplo `allegro_main`) añádelos a la línea de enlace.

---

## Cómo ejecutar

El ejecutable ya compilado (`CuboNormal.exe`) se encuentra en esta misma carpeta junto al código fuente. Simplemente haz doble clic en `CuboNormal.exe` desde el explorador de archivos o ejecútalo desde la terminal:

```bash
./CuboNormal.exe
```

En Windows (cmd/PowerShell) puedes ejecutar:

```powershell
.\CuboNormal.exe
```

---

## Controles / Comportamiento

- Cierra la ventana con el botón de cerrar (la aplicación espera `ALLEGRO_EVENT_DISPLAY_CLOSE`).
- El teclado está inicializado en el código, pero no hay controles predefinidos para rotación en la versión adjunta.

---

## Notas y mejoras sugeridas

- Añadir rotación interactiva o animada (usar `rotateX/rotateY/rotateZ` por frame y redibujar). 
- Implementar un `ALLEGRO_TIMER` para controlar FPS y animaciones suaves.
- Añadir back-face culling o sombreado para una mejor percepción de profundidad.

---

Si quieres, puedo:

1. Añadir rotación automática y controles de teclado (flechas / WASD) al código.
2. Generar un pequeño script de compilación (`Makefile` o tarea de VSCode) adaptado a tu entorno.

El ejecutable compilado ya está en esta misma carpeta.