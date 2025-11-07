# 🔮 Proyecto Parcial: Esfera Neural Interactiva

Este es un proyecto de **Visualización Creativa (Creative Coding)** desarrollado en C++ utilizando la biblioteca **Allegro 5**. El programa renderiza una esfera de partículas 3D de aspecto orgánico que reacciona de forma fluida y dinámica al movimiento de la ventana del programa.

---

## 🚀 Características Principales

* **Renderizado 3D en 2D:** Simula una esfera de partículas en 3D (`Punto3D`) utilizando rotación de ejes (X e Y) y proyección de perspectiva, renderizándola en un lienzo 2D.
* **Física de Rebote ("Squash and Stretch"):** La esfera se deforma, estira y aplasta en respuesta al movimiento brusco de la ventana, simulando inercia y masa.
* **Formación Animada:** Al iniciar, las partículas vuelan desde posiciones aleatorias en el espacio para ensamblarse suavemente en la forma de la esfera.
* **Efecto "Aurora" (Tentáculos):** Genera múltiples "hebras de aurora" procedimentales que ondulan y fluyen desde el núcleo, reaccionando también a la intensidad del movimiento.
* **Núcleo Orgánico:** El núcleo "respira" (se expande y contrae rítmicamente) y las partículas individuales ondulan en su superficie.
* **Efectos de Resplandor (Bloom):** Múltiples capas de círculos difuminados y aditivos crean un complejo efecto de "bloom" (resplandor) alrededor del núcleo y la aurora.

---

## 💡 Conceptos Técnicos Demostrados

### 1. Gráficos con Allegro 5

* **Biblioteca Utilizada:** `allegro5`, `allegro_primitives`.
* **Bucle de Eventos:** El `main` contiene un bucle de eventos estándar de Allegro que maneja el `ALLEGRO_EVENT_TIMER` para la lógica (física) y el `ALLEGRO_EVENT_DISPLAY_CLOSE` para salir.
* **Renderizado Aditivo:** `al_set_blender` se configura en modo `ALLEGRO_ADD` para lograr los brillantes efectos de resplandor donde los colores se suman.

### 2. Estructura de Datos (POO en C)

El programa utiliza `struct` para organizar la data, un enfoque común en C++ para POO ligera.

* **`struct Esfera`**: El "objeto" principal que contiene el estado completo de la simulación (posición, velocidad, rotación, tiempo, listas de partículas, etc.).
* **`struct Punto3D`**: Almacena la posición de una partícula individual.
* **`struct HebraAurora`**: Almacena los parámetros de uno de los tentáculos (amplitud, velocidad, longitud).

### 3. La Lógica de Interacción (En `actualizarEsfera`)

El aspecto más importante del proyecto.

* **Detección de Movimiento:** En cada fotograma, el programa compara la posición actual de la ventana (`currWX`, `currWY`) con la posición anterior (`prevWX`, `prevWY`).
* **Cálculo de Aceleración:** La diferencia (delta) en la posición se usa para aplicar una "fuerza" (aceleración) a la esfera, dándole velocidad (`esfera.velX`).
* **Deformación (Estiramiento):** La misma fuerza se usa para deformar el `estiramientoX` y `estiramientoY` de la esfera.
* **Rotación:** El delta del movimiento también se usa para aplicar rotación a la esfera (`esfera.rotacionY`), haciendo que gire en la dirección en que se "arrastra".

### 4. Renderizado (En `dibujarEsfera`)

El dibujado se realiza en capas, de atrás hacia adelante, para crear una sensación de profundidad:

1.  **Resplandor Exterior (Fondo):** Grandes círculos semitransparentes.
2.  **Tentáculos de Aurora:** Generados procedimentalmente usando `ruidoSimple` y funciones `sinf` para crear ondas complejas.
3.  **Núcleo Difuminado:** Un resplandor interior más denso.
4.  **Red de Puntos (Primer Plano):**
    * Todos los `Punto3D` se rotan en el espacio 3D (`rotarPunto`).
    * Se ordenan por profundidad (`std::sort` por `z`).
    * Se dibujan las líneas de conexión y las partículas, creando la red neural.

---

## 🔧 Cómo Compilar y Ejecutar

Necesitarás tener **Allegro 5** (incluyendo los addons `primitives` y `font`) correctamente instalado y enlazado en tu entorno de desarrollo.

**Enlace (Linker) requerido:**
* `allegro-5.0.10`
* `allegro_primitives-5.0.10`
* `allegro_font-5.0.10`
*(Las versiones pueden variar)*

Una vez configurado el entorno, compila el `Main.cpp` como un proyecto estándar de C++.

---

## 📦 Ejecutable

La carpeta `output/` contiene el archivo ejecutable ya compilado:

* `EsferaNeural`: Ejecutable del proyecto.