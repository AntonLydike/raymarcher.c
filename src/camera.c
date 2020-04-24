#include "../marcher.h"
#include <sys/wait.h>
#include <sys/mman.h>


Camera camera_new(Point direction, unsigned int fov) {
    Camera camera;
    camera.location = pt_new(0,0,0);
    camera.fov = fov;

    // normalize camera direction
    camera.direction = pt_normalize(direction);

    return camera;
}

void camera_set_looking_at(Camera *cam, Point origin, Point thing) {
    cam->location = origin;
    pt_sub(&thing, origin);
    cam->direction = pt_normalize(thing);
}

void camera_iterate_rays_const_angle(Camera camera, int width, int height, int threads, void (*callback)(Point, int, int)) {
    // negative threads => single threaded.
    if (threads < 0) threads = 0;

    Point span_z, span_xy;

    // get rotation axis
    pt_orthogonal_plane(camera.direction, &span_z, &span_xy);

    printf("rendering %ix%i px", width, height);

    pt_print_n("span_xy", span_xy);
    pt_print_n("span_z", span_z);

    // angle between rays
    double angle_step = camera.fov / (double) (width - 1);

    // rotation applied to reach the outmost end of the view
    double angle_start_h = - (camera.fov / 2.0);
    double angle_start_v = ((angle_step * (height - 1)) / 2) ;

    printf("step: %f\nstart_h: %f\nstart_v: %f\n", angle_step, angle_start_h, angle_start_v);

    // calculate both rotation matrices (expensive!)
    Matrix rot_z = mtrx_rotation(span_z, angle_step);
    Matrix rot_xy = mtrx_rotation(span_xy, -angle_step);

    // rotate vector to starting location (bot left of screen)
    // (very expensive!)
    Point starting_point = mtrx_mult(
        mtrx_rotation(span_xy, angle_start_v),
        mtrx_mult(
            mtrx_rotation(span_z, angle_start_h),
            camera.direction
        )
    );

    // initialize threads
    int thread_id = 0;
    for (int i = 0; i < threads - 1; i++) {
        if (fork() == 0) {
            thread_id = i + 1;
            break;
        }
    }

    printf("Thread %i reporting for duty\n", thread_id);

    // this point is rotated for every pixel
    Point curr_pt = starting_point;
    // (0,0) screenspace is bottom left corner
    for (int y = 0; y < height; y++) {
        curr_pt = mtrx_mult(rot_xy, starting_point);
        // move starting point one row down
        starting_point = curr_pt;

        if (y % threads != thread_id) continue;

        for (int x = 0; x < width; x++) {
            callback(curr_pt, x, y);
            curr_pt = mtrx_mult(rot_z, curr_pt); // rotate point
        }
    }

    if (thread_id != 0) {
        printf("Thread %i is finished\n", thread_id);
        exit(0);
    }

    int status;
    for (int i = 0; i < threads - 1; i++) {
        printf("Waiting for threads... %d/%d\n", i, threads);
        while(wait(&status) > 0) {}
    }

    printf("got threads\n");
}

void camera_iterate_rays_const_dist(Camera camera, int width, int height, int threads, void (*callback)(Point, int, int)) {
    // negative threads => single threaded.
    if (threads < 0) threads = 0;

    Point span_z, span_xy;

    // get rotation axis
    pt_orthogonal_plane(camera.direction, &span_z, &span_xy);

    printf("rendering %ix%i px\n", width, height);

    pt_print_n("span_xy", span_xy);
    pt_print_n("span_z", span_z);


    // distance each ray has from anothe on the ortogonal plane
    double step_dist = 2 / (double) (width - 1);

    // vectors to move on the projection plane
    Point move_right = pt_scale(span_xy, step_dist);
    Point move_up =    pt_scale(span_z,  step_dist);;

    printf("step: %f\n", step_dist);
    // set starting point
    Point starting_point = pt_normalize(camera.direction);
    
    // rotate starting point to (0,0)
    pt_add(&starting_point, pt_mult(move_right, - width / (double) 2));
    pt_add(&starting_point, pt_mult(move_up, - height / (double) 2));

    // initialize threads
    int thread_id = 0;
    for (int i = 0; i < threads - 1; i++) {
        if (fork() == 0) {
            thread_id = i + 1;
            break;
        }
    }
    printf("Thread %i reporting for duty\n", thread_id);

    // this point is moved for every pixel
    Point curr_pt = starting_point;

    // (0,0) screenspace is bottom left corner
    for (int y = 0; y < height; y++) {
        // move one row up (this has to be done in every thread!)
        pt_add(&starting_point, move_up);

        // only render the lines this thread is responsible for
        if (y % threads != thread_id) continue;
        
        // display progress in percent
        if (y % (height / 100) == 0 && y != 0) {
            printf("\r%02i%%", (y * 100) / height);
            fflush(stdout);
        }

        // actually iterate this line
        curr_pt = starting_point;
        for (int x = 0; x < width; x++) {
            callback(curr_pt, x, y);
            pt_add(&curr_pt, move_right); // move pt right to next pt
        }
    }

    if (thread_id != 0) {
        printf("Thread %i is finished\n", thread_id);
        exit(0);
    }

    int status;
    for (int i = 0; i < threads - 1; i++) {
        printf("Waiting for threads... %d/%d\n", i, threads);
        while(wait(&status) > 0) {}
    }

    printf("got threads\n");
}