#include "TriangleExampleApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void TriangleExampleApp::run()
{
    init_window();
    init_vulkan();
    main_loop();
    cleanup();
}

void TriangleExampleApp::init_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(m_WIDTH, m_HEIGHT, "Vulkan window", nullptr, nullptr);
}

void TriangleExampleApp::init_vulkan()
{

}

void TriangleExampleApp::main_loop()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
    }
}

void TriangleExampleApp::cleanup()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
