#include <gtk/gtk.h> 
#include <math.h>
#include "images/images.h"
#include "marcher.h"

#define SCENE_MOD 2


/*

    Infinite circle scene object

    This circle repeats every SCENE_MOD units in all directions

    It has some basic shading, calculation the hit angle and making shallower impacts darker

*/

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
    color = color_mix(color, color_new(0,0,0), 1 - (angle / (double) 120));

    return color;
}
// constructs the scene object
SceneObject circle_new(Point loc, double radius) {
    SceneObject so;
    so.location = loc;
    so.args = malloc(sizeof(double) * 2);
    so.args[0] = radius;
    so.distance = circle_dist;
    so.get_color = circle_color;
    so.color = color_new(255,255,255);
    return so;
}


/*

    Mandelbulb scene object

    Currently cannot be set at a specific location, always resides at origin (0,0,0)

    Color function is just a flat shader, detail is displayed with ambient occlusion

*/
double mandelbulb_dist(Point pt, SceneObject *self) {
    int iters = self->args[0];
    double power = self->args[1];

    Point z = pt;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < iters ; i++) {
		r = pt_length(z);

		if (r>2) {
            break;
        }
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan2(z.y,z.x);
		dr =  pow(r, power-1.0)*power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow(r, power);
		theta = theta*power;
		phi = phi*power;
		
		// convert back to cartesian coordinates
		z = pt_mult(pt_new(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta)), zr);
		pt_add(&z, pt);
	}
	
    return 0.5*log(r)*r/dr;
    
}

Color mandelbulb_color(Point hit, Point direction, SceneObject *self) {
    return self->color;
}
// constructs the scene object
SceneObject mandelbulb_new(Point location, int iters, double power) {
    SceneObject so;
    so.location = location;
    so.args = malloc(sizeof(double) * 3);
    so.args[0] = iters;         // iterations
    so.args[1] = power;         // power
    so.args[2] = -1;            // reserved for color calculations
    so.distance = mandelbulb_dist;
    so.get_color = mandelbulb_color;
    so.color = color_new(0,0,0);
    return so;
}

int main(int argc, char* argv[]) {
    int threads = 1;
    Camera cam;
    cam.fov = 90;

    float cam_position = 1.2;
    camera_set_looking_at(&cam, pt_new(cam_position,cam_position,cam_position), pt_new(0,0,0));

    // get thread count from command line
    if (argc > 1) {
        threads = atoi(argv[1]);
    }

    printf("threads: %d\n", threads);

    // create basic scene with up to 10 objects
    Scene scene = scene_new(4000, 4000, 10);
    scene.max_steps = 1000;
    scene.threshold = 0.0001;
    scene.background = color_new(255,255,255);

    //scene_add_obj(&scene, circle_new(pt_new(SCENE_MOD / 2.0, SCENE_MOD/ 2.0, SCENE_MOD / 2.0), .2));

    scene_add_obj(&scene, mandelbulb_new(pt_new(1,1,1), 2000, 2.5));
    
    Image *img = render_scene(&scene, &cam, threads);

    image_save_bmp(*img, "render.bmp");

    image_destroy_shared(*img);
    scene_destroy(scene);

    return 0;  
}

