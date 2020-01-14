#include "model.h"

void Model::update_perspective(int screen_width, int screen_height)
{
    float aspect = (float) screen_width / (float) screen_height;
    glm_perspective(glm_rad(45.0f), aspect, 0.1f, 100.0f, s_perspective);   
}

void Model::init(int screen_width, int screen_height)
{
    char* vertex_shader_src = load_shader_source("shaders/vertex.glsl");
    char* fragment_shader_src = load_shader_source("shaders/fragment.glsl");

    s_vertex_shader = make_vertex_shader(vertex_shader_src);
    s_fragment_shader = make_fragment_shader(fragment_shader_src);

    free(vertex_shader_src);
    free(fragment_shader_src);

    s_program = make_shader_program(s_vertex_shader, s_fragment_shader);

    glUseProgram(s_program);

    s_uniforms.view = glGetUniformLocation(s_program, "view");
    s_uniforms.perspective = glGetUniformLocation(s_program, "perspective");
    s_uniforms.ambient_light = glGetUniformLocation(s_program, "ambient_light");
    s_uniforms.diffuse_light = glGetUniformLocation(s_program, "diffuse_light");

    update_perspective(screen_width, screen_height);
}

Model::Model(const char* path)
{
    uint32_t vertices_data_size;
    uint32_t indices_data_size;

    FILE* fp = fopen(path, "rb");

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);

    if (fsize == (long) -1)
    {
        fclose(fp);
        throw std::runtime_error("Model file couldn't be opened.");
    }

    if (fsize < (long)(sizeof(uint32_t) * 2))
    {
        fclose(fp);
        throw std::runtime_error("Model file size < (sizeof(uint32_t) * 2).");
    }

    fseek(fp, 0, SEEK_SET);

    fread(&vertices_data_size, sizeof(uint32_t), 1, fp);
    fread(&indices_data_size, sizeof(uint32_t), 1, fp);

    size_t expected_fsize = sizeof(uint32_t) * 2 + vertices_data_size + indices_data_size;

    if (expected_fsize != fsize)
    {
        fclose(fp);
        throw std::runtime_error("Model file size doesn't conform to its header.");
    }

    if (vertices_data_size % sizeof(float) != 0)
    {
        fclose(fp);
        throw std::runtime_error("Model file vertices data size is not a multiple of sizeof(float).");
    }

    if (indices_data_size % sizeof(uint32_t) != 0)
    {
        fclose(fp);
        throw std::runtime_error("Model file indices data size is not a multiple of sizeof(uint32_t).");
    }

    if ((vertices_data_size / sizeof(float)) % FLOATS_PER_VERTEX != 0)
    {
        fclose(fp);
        throw std::runtime_error("Model file had unexpected number of floats per vertex.");
    }

    float* vertices_data = (float*) malloc(vertices_data_size);
    uint32_t* indices_data = (uint32_t*) malloc(indices_data_size);

    fread(vertices_data, 1, vertices_data_size, fp);
    fread(indices_data, 1, indices_data_size, fp);

    fclose(fp);

    m_num_indices = indices_data_size / sizeof(uint32_t);

    glGenBuffers(1, &m_vbo_elements);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_elements);
    glBufferData(GL_ARRAY_BUFFER, vertices_data_size, vertices_data, GL_STATIC_DRAW);

    glGenBuffers(1, &m_vbo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_data_size, indices_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 0));
    glEnableVertexAttribArray(0);
    
    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    free(vertices_data);
    free(indices_data);
}

Model::~Model()
{
    glUseProgram(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo_elements);
    glDeleteBuffers(1, &m_vbo_indices);
    glDetachShader(s_program, s_vertex_shader);
    glDetachShader(s_program, s_fragment_shader);
    glDeleteShader(s_vertex_shader);
    glDeleteShader(s_fragment_shader);
    glDeleteProgram(s_program);
}

void Model::render(vec3 origin, vec3 pos)
{
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    vec3 origin_offset = { -origin[0], -origin[1], -20.0f - origin[2] };
    glm_translate_to(view, pos, view);
    glm_translate_to(view, origin_offset, view);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
    
    glUseProgram(s_program);

    glUniformMatrix4fv(s_uniforms.view, 1, GL_FALSE, (float*) view);
    glUniformMatrix4fv(s_uniforms.perspective, 1, GL_FALSE, (float*) s_perspective);
    glUniform3fv(s_uniforms.ambient_light, 1, (float*) s_ambient_light);
    glUniform3fv(s_uniforms.diffuse_light, 1, (float*) s_diffuse_light);

    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, (void*) 0);
}

GLuint Model::s_program = 0;
GLuint Model::s_vertex_shader = 0;
GLuint Model::s_fragment_shader = 0;
Model::uniforms_struct Model::s_uniforms = { 0 };
mat4 Model::s_perspective = { 0 };
vec3 Model::s_ambient_light = { 0.75f, 0.75f, 0.75f };
vec3 Model::s_diffuse_light = { 1.0f, 1.0f, 1.0f };