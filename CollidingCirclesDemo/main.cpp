#include <cassert>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "PhysicsEngine/ParticleWorld.h"
#include "PhysicsEngine/Particle.h"
#include "PhysicsEngine/ParticleForceRegistry.h"
#include "PhysicsEngine/ParticleGravityForce.h"
#include "PhysicsEngine/ParticleLinearDragForce.h"

#include "Circle.h"

struct IntegratorControlState {
    bool stepModeActive;
    bool stepButtonPressed;
    bool resetButtonPressed;
};

IntegratorControlState integratorControlState = {
    false, false, false
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLuint compile_shaders();

void ResolveVelocity(Particle* particleA, Particle* particleB, Vec2 hitNormal);
void HandleCollision(ParticleWorld* world);
void ResolvePenetration(Particle* particleA, Particle* particleB, Vec2 hitNormal, float penetration);
void HandleCollisionWithBorders(Circle* particle);

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

    // Begin: ImGui.

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // End: ImGui.

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
    std::srand(std::time(nullptr));
    ParticleWorld world;

    ParticleGravityForce gravityForce;
    ParticleLinearDragForce dragForce(0.5f);

    Circle circleA(Vec2(-5.5f, 3.5f), Vec2(20.0f, -12.0f), 0.785398f, 0.785398f);
    Circle circleB(Vec2(0.5f, 0.5f), Vec2(-2.0f, 1.0f), 3.0f, 3.0f);
    world.AddParticle(&circleA);
    world.AddParticle(&circleB);

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

        if (integratorControlState.stepModeActive) {
            if (integratorControlState.stepButtonPressed) {
                integratorControlState.stepButtonPressed = false;
                world.Step(0.02f);
            }
        }
        else {
            while (dtAccum > 0.02) {
                std::cout << "fixed update: 0.02" << std::endl;

                world.Step(0.02f);
                dtAccum -= 0.02;
            }
        }

        HandleCollision(&world);

        // Begin: Рендеринг.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        const int sectors = 16;
        const int circlesNum = world.Particles().size();

        const int indexStep = 2;
        const float angleStepRad = 6.28f / sectors;
        for (const Particle* p : world.Particles()) {
            float* points = new float[sectors * 2];

            const float radius = ((Circle*)p)->radius;

            // Вычисляем вектор для каждого сектора.
            float curAngleRad = 0.0f;
            int index = 0;
            for (int i = 0; i < sectors; i++) {
                float x = cos(curAngleRad) * radius;
                float y = sin(curAngleRad) * radius;

                points[index] = p->pos.x + x;
                points[index + 1] = p->pos.y + y;

                index += indexStep;
                curAngleRad += angleStepRad;
            }

            // NOTE: Не создавать каждый кадр.
            glBufferData(GL_ARRAY_BUFFER, sectors * 2 * sizeof(float), points, GL_DYNAMIC_DRAW);

            glBindVertexArray(vao);
            glDrawArrays(GL_LINE_LOOP, 0, sectors);
            glBindVertexArray(0);

            delete[] points;
        }

        glfwPollEvents();

        // End: Рендеринг.

        // Begin: ImGui.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Окно для пошагового выполнения интегратора.
        {
            ImGui::Begin("Integrator");

            if (ImGui::Button("Play")) {
                integratorControlState.stepModeActive = false;

                // Сбрасываем аккумулятор дельты времени.
                // 
                // В пошаговом режиме дельта времени может накопить большое значение.
                // При возврате в обычный режим эта дельта будет раздроблена фиксированной дельтой интегрирования.
                // Это даст очень большое количество шагов интегрирования,
                // которые должны будут выполниться оффлайн до отрисовки следующего кадра,
                // что в свою очередь заметно подвесит картинку.
                dtAccum = 0.0f;
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Pause")) {
                integratorControlState.stepModeActive = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Step")) {
                integratorControlState.stepButtonPressed = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset")) {
                integratorControlState.stepModeActive = false;
                dtAccum = 0.0f;
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // End: ImGui.

        glfwSwapBuffers(window);

        std::cout << "RENDER LOOP END" << std::endl << std::endl;
    }

    //
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();

    // Begin: ImGui.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // End: ImGui.

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

void HandleCollision(ParticleWorld* world) {
    for (Particle* particleA : world->Particles()) {
        Circle* circleA = dynamic_cast<Circle*>(particleA);

        for (Particle* particleB : world->Particles()) {
            Circle* circleB = dynamic_cast<Circle*>(particleB);

            // Не проверяем коллизию с самим собой.
            if (circleA == circleB)
                continue;

            const float actualDist = (circleA->pos - circleB->pos).Length();
            const float touchDist = circleA->radius + circleB->radius;  // Расстояние, при котором круги соприкасаются.
            if (actualDist <= touchDist) {
                // Нормаль удара - смотрит из B в A.
                Vec2 hitNormal = (circleA->pos - circleB->pos).Normalized();

                float penetration = touchDist - actualDist;

                ResolveVelocity(circleA, circleB, hitNormal);
                ResolvePenetration(circleA, circleB, hitNormal, penetration);
            }
        }

        HandleCollisionWithBorders(circleA);
    }
}

void ResolveVelocity(Particle* particleA, Particle* particleB, Vec2 hitNormal) {
    const float e = 1.0f;   // Коэффициент восстановления - определяет эластичность удара.

    // Скорость A в предположении, что B неподвижно - относительная скорость.
    Vec2 velocityARelB = particleA->velocity - particleB->velocity;

    // Нормальная составляющая относительной скорости A в СК удара, где
    // СК удара - это СК с осями нормаль/касательная,
    // причем ось нормали направлена обратно нормали удара,
    // что даёт положительную проекцию относительной скорости A на нормаль при сближении масс.
    Vec2 normalAxis = -hitNormal;
    float velocityARelBNormal = Vec2::Dot(velocityARelB, normalAxis);

    // Если массы в состоянии соприкосновения, но при этом покоятся или отдаляются друг от друга, то удара нет.
    // Такая ситуация возможна, например, сразу после обработки предыдущего удара.
    if (velocityARelBNormal <= 0.0f)
        return;

    // Ударные импульсы возникают вдоль нормали удара, имеют равные модули и противоположные направления.
    // 
    // Почему импульс выражен через обратные массы, а не прямые?
    // Если обратная масса и скорость второй точки равны нулю, то
    // данная формула без изменений будет эквивалентна формуле
    // для вычисления ударного импульса при столкновении точки с неподвижным объектом с бесконечно большой массой.
    // Поэтому, если мы хотим смоделировать удар о неподвижную стену, достаточно представить стену точкой с нулевой обратной массой.
    float invMassA = particleA->invMass;
    float invMassB = particleB->invMass;
    float impulseAbs = velocityARelBNormal * (1 + e) / (invMassA + invMassB);
    Vec2 impulseA = hitNormal * impulseAbs;
    Vec2 impulseB = -hitNormal * impulseAbs;

    particleA->velocity += impulseA * particleA->invMass;
    particleB->velocity += impulseB * particleB->invMass;
}

void ResolvePenetration(Particle* particleA, Particle* particleB, Vec2 hitNormal, float penetration) {
    float invMassA = particleA->invMass;
    float invMassB = particleB->invMass;

    // Мы делим полную величину проникновения между двумя точками по такому принципу:
    // во сколько раз первая масса больше второй, во столько раз смещение первой массы будет меньше смещения второй.
    // Другими словами, отношение смещений точек будет равно обратному отношению их масс.
    //
    // В чем преимущество выражения смещения через обратные массы?
    // Если обратная масса второй точки будет равна нулю,
    // то первая точка будет сдвинута на полную величину проникновения, а вторая точка не изменит положения.
    //
    // Следовательно, если вторая точка представляет, например, неподвижную стену,
    // то в устранении проникновения она участвовать не будет.
    float displacementA = penetration * invMassA / (invMassA + invMassB);
    float displacementB = penetration - displacementA;

    particleA->pos += hitNormal * displacementA;
    particleB->pos += -hitNormal * displacementB;
}

void HandleCollisionWithBorders(Circle* circle) {
    float penetration;
    Vec2 hitNormal;
    bool coll = false;

    // Левая.
    if (circle->pos.x - circle->radius <= -10.0f) {
        std::cout << "Hit left" << std::endl;

        hitNormal = Vec2::Right;
        float actualDist = fabs(-10.0f - circle->pos.x);
        float touchDist = circle->radius;
        penetration = touchDist - actualDist;
        coll = true;
    }

    // Правая.
    if (circle->pos.x + circle->radius >= 10.0f) {
        std::cout << "Hit right" << std::endl;

        hitNormal = Vec2::Left;
        float actualDist = fabs(10.0f - circle->pos.x);
        float touchDist = circle->radius;
        penetration = touchDist - actualDist;
        coll = true;
    }

    // Верхняя.
    if (circle->pos.y + circle->radius >= 7.5f) {
        std::cout << "Hit up" << std::endl;

        hitNormal = Vec2::Down;
        float actualDist = fabs(7.5f - circle->pos.y);
        float touchDist = circle->radius;
        penetration = touchDist - actualDist;
        coll = true;
    }

    // Нижняя.
    if (circle->pos.y - circle->radius <= -7.5f) {
        std::cout << "Hit bottom" << std::endl;

        hitNormal = Vec2::Up;
        float actualDist = fabs(-7.5f - circle->pos.y);
        float touchDist = circle->radius;
        penetration = touchDist - actualDist;
        coll = true;
    }

    if (coll) {
        // Объект с нулевой обратной массой (т.е. с бесконечно большой массой)
        // для представления неподвижных границ экрана.
        Particle immovableObject(Vec2::Zero, Vec2::Zero, 1.0f);
        immovableObject.invMass = 0.0f;

        ResolveVelocity(circle, &immovableObject, hitNormal);
        ResolvePenetration(circle, &immovableObject, hitNormal, penetration);
    }
}