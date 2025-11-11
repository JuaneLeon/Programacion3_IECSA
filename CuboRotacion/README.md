# 🧊 Cubo 3D Giratorio (Wireframe)

Este proyecto es una demostración de **gráficos 3D** básicos implementada desde cero en C++ utilizando la biblioteca **Allegro 5**. El programa renderiza un cubo en **wireframe** (estructura de alambre) y lo anima para que rote continuamente sobre su eje vertical.

---

## 💡 Conceptos Técnicos Clave

* **Renderizado 3D Manual:** Este proyecto **no utiliza OpenGL** ni ninguna otra biblioteca 3D. Toda la matemática 3D se calcula manualmente.
* **Estructura de Datos:** El cubo se define mediante:
    * `std::vector<Vec3>`: Un vector que almacena las 8 coordenadas (x, y, z) de los vértices del cubo.
    * `std::vector<std::pair<int,int>>`: Un vector que define las 12 aristas (líneas) que conectan los vértices.
* **Rotación 3D:** En cada fotograma, el programa aplica manualmente una **matriz de rotación del eje Y** a cada vértice del cubo usando `cos()` y `sin()`, actualizando su posición en el espacio 3D.
* **Proyección de Perspectiva:** Después de rotar, se aplica una **fórmula de proyección de perspectiva** (`x' = (x * f) / z`) a cada vértice. Esto simula una "cámara" y hace que los puntos más lejanos (mayor `z`) parezcan más cercanos al centro de la pantalla, creando una ilusión de profundidad.
* **Renderizado Wireframe:** El programa dibuja el cubo iterando sobre la lista de aristas (`edges`) y usando `al_draw_line()` para conectar los pares de vértices ya proyectados en la pantalla 2D.
* **Bucle de Eventos:** Utiliza un `ALLEGRO_TIMER` y una `ALLEGRO_EVENT_QUEUE` para asegurar una animación fluida a 60 FPS.

---

## ⚙️ Características Visuales

* **Cubo Giratorio:** Un cubo blanco en 3D que gira continuamente en sentido horario.
* **Fondo:** Un fondo gris oscuro para un alto contraste.
* **Efecto de Perspectiva:** Se puede ver claramente cómo la cara frontal del cubo parece más grande que la cara trasera a medida que gira.

---

## ⌨️ Controles

* **Cerrar la ventana:** Es la única forma de interacción, lo que finaliza el programa.

---

## 🔧 Cómo Compilar

Para compilar este proyecto, necesitas tener la biblioteca **Allegro 5** instalada en tu sistema y enlazar correctamente el addon `allegro_primitives`.

---

## 📦 Ejecutable

El archivo ejecutable ya compilado (`Cubo3D`) se encuentra en esta misma carpeta, junto al código fuente.