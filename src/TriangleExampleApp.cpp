#include "TriangleExampleApp.h"
#include <cstdint>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>
#include <set>

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
    if (!m_window) {
        throw std::runtime_error("could not initiate glfw window");
    }
}

void TriangleExampleApp::init_vulkan()
{
    create_instance();
    create_window_surface();
    pick_physical_device();
    create_logical_device();
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

    VkInstanceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    create_info.enabledExtensionCount = glfwExtensionCount;
    create_info.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
    (void)result;
    if (vkCreateInstance(&create_info, nullptr, &m_vk_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vk instance!");
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cerr << "available extensions:" << std::endl;

    for (const auto& extension : extensions) {
        std::cerr << '\t' << extension.extensionName << std::endl;
    }
}

void TriangleExampleApp::create_window_surface()
{
    std::cerr << "TriangleExampleApp::create_window_surface()" << std::endl;

    VkResult err = glfwCreateWindowSurface(m_vk_instance, m_window, nullptr, &m_surface);
    if (err) {
        throw std::runtime_error("failed to create a window surface!");
    }
}

void TriangleExampleApp::create_logical_device()
{
    QueueFamilyIndices indices = find_queue_families(m_physical_device);
    VkDeviceCreateInfo create_info {};

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

    float queuePriority = 1.0f;
    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queuePriority;
        queue_create_infos.push_back(queue_create_info);
    }

    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_presentation_queue);
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

    if (!check_device_extension_support(device)) {
        return 0;
    }

    // Discrete GPUs have a significant performance advantage
    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += device_properties.limits.maxImageDimension2D;

    std::cerr << "device: " << device_properties.deviceName << " rate: " << score << std::endl;

    return score;
}

bool TriangleExampleApp::check_device_extension_support(VkPhysicalDevice device) const
{
    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    std::cerr << "TriangleExampleApp::check_device_extension_support()" << std::endl;

    return requiredExtensions.empty();
}

QueueFamilyIndices TriangleExampleApp::find_queue_families(VkPhysicalDevice device) const
{
    QueueFamilyIndices indices;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    std::cerr << "queue families: " << queue_family_count << std::endl;

    int i = 0;
    for (const auto& queue_family : queue_families) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        if (indices.is_complete()) {
            break;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);
        if (present_support) {
            indices.present_family = i;
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
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_vk_instance, m_surface, nullptr);
    vkDestroyInstance(m_vk_instance, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
