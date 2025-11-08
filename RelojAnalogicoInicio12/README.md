# ⏰ Reloj Analógico (Simulación desde las 12)

Este proyecto es una simulación de un **reloj analógico funcional** creada desde cero en C++ utilizando la biblioteca gráfica **Allegro 5**.

---

## 💡 Conceptos Técnicos Clave

* **Biblioteca Utilizada:** El proyecto se basa en **Allegro 5** y sus addons para dibujar formas (`allegro_primitives`), manejar fuentes (`allegro_font` y `allegro_ttf`) e instalar el teclado.
* **Simulación de Tiempo:** A diferencia de un reloj que lee la hora del sistema, este programa **simula su propio tiempo**. Comienza en 00:00:00 (las 12) y avanza a una velocidad controlada (`incremento_segundos / FPS`), lo que permite ver el movimiento de las manecillas.
* **Cálculos Trigonométricos:** La posición de todas las manecillas, los números y los marcadores en la cara del reloj se calcula dinámicamente usando trigonometría (`cos` y `sin`) para convertir un ángulo en coordenadas (x, y) en el círculo.
* **Bucle de Renderizado:** Utiliza un bucle `while` simple con `al_rest(1.0 / FPS)` al final. Esta es una forma de controlar la velocidad de fotogramas (FPS) sin la complejidad de una cola de eventos y un `ALLEGRO_TIMER`.
* **Movimiento Suave:** La manecilla de la hora no salta de un número a otro; se mueve suavemente (`horas_frac`) en proporción al avance de los minutos, lo que da un comportamiento más realista.

---

## ⚙️ Características Visuales

* **Cara del Reloj:** Un círculo blanco sobre un fondo rosa, con un borde negro y un pivote central negro.
* **Números (1-12):** Los números están correctamente posicionados alrededor de la esfera, con el "12" en la parte superior.
* **Marcadores de Hora:** El reloj dibuja 12 marcadores. Las posiciones cardinales (12, 3, 6 y 9) se dibujan en **rojo** y con mayor grosor, mientras que las demás horas están en azul.
* **Manecillas (Horas, Minutos, Segundos):**
    * **Hora:** Corta y gruesa.
    * **Minuto:** Longitud media y grosor medio.
    * **Segundo:** Larga, fina y de color rojo.

---

## ⌨️ Controles

* **`ESC`**: Cierra la aplicación.

---

## 🔧 Cómo Compilar

Para compilar este proyecto, necesitas tener la biblioteca **Allegro 5** instalada en tu sistema y enlazar correctamente los siguientes addons:
* `allegro`
* `allegro_primitives`
* `allegro_font`
* `allegro_ttf`

---

## 📦 Ejecutable

El archivo ejecutable ya compilado (`RelojAnalogico12`) se encuentra en esta misma carpeta, junto al código fuente.