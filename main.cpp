#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <GLFW/glfw3.h>

void error_callback(int error, const char *description)
{
    printf("GLFW error: %s (%d)\n", description, error);
    exit(-1);
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

    window = glfwCreateWindow(640, 480, "SDF Toy", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
        usleep(0);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
