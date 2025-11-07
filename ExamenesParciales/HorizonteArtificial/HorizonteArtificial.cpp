#include <iostream>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>

// Constantes
const int ANCHO_PANTALLA = 800;
const int ALTO_PANTALLA = 600;
const double PI = 3.14159265358979323846;
const float FPS = 60.0;

// Límites para los ángulos en grados
const double MAX_CABCEO = 20.0;
const double MAX_ALABEO = 20.0;

// Sensibilidad del control
const double VELOCIDAD_CABCEO = 15.0; // grados por segundo
const double VELOCIDAD_ALABEO = 30.0; // grados por segundo

// Frecuencia para el movimiento automático (valores más altos = movimiento más rápido)
const double FRECUENCIA_CABCEO_AUTO = 0.5;
const double FRECUENCIA_ALABEO_AUTO = 0.8;

struct EstadoAvion {
    double cabeceo = 0.0; 
    double alabeo = 0.0; 
};

int main() {
    // Inicialización de Allegro
    if (!al_init()) { // Inicializa Allegro
        std::cerr << "Error al inicializar Allegro." << std::endl;
        return -1;
    }
    if (!al_install_keyboard()) {
        std::cerr << "Error al instalar el teclado." << std::endl;
        return -1;
    }
    if (!al_init_primitives_addon()) {
        std::cerr << "Error al inicializar el addon de primitivas." << std::endl;
        return -1;
    }
    al_init_font_addon();
    if (!al_init_ttf_addon()) {
        std::cerr << "Error al inicializar el addon de ttf." << std::endl;
        return -1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_DISPLAY* display = al_create_display(ANCHO_PANTALLA, ALTO_PANTALLA);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_FONT* font = al_load_ttf_font("C:/Windows/Fonts/consola.ttf", 18, 0); // Cambia la ruta si es necesario
    if (!font) {
        font = al_load_ttf_font("C:/Windows/Fonts/arial.ttf", 18, 0); // Fuente de respaldo
    }
    if (!timer || !display || !event_queue || !font) {
        std::cerr << "Error al crear recursos de Allegro." << std::endl;
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_queue);
        al_destroy_font(font);
        return -1;
    }
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // bucle principal
    EstadoAvion avion;
    bool ejecutando = true;
    bool redibujar = true;
    double tiempo_simulacion = 0.0; // Variable de tiempo para la animación automática

    // Colores
    ALLEGRO_COLOR color_cielo = al_map_rgb(50, 150, 255);
    ALLEGRO_COLOR color_tierra = al_map_rgb(140, 100, 40);
    ALLEGRO_COLOR color_lineas = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR color_avion = al_map_rgb(255, 255, 0);
    al_start_timer(timer);

    while (ejecutando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            double delta_tiempo = 1.0 / FPS;
            tiempo_simulacion += delta_tiempo;
            avion.cabeceo = MAX_CABCEO * sin(tiempo_simulacion * FRECUENCIA_CABCEO_AUTO);
            avion.alabeo = MAX_ALABEO * cos(tiempo_simulacion * FRECUENCIA_ALABEO_AUTO);
            redibujar = true;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            ejecutando = false;
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_Q) ejecutando = false;
            if (ev.keyboard.keycode == ALLEGRO_KEY_R) {
                // Al resetear, también reiniciamos el tiempo de la animación
                tiempo_simulacion = 0.0;
            }
        }

        if (redibujar && al_is_event_queue_empty(event_queue)) {
            redibujar = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));

            //Dibujo del horizonte
            ALLEGRO_TRANSFORM transform;
            al_identity_transform(&transform);
            al_translate_transform(&transform, ANCHO_PANTALLA / 2.0, ALTO_PANTALLA / 2.0);
            al_rotate_transform(&transform, -avion.alabeo * PI / 180.0);
            al_translate_transform(&transform, 0, avion.cabeceo * 8);
            al_use_transform(&transform);

            // Dibujar cielo y tierra con rectangulos
            al_draw_filled_rectangle(-ANCHO_PANTALLA, -ALTO_PANTALLA * 2, ANCHO_PANTALLA, 0, color_cielo);
            al_draw_filled_rectangle(-ANCHO_PANTALLA, 0, ANCHO_PANTALLA, ALTO_PANTALLA * 2, color_tierra);
            // Línea del horizonte
            al_draw_line(-ANCHO_PANTALLA, 0, ANCHO_PANTALLA, 0, color_lineas, 2.0);

            // Restaurar la transformación original para dibujar los elementos fijos
            al_identity_transform(&transform);
            al_use_transform(&transform);

            // dibujo de las lineas de medicion
            float cx = ANCHO_PANTALLA / 2.0;
            float cy = ALTO_PANTALLA / 2.0;

            al_draw_filled_circle(cx, cy, 10, al_map_rgb(200, 200, 200)); // Círculo gris claro
            al_draw_circle(cx, cy, 10, al_map_rgb(0, 0, 0), 2.0); // Borde negro
            al_draw_line(cx - 80, cy, cx - 20, cy, color_avion, 3.0);
            al_draw_line(cx + 20, cy, cx + 80, cy, color_avion, 3.0);
            al_draw_line(cx, cy - 5, cx, cy + 5, color_avion, 3.0);

            const double PIXELS_PER_DEGREE_PITCH = 8.0; // Sensibilidad de movimiento del horizonte
            ALLEGRO_COLOR pitch_line_color = al_map_rgb(200, 200, 200); // Gris claro para las líneas
            ALLEGRO_COLOR pitch_text_color = al_map_rgb(255, 255, 255); // Blanco para el texto

            for (int pitch_deg = -30; pitch_deg <= 30; pitch_deg += 10) {
                if (pitch_deg == 0) continue; 

                float line_y = cy - (pitch_deg * PIXELS_PER_DEGREE_PITCH); // Calcular posición Y de la línea
                float line_length = (std::abs(pitch_deg) == 30) ? 60.0f : 40.0f; // Líneas más largas para 30 grados
                float text_offset = line_length / 2 + 10.0f; // Desplazamiento del texto desde el final de la línea

                // Dibujar la línea de cabeceo
                al_draw_line(cx - line_length / 2, line_y, cx + line_length / 2, line_y, pitch_line_color, 2.0);

                // Dibujar el texto del ángulo de cabeceo
                char pitch_text[10];
                sprintf(pitch_text, "%d", std::abs(pitch_deg)); // Mostrar el valor absoluto del ángulo
                al_draw_text(font, pitch_text_color, cx - text_offset, line_y - al_get_font_line_height(font) / 2, ALLEGRO_ALIGN_LEFT, pitch_text);
                al_draw_text(font, pitch_text_color, cx + text_offset, line_y - al_get_font_line_height(font) / 2, ALLEGRO_ALIGN_RIGHT, pitch_text);
            }

            // Indicador de alabeo superior
            float roll_indicator_cy = 60; 
            float roll_indicator_radius = 50;

            al_draw_line(cx, roll_indicator_cy - roll_indicator_radius, cx, roll_indicator_cy - roll_indicator_radius - 15, color_lineas, 2); // Línea central superior

            for (int angulo : {10, 20}) {
                // Lado izquierdo
                float rad_izq = (-90 - angulo) * PI / 180.0;
                al_draw_line(cx + cos(rad_izq) * roll_indicator_radius, roll_indicator_cy + sin(rad_izq) * roll_indicator_radius, cx + cos(rad_izq) * (roll_indicator_radius + 10), roll_indicator_cy + sin(rad_izq) * (roll_indicator_radius + 10), color_lineas, 2);
                al_draw_textf(font, color_lineas, cx + cos(rad_izq) * (roll_indicator_radius + 25), roll_indicator_cy + sin(rad_izq) * (roll_indicator_radius + 25) - 8, ALLEGRO_ALIGN_CENTER, "%d", angulo);

                // Lado derecho
                float rad_der = (-90 + angulo) * PI / 180.0;
                al_draw_line(cx + cos(rad_der) * roll_indicator_radius, roll_indicator_cy + sin(rad_der) * roll_indicator_radius, cx + cos(rad_der) * (roll_indicator_radius + 10), roll_indicator_cy + sin(rad_der) * (roll_indicator_radius + 10), color_lineas, 2);
                al_draw_textf(font, color_lineas, cx + cos(rad_der) * (roll_indicator_radius + 25), roll_indicator_cy + sin(rad_der) * (roll_indicator_radius + 25) - 8, ALLEGRO_ALIGN_CENTER, "%d", angulo);
            }

            // Triángulo móvil del alabeo
            float alabeo_rad = (-90 - avion.alabeo) * PI / 180.0;
            al_draw_filled_triangle( cx + cos(alabeo_rad) * (roll_indicator_radius - 2), roll_indicator_cy + sin(alabeo_rad) * (roll_indicator_radius - 2), cx + cos(alabeo_rad - 0.1) * (roll_indicator_radius - 12), roll_indicator_cy + sin(alabeo_rad - 0.1) * (roll_indicator_radius - 12), cx + cos(alabeo_rad + 0.1) * (roll_indicator_radius - 12), roll_indicator_cy + sin(alabeo_rad + 0.1) * (roll_indicator_radius - 12), color_lineas );

            // Texto con los valores
            char texto[100];
            sprintf(texto, "Cabeceo: %.1f°", avion.cabeceo);
            al_draw_text(font, color_lineas, 10, 10, 0, texto);
            sprintf(texto, "Alabeo: %.1f°", avion.alabeo);
            al_draw_text(font, color_lineas, 10, 30, 0, texto);
            al_flip_display();
        }
    }

    // --- Limpieza ---
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}
