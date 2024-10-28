#include <cmath>
#include <iostream>
#include <array>
#include <format>

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void init_imgui(GLFWwindow* window);
void shut_imgui();

using glm::mat4;
using glm::vec3;

static const char* shader_vertex_src = R"(
#version 460 core
layout (location=0) in vec3 pos;
uniform mat4 model;
uniform mat4 projection;
void main()
{
    //gl_Position = projection * model * vec4(pos, 1.0);
    gl_Position = vec4(pos, 1.0);
}
)";

static const char* shader_fragment_src = R"(
#version 460 core
layout (location=0) out vec4 frag_color;
void main()
{
	frag_color = vec4(0.0, 0.0, 0.0, 1.0);
};
)";

struct PerFrameData {
    mat4 mvp;
    int isWireframe;
};

int main() {
    glfwSetErrorCallback(
        [](int error, const char* description)
        {
            fprintf(stderr, "Error: %s\n", description);
        }
    );

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Box", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(
        window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    );

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "GL_RENDERER: " << renderer << std::endl;
    std::cout << "GL_VERSION: " << version << std::endl;
    std::cout << std::endl;

    const GLuint shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vertex, 1, &shader_vertex_src, nullptr);
    glCompileShader(shader_vertex);

    const GLuint shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_fragment, 1, &shader_fragment_src, nullptr);
    glCompileShader(shader_fragment);

    const GLuint program = glCreateProgram();
    glAttachShader(program, shader_vertex);
    glAttachShader(program, shader_fragment);
    glLinkProgram(program);
    glUseProgram(program);

    std::array<float, 108> vertices = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };
    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    init_imgui(window);

    while (!glfwWindowShouldClose(window)) {
        std::cout << "BEGIN: RENDER_LOOP" << std::endl;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const mat4 m = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -2.5f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
        //const mat4 m = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -2.0f));
        //const mat4 m = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -2.5f));
        //const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 10.0f);
        const mat4 p = glm::perspective(0.7854f, ratio, 0.1f, 10.0f);
        const mat4 pm = p * m;

        unsigned int model_loc = glGetUniformLocation(program, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(m));

        unsigned int projection_loc = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &p[0][0]);

        float vertices_transformed[108];
        for (int index = 0, i = 0; i < 36; i++, index += 3) {
            glm::vec4 t(vertices[index], vertices[index + 1], vertices[index + 2], 1.0f);
            t = pm * t;
            glm::vec3 result = glm::vec3(t) / t.w;

            vertices_transformed[index] = result.x;
            vertices_transformed[index + 1] = result.y;
            vertices_transformed[index + 2] = result.z;

            std::cout << std::format("vertices[{}]=({},{},{}).\n", index, result.x, result.y, result.z);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_transformed), vertices_transformed, GL_STATIC_DRAW);

        glBindVertexArray(vao);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(3.0f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::cout << "END: RENDER_LOOP" << std::endl << std::endl;
    }

    glDeleteProgram(program);
    glDeleteShader(shader_fragment);
    glDeleteShader(shader_vertex);
    glDeleteVertexArrays(1, &vao);

    shut_imgui();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void init_imgui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsLight();
}

void shut_imgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
