#include "../PrecompiledHeaders/stdafx.h"
#include "Application.h"

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin

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
	glfwWindowHint( GLFW_RESIZABLE , GLFW_FALSE  );

	Window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan3D", nullptr, nullptr );
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

	this->CreateGraphicsPipeline();

	this->CreateFrameBuffers();

	this->CreateCommandPool();

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

//// Use an ordered map to automatically sort candidates by increasing score:
//
//	std::multimap<int32_t, VkPhysicalDevice> candidates;
//
//	for ( const auto& device : devices )
//	{
//		int32_t score = this->RateDeviceSuitability( device );
//
//		candidates.insert( std::make_pair( score, device ) );
//	}
//
//// Check is the best candidate is suitable at all:
//
//	if ( candidates.rbegin()->first > 0 )
//	{
//		PhysicalDevice = candidates.rbegin()->second;
//	}
//	else
//	{
//		throw std::runtime_error( "ERROR::Application::PickPhysicalDevice: Failed to find a suitable GPU!" );
//	}
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
	swapChainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;

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

void Application::CreateGraphicsPipeline()
{
// ------------------------------ TEST SETTINGS ------------------------------
	n_ShaderManager.SetDirectoryGLSL( "D:/VULKAN_3D_SHADERS/GLSL/" );
	n_ShaderManager.SetDirectorySPV(  "D:/VULKAN_3D_SHADERS/SPV/"  );
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

	std::vector<char> vertexShaderCode   = n_ShaderManager.LoadSPV( "vert" );
	std::vector<char> fragmentShaderCode = n_ShaderManager.LoadSPV( "frag" );

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

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount   = 0;
	vertexInputInfo.pVertexBindingDescriptions      = nullptr; // Optional.
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions    = nullptr; // Optional.

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};

	inputAssemblyInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};

	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>( SwapChainExtent.width );
	viewport.height   = static_cast<float>( SwapChainExtent.height );
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};

	scissor.offset = { 0, 0 };
	scissor.extent = SwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportStateInfo{};

	viewportStateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pViewports    = &viewport;
	viewportStateInfo.scissorCount  = 1;
	viewportStateInfo.pScissors     = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizationInfo{};

	rasterizationInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationInfo.depthClampEnable        = VK_FALSE;
	rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationInfo.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterizationInfo.lineWidth               = 1.0f;
	rasterizationInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterizationInfo.frontFace               = VK_FRONT_FACE_CLOCKWISE;
	rasterizationInfo.depthBiasEnable         = VK_FALSE;
	rasterizationInfo.depthBiasConstantFactor = 0.0f;                    // Optional.
	rasterizationInfo.depthBiasClamp          = 0.0f;                    // Optional.
	rasterizationInfo.depthBiasSlopeFactor    = 0.0f;                    // Optional.

	VkPipelineMultisampleStateCreateInfo multisampleInfo{};

	multisampleInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo.sampleShadingEnable   = VK_FALSE;
	multisampleInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisampleInfo.minSampleShading      = 1.0f;                  // Optional.
	multisampleInfo.pSampleMask           = nullptr;               // Optional.
	multisampleInfo.alphaToCoverageEnable = VK_FALSE;              // Optional.
	multisampleInfo.alphaToOneEnable      = VK_FALSE;              // Optional.

//	VkPipelineDepthStencilStateCreateInfo

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};

	colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable         = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;           // Optional.
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional.
	colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;                     // Optional.
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;                 // Optional.
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;                // Optional.
	colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;                     // Optional.

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

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };

	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};

	dynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = 2;
	dynamicStateInfo.pDynamicStates    = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

	pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount         = 0;       // Optional.
	pipelineLayoutInfo.pSetLayouts            = nullptr; // Optional.
	pipelineLayoutInfo.pushConstantRangeCount = 0;       // Optional.
	pipelineLayoutInfo.pPushConstantRanges    = nullptr; // Optional.
	
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
	pipelineInfo.pDepthStencilState  = nullptr;            // Optional.
	pipelineInfo.pColorBlendState    = &colorBlendingInfo;
	pipelineInfo.pDynamicState       = nullptr;            // Optional.
	pipelineInfo.layout              = PipelineLayout;
	pipelineInfo.renderPass          = RenderPass;
	pipelineInfo.subpass             = 0;
	pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;     // Optional.
	pipelineInfo.basePipelineIndex   = -1;                 // Optional.

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

		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = SwapChainExtent.width;
		framebufferInfo.height = SwapChainExtent.height;
		framebufferInfo.layers = 1;

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

	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
	commandPoolInfo.flags = 0; // Optional.

	if ( vkCreateCommandPool( Device, &commandPoolInfo, nullptr, &CommandPool ) != VK_SUCCESS )
	{
		throw std::runtime_error( "ERROR::Application::CreateCommandPool: Failed to create command pool!" );
	}
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

		commandBufferBeginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags            = 0;       // Optional.
		commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional.

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

		vkCmdDraw( CommandBuffers[i], 3, 1, 0, 0 );

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

void Application::DrawFrame()
{
	vkWaitForFences( Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX );

	uint32_t imageIndex;

	vkAcquireNextImageKHR( Device, SwapChain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex );

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
	presentInfo.pResults           = nullptr; // Optional.

	vkQueuePresentKHR( PresentQueue, &presentInfo );

	vkQueueWaitIdle( PresentQueue );

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
		instanceCreateInfo.enabledLayerCount   = 0u;
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
		float32 time = static_cast<float32>(glfwGetTime());

		t3d::TimeStep32  timeStep = time - m_LastFrameTime;

		m_LastFrameTime = time;

		glfwPollEvents();

		this->DrawFrame();

	// Limit the amount of frames per second nearly to 60:

		std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
	}

	vkDeviceWaitIdle( Device );
}

void Application::Cleanup()
{
	for ( size_t i = 0u; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		vkDestroySemaphore( Device, RenderFinishedSemaphores[i], nullptr );
		vkDestroySemaphore( Device, ImageAvailableSemaphores[i], nullptr );
		vkDestroyFence( Device, InFlightFences[i], nullptr );
	}

	vkDestroyCommandPool( Device, CommandPool, nullptr );

	for ( auto framebuffer : SwapChainFramebuffers )
	{
		vkDestroyFramebuffer( Device, framebuffer, nullptr );
	}

	vkDestroyPipeline( Device, GraphicsPipeline, nullptr );

	vkDestroyPipelineLayout( Device, PipelineLayout, nullptr );

	vkDestroyRenderPass( Device, RenderPass, nullptr );

	for ( auto imageView : SwapChainImageViews )
	{
		vkDestroyImageView( Device, imageView, nullptr );
	}

	vkDestroySwapchainKHR( Device, SwapChain, nullptr );

	vkDestroyDevice( Device, nullptr );

	if ( EnableValidationLayers )
	{
		Application::DestroyDebugUtilsMessengerEXT( Instance, DebugMessenger, nullptr );
	}

	vkDestroySurfaceKHR( Instance, Surface, nullptr );

	vkDestroyInstance( Instance, nullptr );

	glfwDestroyWindow( Window );

	glfwTerminate();
}

void Application::CleanupSwapChain()
{

}

void Application::RecreateSwaoChain()
{
	vkDeviceWaitIdle( Device );

	this->CreateSwapChain();

	this->CreateImageViews();

	this->CreateRenderPass();

	this->CreateGraphicsPipeline();

	this->CreateFrameBuffers();

	this->CreateCommandBuffers();
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