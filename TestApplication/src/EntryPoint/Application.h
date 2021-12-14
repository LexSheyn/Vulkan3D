#ifndef APPLICATION_H
#define APPLICATION_H

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin

const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG

	const bool EnableValidationLayers = false;

#else

	const bool EnableValidationLayers = true;

#endif

struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;

// Accessors:

	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR Capabilities{};

	std::vector<VkSurfaceFormatKHR> Formats;

	std::vector<VkPresentModeKHR> PresentModes;
};

class Application
{
public:

// Functions:

	void Run();

private:

// Private Functions:

	void InitWindow();

	void InitVulkan();

	void PickPhisicalDevice();

	void CreateLogicalDevice();

	SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice device );

	bool IsDeviceSuitable( VkPhysicalDevice device );

	bool CheckDeviceExtensionSupport( VkPhysicalDevice device );

	int32_t RateDeviceSuitability( VkPhysicalDevice device );

	QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice device );

	void CreateInstance();

	void PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo );

	void SetupDebugMessenger();

	void CreateSurface();

	void MainLoop();

	void Cleanup();

	bool CheckValidationLayerSupport();

// Private Accessors:

	std::vector<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		                                                 VkDebugUtilsMessageTypeFlagsEXT messgeType,
		                                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		                                                 void* pUserData);

// Proxy Functions:

	/// <summary>
	/// Proxy function...
	/// </summary>
	/// <param name="instance"></param>
	/// <param name="pCreateInfo"></param>
	/// <param name="pAllocator"></param>
	/// <param name="pDebugMessenger"></param>
	/// <returns></returns>
	static VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, 
		                                    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		                                    const VkAllocationCallbacks* pAllocator, 
		                                          VkDebugUtilsMessengerEXT* pDebugMessenger );
	
	/// <summary>
	/// Proxy function...
	/// </summary>
	/// <param name="instance"></param>
	/// <param name="debugMessenger"></param>
	/// <param name="pAllocator"></param>
	static void DestroyDebugUtilsMessengerEXT( VkInstance instance, 
		                                       VkDebugUtilsMessengerEXT debugMessenger, 
		                                 const VkAllocationCallbacks* pAllocator );

// Variables:

	GLFWwindow* Window;

	const uint32_t WIDTH  = 800u;

	const uint32_t HEIGHT = 600u;

	VkInstance Instance;

	VkDebugUtilsMessengerEXT DebugMessenger;

	VkSurfaceKHR Surface;

	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

	VkDevice Device;

	VkQueue  GraphicsQueue;
	VkQueue  PresentQueue;
};

#pragma warning( pop ) // Vulkan SDK - End

#endif // APPLICATION_H