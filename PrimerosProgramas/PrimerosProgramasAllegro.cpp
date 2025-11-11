#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <cmath>

//declaraciones
const float pi = 3.1416;


//funcion principal
int main(){
    //inicializar Allegro
    al_init();
    al_init_primitives_addon();

    //crear ventana
    ALLEGRO_DISPLAY *ventana = al_create_display(800,600);

    //color de fondo
    al_clear_to_color(al_map_rgb(0,0,0));

    //dibujar linea vertical con puntos
    for(int i=50; i<200; i+=1){
        al_draw_pixel(50,i,al_map_rgb(255,255,255));
    }

    //dibujar linea horizontal con puntos
    for(int i=100; i<300; i+=1){
        al_draw_pixel(i,50,al_map_rgb(255,255,255));
    }

    //linea inclinada con coordenadas cartesianas
    for(int i=200; i<300; i+=1){
        for (int j=100; j<300; j+=1){
            if(j == i - 100){
                al_draw_pixel(i,j,al_map_rgb(255,255,255));
            }
        }
    }

    //linea diagonal con coordenadas polares
    for(int i=0; i<150; i+=1){
        int x = 400 + i*cos(pi/4);
        int y = 300 + i*sin(pi/4);
        al_draw_pixel(x,y,al_map_rgb(255,255,255));
    }

    //circulo con ec xcuadrada + ycuadrada = rcuadrada y con modificacion para dibujar las partes que no se dibujan
    int r = 50;
    for(int x=-r; x<=r; x+=1){
        int y = sqrt(r*r - x*x);
        al_draw_pixel(600 + x, 400 + y, al_map_rgb(255,255,255));
        al_draw_pixel(600 + x, 400 - y, al_map_rgb(255,255,255));
    }
    for(int y=-r; y<=r; y+=1){
        int x = sqrt(r*r - y*y);
        al_draw_pixel(600 + x, 400 + y, al_map_rgb(255,255,255));
        al_draw_pixel(600 - x, 400 + y, al_map_rgb(255,255,255));
    }

    //dibujo de lineas con diferentes angulos y un centro (simulando rayos)
    int centerX = 200;
    int centerY = 400;
    for (int angle = 0; angle < 360; angle += 15) {
        float rad = angle * (pi / 180);
        int x = centerX + cos(rad) * 100;
        int y = centerY + sin(rad) * 100;
        al_draw_line(centerX, centerY, x, y, al_map_rgb(255, 255, 255), 1);
    }

    //mostrar en pantalla los dibujos estáticos
    al_flip_display();

    //dibujo de una linea que se va moviendo y nunca se detiene (simulando un radar)
    int radarX = 600;
    int radarY = 100;
    for (int angle = 0; angle < 360; angle += 5) {
        float rad = angle * (pi / 180);
        int x = radarX + cos(rad) * 80;
        int y = radarY + sin(rad) * 80;
        al_draw_line(radarX, radarY, x, y, al_map_rgb(0, 255, 0), 2);
        al_flip_display();
        al_rest(0.05); // Pequeña pausa para ver el efecto
        al_draw_line(radarX, radarY, x, y, al_map_rgb(0, 0, 0), 2); // Borrar la línea dibujada
    }

    //mostrar en pantalla
    al_flip_display();

    //esperar 50 segundos
    al_rest(50.0);

    //destruir ventana
    al_destroy_display(ventana);

    return 0;
}