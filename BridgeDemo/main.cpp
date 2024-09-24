#include <cmath>
#include <iostream>
#include <array>
#include <format>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicsEngine/ParticleWorld.h"
#include "PhysicsEngine/Particle.h"
#include "PhysicsEngine/ParticleForceRegistry.h"
#include "PhysicsEngine/ParticleGravityForce.h"
#include "PhysicsEngine/ParticleLinearDragForce.h"
#include "PhysicsEngine/ParticleCable.h"
#include "PhysicsEngine/ParticleRod.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLuint compile_shaders();
void init_imgui(GLFWwindow* window);
void show_integrator_window(double& dt_accum);
void show_mass_props_window();
void shut_imgui();

int selected_particle_index = 0;

ParticleWorld world;

struct IntegratorControlState {
    bool step_mode_active;
    bool step_button_pressed;
    bool reset_button_pressed;
};

IntegratorControlState integrator_control_state = { true, false, false };

constexpr unsigned int scr_width = 800;
constexpr unsigned int scr_height = 600;

unsigned int cur_width = scr_width;
unsigned int cur_height = scr_height;

const char* vertex_shader_source_point =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 vert_color;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(pos, 1.0);\n"
"   vert_color = color;\n"
"}\0";

const char* fragment_shader_source_point =
"#version 330 core\n"
"in vec3 vert_color;\n"
"out vec4 frag_color;\n"
"void main()\n"
"{\n"
"   frag_color = vec4(vert_color, 1.0);\n"
"}\n\0";

const char* vertex_shader_source_line =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(pos, 1.0);\n"
"}\0";

const char* fragment_shader_source_line =
"#version 330 core\n"
"uniform vec4 in_color;"
"out vec4 frag_color;\n"
"void main()\n"
"{\n"
"   frag_color = in_color;\n"
"}\n\0";

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "Physics Engine", NULL, NULL);
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

    // Компиляция вершинного шейдера.
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source_point, NULL);
    glCompileShader(vertex_shader);
    // Проверка статуса компиляции.
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Компиляция фрагментного шейдера.
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source_point, NULL);
    glCompileShader(fragment_shader);
    // Проверка статуса компиляции.
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Линковка шейдеров.
    unsigned int shader_program_point = glCreateProgram();
    glAttachShader(shader_program_point, vertex_shader);
    glAttachShader(shader_program_point, fragment_shader);
    glLinkProgram(shader_program_point);
    // Проверка на ошибки.
    glGetProgramiv(shader_program_point, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_point, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << '\n';
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Begin: Компиляция шейдеров для отрисовки линий.
    // Компиляция вершинного шейдера.
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source_line, NULL);
    glCompileShader(vertex_shader);
    // Проверка статуса компиляции.
    //int success;
    //char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Компиляция фрагментного шейдера.
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source_line, NULL);
    glCompileShader(fragment_shader);
    // Проверка статуса компиляции.
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Линковка шейдеров.
    unsigned int shader_program_line = glCreateProgram();
    glAttachShader(shader_program_line, vertex_shader);
    glAttachShader(shader_program_line, fragment_shader);
    glLinkProgram(shader_program_line);
    // Проверка на ошибки.
    glGetProgramiv(shader_program_line, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_line, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << '\n';
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    // End.

    // OpenGL version.
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "GL_RENDERER: " << renderer << std::endl;
    std::cout << "GL_VERSION: " << version << std::endl;
    std::cout << std::endl;

    init_imgui(window);

    // Настройки для отрисовки точечных масс.
    GLuint vao_point;
    GLuint vbo_point;

    glGenVertexArrays(1, &vao_point);
    glBindVertexArray(vao_point);

    glGenBuffers(1, &vbo_point);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point);

    // Координаты вершины.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Цвет вершины.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Begin: Настройки для отрисовки линий.
    GLuint vao_line;
    GLuint vbo_line;

    glGenVertexArrays(1, &vao_line);
    glBindVertexArray(vao_line);

    glGenBuffers(1, &vbo_line);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_line);

    // Координаты вершины.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    // End.

    // Настройка физического мира.
    ParticleGravityForce gravity_force;
    ParticleLinearDragForce drag_force(1.0f);

    //--
    std::array<Particle, 12> particle_array;
    for (int i = 0; i < particle_array.size(); i++) {
        Particle& particle = particle_array.at(i);

        if (i == 10) {
            particle.mass = 5.0f;
        }

        world.add_particle(&particle);
        particle.pos = Vec3(float(i / 2) * 2.0f - 5.0f, 4, float(i % 2) * 2.0f - 1.0f);
        particle.velocity = Vec3::zero;

        world.force_registry.add(&particle, &gravity_force);
        //world.force_registry.add(&particle_array[i], &drag_force);
    }

    std::array<ParticleCable, 10> cables;
    for (int i = 0; i < cables.size(); i++) {
        ParticleCable& cable = cables.at(i);
        cable.particle_a = &particle_array.at(i);
        cable.particle_b = &particle_array.at(i + 2);
        cable.max_length = 1.9f;
        cable.restitution = 0.3f;
        world.add_contact_generator(&cable);
    }

    std::array<ParticleCable, 12> supports;
    std::array<Particle, 12> anchor_array;
    for (int i = 0; i < 12; i++) {
        Particle& anchor = anchor_array.at(i);
        world.add_particle(&anchor);
        anchor.inv_mass = 0.0f;
        anchor.pos = Vec3(float(i / 2) * 2.2f - 5.5f, 6, float(i % 2) * 1.6f - 0.8f);

        ParticleCable& support = supports.at(i);
        support.particle_a = &anchor;
        support.particle_b = &particle_array.at(i);

        if (i < 6)
            support.max_length = float(i / 2) * 0.5f + 3.0f;
        else
            support.max_length = 5.5f - float(i / 2) * 0.5f;

        support.restitution = 0.5f;
        world.add_contact_generator(&support);
    }

    std::array<ParticleRod, 6> rods;
    for (int i = 0; i < rods.size(); i++) {
        ParticleRod& rod = rods.at(i);
        rod.particle_a = &particle_array.at(i * 2);
        rod.particle_b = &particle_array.at(i * 2 + 1);
        rod.length = 2.0f;
        world.add_contact_generator(&rod);
    }
    //--

    glfwSetTime(0.0);
    double dt_accum = 0.0;
    while (!glfwWindowShouldClose(window)) {
        std::cout << "RENDER LOOP BEGIN" << std::endl;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Begin: Отрисовка точек.
        glUseProgram(shader_program_point);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, -3.5f, -8.0f));
        //view = glm::rotate(view, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(60.0f), (float)cur_width / (float)cur_height, 1.0f, 500.0f);

        unsigned int model_loc = glGetUniformLocation(shader_program_point, "model");
        unsigned int view_loc = glGetUniformLocation(shader_program_point, "view");

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(shader_program_point, "projection"), 1, GL_FALSE, &projection[0][0]);

        // Симуляция физики.
        double dt = glfwGetTime();
        glfwSetTime(0);

        dt_accum += dt;

        std::cout << "dt: " << dt << "s" << std::endl;
        std::cout << "dtAccum: " << dt_accum << "s" << std::endl;

        if (integrator_control_state.step_mode_active) {
            if (integrator_control_state.step_button_pressed) {
                integrator_control_state.step_button_pressed = false;
                world.step(0.02f);
            }
        }
        else {
            std::cout << std::endl;
            while (dt_accum > 0.02) {
                std::cout << "fixed update: 0.02" << std::endl;

                world.step(0.02f);
                dt_accum -= 0.02;
            }
        }

        // Рендеринг.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Рендеринг масс.
        const int particles_num = world.get_particles().size();
        float* particles_attribs = new float[particles_num * 6];

        int index = 0;
        const int index_step = 6;
        const auto particles = world.get_particles();
        for (int i = 0; i < world.get_particles().size(); i++) {
            // Координаты.
            particles_attribs[index] = particles[i]->pos.x;
            particles_attribs[index + 1] = particles[i]->pos.y;
            particles_attribs[index + 2] = particles[i]->pos.z;

            // Цвет.
            if (i == selected_particle_index) {
                particles_attribs[index + 3] = 1.0f;
                particles_attribs[index + 4] = 0.0f;
                particles_attribs[index + 5] = 0.0f;
            }
            else {
                particles_attribs[index + 3] = 0.0f;
                particles_attribs[index + 4] = 0.0f;
                particles_attribs[index + 5] = 0.0f;
            }

            index += index_step;
        }

        // NOTE: Не создавать каждый кадр.
        glBindBuffer(GL_ARRAY_BUFFER, vbo_point);
        glBufferData(GL_ARRAY_BUFFER, particles_num * sizeof(float) * 6, particles_attribs, GL_DYNAMIC_DRAW);

        glBindVertexArray(vao_point);
        glPointSize(8.0f);
        glDrawArrays(GL_POINTS, 0, particles_num);

        delete[] particles_attribs;

        // Begin: Отрисовка линий.
        glBindVertexArray(vao_line);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
        glUseProgram(shader_program_line);
        model_loc = glGetUniformLocation(shader_program_line, "model");
        view_loc = glGetUniformLocation(shader_program_line, "view");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader_program_line, "projection"), 1, GL_FALSE, &projection[0][0]);

        int in_color_loc = glGetUniformLocation(shader_program_line, "in_color");
        glUniform4f(in_color_loc, 0.7f, 0.7f, 0.7f, 1.0f);
        const int support_vertices_count = 2 * supports.size();
        std::array<float, support_vertices_count * 3> support_vertices;
        for (int i = 0, j = 0; i < supports.size(); i++, j += 6) {
            support_vertices.at(j) = supports.at(i).particle_a->pos.x;
            support_vertices.at(j + 1) = supports.at(i).particle_a->pos.y;
            support_vertices.at(j + 2) = supports.at(i).particle_a->pos.z;

            support_vertices.at(j + 3) = supports.at(i).particle_b->pos.x;
            support_vertices.at(j + 4) = supports.at(i).particle_b->pos.y;
            support_vertices.at(j + 5) = supports.at(i).particle_b->pos.z;
        }
        glBufferData(GL_ARRAY_BUFFER, support_vertices_count * 3 * sizeof(float), support_vertices.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(vao_line);
        glDrawArrays(GL_LINES, 0, supports.size() * 2);

        // Рендеринг верёвок.
        glUniform4f(in_color_loc, 0.0f, 1.0f, 0.0f, 1.0f);
        const int cable_vertices_count = 2 * cables.size();
        std::array<float, cable_vertices_count * 3> cable_vertices;
        for (int i = 0, j = 0; i < cables.size(); i++, j += 6) {
            cable_vertices.at(j) = cables.at(i).particle_a->pos.x;
            cable_vertices.at(j + 1) = cables.at(i).particle_a->pos.y;
            cable_vertices.at(j + 2) = cables.at(i).particle_a->pos.z;

            cable_vertices.at(j + 3) = cables.at(i).particle_b->pos.x;
            cable_vertices.at(j + 4) = cables.at(i).particle_b->pos.y;
            cable_vertices.at(j + 5) = cables.at(i).particle_b->pos.z;
        }

        glBufferData(GL_ARRAY_BUFFER, cable_vertices_count * 3 * sizeof(float), cable_vertices.data(), GL_DYNAMIC_DRAW);

        //glBindVertexArray(vao_line);
        glDrawArrays(GL_LINES, 0, cables.size() * 2);

        // Рендеринг стержней.
        glUniform4f(in_color_loc, 0.0f, 0.0f, 1.0f, 1.0f);
        const int rod_vertices_count = 2 * rods.size();
        std::array<float, rod_vertices_count * 3> rod_vertices;
        for (int i = 0, j = 0; i < rods.size(); i++, j += 6) {
            rod_vertices.at(j) = rods.at(i).particle_a->pos.x;
            rod_vertices.at(j + 1) = rods.at(i).particle_a->pos.y;
            rod_vertices.at(j + 2) = rods.at(i).particle_a->pos.z;

            rod_vertices.at(j + 3) = rods.at(i).particle_b->pos.x;
            rod_vertices.at(j + 4) = rods.at(i).particle_b->pos.y;
            rod_vertices.at(j + 5) = rods.at(i).particle_b->pos.z;
        }

        glBufferData(GL_ARRAY_BUFFER, rod_vertices_count * 3 * sizeof(float), rod_vertices.data(), GL_DYNAMIC_DRAW);

        //glBindVertexArray(vao_line);
        glDrawArrays(GL_LINES, 0, rods.size() * 2);
        glBindVertexArray(0);

        show_integrator_window(dt_accum);

        // ImGui::ShowDemoWindow();
        show_mass_props_window();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::cout << "RENDER LOOP END" << std::endl << std::endl;
    }

    //
    glDeleteProgram(shader_program_point);
    glDeleteVertexArrays(1, &vao_point);
    glDeleteBuffers(1, &vbo_point);

    glDeleteProgram(shader_program_line);
    glDeleteVertexArrays(1, &vao_line);
    glDeleteBuffers(1, &vbo_line);

    glfwTerminate();

    shut_imgui();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    cur_width = width;
    cur_height = height;
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

void show_integrator_window(double& dt_accum) {
    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();

    // Окно для пошагового выполнения интегратора.
    ImGui::Begin("Integrator");

    if (ImGui::Button("Play")) {
        integrator_control_state.step_mode_active = false;

        // Сбрасываем аккумулятор дельты времени.
        // 
        // NOTE: В пошаговом режиме дельта времени может накопить большое значение.
        // При возврате в обычный режим эта дельта будет раздроблена фиксированной дельтой интегрирования.
        // Это даст очень большое количество шагов интегрирования,
        // которые должны будут выполниться оффлайн до отрисовки следующего кадра,
        // что в свою очередь заметно подвесит картинку.
        dt_accum = 0.0;
    }

    ImGui::SameLine();
    if (ImGui::Button("Pause")) {
        integrator_control_state.step_mode_active = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Step")) {
        integrator_control_state.step_button_pressed = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        integrator_control_state.step_mode_active = false;
        dt_accum = 0.0;
    }

    ImGui::End();

    //ImGui::Render();
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void show_mass_props_window() {
    ImGui::SetNextWindowSize(ImVec2(100, 10), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Particle properties"))
    {
        // Список частиц.
        {
            ImGui::BeginChild("Particles", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
            for (int i = 0; i < world.get_particles().size(); i++)
            {
                std::string label = std::format("Particle {}", i);
                if (ImGui::Selectable(label.c_str(), selected_particle_index == i))
                    selected_particle_index = i;
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        // Свойства частиц.
        {
            ImGui::BeginChild("Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
            ImGui::Text("Particle: %d", selected_particle_index);
            ImGui::Separator();

            const std::vector<Particle*>& particles = world.get_particles();
            std::string particle_mass_value_string = std::format("mass: {}", particles.at(selected_particle_index)->mass);
            ImGui::Text(particle_mass_value_string.c_str());

            const Particle* selected_particle = particles.at(selected_particle_index);
            const float pos_x = selected_particle->pos.x;
            const float pos_y = selected_particle->pos.y;
            const float pos_z = selected_particle->pos.z;
            std::string particle_pos_value_string = std::format("pos.x: {}\npos.y: {}\npos.z: {}", pos_x, pos_y, pos_z);
            ImGui::Text(particle_pos_value_string.c_str());

            const float vel_x = selected_particle->velocity.x;
            const float vel_y = selected_particle->velocity.y;
            const float vel_z = selected_particle->velocity.z;
            std::string particle_vel_value_string = std::format("vel.x: {}\nvel.y: {}\nvel.z: {}", vel_x, vel_y, vel_z);
            ImGui::Text(particle_vel_value_string.c_str());

            ImGui::EndChild();
        }
    }
    ImGui::End();
}

void shut_imgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
