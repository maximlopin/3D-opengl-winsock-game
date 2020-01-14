#ifndef OPENGL_H
#define OPENGL_H

#include "glew.h"
#include "glfw3.h"
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

char* load_shader_source(const char* filename);

static void print_shader_log(GLuint id);

GLuint make_vertex_shader(const char* source);

GLuint make_fragment_shader(const char* source);

GLuint make_shader_program(GLuint vertex_shader_id, GLuint fragment_shader_id);

GLFWwindow* make_window(int width, int height, const char* title);

#ifdef __cplusplus
}
#endif

#endif