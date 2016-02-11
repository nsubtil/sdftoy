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

    GLuint vertex_shader;
    GLuint fragment_shader;

    std::map<std::string, GLuint> uniforms;
    std::map<std::string, glsl_attribute> attributes;

    GLuint program;

    glsl_program()
        : vertex_shader(GLuint(-1)),
          fragment_shader(GLuint(-1)),
          program(GLuint(-1))
    { }

    bool has_uniform(const std::string name)
    {
        return uniforms.find(name) != uniforms.end();
    }

    bool has_attribute(const std::string name)
    {
        return attributes.find(name) != attributes.end();
    }

    void clear(void)
    {
        vertex_shader_names.clear();
        fragment_shader_names.clear();

        if (vertex_shader != GLuint(-1))
        {
            glDeleteShader(vertex_shader);
            vertex_shader = GLuint(-1);
        }

        if (fragment_shader != GLuint(-1))
        {
            glDeleteShader(fragment_shader);
            fragment_shader = GLuint(-1);
        }

        uniforms.clear();
        attributes.clear();

        if (program != GLuint(-1))
        {
            glDeleteProgram(program);
            program = GLuint(-1);
        }
    }
};

extern std::map<std::string, std::string> shader_map;
extern void check_gl_errors(void);

extern bool create_program(glsl_program& output,
                           std::vector<std::string> vertex_shaders,
                           std::vector<std::string> fragment_shaders);
