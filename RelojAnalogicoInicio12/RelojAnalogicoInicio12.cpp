#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

// Constantes
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
const float pi = 3.14159;
const double FPS = 60.0;

int main() {
    // Inicialización
    if (!al_init()) {
        std::cerr << "Error: No se pudo inicializar Allegro." << std::endl;
        return -1;
    }
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY *display = al_create_display(ancho, alto);
    if (!display) {
        std::cerr << "Error: No se pudo crear la ventana." << std::endl;
        return -1;
    }

    ALLEGRO_FONT *font = al_create_builtin_font(); // Fuente interna de Allegro

    // Variables para simular el tiempo
    double segundos_sim = 0.0;
    double minutos_sim = 0.0;
    double horas_sim = 0.0;

    // Con esto se incrementa o decrementa el tiempo simulado
    double incremento_segundos = 1.0; 

    // Ciclo principal
    bool running = true;

    while (running) {
        // Manejo de entrada para cerrar la ventana
        ALLEGRO_KEYBOARD_STATE kbd_state;
        al_get_keyboard_state(&kbd_state);
        if (al_key_down(&kbd_state, ALLEGRO_KEY_ESCAPE)) {
            running = false;
        }

        // Actualizar tiempo simulado
        segundos_sim += incremento_segundos / FPS;
        if (segundos_sim >= 60.0) {
            segundos_sim -= 60.0;
            minutos_sim += 1.0;
        }
        if (minutos_sim >= 60.0) {
            minutos_sim -= 60.0;
            horas_sim += 1.0;
        }
        if (horas_sim >= 12.0) {
            horas_sim -= 12.0;
        }

        // Calcular ángulos con el tiempo simulado
        double minutos_frac = minutos_sim + segundos_sim / 60.0;
        double horas_frac = horas_sim + minutos_frac / 60.0;

        float anguloSegundo = static_cast<float>(segundos_sim * 6.0);
        float anguloMinuto = static_cast<float>(minutos_frac * 6.0);
        float anguloHora = static_cast<float>(horas_frac * 30.0);
        
        // Fondo
        al_clear_to_color(al_map_rgb(255, 192, 203));

        // Cara del reloj
        al_draw_filled_circle(centroX, centroY, radioReloj, al_map_rgb(255, 255, 255));
        al_draw_circle(centroX, centroY, radioReloj, al_map_rgb(0, 0, 0), 5);
        
        // Marcas de las horas con colores
        for (int i = 0; i < 12; i++) {
            float angulo = i * 30 * pi / 180.0f;
            float xInicio = centroX + (radioReloj - 20) * cos(angulo);
            float yInicio = centroY + (radioReloj - 20) * sin(angulo);
            float xFin = centroX + radioReloj * cos(angulo);
            float yFin = centroY + radioReloj * sin(angulo);
            // Si el índice es divisible por 3 (corresponde a 3, 6, 9, 12), pintar de rojo
            if (i % 3 == 0) {
                al_draw_line(xInicio, yInicio, xFin, yFin, al_map_rgb(255, 0, 0), 4);
            } else { // Las demás, en azul
                al_draw_line(xInicio, yInicio, xFin, yFin, al_map_rgb(0, 0, 255), 2);
            }
        }

        // Manecilla de la hora
        float anguloHoraRad = (anguloHora - 90) * pi / 180.0f;
        float xHora = centroX + largoManecillaHora * cos(anguloHoraRad);
        float yHora = centroY + largoManecillaHora * sin(anguloHoraRad);
        al_draw_line(centroX, centroY, xHora, yHora, al_map_rgb(0, 0, 0), grosorManecillaHora);

        // Manecilla del minuto
        float anguloMinutoRad = (anguloMinuto - 90) * pi / 180.0f;
        float xMinuto = centroX + largoManecillaMinuto * cos(anguloMinutoRad);
        float yMinuto = centroY + largoManecillaMinuto * sin(anguloMinutoRad);
        al_draw_line(centroX, centroY, xMinuto, yMinuto, al_map_rgb(0, 0, 0), grosorManecillaMinuto);

        // Manecilla del segundo
        float anguloSegundoRad = (anguloSegundo - 90) * pi / 180.0f;
        float xSegundo = centroX + largoManecillaSegundo * cos(anguloSegundoRad);
        float ySegundo = centroY + largoManecillaSegundo * sin(anguloSegundoRad);
        al_draw_line(centroX, centroY, xSegundo, ySegundo, al_map_rgb(255, 0, 0), grosorManecillaSegundo);

        // Círculos decorativos en el centro
        al_draw_filled_circle(centroX, centroY, 10, al_map_rgb(0, 0, 0));
        
        // Números del reloj con el 12 en la parte de arriba
        for (int i = 1; i <= 12; i++) {
            float angulo = (i * 30 - 90) * pi / 180.0f;
            float xTexto = centroX + (radioReloj - 40) * cos(angulo);
            float yTexto = centroY + (radioReloj - 40) * sin(angulo) - al_get_font_line_height(font) / 2;
            std::stringstream ss;
            ss << i;
            al_draw_text(font, al_map_rgb(0, 0, 0), xTexto, yTexto, ALLEGRO_ALIGN_CENTER, ss.str().c_str());
        }

        al_flip_display();
        
        // Pausa para controlar los FPS
        al_rest(1.0 / FPS);
    }

    // Limpieza
    al_destroy_font(font);
    al_destroy_display(display);
    return 0;
}
