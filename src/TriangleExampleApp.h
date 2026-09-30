#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <cstdlib>
#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

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

    VkQueue m_graphics_queue;
    VkDevice m_device;
    void create_logical_device();

    VkSurfaceKHR m_surface;
    void create_window_surface();

    VkSwapchainKHR m_swap_chain;
    VkFormat m_swap_chain_image_format;
    VkExtent2D m_swap_chain_extent;
    std::vector<VkImage> m_swap_chain_images;
    void create_swap_chain();

    std::vector<VkImageView> m_swap_chain_image_views;
    void create_image_views();

    VkShaderModule create_shader_module(const std::vector<char>& code);

    void create_graphics_pipeline();

    VkQueue m_presentation_queue;

    int rate_device_suitability(VkPhysicalDevice device) const;
    bool check_device_extension_support(VkPhysicalDevice device) const;
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device) const;
    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device) const;
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) const;
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) const;
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) const;

    void init_window();
    void init_vulkan();
    void main_loop();
    void cleanup();
};
