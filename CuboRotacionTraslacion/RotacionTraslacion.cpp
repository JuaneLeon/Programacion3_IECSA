// RotacionTraslacion.cpp
// Programa en C++ con Allegro5 que dibuja un cubo proporcionado (lados iguales),
// permite desplazarlo con las flechas y rotarlo a la izquierda/derecha
// en un solo plano (rotación alrededor del eje Y).
// Controles:
//  Flechas: mover el cubo (arriba/abajo/izquierda/derecha)
//  A / D : rotar a la izquierda / derecha
//  R     : reset posición y rotación
//  ESC   : salir
// Compilación (Windows + MinGW, Allegro5 instalado):
// g++ -std=c++11 RotacionTraslacion.cpp -o RotacionTraslacion.exe `pkg-config --libs --cflags allegro-5 allegro_primitives-5`

#include <cmath>
#include <vector>
#include <utility>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

struct Vec3 { double x, y, z; };

int main() {
	const int WIDTH = 800;
	const int HEIGHT = 600;
	if (!al_init()) return -1;
	al_install_keyboard();
	al_init_primitives_addon();

	ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
	if (!display) return -1;

	// Inicializar font addon para poder dibujar texto de ayuda
	al_init_font_addon();

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));

	// Definimos un cubo centrado en el origen con lado = size
	const double size = 200.0; // longitud de arista en unidades del mundo
	const double half = size / 2.0;
	std::vector<Vec3> vertices = {
		{-half, -half, -half}, { half, -half, -half}, { half,  half, -half}, {-half,  half, -half},
		{-half, -half,  half}, { half, -half,  half}, { half,  half,  half}, {-half,  half,  half}
	};
	// Aristas del cubo (pares de índices)
	std::vector<std::pair<int,int>> edges = {
		{0,1},{1,2},{2,3},{3,0}, // cara trasera
		{4,5},{5,6},{6,7},{7,4}, // cara frontal
		{0,4},{1,5},{2,6},{3,7}  // conexiones entre caras
	};

	// Estado de la escena
	double rotAngle = 0.0; // ángulo en radianes — rotación alrededor del eje Z (giro en el plano)
	double tx = 0.0, ty = 0.0; // traslación en píxeles (aplicada después de proyectar)
	const double move_speed = 6.0; // píxeles por tick
	const double rot_speed = 0.04; // rad/tick
	const double scale = 1.0; // escala uniforme global (si se cambia, se mantiene uniforme)

	// Crear fuente builtin (una vez)
	ALLEGRO_FONT* font = al_create_builtin_font();

	bool running = true;
	bool redraw = true;
	al_start_timer(timer);

	while (running) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			// loop de lógica en cada tick; procesar estado de teclado para movimiento/rotación continua
			ALLEGRO_KEYBOARD_STATE kstate;
			al_get_keyboard_state(&kstate);

			if (al_key_down(&kstate, ALLEGRO_KEY_LEFT))  tx -= move_speed;
			if (al_key_down(&kstate, ALLEGRO_KEY_RIGHT)) tx += move_speed;
			if (al_key_down(&kstate, ALLEGRO_KEY_UP))    ty -= move_speed;
			if (al_key_down(&kstate, ALLEGRO_KEY_DOWN))  ty += move_speed;

			if (al_key_down(&kstate, ALLEGRO_KEY_A)) rotAngle -= rot_speed;
			if (al_key_down(&kstate, ALLEGRO_KEY_D)) rotAngle += rot_speed;

			redraw = true;
		} else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			// solo acciones puntuales
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_ESCAPE: running = false; break;
				case ALLEGRO_KEY_R: // reset
					rotAngle = 0.0; tx = 0.0; ty = 0.0; break;
				default: break;
			}
		} else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			running = false;
		}

		if (redraw && al_is_event_queue_empty(queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(30,30,30));

			// Centro de la pantalla
			double cx = WIDTH / 2.0;
			double cy = HEIGHT / 2.0;

			// Transformar y proyectar vértices: rotación en Y, proyección ortográfica
			std::vector<std::pair<double,double>> screen(vertices.size());
			for (size_t i = 0; i < vertices.size(); ++i) {
				double x = vertices[i].x * scale;
				double y = vertices[i].y * scale;
				double z = vertices[i].z * scale;

				// Rotación alrededor de Z (giro en el plano X-Y)
				double cosA = cos(rotAngle);
				double sinA = sin(rotAngle);
				double xr = x * cosA - y * sinA;
				double yr = x * sinA + y * cosA;

				// z permanece igual; lo usamos para dar un ligero efecto de profundidad
				double zr = z;
				double depth_effect = zr * 0.25;

				double sx = cx + xr + tx + depth_effect;
				double sy = cy + yr + ty - depth_effect;

				screen[i] = {sx, sy};
			}

			// Dibujar aristas
			for (auto &e : edges) {
				auto p1 = screen[e.first];
				auto p2 = screen[e.second];
				al_draw_line((float)p1.first, (float)p1.second, (float)p2.first, (float)p2.second,
							 al_map_rgb(220,220,220), 2.0f);
			}

			// Dibujar aristas ocultas con tono más oscuro (opcional)
			// (No calculamos visibilidad; solo damos un feedback visual distinto para la "frontalidad")

			// Texto de ayuda
			al_draw_filled_rectangle(10, 10, 300, 86, al_map_rgba(0,0,0,140));
			if (font) {
				al_draw_textf(font, al_map_rgb(200,200,200), 16, 14, 0,
							  "Flechas: mover | A/D: rotar | R: reset | ESC: salir");
			}

			al_flip_display();
		}
	}

	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	if (font) al_destroy_font(font);
	al_shutdown_primitives_addon();
	al_shutdown_font_addon();
	return 0;
}

