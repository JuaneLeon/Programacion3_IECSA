# ✈️ Proyecto: Radar de Barrido (Radar Simulator)

Este proyecto es una simulación visual de una pantalla de **Radar de Barrido**, similar a las utilizadas en control de tráfico aéreo o en sistemas meteorológicos. El programa ha sido desarrollado en C++ con la biblioteca gráfica **Allegro 5**.

---

## 💡 Conceptos Clave

* **Tecnología:** El proyecto se basa en **Allegro 5** para la creación de la ventana, el manejo de eventos (teclado y temporizador) y el renderizado de todas las formas primitivas (círculos, líneas y vértices).
* **Animación Procedural:** La rotación del barrido del radar es una animación continua controlada por un `ALLEGRO_TIMER`. No simula una detección real, sino que sigue una lógica programada para crear un efecto visual realista.
* **Lógica de Detección:** El programa genera un número fijo de "objetos" en posiciones aleatorias dentro del radio del radar. La "detección" ocurre cuando el ángulo de la línea de barrido principal coincide con el ángulo precalculado de un objeto.
* **Efectos Visuales Avanzados:**
    * **Rastro con Degradado:** En lugar de una simple línea, el barrido deja un rastro (o "afterglow") que se desvanece gradualmente. Esto se logra dibujando múltiples líneas, cada una con un color y una transparencia que disminuyen a medida que se alejan del cabezal de barrido.
    * **Efecto de "Flash" y Brillo Sostenido:** Cuando un objeto es detectado, no solo aparece, sino que primero emite un destello blanco que se expande y se desvanece rápidamente, seguido de un brillo rojo sostenido para mantenerlo visible mientras el barrido pasa.

---

## ⚙️ Características y Representación Visual

* **Rejilla de Radar Estática:** El fondo consiste en una rejilla verde clásica, compuesta por círculos concéntricos y líneas cruzadas que sirven como referencia de distancia y dirección.
* **Barrido Giratorio:** Una línea radial gira 360 grados de forma constante, simulando el barrido del emisor del radar.
* **Rastro de Fósforo Verde:** El barrido principal deja tras de sí una estela verde que se desvanece, imitando el efecto de persistencia de las antiguas pantallas de fósforo.
* **Detección de Objetos ("Blips"):** Los puntos generados aleatoriamente aparecen como "blips" rojos en la pantalla solo después de que el barrido ha pasado sobre ellos, con un llamativo efecto de destello inicial.

---

## 🚀 Flujo de Ejecución

1.  **Inicialización:** Se configura Allegro 5, la pantalla y la cola de eventos.
2.  **Generación de Puntos:** El programa crea un número definido de puntos en coordenadas polares aleatorias (radio y ángulo) dentro del círculo del radar y los convierte a coordenadas cartesianas (x, y).
3.  **Bucle Principal:**
    * El `ALLEGRO_TIMER` actualiza el ángulo de la línea de barrido en cada fotograma.
    * Se verifica si el ángulo de barrido actual ha "pasado" por encima del ángulo de alguno de los puntos.
    * Si hay una coincidencia, se activa el estado de "detectado" para ese punto, iniciando sus efectos visuales (flash y brillo).
    * En cada ciclo de redibujado, se renderiza primero la rejilla, luego el rastro del barrido y finalmente los puntos detectados.

---

## 🔧 Cómo Compilar

Para compilar este proyecto, necesitas tener la biblioteca **Allegro 5** instalada en tu sistema y correctamente enlazada en tu entorno de desarrollo. Asegúrate de incluir el addon `allegro_primitives`.

---

## 📦 Ejecutable

La carpeta `output/` contiene el archivo ejecutable ya compilado:

* `Radar`: Ejecutable del proyecto.