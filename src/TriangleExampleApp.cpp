#include "TriangleExampleApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

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
    create_instance();
    pick_physical_device();
}

void TriangleExampleApp::create_instance()
{
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vk_instance);
    (void)result;
    if (vkCreateInstance(&createInfo, nullptr, &m_vk_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vk instance!");
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions:" << std::endl;

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << std::endl;
    }
}

void TriangleExampleApp::pick_physical_device()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);
    if (device_count == 0) {
        std::cerr << device_count << std::endl;
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_vk_instance, &device_count, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;
    for (const auto& device : devices) {
        int score = rate_device_suitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0) {
        m_physical_device = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

int TriangleExampleApp::rate_device_suitability(VkPhysicalDevice device) const
{
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);
    int score = 0;

    // Application can't function without geometry shaders
    if (!device_features.geometryShader) {
        return 0;
    }

    QueueFamilyIndices indices = find_queue_families(device);
    if (!indices.is_complete()) {
        return 0;
    }

    // Discrete GPUs have a significant performance advantage
    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += device_properties.limits.maxImageDimension2D;

    std::cout << "device: " << device_properties.deviceName << " rate: " << score << std::endl;

    return score;
}

QueueFamilyIndices TriangleExampleApp::find_queue_families(VkPhysicalDevice device) const
{
    QueueFamilyIndices indices;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    std::cout << "queue families: " << queue_family_count << std::endl;

    int i = 0;
    for (const auto& queue_family : queue_families) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        if (indices.is_complete()) {
            break;
        }

        ++i;
    }

    return indices;
}

void TriangleExampleApp::main_loop()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
    }
}

void TriangleExampleApp::cleanup()
{
    vkDestroyInstance(m_vk_instance, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
