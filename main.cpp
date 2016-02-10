#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <map>
#include <vector>

extern std::map<std::string, std::string> shader_map;

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

void show_shader_log(GLuint object)
{
    GLint log_len;
    std::vector<char> log;

    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_len);
    log.resize(log_len);
    glGetShaderInfoLog(object, log_len, nullptr, log.data());
    printf("%s\n", log.data());
}

void show_program_log(GLuint object)
{
    GLint log_len;
    std::vector<char> log;

    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_len);
    log.resize(log_len);
    glGetProgramInfoLog(object, log_len, nullptr, log.data());
    printf("%s\n", log.data());
}

void compile_shader(GLuint shader, const char *name)
{
    const GLchar *src = shader_map[name].c_str();

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    check_gl_errors();

    GLint ret;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
    if(!ret)
    {
        printf("failed to compile %s:\n", name);
        show_shader_log(shader);
    }

    check_gl_errors();
}

GLuint vert_shader, frag_shader, program;
GLuint vert_shader_input_position;
GLuint uniform_screen_resolution_location;

void setup_shader(const char *vertex_name, const char *frag_name)
{
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(vert_shader, vertex_name);

    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(frag_shader, frag_name);

    program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    check_gl_errors();

    GLint ret;
    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if(!ret)
    {
        printf("failed to link %s + %s\n", vertex_name, frag_name);
        show_program_log(program);
    }

    check_gl_errors();

    glUseProgram(program);

    vert_shader_input_position = glGetAttribLocation(program, "position");
    uniform_screen_resolution_location = glGetUniformLocation(program, "screen_resolution");

    check_gl_errors();
}

GLuint vertex_buffer, index_buffer, vao;

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

    setup_shader("vertex/passthrough", "fragment/screen_uv_color");

    check_gl_errors();

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(0,       // shader attribute
                          2,                                // number of components per attribute
                          GL_FLOAT,                         // data type
                          GL_FALSE,                         // normalized?
                          sizeof(GLfloat) * 2,              // vertex stride
                          (void *) 0                        // offset into the array buffer
                          );
    check_gl_errors();

    glEnableVertexAttribArray(0);
    check_gl_errors();
}

void render(int width, int height)
{
    glViewport(0, 0, width, height);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform2f(uniform_screen_resolution_location, float(width), float(height));
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
