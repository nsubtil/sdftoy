#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <map>
#include <vector>

#include "shaders.h"

void error_callback(int error, const char *description)
{
    printf("GLFW error: %s (%d)\n", description, error);
    exit(-1);
}

void check_gl_errors(void)
{
#ifndef NDEBUG
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("GL error: %d\n", err);
        exit(-1);
    }
#endif
}

char *shader_fname;
struct timespec last_timespec;

bool update_shader(void)
{
    FILE *fp;
    struct stat st;
    bool ret;

    fp = fopen(shader_fname, "rb");

    if (fp == nullptr)
    {
        printf("can't open shader\n");
        exit(-1);
    }

    fstat(fileno(fp), &st);

    if (memcmp(&st.st_mtimespec, &last_timespec, sizeof(struct timespec)) != 0)
    {
        fseek(fp, 0, SEEK_END);
        auto size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char buf[size + 1];
        buf[size] = 0;
        fread(buf, size, 1, fp);

        shader_map["external_shader"] = buf;
        last_timespec = st.st_mtimespec;

        ret = true;
    } else {
        ret = false;
    }

    fclose(fp);
    return ret;
}

GLuint vertex_buffer, index_buffer, vao;
glsl_program program;

void glsl_update(void)
{
    if (!update_shader())
        return;

    bool ret;
    ret = create_program(program,
                         {
                            "vertex/passthrough"
                         },
                         {
                           "fragment/shadertoy_interface",
                           "external_shader",
                         });
    if (ret == false)
    {
        ret = create_program(program, { "vertex/passthrough" }, { "fragment/red" });
        if (ret == false)
        {
            exit(-1);
        }
    }

    glUseProgram(program.program);

    check_gl_errors();

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(program.attributes["position"].index,   // shader attribute
                          2,                                      // number of components per attribute
                          GL_FLOAT,                               // data type
                          GL_FALSE,                               // normalized?
                          sizeof(GLfloat) * 2,                    // vertex stride
                          (void *) 0                              // offset into the array buffer
                          );
    check_gl_errors();

    glEnableVertexAttribArray(program.attributes["position"].index);
    check_gl_errors();
}

void init(void)
{
    static const float vertex_buffer_data[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
     };

     static const unsigned short index_buffer_data[] = {
        0, 1, 2, 3
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    glsl_update();
}

void render(int width, int height,
            float global_time,
            float frame_time,
            int frame_no)
{
    glViewport(0, 0, width, height);
    check_gl_errors();

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    check_gl_errors();

    if (program.has_uniform("iResolution"))
    {
        glUniform3f(program.uniforms["iResolution"], float(width), float(height), 1.0f);
        check_gl_errors();
    }

    if (program.has_uniform("iGlobalTime"))
    {
        glUniform1f(program.uniforms["iGlobalTime"], global_time);
        check_gl_errors();
    }

    if (program.has_uniform("iTimeDelta"))
    {
        glUniform1f(program.uniforms["iTimeDelta"], frame_time);
        check_gl_errors();
    }

    if (program.has_uniform("iFrame"))
    {
        glUniform1i(program.uniforms["iFrame"], frame_no);
        check_gl_errors();
    }

    glDrawElements(GL_TRIANGLE_STRIP,
                   4,
                   GL_UNSIGNED_SHORT,
                   (void *) 0);

    check_gl_errors();
}

int main(int argc, char **argv)
{
    GLFWwindow *window;

    shader_fname = argv[1];

    if (!glfwInit())
    {
        exit(-1);
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_STEREO, 0);
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

    window = glfwCreateWindow(640, 480, "SDF Toy", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    printf("OpenGL %s\n", glGetString(GL_VERSION));

    init();
    check_gl_errors();

    glfwSetTime(0.0);
    double last_frame_time = 0.0;
    int frame_number = 0;

    while (!glfwWindowShouldClose(window))
    {
        glsl_update();

        double frame_start, frame_end;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        frame_start = glfwGetTime();

        render(width, height, frame_start, last_frame_time, frame_number);
        glfwSwapBuffers(window);

        frame_end = glfwGetTime();
        last_frame_time = frame_end - frame_start;
        frame_number++;

        glfwPollEvents();
        usleep(0);

        check_gl_errors();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
