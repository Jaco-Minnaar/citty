#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void draw_triangle(unsigned int shader_program, unsigned int vao);
char* read_file_contents(char* file_path);

int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "citty", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    GLenum err = glewInit();

    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return -1;
    }
    
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    char* v_shader_src = read_file_contents("../src/shaders/vertex.glsl");

    if (v_shader_src == NULL) {
        fprintf(stderr, "Could not get vertex shader.");

        return -1;
    }

    char* srcs[] = { v_shader_src };
    unsigned int v_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader, 1, srcs, NULL);
    glCompileShader(v_shader);
    
    int  success;
    char infoLog[512];
    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(v_shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);

        return -1;
    }
    
    char* f_shader_src = read_file_contents("../src/shaders/fragment.glsl");

    if (v_shader_src == NULL) {
        fprintf(stderr, "Could not get vertex shader.");

        return -1;
    }

    char* f_srcs[] = { f_shader_src };
    unsigned int f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader, 1, f_srcs, NULL);
    glCompileShader(f_shader);
    
    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(f_shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);

        return -1;
    }

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, v_shader);
    glAttachShader(shader_program, f_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);

        return -1;
    }

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    glUseProgram(shader_program);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f, // bottom right
         0.5f,  0.5f, 0.0f, // top right
         -0.5f, 0.5f, 0.0f  // top left
    };  

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int ebo, vbo, vao;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        

        draw_triangle(shader_program, vao);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shader_program);
    
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void draw_triangle(unsigned int shader_program, unsigned int vao) {
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

char* read_file_contents(char* file_path) {
    FILE *f_ptr = fopen(file_path, "r");

    if (f_ptr == NULL) {
        fprintf(stderr, "Unable to open file %s\n", file_path);

        return NULL;
    }

    fseek(f_ptr, 0l, SEEK_END);
    long size = ftell(f_ptr);
    fseek(f_ptr, 0, SEEK_SET);
    
    if (size == 0) {
        fprintf(stderr, "File %s is empty\n", file_path);

        return NULL;
    }
    
    int buf_size = size + 1 * sizeof(char);
    char *buf = malloc(buf_size);
    if (buf == NULL) {
        fprintf(stderr, "Could not allocate enough memory to read file %s\n", file_path);

        return NULL;
    }

    if (fread(buf, buf_size, sizeof(char), f_ptr) == -1) {
        fprintf(stderr, "Could not read file %s\n", file_path);

        return NULL;
    }

    fclose(f_ptr);

    buf[size] = '\0';
    

    return buf;
}
