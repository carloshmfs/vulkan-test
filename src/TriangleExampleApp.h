#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <cstdlib>

class TriangleExampleApp {
public:
    void run();

private:
    const uint32_t m_WIDTH = 800;
    const uint32_t m_HEIGHT = 600;
    GLFWwindow* m_window;

    VkInstance m_vk_instance;
    void create_instance();

    void init_window();
    void init_vulkan();
    void main_loop();
    void cleanup();
};
