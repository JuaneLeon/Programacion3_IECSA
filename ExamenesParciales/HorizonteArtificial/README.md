# ✈️ Proyecto: Horizonte Artificial (Attitude Indicator)

Este proyecto es una simulación de un **Horizonte Artificial** (o Indicador de Actitud), uno de los instrumentos de vuelo más importantes en la cabina de una aeronave. El programa utiliza la biblioteca **Allegro 5** para renderizar la pantalla del instrumento y simular el movimiento de la aeronave.

---

## 💡 Conceptos Clave

* **Tecnología:** El proyecto está construido completamente en C++ utilizando la biblioteca gráfica **Allegro 5** y sus addons para primitivas (`allegro_primitives`) y fuentes (`allegro_font`, `allegro_ttf`).
* **Simulación Automática:** En lugar de control manual, el programa ejecuta una **animación pre-programada**. Utiliza funciones trigonométricas (`sin` y `cos`) para simular un movimiento suave y continuo de cabeceo (pitch) y alabeo (roll), creando un ciclo de demostración constante.
* **Transformaciones Gráficas:** La clave de la simulación visual reside en el uso de transformaciones de Allegro. En lugar de mover el símbolo del avión, el programa **mueve y rota el fondo** (el cielo y la tierra) en la dirección opuesta, replicando fielmente cómo funciona un instrumento real.
    * `al_translate_transform`: Se usa para mover el horizonte hacia arriba y hacia abajo, simulando el **cabeceo**.
    * `al_rotate_transform`: Se usa para inclinar el horizonte, simulando el **alabeo**.

---

## ⚙️ Características y Representación Visual

* **Horizonte Dinámico:** La pantalla muestra una clara división entre el cielo (azul) y la tierra (marrón). Esta línea del horizonte se mueve y se inclina para representar la actitud actual del avión.
* **Símbolo Fijo del Avión:** Un símbolo amarillo en el centro de la pantalla representa la aeronave. Este elemento permanece estático, mientras que el mundo se mueve a su alrededor.
* **Indicadores de Medición:**
    * **Escala de Cabeceo (Pitch Ladder):** Líneas horizontales con etiquetas numéricas (10, 20, 30 grados) que se mueven verticalmente para indicar si el morro del avión está apuntando hacia arriba o hacia abajo.
    * **Indicador de Alabeo (Roll Indicator):** Un arco en la parte superior con marcas de grados (10, 20) y un triángulo móvil que indica el ángulo de inclinación de las alas.
* **Visualización de Datos (HUD):** En la esquina superior izquierda, se muestran los valores numéricos exactos de cabeceo y alabeo para una lectura precisa.

---

## ⌨️ Interacción

La simulación es automática, pero se proporcionan los siguientes controles:

* **`Q`**: **Salir** del programa.
* **`R`**: **Reiniciar** la animación a su punto de partida.

---

## 🔧 Cómo Compilar

Para compilar este proyecto, necesitas tener la biblioteca **Allegro 5** instalada en tu sistema y correctamente enlazada en tu entorno de desarrollo. Asegúrate de incluir los siguientes addons:
* `allegro`
* `allegro_primitives`
* `allegro_font`
* `allegro_ttf`

---

## 📦 Ejecutable

La carpeta `output/` contiene el archivo ejecutable ya compilado:

* `HorizonteArtificial`: Ejecutable del proyecto.