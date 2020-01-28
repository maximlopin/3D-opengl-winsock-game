#ifndef MODEL_H
#define MODEL_H

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "glew.h"
#include "glfw3.h"
#include "opengl.h"
#include "cglm/cglm.h"
#include "assert.h"
#include <stdexcept>
#include "logging.h"

#define FLOATS_PER_VERTEX 6

class Model {
public:

    Model(const char* path);

    static void init(int screen_width, int screen_height);
    static void cleanup();

    static void update_perspective(int screen_width, int screen_height);

    ~Model();

    void render(vec3 origin, vec3 pos);

private:
    GLuint m_vbo_elements;
    GLuint m_vbo_indices;
    GLuint m_vao;

    GLsizei m_num_indices;

    static GLuint s_program;
    static GLuint s_vertex_shader;
    static GLuint s_fragment_shader;

    static struct uniforms_struct {
        GLint view;
        GLint perspective;
        GLint ambient_light;
        GLint diffuse_light;
    } s_uniforms;

    static mat4 s_perspective;
    static vec3 s_ambient_light;
    static vec3 s_diffuse_light;
};


#endif