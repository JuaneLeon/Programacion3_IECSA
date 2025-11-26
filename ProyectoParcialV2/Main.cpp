#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <tuple>

//Contantes de Configuración de la esfera
const int ANCHO_PANTALLA = 900;
const int ALTO_PANTALLA = 600;
// Estas variables se ajustan al tamaño real del display (permite fullscreen)
int DISPLAY_W = ANCHO_PANTALLA;
int DISPLAY_H = ALTO_PANTALLA;
const float RADIO_ESFERA = 140.0f;
const int NUM_PUNTOS = 150;      // Aumentado para mayor detalle y consumo de recursos
const int NUM_HEBRAS_AURORA = 100; // Aumentado para mayor detalle y consumo de recursos
const float DISTANCIA_CONEXION = 60.0f;
// Parámetros de núcleo (ajustables para obtener aspecto más pulido)
const int NUM_CORE_BLOBS = 8;           // cantidad de blobs en el núcleo (lava/slime)
const int CORE_PARTICLES = 900;        // partículas finas dentro del núcleo (aumentado)
const float CORE_FILAMENT_DIST = 220.0f; // distancia máxima para crear filamentos entre blobs
const int CORE_FILAMENT_STEPS = 20;    // pasos para dibujar filamentos (más suaves)
// Rayos del núcleo (antimateria)
const int CORE_RAYS = 140; // número de rayos dispersos
// Parámetros visuales adicionales
const float VIGNETTE_INTENSITY = 0.38f;

//Estructuras de Datos
struct HebraAurora {
    float anguloBase, fase, velocidad, amplitud, longitud, desplazamientoColor, desplazamientoRuido;
};

// Blob para núcleo tipo lava/slime
struct Blob {
    float x, y, z;
    float r;
    float phase, speed;
    float colorShift;
};

// Estructura para representar un punto 3D con su posición original y parámetros de animación
struct Punto3D {
    float x, y, z;
    float xOriginal, yOriginal, zOriginal;
    float fase, velocidad;
    // para Verlet
    float prevX, prevY, prevZ;
    float mass;
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
    std::vector<Blob> blobs;
    // rayos dispersos en el núcleo (se moverán desde el interior hacia la esfera)
    struct CoreRay {
        float sx, sy, sz; // origen cerca del núcleo
        float cx, cy, cz; // posición actual del extremo
        float dx, dy, dz; // dirección normalizada
        float speed;      // velocidad de avance
        float traveled;   // distancia recorrida
        float maxDist;    // distancia objetivo (hasta la esfera)
        bool white;       // true = blanco, false = negro (contraste)
        // parámetros para comportamiento errático y render
        float seed;       // semilla para ruido/jitter
        float width;      // grosor visual aproximado
        float branchProb; // probabilidad de ramificar
        float intensity;  // intensidad/flicker
    };
    std::vector<CoreRay> coreRays;
    // Posición de la ventana en pantalla (para interacción multi-instancia)
    int windowX, windowY;
};

// Estructura para almacenar puntos preparados para dibujo con profundidad
struct PuntoDibujo {
    float z;
    Punto3D rotado;
    float pantallaX, pantallaY;
    bool operator<(const PuntoDibujo& otro) const { return z < otro.z; }
};

// Cámara global simple
struct Camera {
    float rotX = 0.0f; // inclinación
    float rotY = 0.0f; // giro
    float distance = 700.0f;
    float panX = 0.0f;
    float panY = 0.0f;
    // zoom smoothing
    float targetDistance = 700.0f;
    float zoomSmooth = 0.12f;
    float minDistance = 120.0f;
    float maxDistance = 2000.0f;
};

// variable global de cámara (definida en este fichero)
Camera globalCamera;

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
    esfera.centroX = DISPLAY_W / 2.0f;
    esfera.centroY = DISPLAY_H / 2.0f;
    esfera.velX = 0; esfera.velY = 0;
    esfera.rotacionY = 0; esfera.rotacionX = 0;
    esfera.tiempo = 0; esfera.respiracion = 1.0f;
    esfera.estiramientoX = 1.0f; esfera.estiramientoY = 1.0f;
    esfera.velEstiramientoX = 0; esfera.velEstiramientoY = 0;
    esfera.progresoFormacion = 0.0f; esfera.estaFormandose = true;
    esfera.faseAurora = 0.0f; esfera.intensidadAurora = 0.0f;
        esfera.windowX = 0; esfera.windowY = 0;

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
        p.prevX = p.x; p.prevY = p.y; p.prevZ = p.z;
        p.mass = 1.0f + (float)rand() / RAND_MAX * 0.6f;
        esfera.puntos.push_back(p);
    }

    // Inicializar blobs del núcleo tipo lava/slime
    esfera.blobs.clear();
    int numBlobs = NUM_CORE_BLOBS;
    float coreRadius = RADIO_ESFERA * 0.62f; // núcleo más grande
    for (int i = 0; i < numBlobs; ++i) {
        Blob b;
        float theta = (float)rand() / RAND_MAX * 2.0f * (float)ALLEGRO_PI;
        float phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        float dist = (float)rand() / RAND_MAX * (coreRadius * 0.45f);
        b.x = sinf(phi) * cosf(theta) * dist;
        b.y = cosf(phi) * dist;
        b.z = sinf(phi) * sinf(theta) * dist;
        b.r = coreRadius * (0.35f + (float)rand() / RAND_MAX * 0.30f); // blobs más grandes
        b.phase = (float)rand() / RAND_MAX * 2.0f * (float)ALLEGRO_PI;
        b.speed = 0.3f + (float)rand() / RAND_MAX * 0.6f;
        b.colorShift = (float)rand() / RAND_MAX;
        esfera.blobs.push_back(b);
    }

    // Inicializar rayos del núcleo: se generan partiendo cerca del centro y apuntando a la superficie
    esfera.coreRays.clear();
    float coreRadius2 = coreRadius * 0.9f;
    for (int i = 0; i < CORE_RAYS; ++i) {
        Esfera::CoreRay r;
        // origen cerca del centro (ligeramente aleatorio)
        float orad = ((float)rand() / RAND_MAX) * (coreRadius2 * 0.28f);
        float theta = ((float)rand() / RAND_MAX) * 2.0f * (float)ALLEGRO_PI;
        float phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        r.sx = sinf(phi) * cosf(theta) * orad;
        r.sy = cosf(phi) * orad * 0.6f; // ligeramente aplanado
        r.sz = sinf(phi) * sinf(theta) * orad;
        // objetivo: punto en la superficie de la esfera (posicionar en RADIO_ESFERA)
        float st = ((float)rand() / RAND_MAX) * 2.0f * (float)ALLEGRO_PI;
        float sp = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        float tx = sinf(sp) * cosf(st) * RADIO_ESFERA;
        float ty = cosf(sp) * RADIO_ESFERA;
        float tz = sinf(sp) * sinf(st) * RADIO_ESFERA;
        float vx = tx - r.sx; float vy = ty - r.sy; float vz = tz - r.sz;
        float mag = sqrtf(vx*vx + vy*vy + vz*vz) + 1e-6f;
        r.dx = vx / mag; r.dy = vy / mag; r.dz = vz / mag;
        r.cx = r.sx; r.cy = r.sy; r.cz = r.sz;
        r.traveled = 0.0f; r.maxDist = mag;
        r.speed = 40.0f + (float)rand() / RAND_MAX * 120.0f; // velocidad variable
        r.white = (rand() % 2 == 0);
        r.seed = ((float)rand() / RAND_MAX) * 6.28318f;
        r.width = 1.0f + ((float)rand() / RAND_MAX) * 3.5f;
        r.branchProb = 0.06f + ((float)rand() / RAND_MAX) * 0.12f;
        r.intensity = 0.6f + ((float)rand() / RAND_MAX) * 0.8f;
        esfera.coreRays.push_back(r);
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
    // actualizar posición de ventana
    esfera.windowX = currWX;
    esfera.windowY = currWY;
    float movimiento = sqrtf(deltaX * deltaX + deltaY * deltaY);
    esfera.intensidadAurora = fminf(esfera.intensidadAurora * 0.92f + movimiento * 0.08f, 4.0f);

    // Actualización de la formación de la esfera
    if (esfera.estaFormandose) {
        esfera.progresoFormacion += dt * 0.4f;
        if (esfera.progresoFormacion >= 1.0f) { esfera.progresoFormacion = 1.0f; esfera.estaFormandose = false; }
    }

    // Actualización de la posición de los puntos 3D mediante Verlet (física simple)
    for (size_t i = 0; i < esfera.puntos.size(); i++) {
        Punto3D& p = esfera.puntos[i];
        // objetivo suave (tendencia hacia la posición en la esfera con oscilaciones)
        float onda = sinf(esfera.tiempo * p.velocidad + p.fase) * 2.0f;
        float onda2 = cosf(esfera.tiempo * p.velocidad * 1.3f + p.fase) * 2.0f;
        float objetivoX = (p.xOriginal * esfera.estiramientoX + onda) * esfera.respiracion;
        float objetivoY = (p.yOriginal * esfera.estiramientoY + onda2) * esfera.respiracion;
        float objetivoZ = p.zOriginal * esfera.respiracion;

        // calcular fuerza de resorte hacia objetivo
        float k = 8.0f; // rigidez
        float fx = (objetivoX - p.x) * k;
        float fy = (objetivoY - p.y) * k;
        float fz = (objetivoZ - p.z) * k;

        // interacción por mouse: si la ventana se está moviendo rápido simular perturbación pequeña
        // (se puede sustituir por arrastre real con mouse)
        float ax = fx / p.mass;
        float ay = fy / p.mass;
        float az = fz / p.mass;

        // Verlet integration: x_new = x + (x - prevX) * damping + a * dt^2
        float damping = 0.985f;
        float nextX = p.x + (p.x - p.prevX) * damping + ax * dt * dt;
        float nextY = p.y + (p.y - p.prevY) * damping + ay * dt * dt;
        float nextZ = p.z + (p.z - p.prevZ) * damping + az * dt * dt;

        p.prevX = p.x; p.prevY = p.y; p.prevZ = p.z;
        p.x = nextX; p.y = nextY; p.z = nextZ;
    }

    // Actualizar blobs del núcleo: movimientos suaves tipo lava lamp
    for (size_t i = 0; i < esfera.blobs.size(); ++i) {
        Blob& b = esfera.blobs[i];
        // movimiento vertical oscilante
        float vy = sinf(esfera.tiempo * b.speed + b.phase) * 8.0f;
        float vx = cosf(esfera.tiempo * (b.speed * 0.6f) + b.phase * 0.7f) * 4.0f;
        float vz = sinf(esfera.tiempo * (b.speed * 0.5f) + b.phase * 1.3f) * 3.5f;
        b.x += vx * 0.02f;
        b.y += vy * 0.02f;
        b.z += vz * 0.02f;

        // atraer ligeramente hacia el centro para evitar que se escapen
        float atrx = -b.x * 0.01f;
        float atry = -b.y * 0.01f;
        float atrz = -b.z * 0.01f;
        b.x += atrx; b.y += atry; b.z += atrz;

        // mantener dentro del núcleo (ajustado al nuevo radio)
        float maxDist = RADIO_ESFERA * 0.62f * 0.95f;
        float dist = sqrtf(b.x*b.x + b.y*b.y + b.z*b.z);
        if (dist > maxDist) {
            b.x *= maxDist / (dist + 1e-6f);
            b.y *= maxDist / (dist + 1e-6f);
            b.z *= maxDist / (dist + 1e-6f);
        }
    }

    // Actualizar rayos del núcleo: avanzar hacia la superficie, regenerar al llegar
    for (size_t ri = 0; ri < esfera.coreRays.size(); ++ri) {
        auto &r = esfera.coreRays[ri];
        // movimiento con ligera curvatura controlada por ruido para aspecto orgánico
        float step = r.speed * dt;
        // perturbar dirección con ruido suave
        float nx = ruidoSimple(r.cx * 0.02f + r.seed, esfera.tiempo * 1.8f + ri * 0.13f);
        float ny = ruidoSimple(r.cy * 0.025f - r.seed, esfera.tiempo * 1.6f + ri * 0.27f);
        float nz = ruidoSimple(r.cz * 0.018f + r.seed * 0.7f, esfera.tiempo * 1.9f + ri * 0.19f);
        // pequeñas correcciones a la dirección
        r.dx += nx * 0.018f;
        r.dy += ny * 0.018f;
        r.dz += nz * 0.018f;
        // normalizar dirección
        float magd = sqrtf(r.dx*r.dx + r.dy*r.dy + r.dz*r.dz) + 1e-6f;
        r.dx /= magd; r.dy /= magd; r.dz /= magd;

        // avanzar punta con velocidad variable (flicker)
        float flick = 0.85f + sinf(esfera.tiempo * 18.0f + r.seed * 6.0f) * 0.25f * r.intensity;
        float realStep = step * flick;
        r.cx += r.dx * realStep;
        r.cy += r.dy * realStep;
        r.cz += r.dz * realStep;
        r.traveled += realStep;

        // jitter posicional adicional
        r.cx += sinf(esfera.tiempo * 6.0f + ri * 1.9f + r.seed) * 0.4f * r.intensity;
        r.cy += cosf(esfera.tiempo * 5.2f + ri * 1.3f + r.seed) * 0.35f * r.intensity;
        r.cz += sinf(esfera.tiempo * 4.8f + ri * 2.7f + r.seed) * 0.38f * r.intensity;

        // probabilidad de estallar/regenerar antes de llegar a la superficie (picos erráticos)
        if (r.traveled >= r.maxDist || ((rand() % 1000) < 10 && (rand() % 1000) < (int)(r.intensity*200.0f))) {
            // a veces generar un pulso: incrementar velocidad momentánea de algunos rayos
            if ((rand() % 100) < 25) {
                r.speed *= 1.6f + ((float)rand() / RAND_MAX) * 1.6f;
            }
            // regenerar rayo
            float orad = ((float)rand() / RAND_MAX) * (RADIO_ESFERA * 0.62f * 0.28f);
            float theta = ((float)rand() / RAND_MAX) * 2.0f * (float)ALLEGRO_PI;
            float phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
            r.sx = sinf(phi) * cosf(theta) * orad;
            r.sy = cosf(phi) * orad * 0.6f;
            r.sz = sinf(phi) * sinf(theta) * orad;
            float st = ((float)rand() / RAND_MAX) * 2.0f * (float)ALLEGRO_PI;
            float sp = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
            float tx = sinf(sp) * cosf(st) * RADIO_ESFERA;
            float ty = cosf(sp) * RADIO_ESFERA;
            float tz = sinf(sp) * sinf(st) * RADIO_ESFERA;
            float vx = tx - r.sx; float vy = ty - r.sy; float vz = tz - r.sz;
            float mag = sqrtf(vx*vx + vy*vy + vz*vz) + 1e-6f;
            r.dx = vx / mag; r.dy = vy / mag; r.dz = vz / mag;
            r.cx = r.sx; r.cy = r.sy; r.cz = r.sz;
            r.traveled = 0.0f; r.maxDist = mag;
            r.speed = 40.0f + (float)rand() / RAND_MAX * 120.0f;
            r.seed = ((float)rand() / RAND_MAX) * 6.28318f;
            r.width = 1.0f + ((float)rand() / RAND_MAX) * 3.5f;
            r.branchProb = 0.06f + ((float)rand() / RAND_MAX) * 0.12f;
            r.intensity = 0.6f + ((float)rand() / RAND_MAX) * 0.8f;
            r.white = (rand() % 2 == 0);
        }
    }
}

void init_opengl(int ancho, int alto) {
    glViewport(0, 0, ancho, alto);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)ancho / (double)alto, 1.0, 3000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(4.0f);
    glClearColor(0.03f, 0.03f, 0.04f, 1.0f);

    // Inicializar textura para bloom (captura del framebuffer)
    static GLuint bloomTex = 0;
    if (bloomTex == 0) {
        glGenTextures(1, &bloomTex);
        glBindTexture(GL_TEXTURE_2D, bloomTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Iluminación simple para dar más volumen a los blobs
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightAmbient[]  = { 0.06f, 0.06f, 0.07f, 1.0f };
    GLfloat lightDiffuse[]  = { 0.85f, 0.85f, 0.9f, 1.0f };
    GLfloat lightSpecular[] = { 0.9f, 0.9f, 1.0f, 1.0f };
    GLfloat lightPos[]      = { 0.0f, 200.0f, 300.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

// Simple bloom: captura framebuffer a textura y dibuja varias capas aditivas desplazadas
static void apply_simple_bloom(int w, int h) {
    static GLuint tex = 0;
    if (tex == 0) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // copiar el framebuffer actual a la textura
    glBindTexture(GL_TEXTURE_2D, tex);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, w, h, 0);

    // dibujar la textura varias veces con blending aditivo y pequeños offsets para simular blur
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // parámetros de efecto
    const int passes = 6;
    const float maxOffset = 18.0f;
    for (int p = 0; p < passes; ++p) {
        float t = (float)p / (float)(passes - 1);
        float offs = maxOffset * t * 0.6f;
        float alpha = 0.09f * (1.0f - t);
        float sx = offs; float sy = offs * 0.5f;

        glColor4f(1.0f, 1.0f, 1.0f, alpha);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f - sx, 0.0f - sy);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(w - sx, 0.0f - sy);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(w - sx, h - sy);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f - sx, h - sy);
        glEnd();
    }

    // Restaurar estados
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// (Se eliminaron las chispas para un aspecto más limpio según petición)

// Dibujar viñeta suave en pantalla para dar acabado profesional
static void draw_vignette(float intensidad) {
    // Cambiar a ortografía 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, DISPLAY_W, DISPLAY_H, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // dibujar un quad con gradiente radial simple (con varios anillos de alpha)
    float cx = DISPLAY_W * 0.5f;
    float cy = DISPLAY_H * 0.5f;
    int steps = 48;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.02f, 0.02f, 0.03f, intensidad * 0.0f);
    glVertex2f(cx, cy);
    for (int i = 0; i <= steps; ++i) {
        float a = (float)i / (float)steps * 6.28318f;
        float vx = cx + cosf(a) * DISPLAY_W * 1.1f;
        float vy = cy + sinf(a) * DISPLAY_H * 1.1f;
        float alpha = 0.0f + intensidad * 0.6f * (0.5f + 0.5f * sinf(a * 3.0f + i));
        glColor4f(0.02f, 0.02f, 0.03f, alpha);
        glVertex2f(vx, vy);
    }
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

// Interacción multi-instancia (detección por archivos y dibujo del "agujero de gusano")
// fue removida: la lógica de escritura/lectura de estados y la función
// `process_peers_and_draw` ya no existen en esta versión.

// Helper: dibuja varias capas esféricas translúcidas para simular un núcleo volumétrico 3D
static void draw_sphere_shells(const Esfera& esfera, float tiempo) {
    const int lonSegs = 24;
    const int latSegs = 12;
    float baseR = RADIO_ESFERA * 0.95f;
    float formFactor = powf(esfera.progresoFormacion, 2.0f);
    for (int layer = 5; layer >= 1; --layer) {
        float t = (float)layer / 5.0f;
        float radio = baseR * t * (1.0f + 0.06f * sinf(tiempo * 1.2f + layer));
        float alpha = (1.0f - t) * 0.25f * formFactor;
        float r = esfera.r / 255.0f, g = esfera.g / 255.0f, b = esfera.b / 255.0f;

        for (int lat = 0; lat < latSegs; ++lat) {
            float lat0 = (float)lat / latSegs * (float)ALLEGRO_PI - (float)ALLEGRO_PI / 2.0f;
            float lat1 = (float)(lat + 1) / latSegs * (float)ALLEGRO_PI - (float)ALLEGRO_PI / 2.0f;
            glBegin(GL_TRIANGLE_STRIP);
            for (int lon = 0; lon <= lonSegs; ++lon) {
                float lonf = (float)lon / lonSegs * 2.0f * (float)ALLEGRO_PI;
                float x0 = cosf(lat0) * cosf(lonf);
                float y0 = sinf(lat0);
                float z0 = cosf(lat0) * sinf(lonf);
                float x1 = cosf(lat1) * cosf(lonf);
                float y1 = sinf(lat1);
                float z1 = cosf(lat1) * sinf(lonf);

                // color ligeramente desplazado por latitud para dar variación
                float shade = 0.6f + 0.4f * (0.5f + 0.5f * y0);
                glColor4f(r * shade, g * shade, b * shade, alpha * (0.9f + 0.1f * t));
                glVertex3f(x0 * radio, y0 * radio, z0 * radio);

                shade = 0.6f + 0.4f * (0.5f + 0.5f * y1);
                glColor4f(r * shade, g * shade, b * shade, alpha * (0.6f + 0.4f * t));
                glVertex3f(x1 * radio, y1 * radio, z1 * radio);
            }
            glEnd();
        }
    }
}

// Dibujar capas de humo alrededor de la esfera para simular un resplandor tipo niebla
static void draw_smoke_shells(const Esfera& esfera, float tiempo) {
    // Más capas y más resolución para un humo que envuelve la esfera
    const int layers = 9;
    const int lonSegs = 72;
    const int latSegs = 30;
    float baseOuter = RADIO_ESFERA * 1.05f;

    // Desactivar escritura en depth buffer para mejor blending del humo
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int layer = 0; layer < layers; ++layer) {
        float tl = (float)layer / (layers - 1);
        // cada capa gira a diferente velocidad para evitar patrones visibles
        float rot = tiempo * (0.05f + 0.02f * layer);
        float radius = baseOuter + tl * RADIO_ESFERA * 1.1f + sinf(tiempo * (0.12f + 0.03f * layer)) * 8.0f;
        // capas interiores más densas, exteriores más suaves
        float alphaLayer = 0.45f * (1.0f - tl) * powf(esfera.progresoFormacion, 2.0f);

        for (int lat = 0; lat < latSegs; ++lat) {
            float lat0 = (float)lat / latSegs * (float)ALLEGRO_PI - (float)ALLEGRO_PI / 2.0f;
            float lat1 = (float)(lat + 1) / latSegs * (float)ALLEGRO_PI - (float)ALLEGRO_PI / 2.0f;
            glBegin(GL_TRIANGLE_STRIP);
            for (int lon = 0; lon <= lonSegs; ++lon) {
                float lonf = (float)lon / lonSegs * 2.0f * (float)ALLEGRO_PI;

                // Rotación aplicada para animación suave
                float lonRot = lonf + rot + tl * 0.6f;

                float nx0 = cosf(lat0) * cosf(lonRot);
                float ny0 = sinf(lat0);
                float nz0 = cosf(lat0) * sinf(lonRot);
                float nx1 = cosf(lat1) * cosf(lonRot);
                float ny1 = sinf(lat1);
                float nz1 = cosf(lat1) * sinf(lonRot);

                // Ruido a diferentes escalas para variación orgánica
                float ruido0 = ruidoSimple(nx0 * 2.2f + tiempo * 0.18f + layer * 1.1f, nz0 * 2.6f + tiempo * 0.12f);
                float ruido1 = ruidoSimple(nx1 * 2.2f + tiempo * 0.18f + layer * 1.1f, nz1 * 2.6f + tiempo * 0.12f);

                float r0 = radius + ruido0 * 26.0f * (0.7f + 0.3f * (1.0f - tl));
                float r1 = radius + ruido1 * 26.0f * (0.7f + 0.3f * (1.0f - tl));

                // tonos de humo (neon purpura/cian) para un aspecto más vibrante
                float shade0 = 0.55f + 0.45f * (0.5f + 0.5f * ny0);
                float shade1 = 0.55f + 0.45f * (0.5f + 0.5f * ny1);
                float alpha0 = alphaLayer * (0.95f + 0.2f * (1.0f - tl)) * (0.9f + 0.6f * ruido0);
                float alpha1 = alphaLayer * (0.95f + 0.2f * (1.0f - tl)) * (0.9f + 0.6f * ruido1);

                // Color purpura/cian suave
                float cr0 = 0.45f * shade0 + 0.25f * (0.5f + 0.5f * sinf(tiempo * 0.5f));
                float cg0 = 0.38f * shade0;
                float cb0 = 0.9f * shade0 + 0.2f * (0.5f + 0.5f * cosf(tiempo * 0.3f));
                float cr1 = 0.45f * shade1 + 0.25f * (0.5f + 0.5f * sinf(tiempo * 0.5f));
                float cg1 = 0.38f * shade1;
                float cb1 = 0.9f * shade1 + 0.2f * (0.5f + 0.5f * cosf(tiempo * 0.3f));

                glColor4f(cr0, cg0, cb0, alpha0);
                glVertex3f(nx0 * r0, ny0 * r0, nz0 * r0);

                glColor4f(cr1, cg1, cb1, alpha1);
                glVertex3f(nx1 * r1, ny1 * r1, nz1 * r1);
            }
            glEnd();
        }
    }

    // Restaurar estado
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

// Dibujar un núcleo interno volumétrico en forma de nube de partículas
static void draw_core(const Esfera& esfera, float tiempo) {
    // Núcleo tipo lava/slime: varios blobs volumétricos suaves que se mezclan
    float coreRadius = RADIO_ESFERA * 0.62f * (0.6f + 0.4f * esfera.respiracion);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Usar gluSphere para dibujar blobs suaves y volumétricos
    static GLUquadric* quad = nullptr;
    if (!quad) quad = gluNewQuadric();

    // --- Núcleo estilo "Xbox intro": núcleo glossy con bandas animadas ---
    // Procedural: dibujado con malla lon/lat para calcular color por vértice
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    const int lonSegsCore = 48;
    const int latSegsCore = 28;
    float bandSpeed = 1.8f; // velocidad de desplazamiento de bandas
    float bandCount = 8.0f; // cantidad de bandas alrededor
    float gloss = 1.6f; // intensidad especular adicional
    float baseCoreScale = coreRadius * 0.38f; // tamaño del núcleo glossy

    for (int lat = 0; lat < latSegsCore; ++lat) {
        float lat0 = (float)lat / latSegsCore * (float)ALLEGRO_PI - (float)ALLEGRO_PI / 2.0f;
        float lat1 = (float)(lat + 1) / latSegsCore * (float)ALLEGRO_PI - (float)ALLEGRO_PI / 2.0f;
        glBegin(GL_TRIANGLE_STRIP);
        for (int lon = 0; lon <= lonSegsCore; ++lon) {
            float lonf = (float)lon / lonSegsCore * 2.0f * (float)ALLEGRO_PI;

            // vértice 1
            float nx0 = cosf(lat0) * cosf(lonf);
            float ny0 = sinf(lat0);
            float nz0 = cosf(lat0) * sinf(lonf);
            float vx0 = nx0 * baseCoreScale;
            float vy0 = ny0 * baseCoreScale * 0.86f; // ligero aplastamiento vertical
            float vz0 = nz0 * baseCoreScale;

            // banda procedural: función senoidal según longitud + latitud
            float band0 = 0.5f + 0.5f * sinf(lonf * bandCount + tiempo * bandSpeed + ny0 * 3.2f);
            // realce central móvil (barrido) para simular reflexión dinámica
            float sweep0 = 0.5f + 0.5f * sinf(tiempo * 2.2f + lonf * 0.8f);

            // color base verde tipo "Xbox" (mezcla dark/bright)
            float darkG = 0.06f;
            float brightG = 0.9f;
            float r0 = 0.06f + 0.14f * band0;
            float g0 = darkG * (1.0f - band0) + brightG * band0 * (0.7f + 0.3f * sweep0);
            float b0 = 0.06f + 0.16f * band0;

            // aproximación de especular usando componente Z de la normal (viewer aproximado)
            float viewDot0 = fmaxf(0.0f, nz0);
            float spec0 = powf(viewDot0, 28.0f) * gloss * (0.6f + 0.4f * sweep0);
            float rim0 = powf(1.0f - viewDot0, 3.2f) * 0.28f;

            float alpha0 = 0.95f * (0.8f + 0.2f * band0);
            // combinar color + specular + rim
            float cr0 = fminf(1.0f, r0 + spec0 + rim0 * 0.25f);
            float cg0 = fminf(1.0f, g0 + spec0 + rim0 * 0.35f);
            float cb0 = fminf(1.0f, b0 + spec0 + rim0 * 0.45f);

            glColor4f(cr0, cg0, cb0, alpha0);
            glVertex3f(vx0, vy0, vz0);

            // vértice 2
            float nx1 = cosf(lat1) * cosf(lonf);
            float ny1 = sinf(lat1);
            float nz1 = cosf(lat1) * sinf(lonf);
            float vx1 = nx1 * baseCoreScale;
            float vy1 = ny1 * baseCoreScale * 0.86f;
            float vz1 = nz1 * baseCoreScale;
            float band1 = 0.5f + 0.5f * sinf(lonf * bandCount + tiempo * bandSpeed + ny1 * 3.2f);
            float sweep1 = 0.5f + 0.5f * sinf(tiempo * 2.2f + lonf * 0.8f);
            float r1 = 0.06f + 0.14f * band1;
            float g1 = darkG * (1.0f - band1) + brightG * band1 * (0.7f + 0.3f * sweep1);
            float b1 = 0.06f + 0.16f * band1;
            float viewDot1 = fmaxf(0.0f, nz1);
            float spec1 = powf(viewDot1, 28.0f) * gloss * (0.6f + 0.4f * sweep1);
            float rim1 = powf(1.0f - viewDot1, 3.2f) * 0.28f;
            float alpha1 = 0.95f * (0.8f + 0.2f * band1);
            float cr1 = fminf(1.0f, r1 + spec1 + rim1 * 0.25f);
            float cg1 = fminf(1.0f, g1 + spec1 + rim1 * 0.35f);
            float cb1 = fminf(1.0f, b1 + spec1 + rim1 * 0.45f);
            glColor4f(cr1, cg1, cb1, alpha1);
            glVertex3f(vx1, vy1, vz1);
        }
        glEnd();
    }

    // Cinturones brillantes rotativos (anillos) para asemejar el estilo del intro
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (int a = 0; a < 140; ++a) {
        float ang = (float)a / 140.0f * 2.0f * (float)ALLEGRO_PI + tiempo * 0.9f;
        float rad = baseCoreScale * (0.48f + 0.06f * sinf(tiempo * 1.2f + a));
        float x = cosf(ang) * rad;
        float y = sinf(ang) * rad * 0.12f;
        float z = sinf(ang * 1.1f + tiempo * 0.2f) * rad * 0.28f;
        float glow = 0.45f + 0.55f * (0.5f + 0.5f * sinf(tiempo * 2.0f + a * 0.12f));
        glColor4f(0.2f * glow, 0.95f * glow, 0.35f * glow, 0.14f);
        glVertex3f(x, y, z);
    }
    glEnd();

    // restaurar estado de luz mezclado pero permitir que blobs y partículas sigan
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);

    int lon = 28, lat = 18;
    // Dibujar cada blob con varias capas (nucleo brillante, cuerpo y halo suave)
    for (size_t i = 0; i < esfera.blobs.size(); ++i) {
        const Blob& b = esfera.blobs[i];
        // calcular color base (puedes tunear para lava: rojo/amarillo o slime: verde/azul)
        float hueBase = fmodf(0.3f + b.colorShift * 0.5f + sinf(esfera.faseAurora * 0.4f + i * 0.7f) * 0.06f, 1.0f);
        float rb = 0.3f + 0.7f * sinf(6.28318f * (hueBase + 0.0f));
        float gb = 0.55f + 0.45f * sinf(6.28318f * (hueBase + 0.33f));
        float bb = 0.4f + 0.6f * sinf(6.28318f * (hueBase + 0.66f));

        // Capa 1: núcleo brillante pequeño
        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        float innerScale = b.r * 0.5f * (0.9f + 0.15f * sinf(tiempo * b.speed + b.phase));
        glColor4f(rb * 1.3f, gb * 1.2f, bb * 1.1f, 0.55f * powf(esfera.progresoFormacion, 1.6f));
        glScalef(innerScale, innerScale, innerScale);
        gluSphere(quad, 1.0, lon, lat);
        glPopMatrix();

        // Capa 2: cuerpo principal
        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        float midScale = b.r * (0.85f + 0.18f * sinf(tiempo * (b.speed * 0.8f) + b.phase * 0.9f));
        glColor4f(rb, gb, bb, 0.36f * powf(esfera.progresoFormacion, 1.4f));
        glScalef(midScale, midScale, midScale);
        gluSphere(quad, 1.0, lon, lat);
        glPopMatrix();

        // Capa 3: halo suave con puntos grandes para glow
        glPointSize(8.0f + (i % 3) * 2.0f);
        float haloAlpha = 0.12f * (1.2f - (float)i / (float)esfera.blobs.size()) * (0.8f + 0.2f * sinf(tiempo * 1.2f + i));
        glBegin(GL_POINTS);
        glColor4f(rb * 0.9f, gb * 1.0f, bb * 1.05f, haloAlpha);
        // Dibujar varios puntos alrededor para dar halo difuso
        for (int p = 0; p < 6; ++p) {
            float ang = p * (2.0f * (float)ALLEGRO_PI / 6.0f) + tiempo * 0.2f * (p % 2 ? 1.0f : -1.0f);
            float rx = b.x + cosf(ang) * b.r * 0.9f;
            float ry = b.y + sinf(ang) * b.r * 0.35f;
            float rz = b.z + sinf(ang * 0.7f) * b.r * 0.6f;
            glVertex3f(rx, ry, rz);
        }
        glEnd();
    }

    // Filamentos: pequeñas cápsulas o series de esferas entre blobs cercanos
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (size_t i = 0; i < esfera.blobs.size(); ++i) {
        for (size_t j = i + 1; j < esfera.blobs.size(); ++j) {
            const Blob& a = esfera.blobs[i];
            const Blob& b = esfera.blobs[j];
            float dx = b.x - a.x; float dy = b.y - a.y; float dz = b.z - a.z;
            float d = sqrtf(dx*dx + dy*dy + dz*dz);
            if (d < CORE_FILAMENT_DIST) {
                // colores de los extremos (mezcla entre blob a y b)
                float hueA = fmodf(0.3f + a.colorShift * 0.5f + sinf(esfera.faseAurora * 0.4f + i * 0.7f) * 0.06f, 1.0f);
                float ra = 0.3f + 0.7f * sinf(6.28318f * (hueA + 0.0f));
                float ga = 0.55f + 0.45f * sinf(6.28318f * (hueA + 0.33f));
                float ba = 0.4f + 0.6f * sinf(6.28318f * (hueA + 0.66f));
                float hueB = fmodf(0.3f + b.colorShift * 0.5f + sinf(esfera.faseAurora * 0.4f + j * 0.7f) * 0.06f, 1.0f);
                float rb_ = 0.3f + 0.7f * sinf(6.28318f * (hueB + 0.0f));
                float gb_ = 0.55f + 0.45f * sinf(6.28318f * (hueB + 0.33f));
                float bb_ = 0.4f + 0.6f * sinf(6.28318f * (hueB + 0.66f));

                // dibujar una linea de pequeñas esferas entre a y b
                for (int s = 0; s <= CORE_FILAMENT_STEPS; ++s) {
                    float t = (float)s / (float)CORE_FILAMENT_STEPS;
                    float ix = a.x + dx * t + sinf(tiempo * 2.0f + i * 0.7f) * 2.0f * (0.5f - t);
                    float iy = a.y + dy * t + cosf(tiempo * 1.5f + j * 0.5f) * 1.6f * (t - 0.5f);
                    float iz = a.z + dz * t + sinf(tiempo * 1.8f + (i+j) * 0.9f) * 1.2f * (t);
                    float scale = (a.r * 0.18f * (1.0f - fabsf(0.5f - t) * 1.8f));
                    glPushMatrix();
                    glTranslatef(ix, iy, iz);
                    float rcol = (ra + rb_) * 0.5f;
                    float gcol = (ga + gb_) * 0.5f;
                    float bcol = (ba + bb_) * 0.5f;
                    glColor4f(rcol, gcol, bcol, 0.12f * (1.0f - t));
                    glScalef(scale, scale, scale);
                    gluSphere(quad, 1.0, 8, 6);
                    glPopMatrix();
                }
            }
        }
    }

    // Añadir partículas finas para textura y transiciones (más densas)
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    int partículas = CORE_PARTICLES;
    for (int i = 0; i < partículas; ++i) {
        float fi = (float)i;
        float seed = sinf(fi * 12.9898f + tiempo * 1.234f) * 43758.5453f;
        float r1 = fabsf(seed - floorf(seed));
        seed = sinf(fi * 78.233f + tiempo * 0.618f) * 43758.5453f;
        float r2 = fabsf(seed - floorf(seed));

        float theta = r1 * 2.0f * (float)ALLEGRO_PI;
        float phi = acosf(2.0f * r2 - 1.0f);
        float radial = powf((float)rand() / RAND_MAX, 0.7f) * coreRadius * (0.45f + 0.55f * esfera.progresoFormacion);

        float x = sinf(phi) * cosf(theta) * radial;
        float y = cosf(phi) * radial;
        float z = sinf(phi) * sinf(theta) * radial;

        float n = ruidoSimple(x * 0.045f + tiempo * 0.9f, z * 0.045f - tiempo * 0.5f);
        float hue = fmodf(0.33f + n * 0.25f + esfera.faseAurora * 0.03f, 1.0f);
        float rf = 0.28f + 0.66f * sinf(6.28318f * hue);
        float gf = 0.5f + 0.5f * sinf(6.28318f * (hue + 0.33f));
        float bf = 0.33f + 0.55f * sinf(6.28318f * (hue + 0.66f));

        float dist = sqrtf(x*x + y*y + z*z) / (coreRadius + 1e-6f);
        float alpha = (1.0f - dist) * 0.28f * (0.7f + 0.3f * n) * powf(esfera.progresoFormacion, 2.2f);
        glColor4f(rf, gf, bf, alpha);
        glVertex3f(x, y, z);
    }
    glEnd();
    glEnd();

    // Bloom/additive pass: puntos grandes alrededor de cada blob para resplandor
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPointSize(14.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < esfera.blobs.size(); ++i) {
        const Blob& b = esfera.blobs[i];
        float hueBase = fmodf(0.3f + b.colorShift * 0.5f + sinf(esfera.faseAurora * 0.4f + i * 0.7f) * 0.06f, 1.0f);
        float rb_ = 0.3f + 0.7f * sinf(6.28318f * (hueBase + 0.0f));
        float gb_ = 0.55f + 0.45f * sinf(6.28318f * (hueBase + 0.33f));
        float bb_ = 0.4f + 0.6f * sinf(6.28318f * (hueBase + 0.66f));
        float alphaGlow = 0.08f * (1.4f - (float)i / (float)esfera.blobs.size()) * powf(esfera.progresoFormacion, 1.2f);
        glColor4f(rb_ * 1.2f, gb_ * 1.1f, bb_ * 1.05f, alphaGlow);
        glVertex3f(b.x, b.y, b.z);
    }
    glEnd();

    // Restaurar blending normal para resto de escena
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // ---------------------- RAYOS DEL NUCLEO (ANTIMATERIA) ----------------------
    // Dibujar rayos que parten desde el núcleo y terminan en la esfera. Blanco/negro caótico.
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glLineWidth(2.0f);
    for (size_t ri = 0; ri < esfera.coreRays.size(); ++ri) {
        const auto &r = esfera.coreRays[ri];
        // construir una serie de segmentos entre sx..(cx) con jitter para simular trazado real
        float dxTot = r.cx - r.sx;
        float dyTot = r.cy - r.sy;
        float dzTot = r.cz - r.sz;
        float segDist = sqrtf(dxTot*dxTot + dyTot*dyTot + dzTot*dzTot) + 1e-6f;
        int segments = 6 + (int)(fminf(18.0f, (r.traveled / (r.maxDist + 1e-6f)) * 12.0f));

        // preparar color base
        float baseRcol = r.white ? 1.0f : 0.05f;
        float baseGcol = r.white ? 1.0f : 0.05f;
        float baseBcol = r.white ? 1.0f : 0.05f;

        // dibujar línea principal como line strip con alpha decreciente
        glBegin(GL_LINE_STRIP);
        for (int s = 0; s <= segments; ++s) {
            float t = (float)s / (float)segments;
            // posición base a lo largo del rayo
            float px = r.sx + r.dx * (t * r.traveled);
            float py = r.sy + r.dy * (t * r.traveled);
            float pz = r.sz + r.dz * (t * r.traveled);
            // jitter perpendicular usando ruido trigonométrico
            float amp = r.width * (1.0f - t) * 0.9f;
            float jx = sinf(r.seed + t * 12.0f + esfera.tiempo * 10.0f) * amp * 0.6f;
            float jy = cosf(r.seed * 1.3f - t * 9.0f + esfera.tiempo * 9.0f) * amp * 0.5f;
            float jz = sinf(r.seed * 0.7f + t * 7.0f - esfera.tiempo * 8.0f) * amp * 0.55f;
            float vx = px + jx, vy = py + jy, vz = pz + jz;

            float alpha = (1.0f - t) * 0.95f * r.intensity;
            glColor4f(baseRcol * 0.9f, baseGcol * 0.9f, baseBcol * 0.95f, alpha);
            glVertex3f(vx, vy, vz);
        }
        glEnd();

        // dibujar puntos brillantes a lo largo del rayo para dar volumen
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        for (int s = 0; s <= segments; ++s) {
            float t = (float)s / (float)segments;
            float px = r.sx + r.dx * (t * r.traveled);
            float py = r.sy + r.dy * (t * r.traveled);
            float pz = r.sz + r.dz * (t * r.traveled);
            float amp = r.width * (1.0f - t) * 0.9f;
            float jx = sinf(r.seed + t * 12.0f + esfera.tiempo * 10.0f) * amp * 0.6f;
            float jy = cosf(r.seed * 1.3f - t * 9.0f + esfera.tiempo * 9.0f) * amp * 0.5f;
            float jz = sinf(r.seed * 0.7f + t * 7.0f - esfera.tiempo * 8.0f) * amp * 0.55f;
            float vx = px + jx, vy = py + jy, vz = pz + jz;
            float alpha = (1.0f - t) * 0.9f * r.intensity;
            if (r.white) glColor4f(1.0f, 1.0f, 1.0f, alpha);
            else glColor4f(0.02f, 0.02f, 0.02f, alpha + 0.2f);
            glVertex3f(vx, vy, vz);
        }
        glEnd();

        // rama ocasional desde un punto intermedio (simula bifurcaciones)
        float branchNoise = 0.5f + 0.5f * sinf(esfera.tiempo * 2.3f + r.seed * 4.0f);
        if (branchNoise < r.branchProb) {
            int mid = segments / 2;
            float t = (float)mid / (float)segments;
            float px = r.sx + r.dx * (t * r.traveled);
            float py = r.sy + r.dy * (t * r.traveled);
            float pz = r.sz + r.dz * (t * r.traveled);
            // crear dirección perpendicular aproximada
            float bx = -r.dy;
            float by = r.dx;
            float bz = 0.0f;
            float blen = sqrtf(bx*bx + by*by + bz*bz) + 1e-6f;
            bx /= blen; by /= blen; bz /= blen;
            float branchLen = 40.0f + ((float)rand() / RAND_MAX) * 120.0f;
            glBegin(GL_LINE_STRIP);
            for (int b = 0; b < 6; ++b) {
                float bt = (float)b / 5.0f;
                float j = sinf(r.seed * 3.0f + bt * 6.0f + esfera.tiempo * 6.0f) * (1.0f - bt) * 6.0f;
                float vx = px + bx * (bt * branchLen) + j;
                float vy = py + by * (bt * branchLen) + j * 0.6f;
                float vz = pz + bz * (bt * branchLen) + j * 0.4f;
                float alpha = (1.0f - bt) * 0.6f * r.intensity;
                if (r.white) glColor4f(1.0f, 1.0f, 1.0f, alpha); else glColor4f(0.02f,0.02f,0.02f,alpha);
                glVertex3f(vx, vy, vz);
            }
            glEnd();
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
}

// Dibujar resplandor tipo aurora con degradado y cintas animadas
static void draw_aurora_glow(const Esfera& esfera, float tiempo) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    int maxHebras = std::min((int)esfera.hebrasAurora.size(), 60);

    for (int h = 0; h < maxHebras; ++h) {
        const HebraAurora& hebra = esfera.hebrasAurora[h];
        // cada hebra será una tira con varios segmentos
        int segments = 28;
        float baseRadius = RADIO_ESFERA * (1.02f + 0.15f * (sinf(hebra.fase + tiempo * hebra.velocidad)));

        glBegin(GL_TRIANGLE_STRIP);
        for (int s = 0; s <= segments; ++s) {
            float t = (float)s / (float)segments;
            // Longitud y desplazamiento animado
            float ang = hebra.anguloBase + t * hebra.longitud * 0.12f + sinf(hebra.desplazamientoRuido + tiempo * 0.6f + h * 0.07f) * 0.4f;
            // altura sobre la esfera
            float elev = sinf(t * (float)ALLEGRO_PI) * hebra.amplitud * (0.6f + 0.4f * sinf(tiempo * hebra.velocidad + hebra.fase));

            // posición central de la cinta
            float nx = cosf(ang);
            float ny = elev / (RADIO_ESFERA * 2.0f);
            float nz = sinf(ang);

            // dos bordes de la cinta (in/out radial)
            float outR = baseRadius + 22.0f * (0.4f + 0.6f * t) + ruidoSimple(nx * 2.0f + tiempo * 0.2f, nz * 2.0f) * 8.0f;
            float inR = baseRadius - 12.0f * (0.6f + 0.4f * (1.0f - t)) + ruidoSimple(nx * 1.5f - tiempo * 0.15f, nz * 1.5f) * 6.0f;

            float cxOut = nx * outR; float cyOut = ny * outR; float czOut = nz * outR;
            float cxIn = nx * inR; float cyIn = ny * inR; float czIn = nz * inR;

            // color degradado tipo aurora (neon): mezcla cian <-> magenta para estética futurista
            float mixv = 0.5f + 0.5f * sinf(esfera.faseAurora * 0.8f + t * 3.4f + h * 0.14f);
            float rcol = 0.35f * (1.0f - mixv) + 0.95f * mixv; // magenta tendency
            float gcol = 0.15f * (1.0f - mixv) + 0.7f * mixv;
            float bcol = 0.6f * (1.0f - mixv) + 1.0f * mixv; // neon cyan

            float alphaOut = 0.38f * (1.0f - t) * (0.8f + 0.6f * esfera.intensidadAurora);
            float alphaIn = 0.18f * (t) * (0.8f + 0.6f * esfera.intensidadAurora);

            glColor4f(rcol, gcol, bcol, alphaOut);
            glVertex3f(cxOut, cyOut, czOut);

            glColor4f(rcol * 0.6f, gcol * 0.9f, bcol * 0.9f, alphaIn);
            glVertex3f(cxIn, cyIn, czIn);
        }
        glEnd();
    }

    // Restaurar blend
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Dibujar un anillo exterior brillante (reemplaza el aura de humo)
static void draw_outer_ring(const Esfera& esfera, float tiempo) {
    // Parámetros del anillo
    float baseR = RADIO_ESFERA * 1.28f;
    int rings = 3;
    int segments = 128;

    // Usar blending aditivo para glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_CULL_FACE);

    for (int r = 0; r < rings; ++r) {
        float t = (float)r / (float)(rings - 1);
        float innerR = baseR + t * RADIO_ESFERA * 0.08f + sinf(tiempo * (0.2f + r*0.05f)) * 6.0f;
        float outerR = innerR + 10.0f + r * 6.0f;
        float hueShift = esfera.faseAurora * 0.5f + r * 0.6f;

        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float a = (float)i / (float)segments * 2.0f * (float)ALLEGRO_PI;
            // ligera oscilación para darle vida
            float wobble = sinf(a * 6.0f + tiempo * (0.6f + r*0.2f)) * 3.0f * (1.0f - t);
            float xOut = cosf(a) * (outerR + wobble);
            float zOut = sinf(a) * (outerR + wobble);
            float xIn  = cosf(a) * (innerR + wobble * 0.4f);
            float zIn  = sinf(a) * (innerR + wobble * 0.4f);

            // color: mezcla magenta-cyan con variación según ángulo
            float mixv = 0.5f + 0.5f * sinf(a * 2.1f + hueShift);
            float rcol = 0.25f * (1.0f - mixv) + 0.95f * mixv;
            float gcol = 0.15f * (1.0f - mixv) + 0.6f * mixv;
            float bcol = 0.6f * (1.0f - mixv) + 1.0f * mixv;

            float alphaOut = 0.28f * (1.0f - t) * (0.9f + 0.4f * cosf(tiempo * 0.7f + a));
            float alphaIn  = 0.10f * (1.0f - t) * (0.6f + 0.4f * sinf(tiempo * 0.9f + a));

            glColor4f(rcol, gcol, bcol, alphaOut);
            glVertex3f(xOut, 0.0f + r * 2.0f, zOut);

            glColor4f(rcol * 0.6f, gcol * 0.8f, bcol * 0.9f, alphaIn);
            glVertex3f(xIn, 0.0f + r * 2.0f, zIn);
        }
        glEnd();
    }

    // Dibujar un halo fino adicional (punto grande) para reforzar el glow
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    float rcol = 0.6f, gcol = 0.3f, bcol = 0.95f;
    glColor4f(rcol, gcol, bcol, 0.12f * powf(esfera.progresoFormacion, 1.3f));
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();

    // Restaurar estados
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
}

// Dibujar un campo de estrellas distante (fondo)
static void draw_starfield(float tiempo) {
    static bool init = false;
    static std::vector<std::tuple<float,float,float,float>> stars;
    if (!init) {
        init = true;
        stars.reserve(400);
        for (int i = 0; i < 400; ++i) {
            float theta = ((float)rand() / RAND_MAX) * 2.0f * (float)ALLEGRO_PI;
            float phi = acosf(((float)rand() / RAND_MAX) * 2.0f - 1.0f);
            float r = 1200.0f + (float)rand() / RAND_MAX * 600.0f;
            float x = cosf(theta) * sinf(phi) * r;
            float y = sinf(theta) * sinf(phi) * r;
            float z = cosf(phi) * r - 1000.0f;
            float br = 0.6f + (float)rand() / RAND_MAX * 0.4f;
            stars.emplace_back(x,y,z,br);
        }
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (const auto &s : stars) {
        float x,y,z,b; std::tie(x,y,z,b) = s;
        float flick = 0.6f + 0.4f * (0.5f + 0.5f * sinf(tiempo * 1.3f + x * 0.0012f));
        glColor4f(0.6f * b * flick, 0.7f * b * flick, 1.0f * b * flick, 0.9f * flick * 0.6f);
        glVertex3f(x, y, z);
    }
    glEnd();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// Dibujar fresnel shells (rim glow) alrededor de la esfera para aspecto más sofisticado
static void draw_fresnel_shells(const Esfera& esfera, float tiempo) {
    float baseR = RADIO_ESFERA * 1.02f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);
    for (int layer = 0; layer < 4; ++layer) {
        float t = (float)layer / 4.0f;
        float scale = baseR * (1.0f + 0.02f * layer + 0.02f * sinf(tiempo * (0.6f + layer * 0.2f)));
        float alpha = 0.14f * (1.0f - t) * powf(esfera.progresoFormacion, 1.4f);
        float hue = 0.5f + 0.5f * sinf(esfera.faseAurora * 0.8f + layer * 1.1f);
        float rcol = 0.4f * hue + 0.2f;
        float gcol = 0.2f * (1.0f - hue) + 0.15f;
        float bcol = 0.6f + 0.4f * (1.0f - hue);

        glPushMatrix();
        glScalef(scale, scale, scale);
        glColor4f(rcol, gcol, bcol, alpha);
        static GLUquadric* quad2 = nullptr;
        if (!quad2) quad2 = gluNewQuadric();
        gluSphere(quad2, 1.0, 32, 18);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Dibujar la esfera usando OpenGL para un aspecto 3D real
void dibujarEsfera(Esfera& esfera) {
    float tiempo = al_get_time();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // La cámara será aplicada desde variables globales si existen
    extern Camera globalCamera;
    Camera cam = globalCamera;
    // ligera oscilación de cámara para dar vida
    float bob = sinf(al_get_time() * 0.6f) * 2.2f;
    cam.panY += bob * 0.08f;
    // calcular posición de la cámara en coordenadas esféricas relativas al target (pan)
    float cx = cam.panX + sinf(cam.rotY) * cosf(cam.rotX) * cam.distance;
    float cy = cam.panY + sinf(cam.rotX) * cam.distance;
    float cz = cam.panX + cosf(cam.rotY) * cosf(cam.rotX) * cam.distance;
    gluLookAt(cx, cy, cz, cam.panX, cam.panY, 0.0, 0.0, 1.0, 0.0);

    // Dibujar estrellas de fondo (ligero parpadeo)
    draw_starfield(al_get_time());

    // Aplicar rotaciones de la esfera (convertir radianes a grados)
    const float RAD_TO_DEG = 180.0f / (float)ALLEGRO_PI;
    glRotatef(esfera.rotacionX * RAD_TO_DEG, 1.0f, 0.0f, 0.0f);
    glRotatef(esfera.rotacionY * RAD_TO_DEG, 0.0f, 1.0f, 0.0f);

    // Dibujar conexiones entre puntos (líneas 3D)
    const float distConexion = DISTANCIA_CONEXION;
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (size_t i = 0; i < esfera.puntos.size(); ++i) {
        Punto3D pi = rotarPunto(esfera.puntos[i], esfera.rotacionX, esfera.rotacionY);
        for (size_t j = i + 1; j < esfera.puntos.size(); ++j) {
            Punto3D pj = rotarPunto(esfera.puntos[j], esfera.rotacionX, esfera.rotacionY);
            float dx = pi.x - pj.x; float dy = pi.y - pj.y; float dz = pi.z - pj.z;
            float d2 = dx*dx + dy*dy + dz*dz;
            if (d2 < distConexion * distConexion) {
                float alpha = 0.12f * powf(esfera.progresoFormacion, 2.0f);
                // color RGB según la dirección del punto para efecto arcoíris
                float rn = 0.5f + 0.5f * (pi.x / RADIO_ESFERA);
                float gn = 0.5f + 0.5f * (pi.y / RADIO_ESFERA);
                float bn = 0.5f + 0.5f * (pi.z / RADIO_ESFERA);
                glColor4f(rn, gn, bn, alpha);
                glVertex3f(pi.x, pi.y, pi.z);
                glVertex3f(pj.x, pj.y, pj.z);
            }
        }
    }
    glEnd();

    // Pase additive para dar glow/neón a las conexiones (segunda pasada más ancha)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for (size_t i = 0; i < esfera.puntos.size(); ++i) {
        Punto3D pi = rotarPunto(esfera.puntos[i], esfera.rotacionX, esfera.rotacionY);
        for (size_t j = i + 1; j < esfera.puntos.size(); ++j) {
            Punto3D pj = rotarPunto(esfera.puntos[j], esfera.rotacionX, esfera.rotacionY);
            float dx = pi.x - pj.x; float dy = pi.y - pj.y; float dz = pi.z - pj.z;
            float d2 = dx*dx + dy*dy + dz*dz;
            if (d2 < distConexion * distConexion) {
                float tcol = 1.0f - (sqrtf(d2) / distConexion);
                // color glow basado en la posición para mantener tema RGB
                float rn = 0.5f + 0.5f * (pi.x / RADIO_ESFERA);
                float gn = 0.5f + 0.5f * (pi.y / RADIO_ESFERA);
                float bn = 0.5f + 0.5f * (pi.z / RADIO_ESFERA);
                float r = rn * tcol * 0.95f;
                float g = gn * tcol * 0.95f;
                float b = bn * tcol * 0.95f;
                glColor4f(r, g, b, 0.16f * powf(esfera.progresoFormacion, 2.0f));
                glVertex3f(pi.x, pi.y, pi.z);
                glVertex3f(pj.x, pj.y, pj.z);
            }
        }
    }
    glEnd();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);
    // Dibujar esferas blancas en los nodos que participan en conexiones
    {
        static GLUquadric* qConn = nullptr;
        if (!qConn) qConn = gluNewQuadric();
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        for (size_t i = 0; i < esfera.puntos.size(); ++i) {
            Punto3D pi = rotarPunto(esfera.puntos[i], esfera.rotacionX, esfera.rotacionY);
            bool tieneConexion = false;
            for (size_t j = 0; j < esfera.puntos.size(); ++j) {
                if (i == j) continue;
                Punto3D pj = rotarPunto(esfera.puntos[j], esfera.rotacionX, esfera.rotacionY);
                float dx = pi.x - pj.x; float dy = pi.y - pj.y; float dz = pi.z - pj.z;
                if (dx*dx + dy*dy + dz*dz < distConexion * distConexion) { tieneConexion = true; break; }
            }
            if (tieneConexion) {
                glPushMatrix();
                glTranslatef(pi.x, pi.y, pi.z);
                float size = 2.2f + 2.6f * (400.0f / (400.0f + pi.z));
                glScalef(size, size, size);
                glColor4f(1.0f, 1.0f, 1.0f, 0.95f);
                gluSphere(qConn, 1.0, 8, 6);
                glPopMatrix();
            }
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LIGHTING);
    }

    // Dibujar puntos (como GL_POINTS, con tamaño según la profundidad)
    glEnable(GL_POINT_SMOOTH);
    for (const auto& p : esfera.puntos) {
        Punto3D pd = rotarPunto(p, esfera.rotacionX, esfera.rotacionY);
        float alpha = 0.9f * powf(esfera.progresoFormacion, 2.0f);
        float sizeFactor = fmaxf(1.0f, 6.0f * (400.0f / (400.0f + pd.z)));
        glPointSize(sizeFactor);
        // color RGB según posición (mapear -R a +R)
        float rn = 0.5f + 0.5f * (pd.x / RADIO_ESFERA);
        float gn = 0.5f + 0.5f * (pd.y / RADIO_ESFERA);
        float bn = 0.5f + 0.5f * (pd.z / RADIO_ESFERA);
        glBegin(GL_POINTS);
        glColor4f(rn, gn, bn, alpha);
        glVertex3f(pd.x, pd.y, pd.z);
        glEnd();
    }

    // Núcleo volumétrico interno (nube/partículas)
    draw_core(esfera, tiempo);
    // Fresnel/rim shells para dar borde y profundidad
    draw_fresnel_shells(esfera, tiempo);
    // Agregar resplandores y cintas tipo aurora + capas de humo para acabado visual
    draw_aurora_glow(esfera, tiempo);
    draw_outer_ring(esfera, tiempo);

    // Efectos extra para acabado profesional: viñeta
    draw_vignette(VIGNETTE_INTENSITY);
}

//Main y Lógica de IPC (Inter-Process Communication)
int main() {
    srand((unsigned)time(nullptr));

    //Inicialización de Allegro
    if (!al_init()) { std::cerr << "No se pudo iniciar Allegro\n"; return -1; }
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();

    // Solicitar un display con contexto OpenGL (modo ventana). Creamos el display con
    // el tamaño del monitor y lo posicionamos en (moninfo.x1,moninfo.y1) para emular
    // pantalla completa pero manteniendo comportamiento de ventana (physics/movimiento).
    al_set_new_display_flags(ALLEGRO_OPENGL);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_SUGGEST);
    // Obtener resolución del monitor y crear una ventana más pequeña (75%)
    // para permitir moverla libremente.
    ALLEGRO_MONITOR_INFO moninfo;
    int initialPosX = 0, initialPosY = 0;
    if (al_get_monitor_info(0, &moninfo)) {
        int monitorW = moninfo.x2 - moninfo.x1;
        int monitorH = moninfo.y2 - moninfo.y1;
        // ventana al 75% del tamaño del monitor
        int windowW = (int)(monitorW * 0.75f);
        int windowH = (int)(monitorH * 0.75f);
        // Centrar la ventana en el monitor
        initialPosX = moninfo.x1 + (monitorW - windowW) / 2;
        initialPosY = moninfo.y1 + (monitorH - windowH) / 2;
        DISPLAY_W = windowW;
        DISPLAY_H = windowH;
    }
    ALLEGRO_DISPLAY* pantalla = al_create_display(DISPLAY_W, DISPLAY_H);
    if (!pantalla) { std::cerr << "No se pudo crear la ventana\n"; return -1; }
    // Posicionar la ventana centrada (y movible por el usuario)
    al_set_window_position(pantalla, initialPosX, initialPosY);
    al_set_window_title(pantalla, "Esfera Neural");
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    // Inicializar parámetros OpenGL con tamaño real del display
    init_opengl(DISPLAY_W, DISPLAY_H);

    ALLEGRO_TIMER* temporizador = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* colaEventos = al_create_event_queue();
    al_register_event_source(colaEventos, al_get_display_event_source(pantalla));
    al_register_event_source(colaEventos, al_get_timer_event_source(temporizador));
    al_register_event_source(colaEventos, al_get_mouse_event_source());
    al_register_event_source(colaEventos, al_get_keyboard_event_source());

    // interacción de cámara / keyframes
    bool dragging = false;
    bool rightDragging = false;
    int lastMouseX = 0, lastMouseY = 0;
    // (Keyframes eliminados: no se usan)
    bool showInstances = false;
    std::vector<Esfera> instances;

    Esfera esfera;
    inicializarEsfera(esfera);

    int prevWX, prevWY;
    al_get_window_position(pantalla, &prevWX, &prevWY);

    al_start_timer(temporizador);
    // asegurar targetDistance inicial coherente
    globalCamera.targetDistance = globalCamera.distance;
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

            // actualizar instancias si están activas
            if (showInstances) {
                for (size_t i = 0; i < instances.size(); ++i) {
                    actualizarEsfera(instances[i], prevWX, prevWY, currWX, currWY, dt);
                }
            }

            prevWX = currWX;
            prevWY = currWY;

            // Interpolación suave del zoom: acercar distance hacia targetDistance
            {
                float diff = globalCamera.targetDistance - globalCamera.distance;
                globalCamera.distance += diff * globalCamera.zoomSmooth;
                if (globalCamera.distance < globalCamera.minDistance) globalCamera.distance = globalCamera.minDistance;
                if (globalCamera.distance > globalCamera.maxDistance) globalCamera.distance = globalCamera.maxDistance;
            }

            // (Se eliminaron las rutinas de grabación/playback de keyframes)
        }

        // Manejo de eventos de entrada (mouse/teclado)
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) { dragging = true; lastMouseX = evento.mouse.x; lastMouseY = evento.mouse.y; }
            if (evento.mouse.button == 2) { rightDragging = true; lastMouseX = evento.mouse.x; lastMouseY = evento.mouse.y; }
        }
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (evento.mouse.button == 1) dragging = false;
            if (evento.mouse.button == 2) rightDragging = false;
        }
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            // desplazamiento de rueda en evento.mouse.z -> actualizar targetDistance suavemente
            if (evento.mouse.z != 0) {
                globalCamera.targetDistance += -evento.mouse.z * 40.0f;
                if (globalCamera.targetDistance < globalCamera.minDistance) globalCamera.targetDistance = globalCamera.minDistance;
                if (globalCamera.targetDistance > globalCamera.maxDistance) globalCamera.targetDistance = globalCamera.maxDistance;
            }
            if (dragging) {
                float dx = evento.mouse.dx;
                float dy = evento.mouse.dy;
                globalCamera.rotY += dx * 0.0065f;
                globalCamera.rotX += dy * 0.0065f;
                // limitar inclinación
                const float limit = 1.45f;
                if (globalCamera.rotX > limit) globalCamera.rotX = limit;
                if (globalCamera.rotX < -limit) globalCamera.rotX = -limit;
            }
            // botón derecho para hacer pan (si está pulsado)
            if (rightDragging) {
                globalCamera.panX -= evento.mouse.dx * 0.6f;
                globalCamera.panY += evento.mouse.dy * 0.6f;
            }
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:
                    enEjecucion = false;
                    break;
                // R/S/P/L removed (keyframes not used)
                case ALLEGRO_KEY_C: // reset camera
                    globalCamera = Camera();
                    std::cout << "Camara reseteada\n";
                    break;
                case ALLEGRO_KEY_1: // toggle instances
                    showInstances = !showInstances;
                    if (showInstances && instances.empty()) {
                        // crear algunas instancias a modo de demo
                        for (int i = 0; i < 6; ++i) {
                            Esfera e; inicializarEsfera(e);
                                // distribuir en X
                                e.centroX = DISPLAY_W/2.0f + (i-2.5f) * 180.0f;
                                e.centroY = DISPLAY_H/2.0f;
                            instances.push_back(e);
                        }
                    }
                    std::cout << "Instancias " << (showInstances ? "ON" : "OFF") << "\n";
                    break;
                case ALLEGRO_KEY_O: // placeholder: carga OBJ no implementada
                    std::cout << "Placeholder: cargador OBJ no implementado. Usa '1' para instancias demo.\n";
                    break;
                default: break;
            }
        }

        // Dibujo
        if (redibujar && al_is_event_queue_empty(colaEventos)) {
            redibujar = false;
            al_clear_to_color(al_map_rgb(8, 8, 10));

            dibujarEsfera(esfera);
            // aplicar bloom simple sin FBO
            apply_simple_bloom(DISPLAY_W, DISPLAY_H);

            al_flip_display();
        }
    }

    //Limpieza de Allegro
    al_destroy_display(pantalla);
    al_destroy_timer(temporizador);
    al_destroy_event_queue(colaEventos);

    return 0;
}