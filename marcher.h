#ifndef __MARCHER_H__
#define __MARCHER_H__

#include "images/images.h"

// define pi if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct __myvec {
    double x;
    double y;
    double z;
} Point;

typedef struct __mymtrx {
    double entries[9];
} Matrix;

inline Point pt_new(double x, double y, double z);
Point pt_scale(Point pt, double length);
inline Point pt_normalize(Point pt);
inline double pt_length(Point pt);
void pt_add(Point* pt, Point add);
inline void pt_sub(Point* pt, Point sub);
inline double pt_dist(Point p1, Point p2);
inline Point pt_mod(Point pt, double mod);
inline double pt_dot(Point a, Point b);
inline Point pt_cross(Point a, Point b);
inline double pt_angle(Point a, Point b);
inline void pt_print(Point pt);
inline void pt_print_n(const char* name, Point pt);


typedef struct __mycam {
    Point location;
    Point direction;
    unsigned int fov;
} Camera;

Camera camera_new(Point direction, unsigned int fov);
void camera_set_looking_at(Camera *cam, Point origin, Point thing);

// Scene objects have a position, some args, and a distance calculation function
// the distance calc function has the following signature:
// double distanceTo(Point myLocation, double * myArgs, Point externalPoint)
// where myLocation is this.location, myArgs is this.args and externalPoint is the point from wich we want to know the distance
// the get_color function takes args: point_hit, direction_hit, myArgs, MyLocation, MyColor
typedef struct __myobject {
    Point location;
    double * args;
    double (*distance)(Point, struct __myobject *); 
    Color (*get_color)(Point, Point, struct __myobject *);
    Color color;
} SceneObject;

typedef struct __myscene {
    unsigned int width;
    unsigned int height;
    SceneObject * objects;
    int object_count;
    int allocated_space;
    // some other settings
    int max_steps;
    double threshold;
    // colors etc
    Color background;
} Scene;

Image* render_scene(Scene *scene, Camera *camera, unsigned int threads);

Scene scene_new(unsigned int width, unsigned int height, int obj_count);

void scene_add_obj(Scene* scene, SceneObject object);

void scene_destroy(Scene scene);

#include "src/point.c"
#include "src/camera.c"
#include "src/scene.c"

#endif