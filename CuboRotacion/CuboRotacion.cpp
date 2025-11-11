#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <vector>
#include <cmath>

struct Vec3 { double x, y, z; };

int main() {
    const int ancho = 800;
    const int alto = 600;
    const int cx = ancho / 2;
    const int cy = alto / 2;

    if (!al_init()) {
        std::cerr << "Fallo al inicializar Allegro\n";
        return -1;
    }
    al_init_primitives_addon();

    ALLEGRO_DISPLAY *display = al_create_display(ancho, alto);
    if (!display) {
        std::cerr << "No se pudo crear la ventana\n";
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // Definición del cubo (8 vértices)
    double s = 150.0; // semilado
    std::vector<Vec3> vertices = {
        {-s,  s, -s}, // 0
        { s,  s, -s}, // 1
        { s, -s, -s}, // 2
        {-s, -s, -s}, // 3
        {-s,  s,  s}, // 4
        { s,  s,  s}, // 5
        { s, -s,  s}, // 6
        {-s, -s,  s}  // 7
    };

    // Aristas (pares de índices)
    std::vector<std::pair<int,int>> edges = {
        {0,1},{1,2},{2,3},{3,0}, // cara trasera
        {4,5},{5,6},{6,7},{7,4}, // cara frontal
        {0,4},{1,5},{2,6},{3,7}  // conexiones
    };

    al_start_timer(timer);

    bool running = true;
    double angle = 0.0; // ángulo en radianes
    const double speed = 1.0; // rad/s (ajustable)
    const double distance = 600.0; // distancia de la cámara al origen

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Avanzar ángulo en sentido horario: disminuimos el ángulo
            double dt = 1.0 / 60.0;
            angle -= speed * dt; // sentido horario

            // Rotar y proyectar
            std::vector<std::pair<float,float>> projected(vertices.size());
            for (size_t i = 0; i < vertices.size(); ++i) {
                // Rotación alrededor del eje Y
                double x = vertices[i].x;
                double y = vertices[i].y;
                double z = vertices[i].z;

                double cosA = cos(angle);
                double sinA = sin(angle);

                double xr =  x * cosA + z * sinA;
                double yr =  y;
                double zr = -x * sinA + z * cosA;

                // Trasladar hacia la cámara
                double zcam = zr + distance;
                double f = 500.0; // factor de proyección (fov)
                double sx = (xr * f) / zcam;
                double sy = (yr * f) / zcam;

                projected[i].first = static_cast<float>(cx + sx);
                projected[i].second = static_cast<float>(cy - sy);
            }

            // Dibujar
            al_clear_to_color(al_map_rgb(30,30,30));

            // Dibujar aristas como líneas (wireframe)
            for (auto &e : edges) {
                auto a = projected[e.first];
                auto b = projected[e.second];
                al_draw_line(a.first, a.second, b.first, b.second, al_map_rgb(255,255,255), 2.0);
            }

            // Indicador simple de dirección
            al_draw_filled_circle(60, 30, 6, al_map_rgb(255,0,0));

            al_flip_display();
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    al_shutdown_primitives_addon();

    return 0;
}
