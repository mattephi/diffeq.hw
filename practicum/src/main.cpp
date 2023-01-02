#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <function.h>
#include <SystemConfiguration.h>

EquationConfiguration::Info* EquationConfiguration::info = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IVP Analysis and Solution", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    bool controller_active = true;
    bool plot_active = true;

    while (!glfwWindowShouldClose(window))
    {
        std::vector<Function*> functions = {
                new Function(EquationConfiguration::get().getExactSolution(), "Exact Solution"),
                new EulerMethod(EquationConfiguration::get().getFunction()),
                new ImprovedEulerMethod(EquationConfiguration::get().getFunction()),
                new RungeKuttaMethod(EquationConfiguration::get().getFunction())
        };

        std::vector<Function*> gteFunctions, lteFunctions, taeFunctions;
        for (int i = 1; i < functions.size(); ++i) {
            gteFunctions.push_back(new GlobalErrorFunction(functions[0], dynamic_cast<IterativeFunction*>(functions[i]), functions[i]->getName() + " GTE"));
            lteFunctions.push_back(new LocalErrorFunction(functions[0], dynamic_cast<IterativeFunction*>(functions[i]), functions[i]->getName() + " LTE"));
            taeFunctions.push_back(new TotalErrorFunction(functions[0], dynamic_cast<IterativeFunction*>(functions[i]), functions[i]->getName() + " TAE"));
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Controller", &controller_active);
        ImGui::DragFloat("x0", &EquationConfiguration::get().x_0, 0.1, -100.0, 100.0);
        ImGui::DragFloat("y0", &EquationConfiguration::get().y_0, 0.1, -100.0, 100.0);
        ImGui::DragFloat("X", &EquationConfiguration::get().x_max, 0.3, -100.0, 100.0);
        ImGui::DragInt("n0", &EquationConfiguration::get().n_0, 1, 0, EquationConfiguration::get().n);
        ImGui::DragInt("N", &EquationConfiguration::get().n, 1, 1, 50);
        EquationConfiguration::get().n_0 = std::min(EquationConfiguration::get().n_0, EquationConfiguration::get().n - 1);
        ImGui::End();

        ImGui::Begin("Solutions Plot", &plot_active);
        if (ImPlot::BeginPlot("Solutions")) {
            for (auto function : functions) {
                function->plot();
            }
            ImPlot::EndPlot();
        }
        ImGui::End();

        ImGui::Begin("GTE Plot", &plot_active);
        if (ImPlot::BeginPlot("Global Truncation Errors")) {
            for (auto function : gteFunctions) {
                function->plot();
            }
            ImPlot::EndPlot();
        }
        ImGui::End();

        ImGui::Begin("LTE Plot", &plot_active);
        if (ImPlot::BeginPlot("Local Truncation Errors")) {
            for (auto function : lteFunctions) {
                function->plot();
            }
            ImPlot::EndPlot();
        }
        ImGui::End();

        ImGui::Begin("TAE Plot", &plot_active);
        if (ImPlot::BeginPlot("Total Approximation Errors")) {
            for (auto function : taeFunctions) {
                function->plot();
            }
            ImPlot::EndPlot();
        }
        ImGui::End();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
