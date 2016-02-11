#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders.h"

static void show_shader_log(GLuint object)
{
    GLint log_len;
    std::vector<char> log;

    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_len);
    log.resize(log_len);
    glGetShaderInfoLog(object, log_len, nullptr, log.data());
    printf("%s\n", log.data());
}

static void show_program_log(GLuint object)
{
    GLint log_len;
    std::vector<char> log;

    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_len);
    log.resize(log_len);
    glGetProgramInfoLog(object, log_len, nullptr, log.data());
    printf("%s\n", log.data());
}

static GLuint compile_shader(GLenum type, const std::vector<std::string>& names)
{
    const GLchar *src[names.size()];

    for(size_t i = 0; i < names.size(); i++)
    {
        if (shader_map.find(names[i]) == shader_map.end())
        {
            printf("couldn't find shader %s\n", names[i].c_str());
            exit(-1);
        }

        src[i] = shader_map[names[i]].c_str();
    }

    GLuint shader;

    check_gl_errors();

    shader = glCreateShader(type);
    check_gl_errors();

    glShaderSource(shader, names.size(), src, nullptr);
    check_gl_errors();

    glCompileShader(shader);
    check_gl_errors();

    GLint ret;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
    check_gl_errors();

    if(!ret)
    {
        printf("failed to compile { ");
        for(auto name : names)
        {
            printf("%s, ", name.c_str());
        }
        printf(" }:\n");
        show_shader_log(shader);

        return GLuint(-1);
    }

    check_gl_errors();

    return shader;
}

bool create_program(glsl_program& output,
                    std::vector<std::string> vertex_shaders,
                    std::vector<std::string> fragment_shaders)
{
    output.clear();

    output.vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shaders);
    if (output.vertex_shader == GLuint(-1))
    {
        return false;
    }

    check_gl_errors();

    output.fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shaders);
    if (output.fragment_shader == GLuint(-1))
    {
        return false;
    }

    check_gl_errors();

    output.program = glCreateProgram();
    glAttachShader(output.program, output.vertex_shader);
    check_gl_errors();
    glAttachShader(output.program, output.fragment_shader);
    check_gl_errors();

    glLinkProgram(output.program);
    check_gl_errors();

    GLint ret;
    glGetProgramiv(output.program, GL_LINK_STATUS, &ret);
    if(!ret)
    {
        printf("link failure:\n");
        show_program_log(output.program);

        return false;
    }

    // extract uniform locations
    GLint uniform_count;
    glGetProgramiv(output.program, GL_ACTIVE_UNIFORMS, &uniform_count);

    if (uniform_count)
    {
        GLint uniform_name_max_len;
        glGetProgramiv(output.program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_name_max_len);

        std::vector<GLchar> uniform_name(uniform_name_max_len + 1);

        for(GLint i = 0; i < uniform_count; i++)
        {
            glGetActiveUniformName(output.program, i, uniform_name.size(), nullptr, uniform_name.data());
            output.uniforms[std::string(uniform_name.data())] = i;
        }
    }

    // extract attribute slots
    GLint attribute_count;
    glGetProgramiv(output.program, GL_ACTIVE_ATTRIBUTES, &attribute_count);

    if (attribute_count)
    {
        GLint attribute_name_max_len;
        glGetProgramiv(output.program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribute_name_max_len);

        std::vector<GLchar> attribute_name(attribute_name_max_len + 1);

        glsl_attribute attribute;

        for(GLint i = 0; i < attribute_count; i++)
        {
            attribute.index = i;
            glGetActiveAttrib(output.program, i, attribute_name.size(), nullptr,
                              &attribute.size, &attribute.type, attribute_name.data());
            output.attributes[std::string(attribute_name.data())] = attribute;
        }
    }

    check_gl_errors();

    return true;
}
