#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <ctime>
#include <chrono> // Para tiempo de alta precisión

const int ancho = 800;
const int alto = 600;
const int centroX = ancho / 2;
const int centroY = alto / 2;
const int radioReloj = 250;
const int largoManecillaHora = 100;
const int largoManecillaMinuto = 150;
const int largoManecillaSegundo = 200;
const int grosorManecillaHora = 8;
const int grosorManecillaMinuto = 5;
const int grosorManecillaSegundo = 2;
const float pi = 3.1416;

int main() {
    // Inicialización
    if (!al_init()) {
        std::cout << "Error al inicializar Allegro." << std::endl;
        return -1;
    }
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY *display = al_create_display(ancho, alto);
    if (!display) {
        std::cout << "Error al crear la ventana." << std::endl;
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60);
    ALLEGRO_FONT *font = al_create_builtin_font(); // Fuente interna de Allegro

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    al_start_timer(timer);

    bool running = true;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            // Obtener hora actual con milisegundos para una animación fluida
            auto now = std::chrono::system_clock::now();
            time_t now_c = std::chrono::system_clock::to_time_t(now);
            struct tm ltm_s;
#if defined(_WIN32) || defined(_MSC_VER)
            localtime_s(&ltm_s, &now_c);
#else
            localtime_r(&now_c, &ltm_s);
#endif
            struct tm* ltm = &ltm_s;

            auto since_epoch = now.time_since_epoch();
            auto s = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
            since_epoch -= s;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);

            // Calcular valores fraccionarios para cada manecilla
            double segundos_frac = ltm->tm_sec + ms.count() / 1000.0;
            double minutos_frac = ltm->tm_min + segundos_frac / 60.0;
            double horas_frac = (ltm->tm_hour % 12) + minutos_frac / 60.0;

            float anguloSegundo = static_cast<float>(segundos_frac * 6.0);
            float anguloMinuto = static_cast<float>(minutos_frac * 6.0);
            float anguloHora = static_cast<float>(horas_frac * 30.0);

            // Fondo
            al_clear_to_color(al_map_rgb(255, 192, 203));

            // Cara del reloj
            al_draw_filled_circle(centroX, centroY, radioReloj, al_map_rgb(255, 255, 255));

            // Marcas de las horas
            for (int i = 0; i < 12; i++) {
                float angulo = i * 30 * pi / 180;
                float xInicio = centroX + (radioReloj - 20) * cos(angulo);
                float yInicio = centroY + (radioReloj - 20) * sin(angulo);
                float xFin = centroX + radioReloj * cos(angulo);
                float yFin = centroY + radioReloj * sin(angulo);

                if (i % 3 == 0) {
                    al_draw_line(xInicio, yInicio, xFin, yFin, al_map_rgb(255, 0, 0), 4);
                } else {
                    al_draw_line(xInicio, yInicio, xFin, yFin, al_map_rgb(0, 0, 255), 2);
                }
            }

            // Contorno del reloj
            al_draw_circle(centroX, centroY, radioReloj, al_map_rgb(0, 0, 0), 5);

            // Manecillas
            al_draw_line(centroX, centroY,
                         centroX + largoManecillaHora * cos(anguloHora * pi / 180 - pi / 2),
                         centroY + largoManecillaHora * sin(anguloHora * pi / 180 - pi / 2),
                         al_map_rgb(0, 0, 0), grosorManecillaHora);

            al_draw_line(centroX, centroY,
                         centroX + largoManecillaMinuto * cos(anguloMinuto * pi / 180 - pi / 2),
                         centroY + largoManecillaMinuto * sin(anguloMinuto * pi / 180 - pi / 2),
                         al_map_rgb(0, 0, 0), grosorManecillaMinuto);

            al_draw_line(centroX, centroY,
                         centroX + largoManecillaSegundo * cos(anguloSegundo * pi / 180 - pi / 2),
                         centroY + largoManecillaSegundo * sin(anguloSegundo * pi / 180 - pi / 2),
                         al_map_rgb(255, 0, 0), grosorManecillaSegundo);

            // Números del reloj con el 12 en la parte de arriba 
            for (int i = 1; i <= 12; i++) {
                float angulo = (i * 30 - 90) * pi / 180;
                float xTexto = centroX + (radioReloj - 40) * cos(angulo);
                float yTexto = centroY + (radioReloj - 40) * sin(angulo);

                std::stringstream ss;
                ss << i;
                std::string numero = ss.str();

                int anchoTexto = al_get_text_width(font, numero.c_str());
                int altoTexto = al_get_font_line_height(font);

                al_draw_text(font, al_map_rgb(0, 0, 0), xTexto - anchoTexto / 2, yTexto - altoTexto / 2, 0, numero.c_str());
            }

            al_flip_display();
            al_rest(0.01); // Pequeña pausa para estabilidad
        }
    }

    // Limpieza
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}