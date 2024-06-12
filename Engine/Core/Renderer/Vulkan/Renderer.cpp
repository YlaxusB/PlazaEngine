// #include "Engine/Core/PreCompiledHeaders.h"
#include "Renderer.h"

#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "Engine/Application/Callbacks/CallbacksHeader.h"
#include "Engine/Core/Renderer/Vulkan/VulkanGuiRenderer.h"
#include "Engine/Core/Renderer/Vulkan/VulkanSkybox.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_vulkan.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <ThirdParty/glm/glm.hpp>
#include <ThirdParty/glm/gtc/matrix_transform.hpp>

#include "VulkanPlazaInitializator.h"

namespace Plaza {
	VulkanShadows*
		VulkanRenderer::GetShadows() {
		return this->mShadows;
	}

#pragma region Vulkan Setup
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	VulkanRenderer*
		VulkanRenderer::GetRenderer() {
		if (Application)
			return (VulkanRenderer*)(Application->mRenderer);
		else
			return nullptr;
	}

	VkResult
		VulkanRenderer::CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanRenderer::PopulateDebugMessengerCreateInfo(
		VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	void VulkanRenderer::SetupDebugMessenger() {
		if (!mEnableValidationLayers)
			return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(
			mVulkanInstance, &createInfo, nullptr, &mDebugMessenger)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME,
		VK_KHR_MULTIVIEW_EXTENSION_NAME
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> graphicsAndComputeFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	QueueFamilyIndices
		findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	std::vector<const char*>
		getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions,
			glfwExtensions + glfwExtensionCount);
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		// if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		//}
		// extensions.push_back(VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME);
		// extensions.push_back(VK_KHR_GET_MULTIVIEW_EXTENSION_NAME);

		return extensions;
	}

	void VulkanRenderer::InitVulkan() {
		std::cout << "appInfo \n";
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Plaza Engine";
		appInfo.pEngineName = "Plaza Engine";
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		std::cout << "getRequiredExtensions \n";
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef GAME_MODE
		mEnableValidationLayers = false;
#endif

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (mEnableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}
		std::cout << "vkCreateInstance \n";
		if (vkCreateInstance(&createInfo, nullptr, &mVulkanInstance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(
			device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(
			device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(),
			deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device, mSurface);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, mSurface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	void VulkanRenderer::PickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				mPhysicalDevice = device;
				break;
			}
		}

		if (mPhysicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	void VulkanRenderer::CreateLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice, mSurface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),
			indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.multiViewport = VK_TRUE;
		deviceFeatures.multiDrawIndirect = VK_TRUE;

		VkPhysicalDeviceFeatures2 physicalFeatures2 = {
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
		};
		physicalFeatures2.features.samplerAnisotropy = VK_TRUE;
		physicalFeatures2.features.multiViewport = VK_TRUE;
		physicalFeatures2.features.multiDrawIndirect = VK_TRUE;

		VkPhysicalDeviceVulkan11Features vulkan11features{};
		vulkan11features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		vulkan11features.multiview = VK_TRUE;
		vulkan11features.pNext = physicalFeatures2.pNext;
		physicalFeatures2.pNext = &vulkan11features;
		VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures = {};
		multiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
		multiviewFeatures.pNext = physicalFeatures2.pNext; // Chain it with the previous structure
		physicalFeatures2.pNext = &multiviewFeatures;

		vkGetPhysicalDeviceFeatures2(mPhysicalDevice, &physicalFeatures2);

		/* Check Bindless textures */
		VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, nullptr
		};
		VkPhysicalDeviceFeatures2 deviceFeatures2{
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &indexingFeatures
		};

		vkGetPhysicalDeviceFeatures2(mPhysicalDevice, &deviceFeatures2);
		bool bindlessTexturesSupported = indexingFeatures.descriptorBindingPartiallyBound && indexingFeatures.runtimeDescriptorArray;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		// createInfo.pNext = &vulkan11Features;
		// createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (physicalFeatures2.features.multiViewport) {
			// Multiview is supported
		}
		else {
			// Multiview is not supported, handle accordingly
		}

		if (bindlessTexturesSupported) {
			indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
			indexingFeatures.runtimeDescriptorArray = VK_TRUE;
			physicalFeatures2.pNext = &indexingFeatures;
			createInfo.pNext = &physicalFeatures2;
		}

		if (mEnableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
		vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);

		/* Compute Queue */
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			mPhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				indices.graphicsAndComputeFamily = i;
			}

			i++;
		}
		vkGetDeviceQueue(
			mDevice, indices.graphicsAndComputeFamily.value(), 0, &mComputeQueue);
	}

	void VulkanRenderer::InitSurface() {
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(Application->Window->glfwWindow);
		createInfo.hinstance = GetModuleHandle(nullptr);
		if (vkCreateWin32SurfaceKHR(
			mVulkanInstance, &createInfo, nullptr, &mSurface)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		// if (glfwCreateWindowSurface(mVulkanInstance,
		// Application->Window->glfwWindow, nullptr, &mSurface) != VK_SUCCESS) {
		// throw
		// std::runtime_error("failed to create window surface!");
		// }
	}

	SwapChainSupportDetails
		VulkanRenderer::QuerySwapChainSupport(VkPhysicalDevice device,
			VkSurfaceKHR surface) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkExtent2D
		VulkanRenderer::ChooseSwapExtent(VkSurfaceCapabilitiesKHR& capabilities) {
		capabilities.minImageExtent.width = 0;
		capabilities.minImageExtent.height = 0;
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(Application->Window->glfwWindow, &width, &height);

			VkExtent2D actualExtent = { static_cast<uint32_t>(width),
				static_cast<uint32_t>(height) };

			actualExtent.width = std::clamp(actualExtent.width,
				capabilities.minImageExtent.width,
				capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height,
				capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	VkPresentModeKHR
		chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes) {
#ifdef GAME_MODE
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) { //  VK_PRESENT_MODE_FIFO_KHR = Vsync
				return availablePresentMode; //  VK_PRESENT_MODE_MAILBOX_KHR = No Limit
			}
		}
#elif EDITOR_MODE
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { //  VK_PRESENT_MODE_FIFO_KHR = Vsync
				return availablePresentMode; //  VK_PRESENT_MODE_MAILBOX_KHR = No Limit
			}
		}
#endif

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkSurfaceFormatKHR
		chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	void VulkanRenderer::InitSwapChain() {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice, mSurface);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice, mSurface);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),
			indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
		mSwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(
			mDevice, mSwapChain, &imageCount, mSwapChainImages.data());

		mSwapChainImageFormat = surfaceFormat.format;
		mSwapChainExtent = extent;
	}
	void VulkanRenderer::CreateImageViews(
		VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED) {
		mSwapChainImageViews.resize(mSwapChainImages.size());
		for (size_t i = 0; i < mSwapChainImages.size(); i++) {
			mSwapChainImageViews[i] = CreateImageView(
				mSwapChainImages[i], mSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = Application->appSizes->sceneSize.x;
		imageCreateInfo.extent.height = Application->appSizes->sceneSize.y;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = mSwapChainImageFormat;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = initialLayout;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		if (vkCreateImage(mDevice, &imageCreateInfo, nullptr, &mFinalSceneImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(mDevice, mFinalSceneImage, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = FindMemoryType(
			memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkDeviceMemory imageMemory;
		if (vkAllocateMemory(mDevice, &allocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}
		vkBindImageMemory(mDevice, mFinalSceneImage, imageMemory, 0);

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = mFinalSceneImage;

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = mSwapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(mDevice, &createInfo, nullptr, &mFinalSceneImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}

		/* Deferred */
		imageCreateInfo.format = mFinalDeferredFormat;

		if (vkCreateImage(mDevice, &imageCreateInfo, nullptr, &mDeferredFinalImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		vkGetImageMemoryRequirements(
			mDevice, mDeferredFinalImage, &memoryRequirements);

		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = FindMemoryType(
			memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(mDevice, &allocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}
		vkBindImageMemory(mDevice, mDeferredFinalImage, imageMemory, 0);

		// this->TransitionImageLayout(mDeferredFinalImage, mFinalDeferredFormat,
		// initialLayout, VK_IMAGE_LAYOUT_GENERAL, 1U);

		createInfo.image = mDeferredFinalImage;
		createInfo.format = mFinalDeferredFormat;
		if (vkCreateImageView(
			mDevice, &createInfo, nullptr, &mDeferredFinalImageView)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}

	void VulkanRenderer::InitCommands() {
	}
	void VulkanRenderer::InitSyncStructures() {
		mImageAvailableSemaphores.resize(mMaxFramesInFlight);
		mRenderFinishedSemaphores.resize(mMaxFramesInFlight);
		mComputeFinishedSemaphores.resize(mMaxFramesInFlight);
		mInFlightFences.resize(mMaxFramesInFlight);
		mComputeInFlightFences.resize(mMaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < mMaxFramesInFlight; i++) {
			if (vkCreateSemaphore(
				mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i])
				!= VK_SUCCESS
				|| vkCreateSemaphore(
					mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i])
				!= VK_SUCCESS
				|| vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error(
					"failed to create synchronization objects for a frame!");
			}
			if (vkCreateSemaphore(
				mDevice, &semaphoreInfo, nullptr, &mComputeFinishedSemaphores[i])
				!= VK_SUCCESS
				|| vkCreateFence(
					mDevice, &fenceInfo, nullptr, &mComputeInFlightFences[i])
				!= VK_SUCCESS) {
				throw std::runtime_error(
					"failed to create compute synchronization objects for a frame!");
			}
		}
	}

	void VulkanRenderer::CreateSwapchainRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		dependencies[0].dependencyFlags = 0;

		dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].dstSubpass = 0;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].srcAccessMask = 0;
		dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependencies[1].dependencyFlags = 0;

		std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = dependencies.size();
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(
			mDevice, &renderPassInfo, nullptr, &mSwapchainRenderPass)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanRenderer::CreateRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT; // FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		dependencies[0].dependencyFlags = 0;

		dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].dstSubpass = 0;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].srcAccessMask = 0;
		dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependencies[1].dependencyFlags = 0;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment,
			depthAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = dependencies.size();
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		colorAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		std::array<VkAttachmentDescription, 2> deferredAttachments = {
			colorAttachment, depthAttachment
		};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(deferredAttachments.size());
		renderPassInfo.pAttachments = deferredAttachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = dependencies.size();
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(
			mDevice, &renderPassInfo, nullptr, &mDeferredRenderPass)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	VkShaderModule
		createShaderModule(const std::vector<char>& code, VkDevice device) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	static std::vector<char>
		readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void VulkanRenderer::CreateGraphicsPipeline() {
		auto vertShaderCode = readFile(Application->exeDirectory + "\\CompiledShaders\\vulkanTriangle.vert.spv");
		auto fragShaderCode = readFile(Application->exeDirectory + "\\CompiledShaders\\vulkanTriangle.frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, mDevice);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, mDevice);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,
			fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)mSwapChainExtent.width;
		viewport.height = (float)mSwapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = mSwapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;

		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // Specify the shader stage(s) using the
		// push constant
		pushConstantRange.offset = 0; // Offset of the push constant block
		pushConstantRange.size = sizeof(PushConstants); // Size of the push constant block

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(
			mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		auto bindingDescription = VertexGetBindingDescription();
		auto attributeDescriptions = VertexGetAttributeDescriptions();

		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = this->mDeferredRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.pDepthStencilState = &depthStencil;

		if (vkCreateGraphicsPipelines(mDevice,
			VK_NULL_HANDLE,
			1,
			&pipelineInfo,
			nullptr,
			&mGraphicsPipeline)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
		vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);
	}

	void VulkanRenderer::CreateFramebuffers() {
		mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

		for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
			std::array<VkImageView, 1> attachments = { mSwapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = this->mSwapchainRenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = mSwapChainExtent.width;
			framebufferInfo.height = mSwapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(
				mDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffers[i])
				!= VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}

		std::vector<VkImageView> attachments = { mFinalSceneImageView };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = this->mSwapchainRenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = Application->appSizes->sceneSize.x;
		framebufferInfo.height = Application->appSizes->sceneSize.y;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(
			mDevice, &framebufferInfo, nullptr, &mFinalSceneFramebuffer)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}

		attachments[0] = mDeferredFinalImageView;
		framebufferInfo.renderPass = mDeferredRenderPass;
		attachments.push_back(mDepthImageView);
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		if (vkCreateFramebuffer(
			mDevice, &framebufferInfo, nullptr, &mDeferredFramebuffer)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	void VulkanRenderer::CreateCommandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice, mSurface);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void VulkanRenderer::CreateCommandBuffers() {
		mCommandBuffers.resize(mMaxFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();

		if (vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	VkCommandBuffer
		VulkanRenderer::CreateCommandBuffer() {
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
		return commandBuffer;
	}

	void VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer,
		uint32_t imageIndex) {
		PLAZA_PROFILE_SECTION("Record Command Buffer");
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		mActiveCommandBuffer = &commandBuffer;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->mShadows->mRenderPass;
		renderPassInfo.framebuffer = this->mShadows->mFramebuffers
			[mCurrentFrame]; // mSwapChainFramebuffers[0];//mSwapChainFramebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent.width = this->mShadows->mShadowResolution;
		renderPassInfo.renderArea.extent.height = this->mShadows->mShadowResolution;
		// renderPassInfo.renderArea.extent = mSwapChainExtent;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkClearValue clearDepth{};
		clearDepth.depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearDepth;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = this->mShadows->mShadowResolution;
		viewport.height = -static_cast<float>(this->mShadows->mShadowResolution);
		viewport.y = this->mShadows->mShadowResolution;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = this->mShadows->mShadowResolution;
		scissor.extent.height = this->mShadows->mShadowResolution;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		{
			PLAZA_PROFILE_SECTION("Update Materials");
			UpdateMaterials();
		}

		{
			PLAZA_PROFILE_SECTION("Group Instances");
			for (const auto& [key, value] :
				Application->activeScene->meshRendererComponents) {
				const auto& transformIt = Application->activeScene->transformComponents.find(key);
				if (transformIt != Application->activeScene->transformComponents.end() && value.renderGroup) {
					// mInstanceModelMatrices.push_back(glm::mat4(1.0f));
					// mInstanceModelMatrices.push_back(transform.modelMatrix);

					value.renderGroup->AddInstance(
						transformIt->second.modelMatrix,
						value.renderGroup->material->mIndexHandle);
					Time::addInstanceCalls++;

					bool continueLoop = true;

					// value.renderGroup->AddCascadeInstance(transform.modelMatrix, 0);
				}
			}
		}

		{
			PLAZA_PROFILE_SECTION("Create Indirect Commands");
			this->mIndirectCommands.clear();
			this->mInstanceModelMatrices.clear();
			this->mInstanceModelMaterialsIndex.clear();
			this->mInstanceModelMaterialsIndex.push_back(0);
			mTotalInstances = 0;
			mIndirectDrawCount = 0;
			for (const auto& [key, value] : Application->activeScene->renderGroups) {
				VkDrawIndexedIndirectCommand indirectCommand{};
				indirectCommand.firstIndex = value->mesh->indicesOffset;
				indirectCommand.vertexOffset = value->mesh->verticesOffset;
				indirectCommand.firstInstance = mTotalInstances;
				indirectCommand.indexCount = value->mesh->indicesCount; // indices.size();
				indirectCommand.instanceCount = value->instanceModelMatrices.size();

				this->mIndirectCommands.push_back(indirectCommand);
				value->mesh->instanceOffset = mTotalInstances;

				for (unsigned int i = 0; i < value->instanceModelMatrices.size(); ++i) {
					this->mInstanceModelMatrices.push_back(value->instanceModelMatrices[i]);
					this->mInstanceModelMaterialsIndex.push_back(
						value->instanceMaterialIndices[i]);
					mTotalInstances++; //= value->instanceModelMatrices.size();
				}

				mIndirectDrawCount++;
				value->instanceModelMatrices.clear();
				value->instanceMaterialIndices.clear();
			}
		}

		{
			PLAZA_PROFILE_SECTION("Render Shadows Depth");
			renderPassInfo.renderPass = this->mShadows->mRenderPass;
			renderPassInfo.framebuffer = this->mShadows->mFramebuffers[mCurrentFrame];
			vkCmdBeginRenderPass(
				commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				this->mShadows->mShadowsShader->mPipeline);

			this->mShadows->UpdateAndPushConstants(commandBuffer, 0);
			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				this->mShadows->mShadowsShader->mPipelineLayout,
				0,
				1,
				&this->mShadows->mCascades[0].mDescriptorSets[mCurrentFrame],
				0,
				nullptr);

			{
				PLAZA_PROFILE_SECTION("Copy Indirect Data");
				VkDeviceSize bufferSize = sizeof(VkDrawIndexedIndirectCommand) * mIndirectCommands.size();
				void* data;
				vkMapMemory(this->mDevice,
					mIndirectBufferMemories[mCurrentFrame],
					0,
					bufferSize,
					0,
					&data);
				memcpy(data, mIndirectCommands.data(), static_cast<size_t>(bufferSize));
				vkUnmapMemory(this->mDevice, mIndirectBufferMemories[mCurrentFrame]);
			}

			{
				PLAZA_PROFILE_SECTION("Copy Data");
				VkDeviceSize bufferSize = sizeof(glm::mat4) * mInstanceModelMatrices.size();
				void* data;
				vkMapMemory(this->mDevice,
					mMainInstanceMatrixBufferMemories[mCurrentFrame],
					0,
					bufferSize,
					0,
					&data);
				memcpy(
					data, mInstanceModelMatrices.data(), static_cast<size_t>(bufferSize));
				vkUnmapMemory(this->mDevice,
					mMainInstanceMatrixBufferMemories[mCurrentFrame]);
			}

			{
				PLAZA_PROFILE_SECTION("Bind the instance's materials");
				VkDeviceSize bufferSize = sizeof(unsigned int) * mInstanceModelMaterialsIndex.size();
				void* data;
				vkMapMemory(this->mDevice,
					mMainInstanceMaterialBufferMemories[mCurrentFrame],
					0,
					bufferSize,
					0,
					&data);
				memcpy(data,
					mInstanceModelMaterialsIndex.data(),
					static_cast<size_t>(bufferSize));
				vkUnmapMemory(this->mDevice,
					mMainInstanceMaterialBufferMemories[mCurrentFrame]);
			}

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mMainVertexBuffer, offsets);
			vkCmdBindVertexBuffers(
				commandBuffer, 1, 1, &mMainInstanceMatrixBuffers[mCurrentFrame], offsets);
			vkCmdBindVertexBuffers(commandBuffer,
				2,
				1,
				&mMainInstanceMaterialBuffers[mCurrentFrame],
				offsets);
			vkCmdBindIndexBuffer(
				commandBuffer, mMainIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexedIndirect(commandBuffer,
				mIndirectBuffers[mCurrentFrame],
				0,
				mIndirectDrawCount,
				sizeof(VkDrawIndexedIndirectCommand));

			// for (const auto& [key, value] : Application->activeScene->renderGroups) {
			//	//	this->DrawRenderGroupShadowDepthMapInstanced(value, 0);
			// }

			vkCmdEndRenderPass(commandBuffer);
		}

		// this->mPicking->DrawSelectedObjectsUuid();

		// vkCmdEndRenderPass(commandBuffer);

		std::array<VkClearValue, 5> geometryPassClear{};
		geometryPassClear[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		geometryPassClear[1].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		geometryPassClear[2].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		geometryPassClear[3].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		geometryPassClear[4].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(geometryPassClear.size());
		renderPassInfo.pClearValues = geometryPassClear.data();
		renderPassInfo.renderPass = this->mGeometryPassRenderer.mRenderPass; // mDeferredRenderPass;
		renderPassInfo.framebuffer = this->mGeometryPassRenderer.mFramebuffer; // mDeferredFramebuffer;

		renderPassInfo.renderArea.extent.width = Application->appSizes->sceneSize.x;
		renderPassInfo.renderArea.extent.height = Application->appSizes->sceneSize.y;

		viewport.width = Application->appSizes->sceneSize.x;
		viewport.height = -Application->appSizes->sceneSize.y;
		viewport.y = Application->appSizes->sceneSize.y;
		scissor.extent.width = Application->appSizes->sceneSize.x;
		scissor.extent.height = Application->appSizes->sceneSize.y;

		vkCmdBeginRenderPass(
			commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			PLAZA_PROFILE_SECTION("Bind Instances Data");
			std::vector<VkDescriptorSet> descriptorSets = vector<VkDescriptorSet>();
			descriptorSets.push_back(this->mGeometryPassRenderer.mShaders
				->mDescriptorSets[this->mCurrentFrame]);

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindIndexBuffer(
				commandBuffer, mMainIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mMainVertexBuffer, offsets);
			vkCmdBindVertexBuffers(
				commandBuffer, 1, 1, &mMainInstanceMatrixBuffers[mCurrentFrame], offsets);
			vkCmdBindVertexBuffers(commandBuffer,
				2,
				1,
				&mMainInstanceMaterialBuffers[mCurrentFrame],
				offsets);
			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				this->mGeometryPassRenderer.mShaders->mPipelineLayout,
				0,
				descriptorSets.size(),
				descriptorSets.data(),
				0,
				nullptr);
		}

		// Render the scene with textures and sampling the shadow map
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->mGeometryPassRenderer.mShaders->mPipeline);
		{
			PLAZA_PROFILE_SECTION("Draw Instances");

			vkCmdDrawIndexedIndirect(commandBuffer,
				mIndirectBuffers[mCurrentFrame],
				0,
				mIndirectDrawCount,
				sizeof(VkDrawIndexedIndirectCommand));
		}

		vkCmdEndRenderPass(commandBuffer);

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		viewport.height = -Application->appSizes->sceneSize.y;
		viewport.y = Application->appSizes->sceneSize.y;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// renderPassInfo.renderPass = scene;
		renderPassInfo.renderPass = mDeferredRenderPass;
		renderPassInfo.framebuffer = mDeferredFramebuffer;

		/* Tiled Lighting */
		this->mLighting->GetLights();
		this->mLighting->UpdateTiles();
		this->mLighting->DrawDeferredPass();

		// vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
		// VK_SUBPASS_CONTENTS_INLINE); this->mSkybox->DrawSkybox();
		// vkCmdEndRenderPass(commandBuffer);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		this->mBloom.Draw();
		/* ----------------- this->mGuiRenderer->RenderText(nullptr) -----------------
		 */
		;

		/* Render to ImGui or swapchain */
		renderPassInfo.renderPass = this->mSwapchainRenderPass;
#ifdef GAME_MODE
		renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex];
#else
		renderPassInfo.framebuffer = mFinalSceneFramebuffer;
#endif

		std::vector<VkDescriptorSet> descriptorSets = vector<VkDescriptorSet>();
		descriptorSets.push_back(this->mSwapchainDescriptorSets[mCurrentFrame]);
		vkCmdBeginRenderPass(
			commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->mSwapchainRenderer.mShaders->mPipeline);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->mSwapchainRenderer.mShaders->mPipelineLayout,
			0,
			1,
			descriptorSets.data(),
			0,
			nullptr);
		this->mSwapchainRenderer.Update();
		SwapChainPushConstant swapChainPushConstant{};
		swapChainPushConstant.exposure = this->exposure;
		swapChainPushConstant.gamma = this->gamma;
		vkCmdPushConstants(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer,
			this->mSwapchainRenderer.mShaders->mPipelineLayout,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SwapChainPushConstant),
			&swapChainPushConstant);
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		// vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffer);

#ifdef EDITOR_MODE
		renderPassInfo.renderPass = mSwapchainRenderPass;
		renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.extent = mSwapChainExtent;

		vkCmdBeginRenderPass(
			commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//		vkCmdBindPipeline(commandBuffer,
		// VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

		viewport.width = static_cast<float>(mSwapChainExtent.width);
		viewport.height = static_cast<float>(mSwapChainExtent.height);
		scissor.extent = mSwapChainExtent;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		// mPipelineLayout, 0, 1, &mFinalSceneDescriptorSet, 0, nullptr);

		{
			PLAZA_PROFILE_SECTION("Render ImGui");
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		}

		vkCmdEndRenderPass(commandBuffer);
#endif

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void VulkanRenderer::CleanupSwapChain() {
		vkDestroyImageView(mDevice, mDepthImageView, nullptr);
		vkDestroyImage(mDevice, mDepthImage, nullptr);
		vkFreeMemory(mDevice, mDepthImageMemory, nullptr);

		for (size_t i = 0; i < mSwapChainFramebuffers.size(); i++) {
			vkDestroyFramebuffer(mDevice, mSwapChainFramebuffers[i], nullptr);
		}

		for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
			vkDestroyImageView(mDevice, mSwapChainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
	}

	void VulkanRenderer::RecreateSwapChain() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(Application->Window->glfwWindow, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(Application->Window->glfwWindow, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(mDevice);

		CleanupSwapChain();

		InitSwapChain();
		CreateImageViews(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		CreateDepthResources();
		CreateFramebuffers();
	}

	uint32_t
		VulkanRenderer::FindMemoryType(uint32_t typeFilter,
			VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void VulkanRenderer::CreateBuffer(VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
	}

	void VulkanRenderer::CopyBuffer(VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize size,
		VkDeviceSize offset) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = offset;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	void VulkanRenderer::CreateIndexBuffer(vector<uint32_t> indices,
		VkBuffer& indicesBuffer,
		VkDeviceMemory& indicesMemoryBuffer,
		VkDeviceSize bufferSize) {
		if (bufferSize == -1)
			bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(mDevice, stagingBufferMemory);

		CreateBuffer(bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indicesBuffer,
			indicesMemoryBuffer);

		CopyBuffer(stagingBuffer, indicesBuffer, bufferSize);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
	}

	void VulkanRenderer::CreateVertexBuffer(vector<Vertex> vertices,
		VkBuffer& vertexBuffer,
		VkDeviceMemory& vertexBufferMemory,
		VkDeviceSize bufferSize) {
		if (bufferSize == -1)
			bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(mDevice, stagingBufferMemory);

		CreateBuffer(bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory);

		CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
	}

	void VulkanRenderer::CreateDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
			/*VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |*/
			VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		static const uint32_t maxBindlessResources = 16536;
		VkDescriptorSetLayoutBinding bindlessTexturesBinding;
		bindlessTexturesBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindlessTexturesBinding.descriptorCount = maxBindlessResources;
		bindlessTexturesBinding.binding = 20;
		bindlessTexturesBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindlessTexturesBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding depthMapLayoutBinding{};
		depthMapLayoutBinding.binding = 9;
		depthMapLayoutBinding.descriptorCount = 1;
		depthMapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depthMapLayoutBinding.pImmutableSamplers = nullptr;
		depthMapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding materialsArrayBinding{};
		materialsArrayBinding.binding = 19;
		materialsArrayBinding.descriptorCount = 1;
		materialsArrayBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		materialsArrayBinding.pImmutableSamplers = nullptr;
		materialsArrayBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 4> bindings = {
			uboLayoutBinding,
			bindlessTexturesBinding,
			depthMapLayoutBinding,
			materialsArrayBinding
		};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,
			nullptr
		};
		extendedInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		VkDescriptorBindingFlagsEXT bindingFlags[] = { 0, bindlessFlags, 0, 0 };
		extendedInfo.pBindingFlags = bindingFlags;
		layoutInfo.pNext = &extendedInfo;

		if (vkCreateDescriptorSetLayout(
			mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void VulkanRenderer::CreateUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		mUniformBuffers.resize(mMaxFramesInFlight);
		mUniformBuffersMemory.resize(mMaxFramesInFlight);
		mUniformBuffersMapped.resize(mMaxFramesInFlight);

		for (size_t i = 0; i < mMaxFramesInFlight; i++) {
			CreateBuffer(bufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				mUniformBuffers[i],
				mUniformBuffersMemory[i]);

			vkMapMemory(mDevice,
				mUniformBuffersMemory[i],
				0,
				bufferSize,
				0,
				&mUniformBuffersMapped[i]);
		}
	}

	void VulkanRenderer::UpdateUniformBuffer(uint32_t currentImage) {
		UniformBufferObject ubo{};
		ubo.projection = Application->activeCamera->GetProjectionMatrix();
		ubo.view = Application->activeCamera->GetViewMatrix();
		ubo.model = glm::mat4(1.0f);

		ubo.cascadeCount = 9;
		ubo.farPlane = 15000.0f;
		ubo.nearPlane = 0.01f;

		glm::vec3 lightDir = this->mShadows
			->mLightDirection; // glm::normalize(glm::vec3(20.0f, 50, 20.0f));
		glm::vec3 lightDistance = glm::vec3(100.0f, 400.0f, 0.0f);
		glm::vec3 lightPos;

		ubo.lightDirection = glm::vec4(lightDir, 1.0f);
		ubo.viewPos = glm::vec4(Application->activeCamera->Position, 1.0f);

		ubo.directionalLightColor = glm::vec4(this->mLighting->directionalLightColor * this->mLighting->directionalLightIntensity);
		ubo.directionalLightColor.a = 1.0f;
		ubo.ambientLightColor = glm::vec4(this->mLighting->ambientLightColor * this->mLighting->ambientLightIntensity);
		ubo.ambientLightColor.a = 1.0f;

		VulkanShadows::ShadowsUniformBuffer ub{};
		for (int i = 0; i < this->mShadows->mCascades.size(); ++i) {
			ubo.lightSpaceMatrices[i] = this->mShadows->mUbo[currentImage].lightSpaceMatrices
				[i]; // this->mShadows->GetLightSpaceMatrices(this->mShadows->shadowCascadeLevels,
			// ub)[i];
			if (i <= 8)
				ubo.cascadePlaneDistances[i] = glm::vec4(this->mShadows->shadowCascadeLevels[i], 1.0f, 1.0f, 1.0f);
			else
				ubo.cascadePlaneDistances[i] = glm::vec4(this->mShadows->shadowCascadeLevels[8], 1.0f, 1.0f, 1.0f);
		}

		// memcpy(&ubo.lightSpaceMatrices, &this->mShadows->mUbo, sizeof(glm::mat4) *
		// 9);

		ubo.showCascadeLevels = Application->showCascadeLevels;

		memcpy(mUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	void VulkanRenderer::CreateDescriptorPool() {
		static const uint32_t maxBindlessTextures = 16536 * 4;

		std::array<VkDescriptorPoolSize, 8> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(mMaxFramesInFlight);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(mMaxFramesInFlight);
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[2].descriptorCount = maxBindlessTextures; // maxBindlessTextures;
		poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[3].descriptorCount = static_cast<uint32_t>(9);
		poolSizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[4].descriptorCount = static_cast<uint32_t>(mMaxFramesInFlight);
		poolSizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[5].descriptorCount = 128;
		poolSizes[6].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[6].descriptorCount = 128;
		poolSizes[7].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		poolSizes[7].descriptorCount = 128;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(mMaxFramesInFlight);
		poolInfo.maxSets = 4 * sizeof(poolSizes);
		poolInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

		if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void VulkanRenderer::CreateDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(mMaxFramesInFlight,
			mDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
		};
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = layouts.size();
		allocInfo.pSetLayouts = layouts.data();

		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT
		};
		static const uint32_t maxBindlessResources = 16536;
		std::vector<uint32_t> maxBinding(mMaxFramesInFlight,
			maxBindlessResources - 1);
		countInfo.descriptorSetCount = 2;
		countInfo.pDescriptorCounts = maxBinding.data();
		allocInfo.pNext = &countInfo;

		mDescriptorSets.resize(mMaxFramesInFlight);
		if (vkAllocateDescriptorSets(mDevice, &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		std::vector<VkDescriptorImageInfo> imageInfos = std::vector<VkDescriptorImageInfo>();
		imageInfos.resize(9);
		for (size_t i = 0; i < imageInfos.size(); ++i) {
			imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			imageInfos[i].imageView = this->mShadows->mCascades[i].mImageView;
			imageInfos[i].sampler = this->mShadows->mShadowsSampler;
		}

		for (size_t i = 0; i < mMaxFramesInFlight; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = mUniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = mTextureImageView;
			imageInfo.sampler = mTextureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = mDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			VkDescriptorImageInfo imageInfo2{};
			imageInfo2.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			imageInfo2.imageView = this->mShadows->mShadowDepthImageViews[i];
			imageInfo2.sampler = this->mShadows->mShadowsSampler;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = mDescriptorSets[i];
			descriptorWrites[1].dstBinding = 9;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo2;

			vkUpdateDescriptorSets(mDevice,
				static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr);
		}
	}

	void VulkanRenderer::CreateImGuiTextureSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(mPhysicalDevice, &properties);
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		if (vkCreateSampler(mDevice, &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}
#pragma endregion Vulkan Setup

	void VulkanRenderer::CreateTextureImage() {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(
			TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(mDevice, stagingBufferMemory);

		stbi_image_free(pixels);

		CreateImage(texWidth,
			texHeight,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mTextureImage,
			mTextureImageMemory);

		TransitionImageLayout(mTextureImage,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer,
			mTextureImage,
			static_cast<uint32_t>(texWidth),
			static_cast<uint32_t>(texHeight));
		TransitionImageLayout(mTextureImage,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
	}

	VkCommandBuffer
		VulkanRenderer::BeginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanRenderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(mGraphicsQueue);

		vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
	}

	void VulkanRenderer::TransitionImageLayout(VkImage image,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkImageAspectFlags aspectMask,
		unsigned int layerCount,
		unsigned int mipCount) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectMask;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipCount;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = 0; // Assuming no access is needed
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer,
			sourceStage,
			destinationStage,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);

		EndSingleTimeCommands(commandBuffer);
	}

	void VulkanRenderer::TransitionTextureLayout(VulkanTexture& texture,
		VkImageLayout newLayout,
		VkImageAspectFlags aspectMask,
		unsigned int layerCount,
		unsigned int mipCount) {
		this->TransitionImageLayout(texture.mImage,
			texture.GetFormat(),
			texture.mLayout,
			newLayout,
			aspectMask,
			layerCount,
			mipCount);
		texture.mLayout = newLayout;
	}

	void VulkanRenderer::CopyBufferToImage(VkBuffer buffer,
		VkImage image,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevel,
		unsigned int arrayLayerCount) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = mipLevel;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = arrayLayerCount;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);

		EndSingleTimeCommands(commandBuffer);
	}

	VkImageView
		VulkanRenderer::CreateImageView(VkImage image,
			VkFormat format,
			VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.aspectMask = aspectFlags;

		VkImageView imageView;
		if (vkCreateImageView(mDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	void VulkanRenderer::CreateTextureImageView() {
		mTextureImageView = CreateImageView(
			mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanRenderer::CreateTextureSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 8;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(mPhysicalDevice, &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		if (vkCreateSampler(mDevice, &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	bool VulkanRenderer::HasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkFormat
		VulkanRenderer::FindSupportedFormat(const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat
		VulkanRenderer::FindDepthFormat() {
		return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT,
								 VK_FORMAT_D32_SFLOAT_S8_UINT,
								 VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	void VulkanRenderer::CreateDepthResources() {
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT; // FindDepthFormat();
		CreateImage(mSwapChainExtent.width,
			mSwapChainExtent.height,
			depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mDepthImage,
			mDepthImageMemory);
		mDepthImageView = CreateImageView(mDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void VulkanRenderer::InitializeGeometryPassRenderer() {
		VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		this->mDeferredPositionTexture.CreateTextureImage(
			this->mDevice,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			Application->appSizes->sceneSize.x,
			Application->appSizes->sceneSize.y,
			false,
			imageUsageFlags);
		this->mDeferredPositionTexture.CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_IMAGE_ASPECT_COLOR_BIT);
		this->AddTrackerToImage(this->mDeferredPositionTexture.mImageView,
			"Deferred Position",
			this->mTextureSampler,
			this->mDeferredPositionTexture.GetLayout());

		this->mDeferredNormalTexture.CreateTextureImage(
			this->mDevice,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			Application->appSizes->sceneSize.x,
			Application->appSizes->sceneSize.y,
			false,
			imageUsageFlags);
		this->mDeferredNormalTexture.CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_IMAGE_ASPECT_COLOR_BIT);
		this->AddTrackerToImage(this->mDeferredNormalTexture.mImageView,
			"Deferred Normal",
			this->mTextureSampler,
			this->mDeferredNormalTexture.GetLayout());

		this->mDeferredDiffuseTexture.CreateTextureImage(
			this->mDevice,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			Application->appSizes->sceneSize.x,
			Application->appSizes->sceneSize.y,
			false,
			imageUsageFlags);
		this->mDeferredDiffuseTexture.CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_IMAGE_ASPECT_COLOR_BIT);
		this->AddTrackerToImage(this->mDeferredDiffuseTexture.mImageView,
			"Deferred Diffuse",
			this->mTextureSampler,
			this->mDeferredDiffuseTexture.GetLayout());

		this->mDeferredOthersTexture.CreateTextureImage(
			this->mDevice,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			Application->appSizes->sceneSize.x,
			Application->appSizes->sceneSize.y,
			false,
			imageUsageFlags);
		this->mDeferredOthersTexture.CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_IMAGE_ASPECT_COLOR_BIT);
		this->AddTrackerToImage(this->mDeferredOthersTexture.mImageView,
			"Deferred Others",
			this->mTextureSampler,
			this->mDeferredOthersTexture.GetLayout());

		// Render Pass
		std::vector<VkAttachmentReference> colorReferences;
		colorReferences.push_back({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		colorReferences.push_back({ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		colorReferences.push_back({ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		colorReferences.push_back({ 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		VkAttachmentReference depthReference = {};
		depthReference.attachment = 4;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.pColorAttachments = colorReferences.data();
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
		subpass.pDepthStencilAttachment = &depthReference;

		std::vector<VkSubpassDependency> dependencies{};
		dependencies.push_back(
			plvk::subpassDependency(VK_SUBPASS_EXTERNAL,
				0,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
				0));
		dependencies.push_back(plvk::subpassDependency(
			VK_SUBPASS_EXTERNAL,
			0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			0));

		std::vector<VkAttachmentDescription> attachmentDescs{};
		for (uint32_t i = 0; i < 5; ++i) {
			VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			if (i == 4) {
				finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

			attachmentDescs.push_back(
				plvk::attachmentDescription(VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					finalLayout));
		}
		attachmentDescs[4].format = VK_FORMAT_D32_SFLOAT_S8_UINT;

		this->mGeometryPassRenderer.InitializeRenderPass(attachmentDescs.data(),
			attachmentDescs.size(),
			&subpass,
			1,
			dependencies.data(),
			dependencies.size());

		// Framebuffer
		std::vector<VkImageView> attachments{
			this->mDeferredPositionTexture.mImageView,
			this->mDeferredNormalTexture.mImageView,
			this->mDeferredDiffuseTexture.mImageView,
			this->mDeferredOthersTexture.mImageView,
			this->mDepthImageView
		};
		this->mGeometryPassRenderer.InitializeFramebuffer(
			attachments.data(),
			attachments.size(),
			Application->appSizes->sceneSize,
			1);

		// Descriptor set
		std::vector<VkDescriptorSetLayoutBinding> descriptorSets{};
		descriptorSets.push_back(plvk::descriptorSetLayoutBinding(
			0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, VK_SHADER_STAGE_ALL));
		static const uint32_t maxBindlessResources = 16536;
		descriptorSets.push_back(
			plvk::descriptorSetLayoutBinding(20,
				maxBindlessResources,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				nullptr,
				VK_SHADER_STAGE_FRAGMENT_BIT));
		descriptorSets.push_back(
			plvk::descriptorSetLayoutBinding(9,
				1,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				nullptr,
				VK_SHADER_STAGE_FRAGMENT_BIT));
		descriptorSets.push_back(
			plvk::descriptorSetLayoutBinding(19,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				nullptr,
				VK_SHADER_STAGE_FRAGMENT_BIT));

		VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,
			nullptr
		};
		VkDescriptorBindingFlagsEXT bindingFlags[] = { 0, bindlessFlags, 0, 0 };
		extendedInfo.pBindingFlags = bindingFlags;
		extendedInfo.bindingCount = static_cast<uint32_t>(descriptorSets.size());

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = plvk::descriptorSetLayoutCreateInfo(
			descriptorSets,
			VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT,
			&extendedInfo);

		vkCreateDescriptorSetLayout(
			this->mDevice,
			&descriptorSetLayoutCreateInfo,
			nullptr,
			&this->mGeometryPassRenderer.mShaders->mDescriptorSetLayout);

		VkPushConstantRange pushConstantRange = plvk::pushConstantRange(
			VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants));
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = plvk::pipelineLayoutCreateInfo(
			1,
			&this->mGeometryPassRenderer.mShaders->mDescriptorSetLayout,
			1,
			&pushConstantRange);

		this->mGeometryPassRenderer.mShaders->mVertexShaderPath = VulkanShadersCompiler::Compile(
			Application->enginePath + "\\Shaders\\Vulkan\\deferred\\geometryPass.vert");
		this->mGeometryPassRenderer.mShaders->mFragmentShaderPath = VulkanShadersCompiler::Compile(
			Application->enginePath + "\\Shaders\\Vulkan\\deferred\\geometryPass.frag");
		auto bindingsArray = VertexGetBindingDescription();
		std::vector<VkVertexInputBindingDescription> bindings(
			std::begin(bindingsArray), std::end(bindingsArray));
		auto attributesArray = VertexGetAttributeDescriptions();
		std::vector<VkVertexInputAttributeDescription> attributes(
			std::begin(attributesArray), std::end(attributesArray));
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = plvk::pipelineVertexInputStateCreateInfo(bindings, attributes);
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = plvk::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
		VkPipelineRasterizationStateCreateInfo rasterizationState = plvk::pipelineRasterizationStateCreateInfo(VK_FALSE,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			1.0f,
			VK_FALSE,
			0.0f,
			0.0f,
			0.0f,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_COUNTER_CLOCKWISE);
		VkPipelineColorBlendAttachmentState blendAttachmentState = plvk::pipelineColorBlendAttachmentState(VK_TRUE);
		std::vector<VkPipelineColorBlendAttachmentState> blendAttachments{
			blendAttachmentState,
			blendAttachmentState,
			blendAttachmentState,
			blendAttachmentState
		};
		VkPipelineColorBlendStateCreateInfo colorBlendState = plvk::pipelineColorBlendStateCreateInfo(4, blendAttachments.data());
		VkPipelineDepthStencilStateCreateInfo depthStencilState = plvk::pipelineDepthStencilStateCreateInfo(
			VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
		VkPipelineViewportStateCreateInfo viewportState = plvk::pipelineViewportStateCreateInfo(1, 1);
		VkPipelineMultisampleStateCreateInfo multisampleState = plvk::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
		std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState = plvk::pipelineDynamicStateCreateInfo(dynamicStateEnables);
		this->mGeometryPassRenderer.mShaders->InitializeFull(
			this->mDevice,
			pipelineLayoutCreateInfo,
			true,
			Application->appSizes->sceneSize.x,
			Application->appSizes->sceneSize.y,
			{},
			vertexInputInfo,
			inputAssemblyState,
			viewportState,
			rasterizationState,
			multisampleState,
			colorBlendState,
			dynamicState,
			this->mGeometryPassRenderer.mRenderPass,
			depthStencilState
			// this->mGeometryPassRenderer.mShaders->mDescriptorSetLayout,
		);

		VkDescriptorSetAllocateInfo allocInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
		};
		allocInfo.descriptorPool = this->mDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &this->mGeometryPassRenderer.mShaders->mDescriptorSetLayout;

		this->mGeometryPassRenderer.mShaders->mDescriptorSets.resize(
			Application->mRenderer->mMaxFramesInFlight);
		for (unsigned int i = 0; i < Application->mRenderer->mMaxFramesInFlight;
			++i) {
			if (vkAllocateDescriptorSets(
				this->mDevice,
				&allocInfo,
				&this->mGeometryPassRenderer.mShaders->mDescriptorSets[i])
				!= VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
			VkDescriptorBufferInfo bufferInfo = plvk::descriptorBufferInfo(
				this->mUniformBuffers[i], 0, sizeof(UniformBufferObject));
			VkDescriptorImageInfo imageInfo = plvk::descriptorImageInfo(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
				this->mShadows->mShadowDepthImageViews[i],
				this->mShadows->mShadowsSampler);

			std::vector<VkWriteDescriptorSet> descriptorWrites = std::vector<VkWriteDescriptorSet>();
			descriptorWrites.push_back(plvk::writeDescriptorSet(
				this->mGeometryPassRenderer.mShaders->mDescriptorSets[i],
				0,
				0,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				nullptr,
				&bufferInfo));
			descriptorWrites.push_back(plvk::writeDescriptorSet(
				this->mGeometryPassRenderer.mShaders->mDescriptorSets[i],
				9,
				0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				&imageInfo));

			vkUpdateDescriptorSets(this->mDevice,
				static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr);
		}
	}

	void VulkanRenderer::Init() {
		Application->mRendererAPI = RendererAPI::Vulkan;
		this->mShadows = new VulkanShadows();
		this->mLighting = new VulkanLighting();
		this->mSkybox = new VulkanSkybox();
		this->mPicking = new VulkanPicking();
		this->mGuiRenderer = new VulkanGuiRenderer();

		VulkanShadersCompiler::mDefaultOutDirectory = Application->exeDirectory + "\\CompiledShaders\\";
		VulkanShadersCompiler::mGlslcExePath = "C:\\VulkanSDK\\1.3.268.0\\Bin\\glslc.exe";

		VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\vulkanTriangle.vert");
		VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\vulkanTriangle.frag");

		std::string shadersFolder = VulkanShadersCompiler::mDefaultOutDirectory;
		std::cout << "Initializing vulkan \n";
		InitVulkan();
		std::cout << "SetupDebugMessenger \n";
		SetupDebugMessenger();
		std::cout << "InitSurface \n";
		InitSurface();
		std::cout << "PickPhysicalDevice \n";
		PickPhysicalDevice();
		std::cout << "CreateLogicalDevice \n";
		CreateLogicalDevice();
		std::cout << "InitSwapChain \n";
		InitSwapChain();
		std::cout << "CreateCommandPool \n";
		CreateCommandPool();
		std::cout << "Initializing Shadows \n";
		this->mShadows->Init();
		std::cout << "Initializing image views \n";
		CreateImageViews(VK_IMAGE_LAYOUT_UNDEFINED);
		std::cout << "CreateRenderPass \n";
		CreateSwapchainRenderPass();
		CreateRenderPass();
		std::cout << "CreateDescriptorSetLayout \n";
		CreateDescriptorSetLayout();
		std::cout << "CreateGraphicsPipeline \n";
		CreateGraphicsPipeline();
		std::cout << "CreateDepthResources \n";
		CreateDepthResources();
		std::cout << "CreateFramebuffers \n";
		CreateFramebuffers();
		std::cout << "CreateTextureImage \n";
		//	CreateTextureImage();
		std::cout << "CreateTextureImageView \n";
		// CreateTextureImageView();
		std::cout << "CreateTextureSampler \n";
		CreateTextureSampler();
		std::cout << "Create CreateUniformBuffers \n";

		CreateBuffer(1024 * 1024 * 8 * sizeof(Vertex),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mMainVertexBuffer,
			mMainVertexBufferMemory);
		CreateBuffer(1024 * 1024 * 8 * sizeof(unsigned int),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mMainIndexBuffer,
			mMainIndexBufferMemory);
		mIndirectBuffers.resize(mMaxFramesInFlight);
		mIndirectBufferMemories.resize(mMaxFramesInFlight);
		mMainInstanceMatrixBuffers.resize(mMaxFramesInFlight);
		mMainInstanceMatrixBufferMemories.resize(mMaxFramesInFlight);
		mMainInstanceMaterialBuffers.resize(mMaxFramesInFlight);
		mMainInstanceMaterialBufferMemories.resize(mMaxFramesInFlight);
		mMaterialBuffers.resize(mMaxFramesInFlight);
		mMaterialBufferMemories.resize(mMaxFramesInFlight);
		for (unsigned int i = 0; i < mMaxFramesInFlight; ++i) {
			CreateBuffer(1024 * 1024 * sizeof(VkDrawIndexedIndirectCommand),
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				mIndirectBuffers[i],
				mIndirectBufferMemories[i]);
			CreateBuffer(1024 * 256 * sizeof(glm::mat4),
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				mMainInstanceMatrixBuffers[i],
				mMainInstanceMatrixBufferMemories[i]);
			CreateBuffer(1024 * 256 * sizeof(unsigned int),
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				mMainInstanceMaterialBuffers[i],
				mMainInstanceMaterialBufferMemories[i]);
			CreateBuffer(1024 * 16 * sizeof(MaterialData),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				mMaterialBuffers[i],
				mMaterialBufferMemories[i]);
		}
		// CreateVertexBuffer({glm::vec3(0.0f)}, mMainVertexBuffer,
		// mMainVertexBufferMemory, 1024 * 1024 * 8 * sizeof(Vertex));
		// CreateIndexBuffer({ 0 }, mMainIndexBuffer, mMainIndexBufferMemory, 1024 *
		// 1024 * 8 * sizeof(unsigned int));
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		InitComputeCommandBuffers();
		InitSyncStructures();
		CreateImGuiTextureSampler();

#ifdef EDITOR_MODE
		Editor::Gui::Init(Application->Window->glfwWindow);
#endif

		this->InitializeGeometryPassRenderer();

		VkFormatProperties vkFormatProperties;
		vkGetPhysicalDeviceFormatProperties(
			mPhysicalDevice, VK_FORMAT_R8G8B8_UNORM, &vkFormatProperties);
		std::cout << "Initializing Skybox \n";
		this->mLighting->Init();
		this->mSkybox->Init();
		this->mPicking->Init();
		this->mGuiRenderer->Init();
		this->mBloom.Init();

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 1> bindings = {
			samplerLayoutBinding
		};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

		if (vkCreateDescriptorSetLayout(
			mDevice, &layoutInfo, nullptr, &mSwapchainDescriptorSetLayout)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
		VkPipelineLayoutCreateInfo mSwapchainPipelineLayoutInfo{};
		mSwapchainPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		mSwapchainPipelineLayoutInfo.setLayoutCount = 1;
		mSwapchainPipelineLayoutInfo.pSetLayouts = &mSwapchainDescriptorSetLayout;

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SwapChainPushConstant);
		mSwapchainPipelineLayoutInfo.pushConstantRangeCount = 1;
		mSwapchainPipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		this->mSwapchainRenderer.mRenderPass = this->mSwapchainRenderPass;
		this->mSwapchainRenderer.mShaders = new VulkanShaders(
			VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\swapchainDraw.vert"),
			VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\swapchainDraw.frag"),
			"");
		this->mSwapchainRenderer.Init(
			VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\swapchainDraw.vert"),
			VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\swapchainDraw.frag"),
			"",
			this->mDevice,
			Application->appSizes->sceneSize,
			mSwapchainDescriptorSetLayout,
			mSwapchainPipelineLayoutInfo);

		std::vector<VkDescriptorSetLayout> layouts(mMaxFramesInFlight,
			mSwapchainDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
		};
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = layouts.size();
		allocInfo.pSetLayouts = layouts.data();

		mSwapchainDescriptorSets.resize(mMaxFramesInFlight);
		if (vkAllocateDescriptorSets(
			mDevice, &allocInfo, mSwapchainDescriptorSets.data())
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		for (size_t i = 0; i < mMaxFramesInFlight; i++) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageInfo.imageView = this->mBloom.mTexture1->mImageView;
			imageInfo.sampler = mTextureSampler;

			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = mSwapchainDescriptorSets[i];
			descriptorWrites[0].dstBinding = 1;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(mDevice,
				static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr);
		}

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &semaphore);
	}

	void VulkanRenderer::InitShaders(std::string shadersFolder) {
	}
	void VulkanRenderer::AddInstancesToRender() {
	}
	void VulkanRenderer::RenderShadowMap(Shader& shader) {
	}
	void VulkanRenderer::RenderInstances(Shader& shader) {
		PLAZA_PROFILE_SECTION("Render Instances");

		// mCurrentFrame = 0;

		{
			PLAZA_PROFILE_SECTION("Wait Fences");
			Application->mThreadsManager->mFrameRendererBeforeFenceThread->Update();
			vkWaitForFences(
				mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
			// vkWaitForFences(mDevice, 1, &mComputeInFlightFences[mCurrentFrame],
			// VK_TRUE, UINT64_MAX);
			Application->mThreadsManager->mFrameRendererAfterFenceThread->Update();
		}

		uint32_t imageIndex;
		VkResult result;
		{
			PLAZA_PROFILE_SECTION("Get Next Image");
			result = vkAcquireNextImageKHR(mDevice,
				mSwapChain,
				UINT64_MAX,
				mImageAvailableSemaphores[mCurrentFrame],
				VK_NULL_HANDLE,
				&imageIndex);
		}

		mCurrentImage = imageIndex;

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		{
			PLAZA_PROFILE_SECTION("Update Uniform Buffers");
			this->mShadows->UpdateUniformBuffer(mCurrentFrame);
			UpdateUniformBuffer(mCurrentFrame);
		}

#ifdef EDITOR_MODE
		{
			PLAZA_PROFILE_SECTION("ImGui::Render");
			ImGui::Render();
		}
#endif

		{
			PLAZA_PROFILE_SECTION(
				"Reset Fences/CommandBuffer and Record command buffer");
			vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);
			vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);
			// vkResetFences(mDevice, 1, &mComputeInFlightFences[mCurrentFrame]);
			// vkResetCommandBuffer(mComputeCommandBuffers[mCurrentFrame], 0);

			RecordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);
		}

		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
		VkPipelineStageFlags waitStages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		};
		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };

		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;
			PLAZA_PROFILE_SECTION("Queue");
			if (vkQueueSubmit(
				mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame])
				!= VK_SUCCESS) {
				throw std::runtime_error("failed to submit draw command buffer!");
			}
		}

		{
			// VkSubmitInfo submitInfo{};
			// submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			// submitInfo.commandBufferCount = 1;
			// submitInfo.pCommandBuffers = &mComputeCommandBuffers[mCurrentFrame];
			// submitInfo.signalSemaphoreCount = 1;
			// submitInfo.pSignalSemaphores =
			// &mComputeFinishedSemaphores[mCurrentFrame]; if
			// (vkQueueSubmit(mComputeQueue, 1, &submitInfo, nullptr) != VK_SUCCESS) {
			//	throw std::runtime_error("failed to submit draw command buffer!");
			// }
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { mSwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		{
			PLAZA_PROFILE_SECTION("QueuePresent");
			result = vkQueuePresentKHR(mPresentQueue, &presentInfo);
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
			mFramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
	}
	void VulkanRenderer::RenderBloom() {
	}
	void VulkanRenderer::RenderScreenSpaceReflections() {
	}
	void VulkanRenderer::RenderFullScreenQuad() {
	}
	void VulkanRenderer::RenderOutline() {
	}
	void VulkanRenderer::RenderHDR() {
	}
	void VulkanRenderer::Destroy() {
		this->mSkybox->Terminate();
		this->mShadows->Terminate();
		/* Clean Renderer */
		CleanupSwapChain();

		vkDestroySampler(mDevice, mTextureSampler, nullptr);
		vkDestroyImageView(mDevice, mTextureImageView, nullptr);
		vkDestroyImage(mDevice, mTextureImage, nullptr);
		vkFreeMemory(mDevice, mTextureImageMemory, nullptr);

		vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);

		vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

		vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);

		for (size_t i = 0; i < mMaxFramesInFlight; i++) {
			vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(mDevice, mInFlightFences[i], nullptr);

			vkDestroyBuffer(mDevice, mUniformBuffers[i], nullptr);
			vkFreeMemory(mDevice, mUniformBuffersMemory[i], nullptr);
		}
		vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);

		vkDestroyBuffer(mDevice, mIndexBuffer, nullptr);
		vkFreeMemory(mDevice, mIndexBufferMemory, nullptr);

		vkDestroyBuffer(mDevice, mVertexBuffer, nullptr);
		vkFreeMemory(mDevice, mVertexBufferMemory, nullptr);

		vkDestroyInstance(mVulkanInstance, nullptr);
		vkDestroyDevice(mDevice, nullptr);
		vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);

		for (auto framebuffer : mSwapChainFramebuffers) {
			vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
		}

		for (auto imageView : mSwapChainImageViews) {
			vkDestroyImageView(mDevice, imageView, nullptr);
		}
	}
	void VulkanRenderer::CopyLastFramebufferToFinalDrawBuffer() {
	}

	static void
		check_vk_result(VkResult err) {
		if (err == 0)
			return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0)
			abort();
	}

	void VulkanRenderer::InitGUI() {
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 100000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		if (vkCreateDescriptorPool(
			mDevice, &pool_info, nullptr, &mImguiDescriptorPool)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Device = mDevice;
		initInfo.DescriptorPool = mImguiDescriptorPool;
		initInfo.Instance = mVulkanInstance;
		initInfo.PhysicalDevice = mPhysicalDevice;
		initInfo.Queue = mGraphicsQueue;
		initInfo.MinImageCount = mMaxFramesInFlight;
		initInfo.ImageCount = mMaxFramesInFlight;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&initInfo, this->mSwapchainRenderPass);

		ImGui_ImplVulkan_SetMinImageCount(mMaxFramesInFlight);

		//		mFinalSceneDescriptorSet =
		// ImGui_ImplVulkan_AddTexture(mTextureSampler,
		// this->mShadows->mCascades[2].mImageView, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		////TODO: FIX VALIDATION ERROR
		mFinalSceneDescriptorSet = ImGui_ImplVulkan_AddTexture(
			mTextureSampler,
			this->mFinalSceneImageView,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR); // TODO: FIX VALIDATION ERROR
		// mFinalSceneDescriptorSet = ImGui_ImplVulkan_AddTexture(mTextureSampler,
		// ((VulkanPicking*)(this->mPicking))->mPickingTextureImageView,
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR); //TODO: FIX VALIDATION ERROR
		// this->TransitionImageLayout(this->mFinalSceneImage, mSwapChainImageFormat,
		// VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		// this->mShadows->mDebugDepthDescriptorSet =
		// ImGui_ImplVulkan_AddTexture(this->mTextureSampler,
		// this->mShadows->mDebugDepthImageView, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}

	void VulkanRenderer::NewFrameGUI() {
		ImGui_ImplVulkan_NewFrame();
	}

	void VulkanRenderer::UpdateGUI() {
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
			mCommandBuffers[mCurrentFrame]);
	}

	ImTextureID
		VulkanRenderer::GetFrameImage() {
		// return (ImTextureID)this->mShadows->mDebugDepthDescriptorSet;
		return (ImTextureID)mFinalSceneDescriptorSet;
	}

	void VulkanRenderer::CreateImage(uint32_t width,
		uint32_t height,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& imageMemory) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(mDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(mDevice, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(mDevice, image, imageMemory, 0);
	}

	Texture*
		VulkanRenderer::LoadTexture(std::string path, uint64_t uuid) {
		VulkanTexture* texture = new VulkanTexture();
		if (AssetsManager::TextureExists(uuid))
			return AssetsManager::GetTexture(uuid);
		if (uuid != 0)
			texture->mAssetUuid = uuid;
		texture->path = path;
		if (std::filesystem::exists(path)) {
			VkFormat form = VK_FORMAT_R8G8B8A8_SRGB;
			texture->CreateTextureImage(mDevice, path, form, true);
			texture->CreateTextureSampler();
			texture->CreateImageView(form, VK_IMAGE_ASPECT_COLOR_BIT);
			texture->InitDescriptorSetLayout();
		}
		return texture;
	}
	Texture*
		VulkanRenderer::LoadImGuiTexture(std::string path) {
		VulkanTexture* texture = new VulkanTexture();
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
		texture->CreateTextureImage(mDevice, path, format, false);
		texture->CreateTextureSampler();
		texture->CreateImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
		texture->mDescriptorSet = ImGui_ImplVulkan_AddTexture(texture->mSampler,
			texture->mImageView,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		return texture;
	}

	Mesh& VulkanRenderer::CreateNewMesh(
		vector<glm::vec3> vertices,
		vector<glm::vec3> normals,
		vector<glm::vec2> uvs,
		vector<glm::vec3> tangent,
		vector<glm::vec3> bitangent,
		vector<unsigned int> indices,
		Material& material,
		bool usingNormal,
		vector<BonesHolder> bonesHolder) {
		VulkanMesh& vulkMesh = *new VulkanMesh(vertices,
			normals,
			uvs,
			tangent,
			bitangent,
			indices,
			usingNormal,
			bonesHolder);

		vector<Vertex> convertedVertices;
		convertedVertices.reserve(vertices.size());

		for (unsigned int i = 0; i < vertices.size(); i++) {
			vulkMesh.CalculateVertexInBoundingBox(vertices[i]);

			convertedVertices.push_back(Vertex{
				vertices[i],
				(normals.size() > i) ? normals[i] : glm::vec3(1.0f),
				(uvs.size() > i) ? uvs[i] : glm::vec2(0.0f),
				(tangent.size() > i) ? tangent[i] : glm::vec3(0.0f),
				(bitangent.size() > i) ? bitangent[i] : glm::vec3(0.0f),
				(bonesHolder.size() > i && bonesHolder[i].mBones.size() > 0) ? bonesHolder[i].GetBoneIds() : std::array<int, MAX_BONE_INFLUENCE>{-1, -1, -1, -1},
				(bonesHolder.size() > i && bonesHolder[i].mBones.size() > 0) ? bonesHolder[i].GetBoneWeights() : std::array<float, MAX_BONE_INFLUENCE>{0, 0, 0, 0} });
		}

		vulkMesh.verticesCount = vertices.size();
		vulkMesh.verticesOffset = this->mBufferTotalVertices;
		vulkMesh.indicesCount = indices.size();
		vulkMesh.indicesOffset = this->mBufferTotalIndices;

		// Add vertices to the big vertex buffer
		VkDeviceSize newDataSize = sizeof(Vertex) * convertedVertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(newDataSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, newDataSize, 0, &data);
		memcpy(data, convertedVertices.data(), static_cast<size_t>(newDataSize));
		vkUnmapMemory(mDevice, stagingBufferMemory);

		CopyBuffer(stagingBuffer,
			mMainVertexBuffer,
			newDataSize,
			mBufferTotalVertices * sizeof(Vertex));

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);

		// this->CreateVertexBuffer(convertedVertices, vulkMesh.mVertexBuffer,
		// vulkMesh.mVertexBufferMemory);

		// this->CreateIndexBuffer(indices, vulkMesh.mIndexBuffer,
		// vulkMesh.mIndexBufferMemory);

		// Add vertices to the big index buffer

		VkDeviceSize indicesDataSize = sizeof(unsigned int) * indices.size();

		VkBuffer indexStagingBuffer;
		VkDeviceMemory indexStagingBufferMemory;
		CreateBuffer(newDataSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			indexStagingBuffer,
			indexStagingBufferMemory);

		void* data2;
		vkMapMemory(mDevice, indexStagingBufferMemory, 0, indicesDataSize, 0, &data2);
		memcpy(data2, indices.data(), static_cast<size_t>(indicesDataSize));
		vkUnmapMemory(mDevice, indexStagingBufferMemory);

		CopyBuffer(indexStagingBuffer,
			mMainIndexBuffer,
			indicesDataSize,
			mBufferTotalIndices * sizeof(unsigned int));

		vkDestroyBuffer(mDevice, indexStagingBuffer, nullptr);
		vkFreeMemory(mDevice, indexStagingBufferMemory, nullptr);

		mBufferTotalVertices += vertices.size();
		mBufferTotalIndices += indices.size();
		VkDrawIndexedIndirectCommand indirectCommand{};
		indirectCommand.firstIndex = mBufferTotalIndices - indices.size();
		indirectCommand.vertexOffset = mBufferTotalVertices - vertices.size();
		indirectCommand.firstInstance = mTotalInstances;
		indirectCommand.indexCount = indices.size();
		indirectCommand.instanceCount = 1;
		this->mIndirectCommands.push_back(indirectCommand);
		mTotalInstances++;
		mIndirectDrawCount++;
		// Create instance buffer
		// VkDeviceSize bufferSize = 16 * sizeof(glm::mat4);
		// this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		// vulkMesh.mInstanceBuffer, vulkMesh.mInstanceBufferMemory);
		return vulkMesh;
	}

	void VulkanRenderer::DrawRenderGroupShadowDepthMapInstanced(
		RenderGroup* renderGroup,
		unsigned int cascadeIndex) {
		if (cascadeIndex >= renderGroup->mCascadeInstances.size() || renderGroup->mCascadeInstances[cascadeIndex].size() <= 0)
			return;
		VulkanMesh* mesh = (VulkanMesh*)renderGroup->mesh;
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(glm::mat4) * renderGroup->mCascadeInstances[cascadeIndex].size();
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		void* data;
		vkMapMemory(this->mDevice,
			renderGroup->mInstanceBufferMemories[mCurrentFrame],
			0,
			bufferInfo.size,
			0,
			&data);
		memcpy(data,
			renderGroup->mCascadeInstances[cascadeIndex].data(),
			static_cast<size_t>(bufferInfo.size));
		vkUnmapMemory(this->mDevice,
			renderGroup->mInstanceBufferMemories[mCurrentFrame]);

		VkDeviceSize offsets[] = { 0, 0 };
		VkCommandBuffer activeCommandBuffer = *this->mActiveCommandBuffer;

		// vkCmdPushConstants(*this->mActiveCommandBuffer, this->mPipelineLayout,
		// VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VulkanRenderer::PushConstants),
		// &pushData);
		vector<VkBuffer> verticesBuffer = {
			mesh->mVertexBuffer, renderGroup->mInstanceBuffers[mCurrentFrame]
		};
		vkCmdBindVertexBuffers(
			activeCommandBuffer, 0, 2, verticesBuffer.data(), offsets);
		vkCmdBindIndexBuffer(
			activeCommandBuffer, mesh->mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(activeCommandBuffer,
			static_cast<uint32_t>(mesh->indices.size()),
			renderGroup->mCascadeInstances[cascadeIndex].size(),
			0,
			0,
			0);
		renderGroup->mCascadeInstances[cascadeIndex].clear();
	}
	void VulkanRenderer::DrawRenderGroupInstanced(RenderGroup* renderGroup) {
		PLAZA_PROFILE_SECTION("DrawRenderGroupInstanced");
		VulkanMesh* mesh;
		{
			PLAZA_PROFILE_SECTION("Copy Data");
			mesh = (VulkanMesh*)renderGroup->mesh;
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = sizeof(glm::mat4) * renderGroup->instanceModelMatrices.size();
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			void* data;
			vkMapMemory(this->mDevice,
				renderGroup->mInstanceBufferMemories[mCurrentFrame],
				0,
				bufferInfo.size,
				0,
				&data);
			memcpy(data,
				renderGroup->instanceModelMatrices.data(),
				static_cast<size_t>(bufferInfo.size));
			vkUnmapMemory(this->mDevice,
				renderGroup->mInstanceBufferMemories[mCurrentFrame]);
		}

		VkDeviceSize offsets[] = { 0, 0 };
		VkCommandBuffer activeCommandBuffer = *this->mActiveCommandBuffer;

		VulkanRenderer::PushConstants pushData;
		pushData.color = renderGroup->material->diffuse->rgba;
		pushData.diffuseIndex = renderGroup->material->diffuse->mIndexHandle;
		pushData.normalIndex = renderGroup->material->normal->mIndexHandle;
		pushData.metalnessIndex = renderGroup->material->metalness->mIndexHandle;
		pushData.roughnessIndex = renderGroup->material->roughness->mIndexHandle;
		pushData.metalnessFloat = renderGroup->material->metalnessFloat;
		pushData.roughnessFloat = renderGroup->material->roughnessFloat;

		{
			PLAZA_PROFILE_SECTION("PushConstants and Descriptor sets");
			vkCmdPushConstants(*this->mActiveCommandBuffer,
				this->mPipelineLayout,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(VulkanRenderer::PushConstants),
				&pushData);
		}

		{
			PLAZA_PROFILE_SECTION("Bind Buffers");
			vector<VkBuffer> verticesBuffer = {
				mesh->mVertexBuffer, renderGroup->mInstanceBuffers[mCurrentFrame]
			};
			std::array<VkBuffer, 2> buffers = {
				mesh->mVertexBuffer, renderGroup->mInstanceBuffers[mCurrentFrame]
			};
			vkCmdBindVertexBuffers(activeCommandBuffer, 0, 2, buffers.data(), offsets);
			// vkCmdBindVertexBuffers(activeCommandBuffer, 1, 1, &mesh->mInstanceBuffer,
			// offsets);
			vkCmdBindIndexBuffer(
				activeCommandBuffer, mesh->mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

		{
			PLAZA_PROFILE_SECTION("Draw Indexed");
			vkCmdDrawIndexed(activeCommandBuffer,
				static_cast<uint32_t>(mesh->indices.size()),
				renderGroup->instanceModelMatrices.size(),
				0,
				0,
				0);
		}
		uint64_t meshTriangles = renderGroup->mesh->indices.size() / 3;
		Time::mUniqueTriangles += meshTriangles;
		Time::mTotalTriangles += meshTriangles * renderGroup->instanceModelMatrices.size();
		Time::drawCalls++;
		renderGroup->instanceModelMatrices.clear();
	}

	void VulkanRenderer::ChangeFinalDescriptorImageView(VkImageView newImageView) {
		this->mFinalSceneDescriptorSet = ImGui_ImplVulkan_AddTexture(
			this->mTextureSampler,
			newImageView,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR); // TODO: FIX VALIDATION ERROR
		UpdateUniformBuffer(mCurrentFrame);
	}

	Mesh* VulkanRenderer::RestartMesh(Mesh* mesh) {
		uint64_t oldUuid = mesh->uuid;
		Mesh* newMesh = &this->CreateNewMesh(mesh->vertices,
			mesh->normals,
			mesh->uvs,
			mesh->tangent,
			mesh->bitangent,
			mesh->indices,
			*new Material(),
			false);
		newMesh = newMesh;
		newMesh->uuid = oldUuid;
		delete (mesh);
		return newMesh;
	}

	void VulkanRenderer::UpdateProjectManager() {
		PLAZA_PROFILE_SECTION("Render Instances");

		// mCurrentFrame = 0;

		{
			PLAZA_PROFILE_SECTION("Wait Fences");
			vkWaitForFences(
				mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
		}

		uint32_t imageIndex;
		VkResult result;
		{
			PLAZA_PROFILE_SECTION("Get Next Image");
			result = vkAcquireNextImageKHR(mDevice,
				mSwapChain,
				UINT64_MAX,
				mImageAvailableSemaphores[mCurrentFrame],
				VK_NULL_HANDLE,
				&imageIndex);
		}

		mCurrentImage = imageIndex;

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		{
			PLAZA_PROFILE_SECTION("Update Uniform Buffers");
			this->mShadows->UpdateUniformBuffer(mCurrentFrame);
			UpdateUniformBuffer(mCurrentFrame);
		}

#ifdef EDITOR_MODE
		{
			PLAZA_PROFILE_SECTION("ImGui::Render");
			ImGui::Render();
		}
#endif

		{
			PLAZA_PROFILE_SECTION(
				"Reset Fences/CommandBuffer and Record command buffer");
			vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);
			vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);

			VkCommandBuffer commandBuffer = mCommandBuffers[mCurrentFrame];
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}
			mActiveCommandBuffer = &commandBuffer;

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = mSwapchainRenderPass;
			renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex];
			renderPassInfo.renderArea.extent = mSwapChainExtent;

			vkCmdBeginRenderPass(
				commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			//		vkCmdBindPipeline(commandBuffer,
			// VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);
			VkViewport viewport{};
			viewport.width = static_cast<float>(mSwapChainExtent.width);
			viewport.height = static_cast<float>(mSwapChainExtent.height);
			VkRect2D scissor{};
			scissor.extent = mSwapChainExtent;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
		VkPipelineStageFlags waitStages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		{
			PLAZA_PROFILE_SECTION("Queue");
			if (vkQueueSubmit(
				mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame])
				!= VK_SUCCESS) {
				throw std::runtime_error("failed to submit draw command buffer!");
			}
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { mSwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		{
			PLAZA_PROFILE_SECTION("QueuePresent");
			result = vkQueuePresentKHR(mPresentQueue, &presentInfo);
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
			mFramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
	}

	void VulkanRenderer::AddTrackerToImage(VkImageView imageView,
		std::string name,
		VkSampler textureSampler,
		VkImageLayout layout) {
#ifdef EDITOR_MODE
		VkDescriptorSet imguiDescriptorSet = ImGui_ImplVulkan_AddTexture(
			textureSampler == VK_NULL_HANDLE ? this->mTextureSampler : textureSampler,
			imageView,
			layout);
		this->mTrackedImages.push_back(TrackedImage{
			ImTextureID(imguiDescriptorSet), std::chrono::system_clock::now(), name
			});
#endif
	}

	void VulkanRenderer::AddMaterial(Material* material) {
		MaterialData materialData{};
		materialData.color = material->diffuse->rgba;
		materialData.diffuseIndex = material->diffuse->mIndexHandle;
		materialData.intensity = material->intensity;
		materialData.metalnessFloat = material->metalnessFloat;
		materialData.metalnessIndex = material->metalness->mIndexHandle;
		materialData.normalIndex = material->normal->mIndexHandle;
		materialData.roughnessFloat = material->roughnessFloat;
		materialData.roughnessIndex = material->roughness->mIndexHandle;

		material->mIndexHandle = this->mUploadedMaterials.size();
		this->mUploadedMaterials.push_back(materialData);

		VkDeviceSize size = sizeof(MaterialData) * this->mUploadedMaterials.size();

		for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			void* data;
			vkMapMemory(
				mDevice, this->mMaterialBufferMemories[mCurrentFrame], 0, size, 0, &data);
			memcpy(data, this->mUploadedMaterials.data(), static_cast<size_t>(size));
			vkUnmapMemory(mDevice, this->mMaterialBufferMemories[mCurrentFrame]);
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = this->mMaterialBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = VK_WHOLE_SIZE;

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = this->mGeometryPassRenderer.mShaders->mDescriptorSets
				[this->mCurrentFrame]; // this->mDescriptorSets[mCurrentFrame];
			descriptorWrite.dstBinding = 19;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(mDevice, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void VulkanRenderer::UpdateMaterials() {
		std::vector<MaterialData> materialDataVector = std::vector<MaterialData>();

		for (auto& [key, value] : Application->activeScene->materials) {
			MaterialData materialData{};
			materialData.color = value->diffuse->rgba;
			materialData.diffuseIndex = value->diffuse->mIndexHandle;
			materialData.intensity = value->intensity;
			materialData.metalnessFloat = value->metalnessFloat;
			materialData.metalnessIndex = value->metalness->mIndexHandle;
			materialData.normalIndex = value->normal->mIndexHandle;
			materialData.roughnessFloat = value->roughnessFloat;
			materialData.roughnessIndex = value->roughness->mIndexHandle;

			value.get()->mIndexHandle = materialDataVector.size();
			value->mIndexHandle = materialDataVector.size();

			materialDataVector.push_back(materialData);
		}

		VkDeviceSize size = sizeof(MaterialData) * materialDataVector.size();

		void* data;
		vkMapMemory(
			mDevice, this->mMaterialBufferMemories[mCurrentFrame], 0, size, 0, &data);
		memcpy(data, materialDataVector.data(), static_cast<size_t>(size));
		vkUnmapMemory(mDevice, this->mMaterialBufferMemories[mCurrentFrame]);

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = this->mMaterialBuffers[mCurrentFrame];
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = this->mGeometryPassRenderer.mShaders->mDescriptorSets
			[this->mCurrentFrame]; // this->mDescriptorSets[mCurrentFrame];
		descriptorWrite.dstBinding = 19;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(mDevice, 1, &descriptorWrite, 0, nullptr);
	}

	void VulkanRenderer::InitComputeCommandBuffers() {
		mComputeCommandBuffers.resize(mMaxFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)mComputeCommandBuffers.size();

		if (vkAllocateCommandBuffers(
			mDevice, &allocInfo, mComputeCommandBuffers.data())
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to allocate compute command buffers!");
		}
	}

	VkDescriptorSet
		VulkanRenderer::GetGeometryPassDescriptorSet(unsigned int frame) {
		return this->mGeometryPassRenderer.mShaders->mDescriptorSets[frame];
	}
} // namespace Plaza