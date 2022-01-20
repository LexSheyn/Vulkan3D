#include "../PrecompiledHeaders/stdafx.h"
#include "Application.h"

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin

#pragma warning( disable : 26819 )
#pragma warning( disable : 6262  )
#define STB_IMAGE_IMPLEMENTATION
#include "../STB_Image/stb_image.h"

// Functions:

void Application::Run()
{
	this->InitWindow();

	this->InitVulkan();

	this->MainLoop();

	this->Cleanup();
}


// Private Functions:

void Application::InitWindow()
{
	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE , GLFW_TRUE   );

	Window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan3D", nullptr, nullptr );

	glfwSetWindowUserPointer(Window, this);

	glfwSetFramebufferSizeCallback(Window, FrameBufferResizeCallback);
}

void Application::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

	application->FrameBufferResized = true;

	application->WIDTH = width;

	application->HEIGHT = height;
}

void Application::InitVulkan()
{
	this->CreateInstance();

	this->SetupDebugMessenger();

	this->CreateSurface();

	this->PickPhisicalDevice();

	this->CreateLogicalDevice();

	this->CreateSwapChain();

	this->CreateImageViews();

	this->CreateRenderPass();

	this->CreateDescriptorSetLayout();

	this->CreateGraphicsPipeline();

	this->CreateFrameBuffers();

	this->CreateCommandPool();

	this->CreateTextureImage();

	this->CreateVertexBuffer();

	this->CreateIndexBuffer();

	this->CreateUniformBuffers();

	this->CreateDescriptorPool();

	this->CreateDescriptorSets();

	this->CreateCommandBuffers();

	this->CreateSyncObjects();
}

void Application::PickPhisicalDevice()
{
	uint32_t deviceCount = 0u;

	#pragma warning( disable : 26812) // Warning C26812 The enum type 'VkResult' is unscoped.Prefer 'enum class' over 'enum' (Enum.3).

	vkEnumeratePhysicalDevices( Instance, &deviceCount, nullptr ); // C26812

	if ( deviceCount == 0u )
	{
		throw std::runtime_error( "ERROR::Application::PickPhysicalDevice: Failed to find GPUs with Vulkan support." );
	}

	std::vector<VkPhysicalDevice> devices( deviceCount );

	vkEnumeratePhysicalDevices( Instance, &deviceCount, devices.data() );

// Check if any of physical devices meet the requirements.

	for ( const auto& device : devices )
	{
		if ( this->IsDeviceSuitable( device ) )
		{
			PhysicalDevice = device;

			break;
		}
	}

	if ( PhysicalDevice == VK_NULL_HANDLE )
	{
		throw std::runtime_error( "ERROR::Application::PickPhysicalDevice: Failed to find a suitable GPU!" );
	}

// Use an ordered map to automatically sort candidates by increasing score:

	std::multimap<int32_t, VkPhysicalDevice> candidates;

	for ( const auto& device : devices )
	{
		int32_t score = this->RateDeviceSuitability( device );

		candidates.insert( std::make_pair( score, device ) );
	}

// Check is the best candidate is suitable at all:

	if ( candidates.rbegin()->first > 0 )
	{
		PhysicalDevice = candidates.rbegin()->second;
	}
	else
	{
		throw std::runtime_error( "ERROR::Application::PickPhysicalDevice: Failed to find a suitable GPU!" );
	}
}

void Application::CreateLogicalDevice()
{
	QueueFamilyIndices indices = this->FindQueueFamilies( PhysicalDevice );

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

	float queuePriority = 1.0f;

	for ( uint32_t queueFamily : uniqueQueueFamilies )
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};

		queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		
		queueCreateInfos.push_back( queueCreateInfo );
	}

	VkPhysicalDeviceFeatures physicalDeviceFeatures{};

	//

	VkDeviceCreateInfo deviceCreateInfo{};

	deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount    = static_cast<uint32_t>( queueCreateInfos.size() );
	deviceCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures        = &physicalDeviceFeatures;
	deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>( DeviceExtensions.size() );
	deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

	if ( EnableValidationLayers )
	{
		deviceCreateInfo.enabledLayerCount   = static_cast<uint32_t>( ValidationLayers.size() );
		deviceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
	}
	else
	{
		deviceCreateInfo.enabledLayerCount   = 0;
	}

	if ( vkCreateDevice( PhysicalDevice, &deviceCreateInfo, nullptr, &Device ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateLogicalDevice: Failed to create logical device!" );
	}

	vkGetDeviceQueue( Device, indices.GraphicsFamily.value(), 0, &GraphicsQueue );
	vkGetDeviceQueue( Device, indices.PresentFamily.value() , 0, &PresentQueue  );
}

void Application::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = this->QuerySwapChainSupport( PhysicalDevice );

	VkSurfaceFormatKHR surfaceFormat = this->ChooseSwapSurfaceFormat( swapChainSupport.Formats );
	VkPresentModeKHR   presentMode   = this->ChooseSwapPresentMode( swapChainSupport.PresentModes );
	VkExtent2D         extent        = this->ChooseSwapExtent( swapChainSupport.Capabilities );

	uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1u;

	if ( swapChainSupport.Capabilities.maxImageCount > 0u && imageCount > swapChainSupport.Capabilities.maxImageCount )
	{
		imageCount = swapChainSupport.Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapChainCreateInfo{};

	swapChainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface          = Surface;
	swapChainCreateInfo.minImageCount    = imageCount;
	swapChainCreateInfo.imageFormat      = surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace  = surfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent      = extent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = this->FindQueueFamilies( PhysicalDevice );

	uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

	if ( indices.GraphicsFamily != indices.PresentFamily )
	{
		swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	else
	{
		swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;      // Optional.
		swapChainCreateInfo.pQueueFamilyIndices   = nullptr; // Optional.
	}

	swapChainCreateInfo.preTransform   = swapChainSupport.Capabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode    = presentMode;
	swapChainCreateInfo.clipped        = VK_TRUE;

	if ( vkCreateSwapchainKHR( Device, &swapChainCreateInfo, nullptr, &SwapChain ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateSwapChain: Failed to create swap chain!" );
	}

	vkGetSwapchainImagesKHR( Device, SwapChain, &imageCount, nullptr );

	SwapChainImages.resize( imageCount );

	vkGetSwapchainImagesKHR( Device, SwapChain, &imageCount, SwapChainImages.data() );

	SwapChainImageFormat = surfaceFormat.format;

	SwapChainExtent      = extent;
}

void Application::CreateImageViews()
{
	SwapChainImageViews.resize( SwapChainImages.size() );

	for ( size_t i = 0u; i < SwapChainImages.size(); i++ )
	{
		VkImageViewCreateInfo imageViewCreateInfo{};

		imageViewCreateInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image    = SwapChainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format   = SwapChainImageFormat;

		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
		imageViewCreateInfo.subresourceRange.levelCount     = 1; // L E V E L
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount     = 1; // L A Y E R

		if ( vkCreateImageView( Device, &imageViewCreateInfo, nullptr, &SwapChainImageViews[i] ) != VK_SUCCESS )
		{
			throw std::runtime_error( "ERROR::Application::CreateImageViews: Failed to create image views!" );
		}
	}
}

void Application::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};

	colorAttachment.format         = SwapChainImageFormat;
	colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference{};

	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription{};

	subpassDescription.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments    = &colorAttachmentReference;

	VkSubpassDependency subpassDependency{};

	subpassDependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass    = 0;
	subpassDependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};

	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments    = &colorAttachment;
	renderPassInfo.subpassCount    = 1;
	renderPassInfo.pSubpasses      = &subpassDescription;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies   = &subpassDependency;

	if ( vkCreateRenderPass( Device, &renderPassInfo, nullptr, &RenderPass ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateRenderPass: Failed to create render pass!" );
	}
}

void Application::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uniformBufferObjectLayoutBinding{};

	uniformBufferObjectLayoutBinding.binding            = 0;
	uniformBufferObjectLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectLayoutBinding.descriptorCount    = 1;
	uniformBufferObjectLayoutBinding.pImmutableSamplers = nullptr; // Optional.
	uniformBufferObjectLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};

	layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings    = &uniformBufferObjectLayoutBinding;

	if (vkCreateDescriptorSetLayout(Device, &layoutInfo, nullptr, &DescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR::Application::CreateDescriptorSetLayout: Failed to create descriptor set layout!");
	}
}

void Application::CreateGraphicsPipeline()
{
// ------------------------------ TEST SETTINGS ------------------------------
	m_ShaderManager.SetDirectoryGLSL( "D:/VULKAN_3D_SHADERS/GLSL/" );
	m_ShaderManager.SetDirectorySPV(  "D:/VULKAN_3D_SHADERS/SPV/"  );
// ---------------------------------------------------------------------------

//	std::string shaderCode = m_ShaderCompiler.LoadGLSL( "shader.frag" );
//
//	std::cout << shaderCode << std::endl;
//
//	shaderc::AssemblyCompilationResult compilationResult = m_ShaderCompiler.CompileFragmentShader( shaderCode );
//
//	m_ShaderCompiler.SaveToSPV( "fragment_shader", { compilationResult.cbegin(), compilationResult.cend() } );

//	std::string TEST =  m_ShaderCompiler.LoadGLSL("shader.vert");
//
//	std::cout << TEST << std::endl;

	std::vector<char> vertexShaderCode   = m_ShaderManager.LoadSPV( "vert" );
	std::vector<char> fragmentShaderCode = m_ShaderManager.LoadSPV( "frag" );

//	std::cout << "Vertex Shader size: "   << vertexShaderCode.size()   * sizeof(char) << std::endl;
//	std::cout << "Fragment Shader size: " << fragmentShaderCode.size() * sizeof(char) << std::endl;

	VkShaderModule vertexShaderModule   = this->CreateShaderModule( vertexShaderCode );
	VkShaderModule fragmentShaderModule = this->CreateShaderModule( fragmentShaderCode );

// Vertex shader stage create info:

	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};

	vertexShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = vertexShaderModule;
	vertexShaderStageInfo.pName  = "main";

// Fragment shader stage create info:

	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};

	fragmentShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = fragmentShaderModule;
	fragmentShaderStageInfo.pName  = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

	VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount   = 1;
	vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};

	inputAssemblyInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

//	VkViewport viewport{};

	Viewport.x        = 0.0f;
	Viewport.y        = 0.0f;
	Viewport.width    = static_cast<float>( SwapChainExtent.width );
	Viewport.height   = static_cast<float>( SwapChainExtent.height );
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;

//	VkRect2D scissor{};

	Scissor.offset = { 0, 0 };
	Scissor.extent = SwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportStateInfo{};

	viewportStateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pViewports    = &Viewport;
	viewportStateInfo.scissorCount  = 1;
	viewportStateInfo.pScissors     = &Scissor;

	VkPipelineRasterizationStateCreateInfo rasterizationInfo{};

	rasterizationInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationInfo.depthClampEnable        = VK_FALSE;
	rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationInfo.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterizationInfo.lineWidth               = 1.0f;
	rasterizationInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterizationInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationInfo.depthBiasEnable         = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampleInfo{};

	multisampleInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo.sampleShadingEnable   = VK_FALSE;
	multisampleInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;

//	VkPipelineDepthStencilStateCreateInfo

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};

	colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable         = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};

	colorBlendingInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingInfo.logicOpEnable     = VK_FALSE;
	colorBlendingInfo.logicOp           = VK_LOGIC_OP_COPY;      // Optional.
	colorBlendingInfo.attachmentCount   = 1;
	colorBlendingInfo.pAttachments      = &colorBlendAttachment;
	colorBlendingInfo.blendConstants[0] = 0.0f;                  // Optional.
	colorBlendingInfo.blendConstants[1] = 0.0f;                  // Optional.
	colorBlendingInfo.blendConstants[2] = 0.0f;                  // Optional.
	colorBlendingInfo.blendConstants[3] = 0.0f;                  // Optional.

	VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};

	dynamicStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.pNext             = nullptr;
	dynamicStateCreateInfo.flags             = 0;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates    = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

	pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts    = &DescriptorSetLayout;
	
	if ( vkCreatePipelineLayout( Device, &pipelineLayoutInfo, nullptr, &PipelineLayout ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateGraphicsPipeline: Failed to create pipeline layout!" );
	}

// Create pipeline:

	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount          = 2;
	pipelineInfo.pStages             = shaderStages;
	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState      = &viewportStateInfo;
	pipelineInfo.pRasterizationState = &rasterizationInfo;
	pipelineInfo.pMultisampleState   = &multisampleInfo;
	pipelineInfo.pColorBlendState    = &colorBlendingInfo;
	pipelineInfo.pDynamicState       = &dynamicStateCreateInfo;
	pipelineInfo.layout              = PipelineLayout;
	pipelineInfo.renderPass          = RenderPass;
	pipelineInfo.subpass             = 0;
	pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;     // Optional.

	if ( vkCreateGraphicsPipelines( Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &GraphicsPipeline ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateGraphicsPipeline: Failed to create graphics pipeline!" );
	}

// Delete shader modules:

	vkDestroyShaderModule( Device, fragmentShaderModule, nullptr );
	vkDestroyShaderModule( Device, vertexShaderModule  , nullptr );
}

void Application::CreateFrameBuffers()
{
	SwapChainFramebuffers.resize( SwapChainImageViews.size() );

	for (size_t i = 0u; i < SwapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { SwapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};

		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments    = attachments;
		framebufferInfo.width           = SwapChainExtent.width;
		framebufferInfo.height          = SwapChainExtent.height;
		framebufferInfo.layers          = 1;

		if ( vkCreateFramebuffer( Device, &framebufferInfo, nullptr, &SwapChainFramebuffers[i] ) != VK_SUCCESS )
		{
			throw std::runtime_error( "ERROR::Application::CreateFrameBuffers: Failed to create framebuffer!" );
		}
	}
}

void Application::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = this->FindQueueFamilies( PhysicalDevice );

	VkCommandPoolCreateInfo commandPoolInfo{};

	commandPoolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

	if ( vkCreateCommandPool( Device, &commandPoolInfo, nullptr, &CommandPool ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateCommandPool: Failed to create command pool!" );
	}
}

void Application::CreateTextureImage()
{
	int texWidth;
	int texHeight;
	int texChannels;

	stbi_uc* pixels = stbi_load("D:/VULKAN_3D_TEXTURES/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
	{
		throw std::runtime_error("ERROR::Application::CreateTextureImage: Failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	this->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(Device, stagingBufferMemory, 0, imageSize, 0, &data);

	memcpy_s(data, static_cast<size_t>(imageSize), pixels, static_cast<size_t>(imageSize));

	vkUnmapMemory(Device, stagingBufferMemory);

	stbi_image_free(pixels);

	this->CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, TextureImage, TextureImageMemory);

	this->TransitionImageLayout(TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	this->CopyBufferToImage(stagingBuffer, TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	this->TransitionImageLayout(TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(Device, stagingBuffer, nullptr);
	vkFreeMemory(Device, stagingBufferMemory, nullptr);
}

void Application::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo{};

	imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType     = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width  = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth  = 1;
	imageInfo.mipLevels     = 1;
	imageInfo.arrayLayers   = 1;
	imageInfo.format        = format;
	imageInfo.tiling        = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage         = usage;
	imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(Device, &imageInfo, nullptr, &TextureImage) != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR::Application::CreateImage: Failed to create image!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(Device, TextureImage, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo{};

	allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize  = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = this->FindMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(Device, &allocateInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR::Application::CreateImage: Failed to allocate image memory!");
	}

	vkBindImageMemory(Device, image, imageMemory, 0);
}

void Application::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = this->BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};

	barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout                       = oldLayout;
	barrier.newLayout                       = newLayout;
	barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	barrier.image                           = image;
	barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("ERROR::Application::TransitionImageLayout: Unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, 
		                 sourceStage, 
		                 destinationStage, 
		                 0, 
		                 0, nullptr, 
		                 0, nullptr, 
		                 1, &barrier);

	this->EndSingleTimeCommands(commandBuffer);
}

void Application::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = this->BeginSingleTimeCommands();

	VkBufferImageCopy region{};

	region.bufferOffset                    = 0;
	region.bufferRowLength                 = 0;
	region.bufferImageHeight               = 0;	
	region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;
	region.imageOffset                     = { 0, 0, 0 };
	region.imageExtent                     = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	// PREPARING THE TEXTURE IMAGE...

	this->EndSingleTimeCommands(commandBuffer);
}

void Application::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(Vertices[0]) * Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(Device, stagingBufferMemory, 0, bufferSize, 0, &data);

	memcpy_s(data, static_cast<size_t>(bufferSize), Vertices.data(), static_cast<size_t>(bufferSize));

	vkUnmapMemory(Device, stagingBufferMemory);

	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory);

	this->CopyBuffer(stagingBuffer, VertexBuffer, bufferSize);

	vkDestroyBuffer(Device, stagingBuffer, nullptr);
	vkFreeMemory(Device, stagingBufferMemory, nullptr);
}

void Application::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(Indices[0]) * Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(Device, stagingBufferMemory, 0, bufferSize, 0, &data);

	memcpy_s(data, static_cast<size_t>(bufferSize), Indices.data(), static_cast<size_t>(bufferSize));

	vkUnmapMemory(Device, stagingBufferMemory);

	this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, IndexBufferMemory);

	this->CopyBuffer(stagingBuffer, IndexBuffer, bufferSize);

	vkDestroyBuffer(Device, stagingBuffer, nullptr);
	vkFreeMemory(Device, stagingBufferMemory, nullptr);
}

void Application::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBuffersMemory);

	UniformBuffers.resize(SwapChainImages.size());
	UniformBuffersMemory.resize(SwapChainImages.size());

	for (size_t i = 0u; i < SwapChainImages.size(); i++)
	{
		this->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffers[i], UniformBuffersMemory[i]);
	}
}

void Application::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};

	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR::Application::CreateBuffer: Failed to create buffer!");
	}

	VkMemoryRequirements memoryRequirements;

	vkGetBufferMemoryRequirements(Device, buffer, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo{};

	allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize  = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = this->FindMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(Device, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR::Application::CreateBuffer: Failed to allocate buffer memory!");
	}

	vkBindBufferMemory(Device, buffer, bufferMemory, 0);
}

VkCommandBuffer Application::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocateInfo{};

	allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool        = CommandPool;
	allocateInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(Device, &allocateInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Application::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};

	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;

	vkQueueSubmit(GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GraphicsQueue);

	vkFreeCommandBuffers(Device, CommandPool, 1, &commandBuffer);
}

void Application::CopyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = this->BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	
	copyRegion.size = size;
	
	vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion);

	this->EndSingleTimeCommands(commandBuffer);
}

uint32_t Application::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;

	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &memoryProperties);

	for (uint32_t i = 0u; i < memoryProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("ERROR::Application::CreateVertexBuffer: Failed to find suitable memory type!");
}

void Application::CreateCommandBuffers()
{
	CommandBuffers.resize( SwapChainFramebuffers.size() );

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};

	commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool        = CommandPool;
	commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>( CommandBuffers.size() );

	if ( vkAllocateCommandBuffers( Device, &commandBufferAllocateInfo, CommandBuffers.data() ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateCommandBuffers: Failed to create command buffers!" );
	}

	for ( size_t i = 0u; i < CommandBuffers.size(); i++ )
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo{};

		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if ( vkBeginCommandBuffer( CommandBuffers[i], &commandBufferBeginInfo ) != VK_SUCCESS )
		{
			throw std::runtime_error( "ERROR::Application::CreateCommandBuffers: Failed to begin recording command buffer!" );
		}

		VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

		VkRenderPassBeginInfo renderPassInfo{};

		renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass        = RenderPass;
		renderPassInfo.framebuffer       = SwapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = SwapChainExtent;
		renderPassInfo.clearValueCount   = 1;
		renderPassInfo.pClearValues      = &clearColor;

		vkCmdBeginRenderPass( CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
		
		vkCmdBindPipeline( CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline );

		VkBuffer vertexBuffers[] = {VertexBuffer};

		VkDeviceSize offsets[] = {0};

		vkCmdSetViewport(CommandBuffers[i], 0, 1, &Viewport);

		vkCmdSetScissor(CommandBuffers[i], 0, 1, &Scissor);

		vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(CommandBuffers[i], IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSets[i], 0, nullptr);

		vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass( CommandBuffers[i] );

		if ( vkEndCommandBuffer( CommandBuffers[i] ) != VK_SUCCESS )
		{
			throw std::runtime_error( "ERROR::Appliaction::CreateCommandBuffers: Failed to record command buffer!" );
		}
	}
}

void Application::CreateSyncObjects()
{
	ImageAvailableSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	RenderFinishedSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	InFlightFences.resize( MAX_FRAMES_IN_FLIGHT );
	ImagesInFlight.resize( SwapChainImages.size(), VK_NULL_HANDLE );

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for ( size_t i = 0u; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		if ( vkCreateSemaphore( Device, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i] ) != VK_SUCCESS )
		{
			throw std::runtime_error( "ERROR::Application::CreateSyncObjects: Failed to create ImageAvailableSemaphore!" );
		}

		if ( vkCreateSemaphore( Device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i] ) != VK_SUCCESS )
		{
			throw std::runtime_error( "ERROR::Application::CreateSyncObjects: Failed to create RenderFinishedSemaphore!" );
		}

		if ( vkCreateFence( Device, &fenceInfo, nullptr, &InFlightFences[i] ) != VK_SUCCESS )
		{
			throw std::runtime_error( "ERROR::Application::CreateSyncObjects: Failed to create fence!" );
		}
	}
}

void Application::UpdateUniformBuffers(uint32_t currentImage)
{
	static std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject uniformBufferObject{};

	uniformBufferObject.model      = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	uniformBufferObject.view       = glm::lookAt(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	uniformBufferObject.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SwapChainExtent.width) / static_cast<float>(SwapChainExtent.height), 0.1f, 10.0f);

	uniformBufferObject.projection[1][1] *= -1; // Y-axis are flipped upside down in GLM by default, so flip it back for Vulkan by multiplying scale by -1.

	void* data;

	vkMapMemory(Device, UniformBuffersMemory[currentImage], 0, sizeof(uniformBufferObject), 0, &data);

	memcpy_s(data, sizeof(uniformBufferObject), &uniformBufferObject, sizeof(uniformBufferObject));

	vkUnmapMemory(Device, UniformBuffersMemory[currentImage]);
}

void Application::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize{};

	poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(SwapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo{};

	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes    = &poolSize;
	poolInfo.maxSets       = static_cast<uint32_t>(SwapChainImages.size());

	if (vkCreateDescriptorPool(Device, &poolInfo, nullptr, &DescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR::Application::CreateDescriptorPool: Failed to create descriptor pool!");
	}
}

void Application::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(SwapChainImages.size(), DescriptorSetLayout);

	VkDescriptorSetAllocateInfo allocateInfo{};

	allocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool     = DescriptorPool;
	allocateInfo.descriptorSetCount = static_cast<uint32_t>(SwapChainImages.size());
	allocateInfo.pSetLayouts        = layouts.data();

	DescriptorSets.resize(SwapChainImages.size());

	if (vkAllocateDescriptorSets(Device, &allocateInfo, DescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("ERROR::Application::CreateDescriptorSets: Failed to allocate descriptor sets!");
	}

	for (size_t i = 0u; i < SwapChainImages.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo{};

		bufferInfo.buffer = UniformBuffers[i];
		bufferInfo.offset = 0;
	//	bufferInfo.range  = sizeof(UniformBufferObject);
		bufferInfo.range  = VK_WHOLE_SIZE;

		VkWriteDescriptorSet descriptorWrite{};

		descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet           = DescriptorSets[i];
		descriptorWrite.dstBinding       = 0;
		descriptorWrite.dstArrayElement  = 0;
		descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount  = 1;
		descriptorWrite.pBufferInfo      = &bufferInfo;
		descriptorWrite.pImageInfo       = nullptr; // Optional.
		descriptorWrite.pTexelBufferView = nullptr; // Optional.

		vkUpdateDescriptorSets(Device, 1, &descriptorWrite, 0, nullptr);
	}
}

void Application::DrawFrame()
{
	vkWaitForFences( Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX );

	uint32_t imageIndex;

	VkResult result = vkAcquireNextImageKHR( Device, SwapChain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex );

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		this->RecreateSwapChain();

		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("ERROR::Application::DrawFrane: Failed to acquire swap chain image!");
	}

	this->UpdateUniformBuffers(imageIndex);

// Chack if a previous frame is using this image ( i. e. there is its fence to wait on ):

	if ( ImagesInFlight[imageIndex] != VK_NULL_HANDLE )
	{
		vkWaitForFences( Device, 1, &ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX );
	}

// Mark the image as now being in use bu this frame:

	ImagesInFlight[imageIndex] = InFlightFences[CurrentFrame];

	VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[CurrentFrame] };

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[CurrentFrame] };

	VkSubmitInfo submitInfo{};

	submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount   = 1;
	submitInfo.pWaitSemaphores      = waitSemaphores;
	submitInfo.pWaitDstStageMask    = waitStages;
	submitInfo.commandBufferCount   = 1;
	submitInfo.pCommandBuffers      = &CommandBuffers[imageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = signalSemaphores;

	vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);

	if ( vkQueueSubmit( GraphicsQueue, 1, &submitInfo, InFlightFences[CurrentFrame] ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::DrawFrame: Failed to submit draw command buffer!" );
	}

	VkSwapchainKHR swapChains[] = { SwapChain };

	VkPresentInfoKHR presentInfo{};

	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;
	presentInfo.swapchainCount     = 1;
	presentInfo.pSwapchains        = swapChains;
	presentInfo.pImageIndices      = &imageIndex;

	result = vkQueuePresentKHR( PresentQueue, &presentInfo );

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || FrameBufferResized)
	{
		FrameBufferResized = false;

		this->RecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image!");
	}

	CurrentFrame = ( CurrentFrame + 1u ) % MAX_FRAMES_IN_FLIGHT ;
}

VkShaderModule Application::CreateShaderModule( const std::vector<char>& code )
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo{};

	shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = code.size();
	shaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t*>( code.data() );

	VkShaderModule shaderModule;

	if ( vkCreateShaderModule( Device, &shaderModuleCreateInfo, nullptr, &shaderModule ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateShaderModule: Failed to create shader module!" );
	}

	return shaderModule;
}

VkSurfaceFormatKHR Application::ChooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats )
{
	for ( const auto& availableFormat : availableFormats )
	{
		if ( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
		{
			return availableFormat;
		}		
	}

	return availableFormats[0];
}

VkPresentModeKHR Application::ChooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes )
{
	for ( const auto& availablePresentMode : availablePresentModes )
	{
		if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Application::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	if ( surfaceCapabilities.currentExtent.width != UINT32_MAX )
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{
		int32_t width;
		int32_t height;
		
		glfwGetFramebufferSize( Window, &width, &height );

		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>( width  ),
			static_cast<uint32_t>( height )
		};

		actualExtent.width  = std::clamp( actualExtent.width , surfaceCapabilities.minImageExtent.width , surfaceCapabilities.maxImageExtent.width  );
		actualExtent.height = std::clamp( actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height );

		return actualExtent;
	}
}

SwapChainSupportDetails Application::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, Surface, &details.Capabilities );

	uint32_t formatCount;

	vkGetPhysicalDeviceSurfaceFormatsKHR( device, Surface, &formatCount, nullptr );

	if ( formatCount != 0u )
	{
		details.Formats.resize( formatCount );

		vkGetPhysicalDeviceSurfaceFormatsKHR( device, Surface, &formatCount, details.Formats.data() );
	}

	uint32_t presentModeCount;

	vkGetPhysicalDeviceSurfacePresentModesKHR( device, Surface, &presentModeCount, nullptr );

	if ( presentModeCount != 0u )
	{
		details.PresentModes.resize( presentModeCount );

		vkGetPhysicalDeviceSurfacePresentModesKHR( device, Surface, &presentModeCount, details.PresentModes.data() );
	}

	return details;
}

bool Application::IsDeviceSuitable( VkPhysicalDevice device)
{
	QueueFamilyIndices indices = this->FindQueueFamilies( device );

	bool extensionsSupported = this->CheckDeviceExtensionSupport( device );

	bool swapChainAdequate = false;

	if ( extensionsSupported )
	{
		SwapChainSupportDetails swapChainSupport = this->QuerySwapChainSupport( device );
				
		swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

bool Application::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;

	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

	std::vector<VkExtensionProperties> availableExtensions( extensionCount );

	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );

	std::set<std::string> requiredExtensions( DeviceExtensions.begin(), DeviceExtensions.end() );

	for ( const auto& extension : availableExtensions )
	{
		requiredExtensions.erase( extension.extensionName );
	}

	return requiredExtensions.empty();
}

int32_t Application::RateDeviceSuitability( VkPhysicalDevice device )
{
// Check Queue Families:

	QueueFamilyIndices indices = this->FindQueueFamilies( device );

	if ( !indices.IsComplete() )
	{
		return 0;
	}

// Chech properties and features:

	VkPhysicalDeviceProperties deviceProperties;

	VkPhysicalDeviceFeatures   deviceFeatures;

	vkGetPhysicalDeviceProperties( device, &deviceProperties );	

	vkGetPhysicalDeviceFeatures( device, &deviceFeatures );

	int32_t score = 0;

// Application can't function without geometry shaders:

	if ( !deviceFeatures.geometryShader )
	{
		return 0;
	}

// Discrete GPUs have a significant performance advantage:

	if ( deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
	{
		score += 1000;
	}

// Maximum possible size of textures affects graphics quality:

	score += deviceProperties.limits.maxImageDimension2D;

	return score;
}

QueueFamilyIndices Application::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0u;

	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

	std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );

	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

// Find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT:

	uint32_t i = 0;

	for ( const auto& queueFamily : queueFamilies )
	{
		if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
		{
			indices.GraphicsFamily = i;
		}

		VkBool32 presentSupport = false;

		vkGetPhysicalDeviceSurfaceSupportKHR( device, i, Surface, &presentSupport );

		if ( presentSupport )
		{
			indices.PresentFamily = i;
		}

		if ( indices.IsComplete() )
		{
			break;
		}

		i++;
	}

	return indices;
}

void Application::CreateInstance()
{
// Check validation layers support:

	#pragma warning( disable : 6237) // Warning C6237(<zero> && <expression>) is always zero. <expression> is never evaluatedand might have side effects.

	if ( EnableValidationLayers && !this->CheckValidationLayerSupport() ) // C6237
	{
		throw std::runtime_error( "Validation layers requested, but not available!" );
	}

// Application info:

	VkApplicationInfo applicationInfo{};

	applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName   = "Hello Triangle!";
	applicationInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
	applicationInfo.pEngineName        = "No Engine";
	applicationInfo.engineVersion      = VK_MAKE_VERSION( 1, 0, 0 );
	applicationInfo.apiVersion         = VK_API_VERSION_1_0;

// Create info:

	std::vector<const char*> glfwExtensions = this->GetRequiredExtensions();

	VkInstanceCreateInfo instanceCreateInfo{};

	instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo        = &applicationInfo;
	instanceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>( glfwExtensions.size() );
	instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions.data();
	
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;

	if ( EnableValidationLayers )
	{
		this->PopulateDebugMessengerCreateInfo(debugMessengerCreateInfo);

		instanceCreateInfo.enabledLayerCount   = static_cast<uint32_t>( ValidationLayers.size() );
		instanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
		instanceCreateInfo.pNext               = ( VkDebugUtilsMessengerCreateInfoEXT* )&debugMessengerCreateInfo;
	}
	else
	{
		instanceCreateInfo.enabledLayerCount   = 0;
		instanceCreateInfo.pNext               = nullptr;
	}

// Create instance:

	#pragma warning( disable : 26812) // Warning C26812 The enum type 'VkResult' is unscoped.Prefer 'enum class' over 'enum' (Enum.3).

	if ( vkCreateInstance( &instanceCreateInfo, nullptr, &Instance ) != VK_SUCCESS ) // 26812
	{
		throw std::runtime_error( "ERROR::Application::CreateInstance: Failed to create instance!" );
	}

// Check for extensions support:

	uint32_t extensionCount = 0u;

	vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );

	std::vector<VkExtensionProperties> extensions( extensionCount );

	vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );

	std::cout << "\n";
	std::cout << "\tAvailable extensions" << " [" << extensionCount << "]: " << "\n\n";

	for ( size_t i = 0u; i < extensions.size(); i++ )
	{
		std::cout << "\t" << ( i + 1u ) << ".\t" << extensions[i].extensionName << " [version " << extensions[i].specVersion << "]" << "\n\n";
	}

	std::cout << std::endl;
}

void Application::PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo )
{
	debugMessengerCreateInfo = {};

	debugMessengerCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugMessengerCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugMessengerCreateInfo.pfnUserCallback = Application::DebugCallback;
	debugMessengerCreateInfo.pUserData       = nullptr; // Optional.
}

void Application::SetupDebugMessenger()
{
	if ( !EnableValidationLayers )
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};

	this->PopulateDebugMessengerCreateInfo( debugMessengerCreateInfo );

	if ( Application::CreateDebugUtilsMessengerEXT( Instance, &debugMessengerCreateInfo, nullptr, &DebugMessenger ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::SetupDebugMessenger: Failed to set up debug messenger!" );
	}
}

void Application::CreateSurface()
{
// Create window surface to render to:

	if ( glfwCreateWindowSurface( Instance, Window, nullptr, &Surface ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateSurface: Failed to create window surface!" );
	}
}

void Application::MainLoop()
{
	while ( !glfwWindowShouldClose( Window ) )
	{
		glfwPollEvents();

		this->DrawFrame();

	// Limit the amount of frames per second nearly to 60:

		std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
	}

	vkDeviceWaitIdle( Device );
}

void Application::CleanupSwapChain()
{
	for (size_t i = 0u; i < SwapChainFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(Device, SwapChainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(Device, CommandPool, static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());

	vkDestroyPipeline(Device, GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
	vkDestroyRenderPass(Device, RenderPass, nullptr);

	for (size_t i = 0u; i < SwapChainImageViews.size(); i++)
	{
		vkDestroyImageView(Device, SwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(Device, SwapChain, nullptr);

	for (size_t i = 0u; i < SwapChainImages.size(); i++)
	{
		vkDestroyBuffer(Device, UniformBuffers[i], nullptr);
		vkFreeMemory(Device, UniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
}

void Application::Cleanup()
{
	this->CleanupSwapChain();

	vkDestroyImage(Device, TextureImage, nullptr);
	vkFreeMemory(Device, TextureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(Device, DescriptorSetLayout, nullptr);

	vkDestroyBuffer(Device, VertexBuffer, nullptr);
	vkFreeMemory(Device, VertexBufferMemory, nullptr);

	vkDestroyBuffer(Device, IndexBuffer, nullptr);
	vkFreeMemory(Device, IndexBufferMemory, nullptr);

	for ( size_t i = 0u; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		vkDestroySemaphore( Device, RenderFinishedSemaphores[i], nullptr );
		vkDestroySemaphore( Device, ImageAvailableSemaphores[i], nullptr );
		vkDestroyFence( Device, InFlightFences[i], nullptr );
	}

	vkDestroyCommandPool( Device, CommandPool, nullptr );

	vkDestroyDevice(Device, nullptr);

	if (EnableValidationLayers)
	{
		Application::DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(Instance, Surface, nullptr);

	vkDestroyInstance(Instance, nullptr);

	glfwDestroyWindow(Window);

	glfwTerminate();
}

void Application::RecreateSwapChain()
{
	int width  = 0;
	int height = 0;

	glfwGetFramebufferSize(Window, &width, &height);

	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(Window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle( Device );

	this->CleanupSwapChain();


	this->CreateSwapChain();

	this->CreateImageViews();

	this->CreateRenderPass();

	this->CreateGraphicsPipeline();

	this->CreateFrameBuffers();

	this->CreateUniformBuffers();

	this->CreateDescriptorPool();

	this->CreateDescriptorSets();

	this->CreateCommandBuffers();


	ImagesInFlight.resize(SwapChainImages.size(), VK_NULL_HANDLE);
}

bool Application::CheckValidationLayerSupport()
{
// Check for all requested layers are available:

	uint32_t layerCount;

	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector<VkLayerProperties> availableLayers( layerCount );

	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

// Check if all of the layers are exist in the availableLayers:

	for ( const char* layerName : ValidationLayers )
	{
		bool layerFound = false;

		for ( const auto& layerProperties : availableLayers )
		{
			if ( std::strcmp( layerName, layerProperties.layerName ) == 0 )
			{
				layerFound = true;

				break;
			}
		}

		if ( !layerFound )
		{
			return false;
		}
	}

	return true;
}


// Private Accessors:

std::vector<const char*> Application::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0u;

	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	std::vector<const char*> extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

	if ( EnableValidationLayers )
	{
		extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Application::DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                                       VkDebugUtilsMessageTypeFlagsEXT messgeType, 
	                                                 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	                                                       void* pUserData )
{
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	if ( messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
	{
		// Message is important enough to show.
	}

	return VK_FALSE;
}


// Proxy Functions:

VkResult Application::CreateDebugUtilsMessengerEXT( VkInstance instance, 
	                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	                                          const VkAllocationCallbacks* pAllocator, 
	                                                VkDebugUtilsMessengerEXT* pDebugMessenger )
{
	auto debugFunction = ( PFN_vkCreateDebugUtilsMessengerEXT )vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );

	if ( debugFunction != nullptr  )
	{
		#pragma warning( disable : 26812) // Warning C26812 The enum type 'VkResult' is unscoped.Prefer 'enum class' over 'enum' (Enum.3).

		return debugFunction( instance, pCreateInfo, pAllocator, pDebugMessenger );
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Application::DestroyDebugUtilsMessengerEXT( VkInstance instance, 
	                                             VkDebugUtilsMessengerEXT debugMessenger, 
	                                       const VkAllocationCallbacks* pAllocator )
{
	auto debugFunction = ( PFN_vkDestroyDebugUtilsMessengerEXT )vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );

	if ( debugFunction != nullptr )
	{
		debugFunction( instance, debugMessenger, pAllocator );
	}
}

#pragma warning( pop ) // Vulkan SDK - End