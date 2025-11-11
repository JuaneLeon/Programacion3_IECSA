# ⏰ Reloj Analógico (con Hora del Sistema)

Este proyecto es una simulación de un **reloj analógico funcional** creada en C++ con la biblioteca **Allegro 5**. A diferencia de otras versiones, este reloj **lee la hora actual de tu computadora** y la muestra en tiempo real.

---

## 💡 Conceptos Técnicos Clave

* **Biblioteca Utilizada:** El proyecto se basa en **Allegro 5** y sus addons para dibujar formas (`allegro_primitives`) y manejar fuentes (`allegro_font`, `allegro_ttf`).
* **Lectura de Hora del Sistema:** Es la característica principal. Utiliza la biblioteca `<ctime>` (con `time(0)` y `localtime()`) para obtener la hora, minutos y segundos reales del sistema operativo en cada fotograma.
* **Bucle de Eventos (Event-Driven):** Este programa utiliza la forma robusta de Allegro para manejar el tiempo. Un `ALLEGRO_TIMER` se ejecuta 60 veces por segundo (FPS) y un `ALLEGRO_EVENT_QUEUE` espera los eventos (como el "tick" del temporizador o el cierre de la ventana) para actualizar la pantalla.
* **Cálculos Trigonométricos:** La posición de todas las manecillas y los números se calcula dinámicamente usando trigonometría (`cos` y `sin`) para convertir los ángulos en coordenadas (x, y).
* **Movimiento Suave:** El programa calcula la posición de las manecillas de hora y minuto usando fracciones (`minutos / 60.0`), lo que permite un movimiento fluido y realista en lugar de saltos discretos.

---

## ⚙️ Características Visuales

* **Cara del Reloj:** Un círculo blanco sobre un fondo rosa, con un borde negro.
* **Números (1-12):** Los números están correctamente posicionados alrededor de la esfera, con el "12" en la parte superior.
* **Marcadores de Hora:** El reloj dibuja 12 marcadores. Las posiciones cardinales (12, 3, 6 y 9) se dibujan en **rojo** y con mayor grosor, mientras que las demás horas están en azul.
* **Manecillas (Horas, Minutos, Segundos):**
    * **Hora:** Corta y gruesa.
    * **Minuto:** Longitud media y grosor medio.
    * **Segundo:** Larga, fina y de color rojo, se mueve cada segundo.

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

El archivo ejecutable ya compilado (`RelojAnaligico`) se encuentra en esta misma carpeta, junto al código fuente.