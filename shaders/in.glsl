float diffF(vec3 norm, vec3 lightDir) {
    return max(dot(norm, lightDir), 0.0);
}
