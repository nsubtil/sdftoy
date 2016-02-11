#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("GL error: %d\n", err);
        exit(-1);
    }
}

GLuint vertex_buffer, index_buffer, vao;
glsl_program program;

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

    // setup_shader("vertex/passthrough", "fragment/screen_uv_color");

    bool ret;
    ret = create_program(program, { "vertex/passthrough" }, { "fragment/screen_uv_color" });
    if (ret == false)
    {
        exit(1);
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

void render(int width, int height)
{
    glViewport(0, 0, width, height);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform2f(program.uniforms["screen_resolution"], float(width), float(height));
    glDrawElements(GL_TRIANGLE_STRIP,
                   4,
                   GL_UNSIGNED_SHORT,
                   (void *) 0);
}

int main(int argc, char **argv)
{
    GLFWwindow *window;

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

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        render(width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
        usleep(0);

        check_gl_errors();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
