#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

//Contantes de Configuración de la esfera
const int ANCHO_PANTALLA = 900;
const int ALTO_PANTALLA = 600;
const float RADIO_ESFERA = 140.0f;
const int NUM_PUNTOS = 150;      // Aumentado para mayor detalle y consumo de recursos
const int NUM_HEBRAS_AURORA = 100; // Aumentado para mayor detalle y consumo de recursos
const float DISTANCIA_CONEXION = 60.0f;

//Estructuras de Datos
struct HebraAurora {
    float anguloBase, fase, velocidad, amplitud, longitud, desplazamientoColor, desplazamientoRuido;
};

// Estructura para representar un punto 3D con su posición original y parámetros de animación
struct Punto3D {
    float x, y, z;
    float xOriginal, yOriginal, zOriginal;
    float fase, velocidad;
};

// Estructura principal de la esfera
struct Esfera {
    float centroX, centroY;
    float velX, velY;
    float rotacionY, rotacionX;
    float tiempo, respiracion, estiramientoX, estiramientoY, velEstiramientoX, velEstiramientoY;
    float progresoFormacion;
    bool estaFormandose;
    float faseAurora, intensidadAurora;
    int r, g, b; // Color base del núcleo
    std::vector<Punto3D> puntos;
    std::vector<HebraAurora> hebrasAurora;
};

// Estructura para almacenar puntos preparados para dibujo con profundidad
struct PuntoDibujo {
    float z;
    Punto3D rotado;
    float pantallaX, pantallaY;
    bool operator<(const PuntoDibujo& otro) const { return z < otro.z; }
};

// Funciones para la lógica de la esfera
//Funcion para limitar un entero entre dos valores
static inline int limitarEntero(int v, int a = 0, int b = 255) {
    return (v < a) ? a : (v > b) ? b : v;
}

// Ruido Simple para variaciones orgánicas
float ruidoSimple(float x, float y) {
    return sinf(x * 1.3f + y * 0.7f) * 0.5f + sinf(x * 2.1f - y * 1.4f) * 0.25f + sinf(x * 0.8f + y * 2.3f) * 0.125f;
}

// Rotación de un punto 3D alrededor de los ejes X e Y
Punto3D rotarPunto(Punto3D p, float rotX, float rotY) {
    Punto3D resultado;
    float x1 = p.x * cosf(rotY) - p.z * sinf(rotY);
    float z1 = p.x * sinf(rotY) + p.z * cosf(rotY);
    resultado.x = x1;
    resultado.y = p.y * cosf(rotX) - z1 * sinf(rotX);
    resultado.z = p.y * sinf(rotX) + z1 * cosf(rotX);
    return resultado;
}

//Logica Principal de la Esfera
void inicializarEsfera(Esfera& esfera) {
    esfera.centroX = ANCHO_PANTALLA / 2.0f;
    esfera.centroY = ALTO_PANTALLA / 2.0f;
    esfera.velX = 0; esfera.velY = 0;
    esfera.rotacionY = 0; esfera.rotacionX = 0;
    esfera.tiempo = 0; esfera.respiracion = 1.0f;
    esfera.estiramientoX = 1.0f; esfera.estiramientoY = 1.0f;
    esfera.velEstiramientoX = 0; esfera.velEstiramientoY = 0;
    esfera.progresoFormacion = 0.0f; esfera.estaFormandose = true;
    esfera.faseAurora = 0.0f; esfera.intensidadAurora = 0.0f;

    // Esquema de color fijo
    esfera.r = 255; esfera.g = 140; esfera.b = 50;

    // Inicialización de los tentáculos de aurora
    esfera.hebrasAurora.clear();
    for (int i = 0; i < NUM_HEBRAS_AURORA; i++) {
        esfera.hebrasAurora.push_back({
            (float)i / NUM_HEBRAS_AURORA * 2.0f * (float)ALLEGRO_PI,
            (float)rand() / RAND_MAX * 2.0f * (float)ALLEGRO_PI,
            0.4f + (float)rand() / RAND_MAX * 0.6f,
            35.0f + (float)rand() / RAND_MAX * 45.0f,
            120.0f + (float)rand() / RAND_MAX * 80.0f,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX * 100.0f
        });
    }

    // Inicialización de los puntos 3D en la superficie de la esfera
    esfera.puntos.clear();
    const float anguloDorado = (float)ALLEGRO_PI * (3.0f - sqrtf(5.0f));
    for (int i = 0; i < NUM_PUNTOS; i++) {
        Punto3D p;
        float y = 1.0f - (i / (float)(NUM_PUNTOS - 1)) * 2.0f;
        float radio = sqrtf(1.0f - y * y);
        float theta = anguloDorado * i;
        p.xOriginal = cosf(theta) * radio * RADIO_ESFERA;
        p.yOriginal = y * RADIO_ESFERA;
        p.zOriginal = sinf(theta) * radio * RADIO_ESFERA;
        float distAleatoria = 300.0f + (float)rand() / RAND_MAX * 200.0f;
        float anguloAleatorio = (float)rand() / RAND_MAX * 2.0f * (float)ALLEGRO_PI;
        float alturaAleatoria = ((float)rand() / RAND_MAX - 0.5f) * 400.0f;
        p.x = cosf(anguloAleatorio) * distAleatoria;
        p.y = alturaAleatoria;
        p.z = sinf(anguloAleatorio) * distAleatoria;
        p.fase = (float)rand() / RAND_MAX * 2.0f * (float)ALLEGRO_PI;
        p.velocidad = 0.3f + (float)rand() / RAND_MAX * 0.5f;
        esfera.puntos.push_back(p);
    }
}

// Actualización de la lógica de la esfera basada en el movimiento de la ventana
void actualizarEsfera(Esfera& esfera, int prevWX, int prevWY, int currWX, int currWY, float dt) {
    float deltaX = (float)(currWX - prevWX);
    float deltaY = (float)(currWY - prevWY);
    float acelX = deltaX * -0.15f;
    float acelY = deltaY * -0.15f;

    // Actualización de la posición y velocidad de la esfera
    esfera.velX += acelX; esfera.velY += acelY;
    esfera.velX += -esfera.velX * 0.25f; esfera.velY += -esfera.velY * 0.25f;
    esfera.velX *= 0.80f; esfera.velY *= 0.80f;

    // Actualización del estiramiento basado en el movimiento
    float fuerza = sqrtf(deltaX * deltaX + deltaY * deltaY);
    if (fuerza > 0.5f) {
        float dirX = deltaX / fuerza; float dirY = deltaY / fuerza;
        esfera.velEstiramientoX -= dirX * fuerza * 0.003f;
        esfera.velEstiramientoY -= dirY * fuerza * 0.003f;
    }

    // Limitación de la velocidad de estiramiento
    esfera.estiramientoX += esfera.velEstiramientoX; esfera.estiramientoY += esfera.velEstiramientoY;
    esfera.velEstiramientoX += (1.0f - esfera.estiramientoX) * 0.20f; esfera.velEstiramientoY += (1.0f - esfera.estiramientoY) * 0.20f;
    esfera.velEstiramientoX *= 0.75f; esfera.velEstiramientoY *= 0.75f;
    esfera.estiramientoX = fmaxf(0.85f, fminf(1.15f, esfera.estiramientoX));
    esfera.estiramientoY = fmaxf(0.85f, fminf(1.15f, esfera.estiramientoY));

    // Actualización de la rotación y otros parámetros
    esfera.rotacionY += deltaX * 0.003f + dt * 0.15f;
    esfera.rotacionX += deltaY * 0.003f;
    esfera.tiempo += dt;
    esfera.respiracion = 1.0f + sinf(esfera.tiempo * 1.2f) * 0.05f;
    esfera.faseAurora += dt * 1.2f;
    float movimiento = sqrtf(deltaX * deltaX + deltaY * deltaY);
    esfera.intensidadAurora = fminf(esfera.intensidadAurora * 0.92f + movimiento * 0.08f, 4.0f);

    // Actualización de la formación de la esfera
    if (esfera.estaFormandose) {
        esfera.progresoFormacion += dt * 0.4f;
        if (esfera.progresoFormacion >= 1.0f) { esfera.progresoFormacion = 1.0f; esfera.estaFormandose = false; }
    }

    // Actualización de la posición de los puntos 3D
    for (size_t i = 0; i < esfera.puntos.size(); i++) {
        Punto3D& p = esfera.puntos[i];
        float onda = sinf(esfera.tiempo * p.velocidad + p.fase) * 2.0f;
        float onda2 = cosf(esfera.tiempo * p.velocidad * 1.3f + p.fase) * 2.0f;
        float objetivoX = (p.xOriginal * esfera.estiramientoX + onda) * esfera.respiracion;
        float objetivoY = (p.yOriginal * esfera.estiramientoY + onda2) * esfera.respiracion;
        float objetivoZ = p.zOriginal * esfera.respiracion;
        float retrasoPunto = (float)i / NUM_PUNTOS * 0.3f;
        float progresoPunto = fmaxf(0.0f, fminf(1.0f, (esfera.progresoFormacion - retrasoPunto) / 0.7f));
        progresoPunto = 1.0f - (1.0f - progresoPunto) * (1.0f - progresoPunto);
        p.x += (objetivoX - p.x) * progresoPunto * 0.15f;
        p.y += (objetivoY - p.y) * progresoPunto * 0.15f;
        p.z += (objetivoZ - p.z) * progresoPunto * 0.15f;
    }
}

// Dibujar la esfera
void dibujarEsfera(Esfera& esfera, float escala = 1.0f, float offsetX = 0.0f, float offsetY = 0.0f) {
    float centroX = (esfera.centroX + esfera.velX) * escala + offsetX;
    float centroY = (esfera.centroY + esfera.velY) * escala + offsetY;
    float tiempo = al_get_time(); // Usar un tiempo global para sincronizar animaciones

    //Resplandor Exterior
    if (esfera.progresoFormacion > 0.1f) {
        float alfaResplandorExterior = std::min(1.0f, (esfera.progresoFormacion - 0.1f) / 0.9f);
        for (int resplandor = 80; resplandor > 0; resplandor--) {
            float t = (float)resplandor / 80.0f;
            float radioResplandor = RADIO_ESFERA * 0.5f + powf(t, 0.4f) * 300.0f;
            int alfa = limitarEntero((int)(powf(1.0f - t, 2.0f) * 4.5f * alfaResplandorExterior));
            float tiempoColor = tiempo * 0.4f;
            float desplazamientoTono = t * 1.5f; // Desfase de color
            int r = (int)(155.0f + 100.0f * sinf(tiempoColor + desplazamientoTono));
            int g = (int)(155.0f + 100.0f * sinf(tiempoColor + desplazamientoTono + 2.0f * (float)ALLEGRO_PI / 3.0f));
            int b = (int)(155.0f + 100.0f * sinf(tiempoColor + desplazamientoTono + 4.0f * (float)ALLEGRO_PI / 3.0f));
            al_draw_filled_circle(centroX, centroY, radioResplandor * escala, al_map_rgba(r, g, b, alfa));
        }
    }

    //Tentáculos
    if (esfera.progresoFormacion > 0.25f) {
        float alfaAurora = std::min(1.0f, (esfera.progresoFormacion - 0.25f) / 0.75f);

        // Dibuja un resplandor en la base de los tentáculos para difuminar su origen
        float radioResplandorNucleo = RADIO_ESFERA * 1.2f;
        for (int i = 0; i < 15; i++) {
            float t = (float)i / 14.0f;
            float radio = RADIO_ESFERA * (1.0f - t) + radioResplandorNucleo * t;
            float alfa = powf(1.0f - t, 2.0f) * 30.0f * alfaAurora;
            float tiempoColor = tiempo * 0.4f;
            int r = limitarEntero((int)(155.0f + 100.0f * sinf(tiempoColor)));
            int g = limitarEntero((int)(155.0f + 100.0f * sinf(tiempoColor + 2.0f * (float)ALLEGRO_PI / 3.0f)));
            int b = limitarEntero((int)(155.0f + 100.0f * sinf(tiempoColor + 4.0f * (float)ALLEGRO_PI / 3.0f)));
            al_draw_filled_circle(centroX, centroY, radio * escala, al_map_rgba(r, g, b, (int)alfa));
        }

        // Dibuja los tentáculos de aurora
        for (size_t i = 0; i < esfera.hebrasAurora.size(); i++) {
            HebraAurora& hebra = esfera.hebrasAurora[i];
            int segmentos = 30;
            std::vector<float> puntosX(segmentos), puntosY(segmentos);
            for (int seg = 0; seg < segmentos; seg++) {
                float t = (float)seg / (segmentos - 1);
                float angulo = hebra.anguloBase + esfera.faseAurora * 0.1f;
                float radio = RADIO_ESFERA + t * hebra.longitud;
                float ruidoX = ruidoSimple(t * 2.0f + tiempo * 0.3f + hebra.desplazamientoRuido, tiempo * 0.5f);
                float ruidoY = ruidoSimple(t * 3.0f + tiempo * 0.4f + hebra.desplazamientoRuido + 10.0f, tiempo * 0.6f);
                float onda1 = sinf(t * 4.0f + tiempo * hebra.velocidad + hebra.fase) * hebra.amplitud * t;
                float ondaCortina = sinf(t * 1.5f + tiempo * 0.4f + hebra.fase) * 40.0f * t * t;
                float ondaMovimiento = esfera.intensidadAurora * sinf(angulo * 3.0f + tiempo * 1.5f) * t * 12.0f;
                float ondaTotal = onda1 + ondaCortina + ondaMovimiento + ruidoX * 25.0f * t;
                puntosX[seg] = centroX + cosf(angulo) * radio * escala + sinf(angulo) * ondaTotal * escala;
                puntosY[seg] = centroY + sinf(angulo) * radio * escala - cosf(angulo) * ondaTotal * escala + ruidoY * 15.0f * t * escala;
            }

            // Dibuja las líneas entre los puntos
            for (int seg = 0; seg < segmentos - 1; seg++) {
                float t = (float)seg / (segmentos - 1);
                float desvanecimientoAlfa = (1.0f - powf(t, 0.4f)) * (0.75f + 0.25f * sinf(tiempo * 1.5f + t * 3.0f));
                int alfa = limitarEntero((int)(150 * desvanecimientoAlfa * alfaAurora), 0, 210);
                float tSuave = t * t * (3.0f - 2.0f * t);
                float faseColor = hebra.desplazamientoColor + tiempo * 0.15f + tSuave * 0.8f;
                float ondaColor = (sinf(faseColor) + 1.0f) * 0.5f;
                int r = limitarEntero((255 - esfera.r) + (int)(ondaColor * 100) - 50);
                int g = limitarEntero((255 - esfera.g) + (int)(ondaColor * 100) - 50);
                int b = limitarEntero((255 - esfera.b) + (int)(ondaColor * 100) - 50);
                float grosor = (2.2f - tSuave * 1.7f) * (1.0f + sinf(tiempo * 0.8f + t * 2.0f) * 0.15f);
                al_draw_line(puntosX[seg], puntosY[seg], puntosX[seg + 1], puntosY[seg + 1], al_map_rgba(r, g, b, alfa), grosor);
                if (t < 0.4f) {
                    int alfaResplandor = limitarEntero((int)(alfa * 0.4f * (1.0f - t * 2.5f)));
                    al_draw_line(puntosX[seg], puntosY[seg], puntosX[seg + 1], puntosY[seg + 1], al_map_rgba(r + 50, g + 50, b + 50, alfaResplandor), grosor + 1.5f);
                }
            }
        }
    }

    //Núcleo Difuminado
    float alfaFormacionNucleo = powf(esfera.progresoFormacion, 2.0f);
    for (int i = 15; i > 0; i--) {
        float t = (float)i / 15.0f;
        float radio = RADIO_ESFERA * 0.95f * t;
        int alfa = limitarEntero((int)(powf(1.0f - t, 2.0f) * 40.0f * alfaFormacionNucleo));
        float tiempoColor = tiempo * 0.4f;
        int r = limitarEntero((int)(155.0f + 100.0f * sinf(tiempoColor)) + (int)(100 * (1.0f - t)));
        int g = limitarEntero((int)(155.0f + 100.0f * sinf(tiempoColor + 2.0f * (float)ALLEGRO_PI / 3.0f)) + (int)(100 * (1.0f - t)));
        int b = limitarEntero((int)(155.0f + 100.0f * sinf(tiempoColor + 4.0f * (float)ALLEGRO_PI / 3.0f)) + (int)(100 * (1.0f - t)));
        al_draw_filled_circle(centroX, centroY, radio * escala, al_map_rgba(r, g, b, alfa));
    }

    //Red de Puntos y Conexiones
    std::vector<PuntoDibujo> puntosParaDibujar;
    for (const auto& p : esfera.puntos) {
        PuntoDibujo pd;
        pd.rotado = rotarPunto(p, esfera.rotacionX, esfera.rotacionY);
        pd.z = pd.rotado.z;
        float perspectiva = 350.0f / (350.0f + pd.z);
        pd.pantallaX = centroX + pd.rotado.x * perspectiva * escala;
        pd.pantallaY = centroY + pd.rotado.y * perspectiva * escala;
        puntosParaDibujar.push_back(pd);
    }
    std::sort(puntosParaDibujar.begin(), puntosParaDibujar.end());
    float alfaPunto = powf(esfera.progresoFormacion, 2.0f);
    const float distConexionSq = (DISTANCIA_CONEXION * escala) * (DISTANCIA_CONEXION * escala);
    for (size_t i = 0; i < puntosParaDibujar.size(); ++i) {
        for (size_t j = i + 1; j < puntosParaDibujar.size(); ++j) {
            float dx = puntosParaDibujar[i].pantallaX - puntosParaDibujar[j].pantallaX;
            float dy = puntosParaDibujar[i].pantallaY - puntosParaDibujar[j].pantallaY;
            if ((dx * dx + dy * dy) < distConexionSq) {
                al_draw_line(puntosParaDibujar[i].pantallaX, puntosParaDibujar[i].pantallaY, puntosParaDibujar[j].pantallaX, puntosParaDibujar[j].pantallaY, al_map_rgba(255, 255, 255, 30 * alfaPunto), 1.0f);
            }
        }
    }
    for (const auto& pd : puntosParaDibujar) {
        float radio = fmaxf(0.5f, (1.0f - fmaxf(0, fminf(1, pd.z / RADIO_ESFERA))) * 2.8f * escala);
        al_draw_filled_circle(pd.pantallaX, pd.pantallaY, radio, al_map_rgba(255, 255, 255, 180 * alfaPunto));
    }
}

//Main y Lógica de IPC (Inter-Process Communication)
int main() {
    srand((unsigned)time(nullptr));

    //Inicialización de Allegro
    if (!al_init()) { std::cerr << "No se pudo iniciar Allegro\n"; return -1; }
    al_init_primitives_addon();
    al_install_mouse();

    ALLEGRO_DISPLAY* pantalla = al_create_display(ANCHO_PANTALLA, ALTO_PANTALLA);
    if (!pantalla) { std::cerr << "No se pudo crear la ventana\n"; return -1; }
    al_set_window_title(pantalla, "Esfera Neural");
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    ALLEGRO_TIMER* temporizador = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* colaEventos = al_create_event_queue();
    al_register_event_source(colaEventos, al_get_display_event_source(pantalla));
    al_register_event_source(colaEventos, al_get_timer_event_source(temporizador));
    al_register_event_source(colaEventos, al_get_mouse_event_source());

    Esfera esfera;
    inicializarEsfera(esfera);

    int prevWX, prevWY;
    al_get_window_position(pantalla, &prevWX, &prevWY);

    al_start_timer(temporizador);
    bool enEjecucion = true;
    bool redibujar = true;
    const float dt = 1.0f / 60.0f;

    //Bucle Principal
    while (enEjecucion) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(colaEventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { enEjecucion = false; }
        
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            redibujar = true;

            int currWX, currWY;
            al_get_window_position(pantalla, &currWX, &currWY);

            actualizarEsfera(esfera, prevWX, prevWY, currWX, currWY, dt);

            prevWX = currWX;
            prevWY = currWY;
        }

        // Dibujo
        if (redibujar && al_is_event_queue_empty(colaEventos)) {
            redibujar = false;
            al_clear_to_color(al_map_rgb(8, 8, 10));

            dibujarEsfera(esfera);

            al_flip_display();
        }
    }

    //Limpieza de Allegro
    al_destroy_display(pantalla);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(colaEventos);

    return 0;
}