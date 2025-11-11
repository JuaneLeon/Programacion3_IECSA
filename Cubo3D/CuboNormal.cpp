#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <vector>
#include <cmath>

const float SCREEN_WIDTH = 800;
const float SCREEN_HEIGHT = 600;

struct Point3D {
    float x, y, z;
};

struct Point2D {
    float x, y;
};

Point2D project(Point3D p) {
    float scale = 80;
    float depth_x = 0.5;
    float depth_y = 0.5;
    return {
        SCREEN_WIDTH / 2 + (p.x + p.z * depth_x) * scale,
        SCREEN_HEIGHT / 2 - (p.y - p.z * depth_y) * scale
    };
}

void rotateX(Point3D &p, float angle) {
    float rad = angle * ALLEGRO_PI / 180.0;
    float y = p.y;
    p.y = y * cos(rad) - p.z * sin(rad);
    p.z = y * sin(rad) + p.z * cos(rad);
}

void rotateY(Point3D &p, float angle) {
    float rad = angle * ALLEGRO_PI / 180.0;
    float x = p.x;
    p.x = x * cos(rad) + p.z * sin(rad);
    p.z = -x * sin(rad) + p.z * cos(rad);
}

void rotateZ(Point3D &p, float angle) {
    float rad = angle * ALLEGRO_PI / 180.0;
    float x = p.x;
    p.x = x * cos(rad) - p.y * sin(rad);
    p.y = x * sin(rad) + p.y * cos(rad);
}


int main() {
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    std::vector<Point3D> vertices;
    vertices.push_back({-1, -1, -1});
    vertices.push_back({1, -1, -1});
    vertices.push_back({1, 1, -1});
    vertices.push_back({-1, 1, -1});
    vertices.push_back({-1, -1, 1});
    vertices.push_back({1, -1, 1});
    vertices.push_back({1, 1, 1});
    vertices.push_back({-1, 1, 1});

    std::vector<std::pair<int, int>> edges;
    edges.push_back({0, 1});
    edges.push_back({1, 2});
    edges.push_back({2, 3});
    edges.push_back({3, 0});
    edges.push_back({4, 5});
    edges.push_back({5, 6});
    edges.push_back({6, 7});
    edges.push_back({7, 4});
    edges.push_back({0, 4});
    edges.push_back({1, 5});
    edges.push_back({2, 6});
    edges.push_back({3, 7});

    std::vector<ALLEGRO_COLOR> colors;
    colors.push_back(al_map_rgb(255, 0, 0));     // Red
    colors.push_back(al_map_rgb(0, 255, 0));     // Green
    colors.push_back(al_map_rgb(0, 0, 255));     // Blue
    colors.push_back(al_map_rgb(255, 255, 0));   // Yellow
    colors.push_back(al_map_rgb(255, 0, 255));   // Magenta
    colors.push_back(al_map_rgb(0, 255, 255));   // Cyan
    colors.push_back(al_map_rgb(255, 165, 0));   // Orange
    colors.push_back(al_map_rgb(128, 0, 128));   // Purple
    colors.push_back(al_map_rgb(0, 128, 0));     // Dark Green
    colors.push_back(al_map_rgb(255, 192, 203)); // Pink
    colors.push_back(al_map_rgb(165, 42, 42));   // Brown
    colors.push_back(al_map_rgb(255, 255, 255)); // White

    std::vector<Point3D> transformed_vertices = vertices;

    al_clear_to_color(al_map_rgb(0, 0, 0));

    int color_index = 0;
    for (const auto &edge : edges) {
        Point2D p1 = project(transformed_vertices[edge.first]);
        Point2D p2 = project(transformed_vertices[edge.second]);
        al_draw_line(p1.x, p1.y, p2.x, p2.y, colors[color_index++], 2);
    }

    al_flip_display();

    bool running = true;
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
    }

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_shutdown_primitives_addon();

    return 0;
}
