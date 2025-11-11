# ⏰ Reloj Analógico (Animación Ultra Suave)

Este proyecto es la versión más avanzada de la simulación de un **reloj analógico funcional**, desarrollada en C++ con **Allegro 5**. La principal mejora de esta versión es una animación **extremadamente fluida** de todas las manecillas, especialmente la del segundero.

---

## 💡 Conceptos Técnicos Clave

* **Biblioteca Utilizada:** El proyecto se basa en **Allegro 5** y sus addons para dibujar formas (`allegro_primitives`) y manejar fuentes (`allegro_font`, `allegro_ttf`).
* **Tiempo de Alta Precisión (`std::chrono`):** Esta es la característica estrella. En lugar de leer solo el segundo actual, el programa utiliza la biblioteca `<chrono>` de C++ para obtener el tiempo del sistema con **precisión de milisegundos**.
* **Animación Continua (Sweeping Motion):** Gracias a la precisión de milisegundos, el segundero ya no "salta" de un segundo a otro. En su lugar, tiene un movimiento de barrido continuo y perfectamente suave, similar al de muchos relojes de alta gama. Todas las demás manecillas también se benefician de esta fluidez.
* **Bucle de Eventos (Event-Driven):** Utiliza un `ALLEGRO_TIMER` y una `ALLEGRO_EVENT_QUEUE` para manejar el tiempo y los eventos de la ventana de forma eficiente, actualizando la pantalla 60 veces por segundo.
* **Cálculos Trigonométricos:** La posición de todas las manecillas y números se sigue calculando dinámicamente usando `cos` y `sin` para un posicionamiento preciso en el círculo del reloj.

---

## ⚙️ Características Visuales

* **Cara del Reloj:** Un círculo blanco sobre un fondo rosa, con un borde negro.
* **Números (1-12):** Los números están correctamente posicionados alrededor de la esfera, con el "12" en la parte superior.
* **Marcadores de Hora:** El reloj dibuja 12 marcadores. Las posiciones cardinales (12, 3, 6 y 9) se dibujan en **rojo** y con mayor grosor, mientras que las demás horas están en azul.
* **Manecillas (Horas, Minutos, Segundos):**
    * **Hora:** Corta y gruesa.
    * **Minuto:** Longitud media y grosor medio.
    * **Segundo:** Larga, fina y de color rojo, con un movimiento de barrido continuo.

---

## ⌨️ Controles

* **Cerrar la ventana:** Es la única forma de interacción, lo que finaliza el programa.

---

## 🔧 Cómo Compilar

Para compilar este proyecto, necesitas tener la biblioteca **Allegro 5** instalada en tu sistema y enlazar correctamente los siguientes addons:
* `allegro`
* `allegro_primitives`
* `allegro_font`
* `allegro_ttf`

---

## 📦 Ejecutable

El archivo ejecutable ya compilado (`RelojMejorAnimacion`) se encuentra en esta misma carpeta, junto al código fuente.