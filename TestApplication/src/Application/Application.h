#ifndef APPLICATION_H
#define APPLICATION_H

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin
#pragma warning( disable : 26495 ) // Uninitialized variable.
#pragma warning( disable : 26439 ) // Function can not throw exception.

#include "../ShaderManager/ShaderManager.h"

#include "../Timer/ScopeTimer32.h"
#include "../Timer/Timer32.h"
#include "../Timer/TimeStep32.h"

const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

const size_t MAX_FRAMES_IN_FLIGHT = 2;

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

	void CreateSwapChain();

	void CreateImageViews();

	void CreateRenderPass();

	void CreateGraphicsPipeline();

	void CreateFrameBuffers();

	void CreateCommandPool();

	void CreateCommandBuffers();

	void CreateSyncObjects();

	void DrawFrame();

	VkShaderModule CreateShaderModule( const std::vector<char>& code );

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats );

	VkPresentModeKHR ChooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes );

	VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& surfaceCapabilities );

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

	void CleanupSwapChain();

	void RecreateSwaoChain();

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

	VkSwapchainKHR SwapChain;

	std::vector<VkImage> SwapChainImages;

	VkFormat SwapChainImageFormat;

	VkExtent2D SwapChainExtent;

	std::vector<VkImageView> SwapChainImageViews;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	VkRenderPass RenderPass;
	VkPipelineLayout PipelineLayout;

	VkPipeline GraphicsPipeline;

	VkCommandPool CommandPool;

	std::vector<VkCommandBuffer> CommandBuffers;

// Multithreded rendering:

	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;

	std::vector<VkFence> InFlightFences;
	std::vector<VkFence> ImagesInFlight;

	size_t CurrentFrame = 0u;

	// TEST
	t3d::ShaderManager n_ShaderManager;

	float32 m_LastFrameTime = 0.0f;
};

#pragma warning( pop ) // Vulkan SDK - End

#endif // APPLICATION_H