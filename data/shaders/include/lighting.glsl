float lighting_diffuse(
    vec3    a_light_vector,
    vec3    a_normal
) {
    return dot(a_light_vector, a_normal);
}

float lighting_specular(
    vec3    a_light_vector,
    vec3    a_eye_vector,
    vec3    a_normal
) {
    vec3 r = normalize(a_eye_vector);
    vec3 v = reflect(r, a_normal);

    return dot(a_light_vector, v);
}
