# 🚀 Primeros Programas con Allegro 5

Este proyecto es una colección de "mini-programas" o bocetos (sketches) en un solo archivo, diseñados para probar los fundamentos del dibujo de primitivas en C++ con la biblioteca **Allegro 5**.

El programa dibuja varias formas geométricas estáticas y concluye con una animación simple de un "radar".

---

## 💡 Conceptos Técnicos Demostrados

Este código es una excelente demostración de los fundamentos de **Allegro 5** y de la **geometría computacional**.

* **Biblioteca Utilizada:** El proyecto se basa en **Allegro 5** y su addon para dibujar formas (`allegro_primitives`).
* [cite_start]**Inicialización:** Configura Allegro, el addon y crea una ventana de 800x600[cite: 1].
* **Renderizado Estático vs. Animado:**
    * [cite_start]**Estático:** La mayoría de las formas se dibujan una sola vez y se muestran con un único `al_flip_display()`[cite: 1].
    * [cite_start]**Animado:** El radar al final utiliza un bucle `for` que llama a `al_flip_display()` y `al_rest()` [cite: 1] en cada iteración para crear el efecto de movimiento.
* [cite_start]**Dibujo Píxel por Píxel:** Demuestra cómo dibujar formas complejas (líneas, círculos) usando la función `al_draw_pixel()` [cite: 1] dentro de bucles `for`.
* [cite_start]**Coordenadas Cartesianas:** Dibuja líneas horizontales, verticales e inclinadas [cite: 1] manipulando directamente las coordenadas `x` e `y`.
* [cite_start]**Coordenadas Polares:** Demuestra cómo dibujar líneas y círculos [cite: 1] usando trigonometría (`cos` y `sin`) para convertir un ángulo y un radio en coordenadas `x, y`.

---

## ⚙️ Características Visuales (El Programa Dibuja:)

1.  [cite_start]**Línea Vertical:** Creada con `al_draw_pixel` en un bucle[cite: 1].
2.  [cite_start]**Línea Horizontal:** Creada de manera similar[cite: 1].
3.  [cite_start]**Línea Inclinada (Cartesiana):** Dibujada píxel por píxel usando la ecuación `y = x - 100`[cite: 1].
4.  [cite_start]**Línea Diagonal (Polar):** Dibujada convirtiendo un ángulo constante (`pi/4`) y un radio creciente[cite: 1].
5.  **Círculo (Ecuación General):** Se dibuja usando la ecuación $x^2 + y^2 = r^2$. [cite_start]Se implementa en dos bucles (`x=-r` a `r` y `y=-r` a `r`) para rellenar los huecos que deja un solo bucle[cite: 1].
6.  [cite_start]**Explosión de Rayos:** Dibuja 24 líneas (`al_draw_line`) que radian desde un punto central común[cite: 1], cambiando el ángulo en 15 grados cada vez.
7.  **Radar (Animación):** La parte final del programa. [cite_start]Dibuja una línea verde, la muestra, hace una pausa (`al_rest`), y luego la "borra" (dibujándola de negro) para animar el siguiente fotograma, creando un efecto de barrido[cite: 1].

---

## 🔧 Cómo Compilar

Para compilar este proyecto, necesitas tener la biblioteca **Allegro 5** instalada en tu sistema y enlazar correctamente el addon `allegro_primitives`.

---

## 📦 Ejecutable

El archivo ejecutable ya compilado (`PrimerosProgramasAllegro.exe`) se encuentra en esta misma carpeta, junto al código fuente.