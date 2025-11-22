// Envolvente convexa con Allegro 5
// Programa de ejemplo en C++ que permite añadir puntos con el ratón
// y dibuja la envolvente convexa usando el algoritmo Monotone Chain.
// Controles:
//  - Clic izquierdo: añadir punto
//  - R: generar puntos aleatorios
//  - C: limpiar puntos
//  - ESC: salir
// Compilación (ejemplo para MSYS/MinGW y Allegro 5):
// g++ EnvolventeConvexaAllegro.cpp -o EnvolventeConvexaAllegro.exe \
//   -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf
// En VSCode puede usar la tarea "Compilar C++ con Allegro" en el workspace.

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// ----- Estructura de datos -----
// Punto simple con coordenadas x,y en pantalla (float para suavidad en dibujo).
struct Point {
    float x, y;
    // Operador < necesario para ordenar puntos lexicográficamente
    // por x y luego por y. Esto se usa en el algoritmo de Monotone Chain.
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

// ----- Funciones geométricas -----
// Producto cruzado (b - a) x (c - a).
// El signo del resultado indica la orientación de la tripleta (a,b,c):
//  >0 : c está a la izquierda de la recta a->b (giro antihorario)
//  <0 : c está a la derecha de la recta a->b (giro horario)
//  =0 : colineales
static double cross(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// ----- Algoritmo: Monotone Chain para envolvente convexa -----
// Entrada: vector de puntos (puede contener duplicados)
// Salida : vector con los vértices de la envolvente convexa en orden (horario/antihorario)
// Complejidad: O(n log n) por la ordenación.
static std::vector<Point> convex_hull(std::vector<Point> pts) {
    std::vector<Point> H;
    size_t n = pts.size();
    // Casos triviales: 0 o 1 punto
    if (n <= 1) return pts;

    // 1) Ordenar puntos lexicográficamente por x, luego y
    //    Esto permite construir las dos mitades (inferior y superior)
    std::sort(pts.begin(), pts.end());

    // 2) Construir la mitad inferior de la envolvente.
    //    Recorremos los puntos ordenados de izquierda a derecha y aplicamos
    //    una pila (vector) donde mantenemos los vértices actuales. Si el
    //    nuevo punto produce un giro no-convexo (cross <= 0), se hace pop.
    std::vector<Point> lower;
    for (size_t i = 0; i < n; ++i) {
        while (lower.size() >= 2 && cross(lower[lower.size()-2], lower[lower.size()-1], pts[i]) <= 0) {
            // Sacar el último punto porque provoca una curva no-convexa
            lower.pop_back();
        }
        lower.push_back(pts[i]);
    }

    // 3) Construir la mitad superior de la envolvente.
    //    Recorremos los puntos en orden inverso. La lógica es análoga a la inferior.
    std::vector<Point> upper;
    for (size_t i = n; i-- > 0;) {
        while (upper.size() >= 2 && cross(upper[upper.size()-2], upper[upper.size()-1], pts[i]) <= 0) {
            upper.pop_back();
        }
        upper.push_back(pts[i]);
    }

    // 4) Concatenar las mitades inferior y superior.
    //    Los extremos están duplicados en lower[0] y upper[0], por eso
    //    al concatenar evitamos duplicar los extremos.
    H = lower;
    for (size_t i = 1; i + 1 < upper.size(); ++i) H.push_back(upper[i]);
    return H;
}

int main() {
    // Inicialización básica de Allegro
    if (!al_init()) {
        std::cerr << "No se pudo inicializar Allegro5\n";
        return -1;
    }

    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    al_install_mouse();
    al_install_keyboard();

    const int WIDTH = 1024;
    const int HEIGHT = 768;

    ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
    if (!display) {
        std::cerr << "No se pudo crear la ventana\n";
        return -1;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // Intentar cargar una fuente TTF de Windows como ejemplo. Si falla, no mostramos texto.
    // Nota: en algunas instalaciones de Allegro la carga puede fallar si no hay
    // soporte para TTF o la ruta es diferente. El programa seguirá funcionando
    // sin texto si la fuente no está disponible.
    ALLEGRO_FONT* font = nullptr;
    const char* font_path = "C:\\Windows\\Fonts\\arial.ttf"; // ruta típica en Windows
    font = al_load_ttf_font(font_path, 16, 0);

    std::vector<Point> points;
    std::vector<Point> hull;

    bool redraw = true;
    bool running = true;

    std::srand((unsigned)std::time(nullptr));

    al_start_timer(timer);

    // Bucle principal de eventos.
    // Usamos un timer para limitar el redraw a ~60 FPS y evitar repintados innecesarios.
    while (running) {
        ALLEGRO_EVENT ev;
        // Al esperar el siguiente evento (bloqueante) el bucle es eficiente.
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // El timer nos indica que es momento de redibujar si hay cambios.
            if (redraw) {
                // Limpiar pantalla con color oscuro
                al_clear_to_color(al_map_rgb(30, 30, 30));

                // Dibujar cada punto como un pequeño círculo
                for (const auto& p : points) {
                    al_draw_filled_circle(p.x, p.y, 4.0f, al_map_rgb(240, 240, 240));
                    al_draw_circle(p.x, p.y, 6.0f, al_map_rgb(100, 100, 100), 1.0f);
                }

                // Si hay al menos 2 puntos en la envolvente, dibujamos sus aristas
                if (hull.size() >= 2) {
                    // Dibujar líneas entre vértices consecutivos
                    for (size_t i = 0; i < hull.size(); ++i) {
                        size_t j = (i + 1) % hull.size();
                        al_draw_line(hull[i].x, hull[i].y, hull[j].x, hull[j].y, al_map_rgb(0, 200, 100), 2.5f);
                    }
                    // Relleno aproximado: dibujamos triángulos en modo "fan" desde hull[0]
                    // para dar sensación de área rellena. El color es semitransparente.
                    ALLEGRO_COLOR fill = al_map_rgba_f(0.0f, 0.8f, 0.4f, 0.08f);
                    for (size_t i = 1; i + 1 < hull.size(); ++i) {
                        al_draw_filled_triangle(hull[0].x, hull[0].y,
                                                hull[i].x, hull[i].y,
                                                hull[i+1].x, hull[i+1].y,
                                                fill);
                    }
                }

                // Mostrar texto de ayuda e información si la fuente fue cargada
                if (font) {
                    al_draw_textf(font, al_map_rgb(220,220,220), 10, 10, 0,
                                  "Clic: añadir punto | R: aleatorio | C: limpiar | ESC: salir | Puntos: %d | Hull: %d",
                                  (int)points.size(), (int)hull.size());
                }

                // Mostrar el buffer en pantalla
                al_flip_display();
                redraw = false;
            }
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // El usuario cerró la ventana
            running = false;
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Manejo de eventos del ratón: añadir punto con botón izquierdo
            if (ev.mouse.button & 1) { // botón izquierdo
                Point p{(float)ev.mouse.x, (float)ev.mouse.y};
                points.push_back(p);
                // Recalcular la envolvente cada vez que se añade un punto
                hull = convex_hull(points);
                redraw = true;
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Manejo de teclado: ESC salir, C limpiar, R generar aleatorios
            int k = ev.keyboard.keycode;
            if (k == ALLEGRO_KEY_ESCAPE) {
                running = false;
            } else if (k == ALLEGRO_KEY_C) {
                points.clear();
                hull.clear();
                redraw = true;
            } else if (k == ALLEGRO_KEY_R) {
                // Generar puntos aleatorios dentro de un margen para evitar
                // que queden fuera de la ventana.
                points.clear();
                int N = 30; // por defecto
                for (int i = 0; i < N; ++i) {
                    Point p{(float)(50 + std::rand() % (WIDTH - 100)), (float)(50 + std::rand() % (HEIGHT - 100))};
                    points.push_back(p);
                }
                hull = convex_hull(points);
                redraw = true;
            }
        }
    }

    // limpieza
    if (font) al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}
