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

char* load_shader_source(const char* filename)
{
    FILE* fp = fopen(filename, "r");

    fseek(fp, 0, SEEK_END);
    long file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_length == -1)
        fprintf(stderr, "Failed to read %s", filename);

    char* data = (char*) calloc(file_length + 1, 1);
    fread(data, 1, file_length, fp);

    fclose(fp);

    return data;
}

static void print_shader_log(GLuint id)
{
    GLint length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

    char* msg = (char*) calloc(1, length + 1);
    glGetShaderInfoLog(id, length, &length, msg);

    fprintf(stderr, "%s", msg);

    free(msg);
}

GLuint make_vertex_shader(const char* source)
{
    GLuint id = glCreateShader(GL_VERTEX_SHADER);
    GLint length = strlen(source);
    glShaderSource(id, 1, &source, &length);
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    
    if (!status)
        print_shader_log(id);
    
    return id;
}

GLuint make_fragment_shader(const char* source)
{
    GLuint id = glCreateShader(GL_FRAGMENT_SHADER);
    GLint length = strlen(source);
    glShaderSource(id, 1, &source, &length);
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);

    if (!status)
        print_shader_log(id);

    return id;
}

GLuint make_shader_program(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    GLuint id = glCreateProgram();
    glAttachShader(id, vertex_shader_id);
    glAttachShader(id, fragment_shader_id);
    glLinkProgram(id);

    return id;
}

GLFWwindow* make_window(int width, int height, const char* title)
{
    assert(glfwInit() == GL_TRUE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

    glViewport(0, 0, width, height);

    glfwMakeContextCurrent(window);

    assert(glewInit() == GLEW_OK);

    return window;
}

#ifdef __cplusplus
}
#endif

#endif