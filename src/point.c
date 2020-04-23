#include <math.h>
#include <stdio.h>

// basically a vector3
inline Point pt_new(double x, double y, double z) {
    Point pt;
    pt.x = x;
    pt.y = y;
    pt.z = z;
    return pt;
}

// scale vector to length
Point pt_scale(Point pt, double length) {
    double f = length / pt_length(pt);
    return pt_new(
        pt.x * f,
        pt.y * f,
        pt.z * f
    );
}

inline Point pt_mult(Point pt, double scalar) {
    return pt_new(
        pt.x * scalar,
        pt.y * scalar,
        pt.z * scalar
    );
}

// return internal angle between a and b
inline double pt_angle(Point a, Point b) {
    return acos(pt_dot(
        pt_normalize(a),
        pt_normalize(b)
    ));
}

// get the length of vector
inline double pt_length(Point pt) {
    return sqrt((pt.x * pt.x) + (pt.y * pt.y) + (pt.z * pt.z));
}

// add the vector add to the vector pt
void pt_add(Point* pt, Point add) {
    pt->x = pt->x + add.x;
    pt->y = pt->y + add.y;
    pt->z = pt->z + add.z;
}

// add the vector add to the vector pt
inline void pt_sub(Point* pt, Point sub) {
    pt->x -= sub.x;
    pt->y -= sub.y;
    pt->z -= sub.z;
}

inline double pt_dist(Point p1, Point p2) {
    pt_sub(&p1, p2);
    return pt_length(p1);
}

// normalize a vector
inline Point pt_normalize(Point pt) {
    return pt_scale(pt, 1);
}

// dot product of two vectors
inline double pt_dot(Point a, Point b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

// cross product of two vectors
inline Point pt_cross(Point a, Point b) {
    return pt_new(
        a.y*b.z - a.z*b.y, 
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}

inline void pt_print(Point pt) {
    printf("(%f, %f, %f)\n", pt.x, pt.y, pt.z);
}

inline void pt_print_n(const char* name, Point pt) {
    printf("%s: (%f, %f, %f)\n", name, pt.x, pt.y, pt.z);
}

// find two vectors that span the orthogonal plane, where 
// span_xy is a vector lying on the xy-plane (and pointing left)
// and span_z is orthogonal to span_xy pointing "upwards"
void pt_orthogonal_plane(Point pt, Point *span_z, Point *span_xy) {
    pt = pt_normalize(pt);

    // get the vector lying on the xy axis
    // this is done by 
    *span_xy = pt_normalize(pt_cross(pt_new(0,0,1), pt)); // points to the "left" (of the viewing direction)

    // now use this, to find the vector 
    *span_z = pt_normalize(pt_cross(pt, *span_xy));
}

inline Point pt_mod(Point pt, double mod) {
    return pt_new(
        fmod(pt.x, mod),
        fmod(pt.y, mod),
        fmod(pt.z, mod)
    );
}


///////////////////////////////
////// Matrix operations //////
///////////////////////////////


/* create a new matrix with entries:
    x1 x2 x3
    y1 y2 y3
    z1 z2 z3
*/
inline Matrix mtrx_new(double x1, double x2, double x3,
                       double y1, double y2, double y3,
                       double z1, double z2, double z3) 
{
    Matrix m;
    m.entries[0] = x1;
    m.entries[1] = y1;
    m.entries[2] = z1;
    m.entries[3] = x2;
    m.entries[4] = y2;
    m.entries[5] = z2;
    m.entries[6] = x3;
    m.entries[7] = y3;
    m.entries[8] = z3;
    return m;
}

inline Point mtrx_mult(Matrix mtrx, Point pt) {
    Point result;

    double *m = mtrx.entries;

    result.x = m[0] * pt.x + m[3] * pt.y + m[6] * pt.z;
    result.y = m[1] * pt.x + m[4] * pt.y + m[7] * pt.z;
    result.z = m[2] * pt.x + m[5] * pt.y + m[8] * pt.z;

    return result;
}

// create a rotation matrix around an axis given by the normalized axis vector (u)
// taken from https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
Matrix mtrx_rotation(Point u, double theta) {
    double theta_rad = theta * (M_PI / 180);
    double cost = cos(theta_rad);
    double sint = sin(theta_rad);

    return mtrx_new(
        cost+u.x*u.x*(1-cost),     u.x*u.y*(1-cost)-u.z*sint, u.x*u.z*(1-cost)+u.y*sint,
        u.y*u.x*(1-cost)+u.z*sint, cost+u.y*u.y*(1-cost),     u.y*u.z*(1-cost)-u.x*sint,
        u.z*u.x*(1-cost)-u.y*sint, u.z*u.y*(1-cost)+u.x*sint, cost+u.z*u.z*(1-cost)
    );
}

void mtrx_print(Matrix mtrx) {
    printf("  %8.2f %8.2f %8.2f\n  %8.2f %8.2f %8.2f\n  %8.2f %8.2f %8.2f\n",
        mtrx.entries[0], mtrx.entries[3], mtrx.entries[6],
        mtrx.entries[1], mtrx.entries[4], mtrx.entries[7],
        mtrx.entries[2], mtrx.entries[5], mtrx.entries[8]
    );
}

inline Matrix mtrx_outer_prod(Point a, Point b) {
    return mtrx_new(
        a.x*b.x, a.x*b.y, a.x*b.z,
        a.y*b.x, a.y*b.y, a.y*b.z,
        a.z*b.x, a.z*b.y, a.z*b.z
    );
}