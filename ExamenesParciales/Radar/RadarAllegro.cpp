//Juan Eduardo Leon Granados 10775
//librerias
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <random>
#include <iostream>
#include <vector>

// Constantes
const int ANCHO = 800; 
const int ALTO = 600; 
const int CENTRO_X = ANCHO / 2; 
const int CENTRO_Y = ALTO / 2; 
const int RADIO_RADAR = 250; 
const float PI = 3.14159265; 
const int NUM_PUNTOS = 5;
const double FPS = 60.0;

const int TRAIL_LENGTH = 45; // Longitud del rastro del barrido en grados
struct Punto {
    float x, y;
    float angulo; // Ángulo en grados
    bool detectado = false; // Para saber si ha sido detectado 
    float brillo = 0.0f; //el 0.0f es sin brillo, 1.0f es brillo total donde f es por que es float
    float flash_timer = 0.0f; // Temporizador para el destello inicial.
};

//funcion principal
int main (){
    // Inicialización de Allegro
    if (!al_init()) {
        std::cerr << "Error al inicializar Allegro." << std::endl;
        return -1;
    }
    al_init_primitives_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY *display = al_create_display(ANCHO, ALTO);
    if (!display) {
        std::cerr << "Error al crear la ventana." << std::endl;
        return -1;
    }

    //creacion de timer y cola de eventos
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Generar puntos aleatorios dentro del radar
    std::vector<Punto> puntos(NUM_PUNTOS);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis_radio(0.0, RADIO_RADAR);
    std::uniform_real_distribution<> dis_angulo(0.0, 360.0);

    // Generar puntos aleatorios y calcular sus ángulos
    for (int i = 0; i < NUM_PUNTOS; ++i) {
        float angulo_rad = dis_angulo(gen) * PI / 180.0f;
        float radio = dis_radio(gen);
        puntos[i].x = CENTRO_X + radio * cos(angulo_rad);
        puntos[i].y = CENTRO_Y + radio * sin(angulo_rad);
        puntos[i].angulo = atan2(puntos[i].y - CENTRO_Y, puntos[i].x - CENTRO_X) * 180.0f / PI;
        if (puntos[i].angulo < 0) {
            puntos[i].angulo += 360;
        }
    }

    // Bucle principal
    bool corriendo = true;
    bool redibujar = true;
    float angulo_barrido = 0.0f; // En grados
    float velocidad_barrido = 360.0 / (FPS * 4); // Una vuelta cada 4 segundos
    al_start_timer(timer);

    // Bucle Inicial
    while (corriendo) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(queue, &evento);
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            // Actualizar ángulo de barrido
            angulo_barrido += velocidad_barrido;
            if (angulo_barrido >= 360.0f) {
                angulo_barrido -= 360.0f;
            }

            // Comprobar detección de puntos y actualizar brillo
            for (auto& p : puntos) {
                // diff es la distancia desde el punto hasta la línea de barrido actual.
                // Un valor pequeño significa que la línea de barrido está justo después del punto.
                float diff = fmod(angulo_barrido - p.angulo + 360.0f, 360.0f); 

                // Detección: Si la línea de barrido principal acaba de pasar el punto.
                // Se usa un pequeño margen para asegurar la detección.
                if (diff >= 0 && diff < velocidad_barrido * 1.5) { 
                    if (!p.detectado || p.brillo == 0.0f) { // Si es la primera vez que lo detecta en este ciclo
                        p.flash_timer = 0.15f; // Duración del flash en segundos
                    }
                    p.detectado = true; // Marcar como detectado
                    p.brillo = 1.0f; // Activar brillo sostenido
                }
                // Desvanecimiento: solo si el rastro completo ya pasó por el punto.
                else if (p.brillo > 0 && diff > TRAIL_LENGTH) {
                    p.brillo = 0.0f; // Apagar el punto por completo
                }
                if (p.flash_timer > 0) {
                    p.flash_timer -= 1.0f / FPS; // Reducir el temporizador del flash
                }
            }

            // Redibujar la pantalla
            redibujar = true;
        } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE || evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            corriendo = false;
        }
        // Dibujar solo si es necesario y la cola de eventos está vacía
        if (redibujar && al_is_event_queue_empty(queue)) {
            redibujar = false;
            al_clear_to_color(al_map_rgb(0, 0, 0)); // Fondo negro

            // Dibujar las lineas dentro del radar
            ALLEGRO_COLOR color_rejilla = al_map_rgb(0, 100, 0);
            al_draw_circle(CENTRO_X, CENTRO_Y, RADIO_RADAR, color_rejilla, 2);
            al_draw_circle(CENTRO_X, CENTRO_Y, RADIO_RADAR * 0.66, color_rejilla, 1);
            al_draw_circle(CENTRO_X, CENTRO_Y, RADIO_RADAR * 0.33, color_rejilla, 1);
            al_draw_line(CENTRO_X - RADIO_RADAR, CENTRO_Y, CENTRO_X + RADIO_RADAR, CENTRO_Y, color_rejilla, 1);
            al_draw_line(CENTRO_X, CENTRO_Y - RADIO_RADAR, CENTRO_X, CENTRO_Y + RADIO_RADAR, color_rejilla, 1);

            // Dibujar el rastro del barrido con degradado en cada línea
            ALLEGRO_VERTEX vertices[2]; 
            for (int i = 0; i < TRAIL_LENGTH; ++i) {
                float angulo_rastro = angulo_barrido - i;
                
                // El progreso va de 1.0 (línea principal) a 0.0 (cola del rastro)
                float progreso = 1.0f - (float)i / TRAIL_LENGTH;

                //para el cambio de color
                float r = 0.4f * progreso;
                float g = 1.0f * progreso;
                float b = 0.4f * progreso;

                // Asegurarse de que el ángulo esté entre 0 y 360
                float angulo_rad_rastro = angulo_rastro * PI / 180.0f;
                
                // Definir los vértices de la línea con colores
                vertices[0].x = CENTRO_X;
                vertices[0].y = CENTRO_Y;
                vertices[0].z = 0;
                vertices[0].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0); // Centro siempre negro y transparente
                
                //
                vertices[1].x = CENTRO_X + RADIO_RADAR * cos(angulo_rad_rastro);
                vertices[1].y = CENTRO_Y + RADIO_RADAR * sin(angulo_rad_rastro);
                vertices[1].z = 0;
                vertices[1].color = al_map_rgba_f(r, g, b, progreso * 0.8f); // Aplicamos el color y una transparencia que también se desvanece
                
                // Dibujar la línea con el degradado
                al_draw_prim(vertices, NULL, NULL, 0, 2, ALLEGRO_PRIM_LINE_LIST);
            }

            // Dibujar puntos detectados con efecto de desvanecimiento
            for (auto& p : puntos) {
                if (p.detectado && p.brillo > 0) { // Solo dibujar si ha sido detectado y tiene brillo
                    // 1. Efecto de Flash inicial
                    if (p.flash_timer > 0) {
                        float flash_progress = p.flash_timer / 0.15f; // De 1.0 a 0.0
                        float flash_radius = 12.0f * (1.0f - flash_progress); // Crece de 0 a 12
                        float flash_alpha = flash_progress; // Se desvanece de 1.0 a 0.0
                        al_draw_filled_circle(p.x, p.y, flash_radius, al_map_rgba_f(1.0, 1.0, 1.0, flash_alpha)); // Flash blanco
                    }

                    // 2. Brillo sostenido y punto central
                    // Resplandor rojo sostenido
                    al_draw_filled_circle(p.x, p.y, 6, al_map_rgba_f(1.0, 0.1, 0.1, 0.5));
                    // Punto central brillante
                    al_draw_filled_circle(p.x, p.y, 3, al_map_rgb(255, 180, 180));
                }
            }
            // al_flip_display actualiza la pantalla con todo lo que se dibujo
            al_flip_display();
        }
    }

    // --- Limpieza ---
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}