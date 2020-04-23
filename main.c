#include <gtk/gtk.h> 
#include <math.h>
#include "images/images.h"
#include "marcher.h"

#define SCENE_MOD 2

// this is the circle distance function definition
double circle_dist(Point x, SceneObject *self) {
    double r = self->args[0];
    return pt_dist(pt_mod(x, SCENE_MOD), self->location) - r;
}

Color circle_color(Point hit, Point direction, SceneObject *self) {
    Point obj_direction = self->location;

    pt_sub(&obj_direction, pt_mod(hit, SCENE_MOD));

    double angle = pt_angle(direction, obj_direction) / M_PI * 180;
    Color color = self->color;

    if (angle > 90) angle = 180 - angle ; // clamp angle to 0-90
    color = color_mix(color, color_new(0,0,0), 1 - (angle / (double) 90));

    return color;
}

SceneObject circle_new(Point loc, double radius) {
    SceneObject so;
    so.location = loc;
    so.args = malloc(sizeof(double) * 2);
    so.args[0] = radius;
    so.distance = circle_dist;
    so.get_color = circle_color;
    so.color = color_new(255,0,0);
    return so;
}

int main(int argc, char* argv[]) {
    int threads = 1;
    Camera cam;
    cam.fov = 90;
    camera_set_looking_at(&cam, pt_new(-.6,0,0), pt_new(-.5,1,0));

    if (argc > 1) {
        threads = atoi(argv[1]);
    }

    printf("threads: %d\n", threads);

    // create basic scene with up to 10 objects
    Scene scene = scene_new(800, 600, 10);
    scene.max_steps = 64;
    scene.threshold = 0.02;

    scene_add_obj(&scene, circle_new(pt_new(0,1,0), .2));

    //scene_add_obj(&scene, circle_new(pt_new(0,2,0), 0.5));
    //scene_add_obj(&scene, circle_new(pt_new(0,-2,0), 0.5));
    //scene_add_obj(&scene, circle_new(pt_new(0,-4,0), 0.5));

    Image *img = render_scene(&scene, &cam, threads);

    image_save_bmp(*img, "render.bmp");

    image_destroy_shared(*img);
    scene_destroy(scene);

    return 0;  
}

