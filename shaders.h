#include <string>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct glsl_attribute
{
    GLuint index;
    GLint size;
    GLenum type;
};

struct glsl_program
{
    std::vector<std::string> vertex_shader_names;
    std::vector<std::string> fragment_shader_names;

    std::vector<GLuint> vertex_shaders;
    std::vector<GLuint> fragment_shaders;

    std::map<std::string, GLuint> uniforms;
    std::map<std::string, glsl_attribute> attributes;

    GLuint program;

    void clear(void)
    {
        vertex_shader_names.clear();
        fragment_shader_names.clear();

        for(auto i : vertex_shaders)
        {
            glDeleteShader(i);
        }

        vertex_shaders.clear();

        for(auto i : fragment_shaders)
        {
            glDeleteShader(i);
        }

        fragment_shaders.clear();

        uniforms.clear();
        attributes.clear();

        glDeleteProgram(program);
    }
};

extern std::map<std::string, std::string> shader_map;
extern void check_gl_errors(void);

extern bool create_program(glsl_program& output,
                           std::vector<std::string> vertex_shaders,
                           std::vector<std::string> fragment_shaders);
