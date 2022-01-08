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
#include "../Logger/Logger.h"

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

struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding   = 0;
		bindingDescription.stride    = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding  = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset   = offsetof(Vertex, position);

		attributeDescriptions[1].binding  = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset   = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

const std::vector<Vertex> Vertices =
{
	{{ -0.5f, -0.5f}, { 1.0f, 0.0f, 0.0f }},
	{{  0.5f, -0.5f}, { 0.0f, 1.0f, 0.0f }},
	{{  0.5f,  0.5f}, { 0.0f, 0.0f, 1.0f }},
	{{ -0.5f,  0.5f}, { 1.0f, 1.0f, 1.0f }}
};

const std::vector<uint16_t> Indices =
{
	0, 1, 2, 2, 3, 0
};

class Application
{
public:

// Functions:

	void Run();

private:

// Private Functions:

	void InitWindow();

	static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

	void InitVulkan();

	void PickPhisicalDevice();

	void CreateLogicalDevice();

	void CreateSwapChain();

	void CreateImageViews();

	void CreateRenderPass();

	void CreateGraphicsPipeline();

	void CreateFrameBuffers();

	void CreateCommandPool();

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

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

	void CleanupSwapChain();

	void Cleanup();

	void RecreateSwapChain();

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

	uint32_t WIDTH  = 800u;

	uint32_t HEIGHT = 600u;

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

	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;

	VkBuffer IndexBuffer;
	VkDeviceMemory IndexBufferMemory;

	std::vector<VkCommandBuffer> CommandBuffers;

// Multithreded rendering:

	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;

	std::vector<VkFence> InFlightFences;
	std::vector<VkFence> ImagesInFlight;

	size_t CurrentFrame = 0u;

	bool FrameBufferResized = false;



	// TEST
	VkViewport Viewport;

	VkRect2D Scissor;

	t3d::ShaderManager m_ShaderManager;

	float32 m_LastFrameTime = 0.0f;
};

#pragma warning( pop ) // Vulkan SDK - End

#endif // APPLICATION_H