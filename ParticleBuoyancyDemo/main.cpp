#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "PhysicsEngine/ParticleWorld.h"
#include "PhysicsEngine/Particle.h"
#include "PhysicsEngine/ParticleForceRegistry.h"
#include "PhysicsEngine/ParticleGravityForce.h"
#include "PhysicsEngine/ParticleLinearDragForce.h"
#include "PhysicsEngine/ParticleSpringForce.h"
#include "PhysicsEngine/ParticleAnchoredSpringForce.h"
#include "PhysicsEngine/ParticleBungeeForce.h"
#include "PhysicsEngine/ParticleBuoyantForce.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLuint compile_shaders();

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Physics Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // OpenGL version.
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "GL_RENDERER: " << renderer << std::endl;
    std::cout << "GL_VERSION: " << version << std::endl;
    std::cout << std::endl;

    //
    GLuint program = compile_shaders();

    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Устанавливаем матрицу ортографической проекции.
    float ortho[] = {
        2.0f / (10.0f - (-10.0f)),      0.0f,                       0.0f, 0.0f,
        0.0f,                           2.0f / (7.5f - (-7.5f)),    0.0f, 0.0f,
        0.0f,                           0.0f,                       1.0f, 0.0f,
        0.0f,                           0.0f,                       0.0f, 1.0f
    };
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, false, ortho);

    // Настройка физического мира.
    ParticleWorld world;

    ParticleGravityForce gravityForce;
    ParticleLinearDragForce dragForce(100.0f);

    float liquidSurfaceY = 0.0f;
    float waterDensity = 1000.0f;
    float hydroDragCoeff = 1000.0f;
    float particleSize = 1.0f;
    //Particle pA(Vec2(-9.0f, -7.5f), Vec2::Zero, 300.0f); // Кубический метр дерева.
    Particle pA(Vec2(-9.0f, 7.5f), Vec2::Zero, 500.0f); // Кубический метр дерева.

    ParticleBuoyantForce buoyantForceOnA(liquidSurfaceY, particleSize, waterDensity, hydroDragCoeff);
    world.AddParticle(&pA);
    world.forceRegistry.Add(&pA, &buoyantForceOnA);
    world.forceRegistry.Add(&pA, &gravityForce);
    world.forceRegistry.Add(&pA, &dragForce);

    // Для отрисовки вспомогательных объектов.
    Vec2 liquidLeftPoint(-10.0f, liquidSurfaceY);
    Vec2 liquidRightPoint(10.0f, liquidSurfaceY);

    glfwSetTime(0);
    double dtAccum = 0;
    while (!glfwWindowShouldClose(window)) {
        std::cout << "RENDER LOOP BEGIN" << std::endl;

        // Симуляция физики.
        double dt = glfwGetTime();
        glfwSetTime(0);

        dtAccum += dt;

        std::cout << "dt: " << dt << "s" << std::endl;
        std::cout << "dtAccum: " << dtAccum << "s" << std::endl;

        std::cout << std::endl;
        while (dtAccum > 0.02) {
            std::cout << "fixed update: 0.02" << std::endl;

            world.Step(0.02f);
            dtAccum -= 0.02;
        }

        // Рендеринг.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        /*
        // Begin: Отрисовка частиц точками.
        const int particlesNum = world.Particles().size();
        float* particles = new float[particlesNum * 2];

        int index = 0;
        const int indexStep = 2;
        for (const Particle* p : world.Particles()) {
            particles[index] = p->pos.x;
            particles[index + 1] = p->pos.y;
            index += indexStep;
        }

        // NOTE: Не создавать каждый кадр.
        glBufferData(GL_ARRAY_BUFFER, particlesNum * sizeof(float) * 2, particles, GL_DYNAMIC_DRAW);

        glBindVertexArray(vao);
        glPointSize(8.0f);
        glDrawArrays(GL_POINTS, 0, particlesNum);
        glBindVertexArray(0);

        delete[] particles;
        // End: Отрисовка частиц точками.
        */

        // Begin: Отрисовка поверхности жидкости.
        float liquidPoints[] = {
            liquidLeftPoint.x, liquidLeftPoint.y,
            liquidRightPoint.x, liquidRightPoint.y
        };

        glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(float), liquidPoints, GL_DYNAMIC_DRAW);

        glBindVertexArray(vao);
        glPointSize(0.0f);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        // End: Отрисовка поверхности жидкости.

        // Begin: Отрисовка частиц квадратами.
        const int particlesNum = world.Particles().size();
        float* points = new float[particlesNum * 4 * 2];

        int index = 0;
        const int indexStep = 8;
        for (const Particle* p : world.Particles()) {
            points[index] = p->pos.x - particleSize / 2.0f;
            points[index + 1] = p->pos.y - particleSize / 2.0f;

            points[index + 2] = p->pos.x - particleSize / 2.0f;
            points[index + 3] = p->pos.y + particleSize / 2.0f;

            points[index + 4] = p->pos.x + particleSize / 2.0f;
            points[index + 5] = p->pos.y + particleSize / 2.0f;

            points[index + 6] = p->pos.x + particleSize / 2.0f;
            points[index + 7] = p->pos.y - particleSize / 2.0f;

            index += indexStep;
        }

        // NOTE: Не создавать каждый кадр.
        glBufferData(GL_ARRAY_BUFFER, particlesNum * 4 * 2 * sizeof(float), points, GL_DYNAMIC_DRAW);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_LOOP, 0, particlesNum * 4);
        glBindVertexArray(0);

        delete[] points;
        // End: Отрисовка частиц квадратами.

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::cout << "RENDER LOOP END" << std::endl << std::endl;
    }

    //
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint compile_shaders() {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    static const GLchar* vertex_shader_source[] = {
        "#version 450 core\n"
        "layout(location = 0) in vec2 pos;\n"
        "\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "gl_Position = projection * vec4(pos, 0.0, 1.0);\n"
        "}\n"
    };

    static const GLchar* fragment_shader_source[] = {
        "#version 450 core\n"
        "\n"
        "out vec4 color;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "color = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n"
    };

    // Create and compile vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    GLint success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    std::cout << "vertex_shader compiled: " << success << std::endl;

    // Create and compile fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    success = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    std::cout << "fragment_shader compiled: " << success << std::endl;

    // Create program, attach shaders to it, and link it
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&success);
    std::cout << "program linked: " << success << std::endl;

    // Delete the shaders as the program has them now
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}