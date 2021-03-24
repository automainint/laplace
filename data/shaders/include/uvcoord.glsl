float uvcoord_solve_u(float x, float y)
{
    return atan(y, x) * 0.31830988618;
}

float uvcoord_solve_v(float z)
{
    return asin(z) * 0.63661977236;
}

/*  Spherical coordinates.
 *
 *  x in [-1; 1] for u
 *  y in [-1; 1] for v
 *  z in [0; +inf)
 */
vec3 uvcoord_of(vec3 r)
{
    float depth = length(r);
    float u = 0;
    float v = 0;

    if (depth > 0)
    {
        vec3 n = r / depth;
        u = uvcoord_solve_u(n.x, n.y);
        v = uvcoord_solve_v(n.z);
    }

    return vec3(u, v, depth);
}

vec3 uvcoord(vec3 r)
{
    float u = r.x * 3.14159265359;
    float v = r.y * 1.57079632679;

    return vec3(cos(u), sin(u), sin(v)) * r.z;
}
