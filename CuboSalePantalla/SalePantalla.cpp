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
	al_init_primitives_addon();

	ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
	if (!display) return -1;

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
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

	// Estado de la escena (ahora automático)
	double rotAngle = 0.0; // ángulo en radianes — rotación alrededor del eje Z (giro en el plano)
	double tx = 0.0, ty = 0.0; // traslación en píxeles (aplicada después de proyectar)
	const double scale = 1.0; // escala uniforme global (si se cambia, se mantiene uniforme)

	// Velocidades automáticas (pixels/tick y rad/tick)
	double vx = 3.5; // velocidad horizontal
	double vy = 2.5; // velocidad vertical
	double rotVel = 0.03; // velocidad angular (rad/tick)

	// No hay control por teclado: movimiento y rotación son automáticos

	bool running = true;
	bool redraw = true;
	al_start_timer(timer);

	while (running) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			// Movimiento y rotación automáticos
			tx += vx;
			ty += vy;
			rotAngle += rotVel;

			// Comprobar colisiones con los bordes de la pantalla basadas en los vértices proyectados
			double cx = WIDTH / 2.0;
			double cy = HEIGHT / 2.0;

			double minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9;
			for (size_t i = 0; i < vertices.size(); ++i) {
				double x = vertices[i].x * scale;
				double y = vertices[i].y * scale;
				double z = vertices[i].z * scale;

				double cosA = cos(rotAngle);
				double sinA = sin(rotAngle);
				double xr = x * cosA - y * sinA;
				double yr = x * sinA + y * cosA;
				double zr = z;
				double depth_effect = zr * 0.25;

				double sx = cx + xr + tx + depth_effect;
				double sy = cy + yr + ty - depth_effect;

				if (sx < minX) minX = sx;
				if (sx > maxX) maxX = sx;
				if (sy < minY) minY = sy;
				if (sy > maxY) maxY = sy;
			}

			// Si el cubo sale completamente por un lado, aparecerá por el lado opuesto (wrap-around)
			double bboxW = maxX - minX;
			double bboxH = maxY - minY;

			// Ajuste de wrap-around por múltiplos de pantalla (±WIDTH / ±HEIGHT)
			// Usamos bucles para soportar velocidades altas que puedan cruzar
			// más de una pantalla en un único tick.
			while (maxX < 0) {
				tx += WIDTH;
				maxX += WIDTH; minX += WIDTH; // actualizar bbox referencias
			}
			while (minX > WIDTH) {
				tx -= WIDTH;
				maxX -= WIDTH; minX -= WIDTH;
			}

			while (maxY < 0) {
				ty += HEIGHT;
				maxY += HEIGHT; minY += HEIGHT;
			}
			while (minY > HEIGHT) {
				ty -= HEIGHT;
				maxY -= HEIGHT; minY -= HEIGHT;
			}

			redraw = true;
		} else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			running = false;
		}

		if (redraw && al_is_event_queue_empty(queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(30,30,30));

			// Centro de la pantalla
			double cx = WIDTH / 2.0;
			double cy = HEIGHT / 2.0;

			// Dibujar hasta 9 copias (vecindario) con alpha según cuánto de su caja
			// proyectada esté dentro de la ventana — produce aparición/desaparición gradual.
			auto clamp01 = [](double v){ if (v < 0.0) return 0.0; if (v > 1.0) return 1.0; return v; };

			for (int ox = -1; ox <= 1; ++ox) {
				for (int oy = -1; oy <= 1; ++oy) {
					// calcular las proyecciones con offset de pantalla
					std::vector<std::pair<double,double>> screen(vertices.size());
					double minSX = 1e9, maxSX = -1e9, minSY = 1e9, maxSY = -1e9;
					for (size_t i = 0; i < vertices.size(); ++i) {
						double x = vertices[i].x * scale;
						double y = vertices[i].y * scale;
						double z = vertices[i].z * scale;

						double cosA = cos(rotAngle);
						double sinA = sin(rotAngle);
						double xr = x * cosA - y * sinA;
						double yr = x * sinA + y * cosA;
						double zr = z;
						double depth_effect = zr * 0.25;

						double sx = cx + xr + tx + ox * WIDTH + depth_effect;
						double sy = cy + yr + ty + oy * HEIGHT - depth_effect;

						screen[i] = {sx, sy};
						if (sx < minSX) minSX = sx;
						if (sx > maxSX) maxSX = sx;
						if (sy < minSY) minSY = sy;
						if (sy > maxSY) maxSY = sy;
					}

					// calcular fracción dentro de la ventana (ejes separados)
					double bboxW = maxSX - minSX;
					double bboxH = maxSY - minSY;
					double xInside = 0.0, yInside = 0.0;
					if (bboxW > 0.0) {
						double left = std::max(minSX, 0.0);
						double right = std::min(maxSX, (double)WIDTH);
						xInside = clamp01((right - left) / bboxW);
					}
					if (bboxH > 0.0) {
						double top = std::max(minSY, 0.0);
						double bottom = std::min(maxSY, (double)HEIGHT);
						yInside = clamp01((bottom - top) / bboxH);
					}

					double alpha = xInside * yInside;
					if (alpha <= 0.0) continue;

					ALLEGRO_COLOR col = al_map_rgba_f(1.0f, 1.0f, 1.0f, (float)alpha);
					for (auto &e : edges) {
						auto p1 = screen[e.first];
						auto p2 = screen[e.second];
						al_draw_line((float)p1.first, (float)p1.second, (float)p2.first, (float)p2.second,
									 col, 2.0f);
					}
				}
			}

			// Dibujar aristas ocultas con tono más oscuro (opcional)
			// (No calculamos visibilidad; solo damos un feedback visual distinto para la "frontalidad")

			// (sin texto de ayuda — movimiento automático)
			al_flip_display();
		}
	}

	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	al_shutdown_primitives_addon();
	return 0;
}

