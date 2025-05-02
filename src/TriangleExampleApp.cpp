#include "TriangleExampleApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <cstdlib>
#include <iostream>
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

static bool is_device_suitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);

    return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader;
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

    for (const auto& device : devices) {
        if (is_device_suitable(device)) {
            m_physical_device = device;
        }
    }

    if (m_physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
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
