#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <cstdlib>

class TriangleExampleApp {
public:
    void run();

private:
    static constexpr uint32_t m_WIDTH = 800;
    static constexpr uint32_t m_HEIGHT = 600;
    GLFWwindow* m_window;

    VkInstance m_vk_instance;
    void create_instance();

    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    void pick_physical_device();

    int rate_device_suitability(VkPhysicalDevice device) const;

    void init_window();
    void init_vulkan();
    void main_loop();
    void cleanup();
};
