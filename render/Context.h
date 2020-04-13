#include <vulkan/vulkan.h>

struct VkhContext
	{
		VkInstance				instance;
		VkhSurface				surface;
		VkhPhysicalDevice		gpu;
		VkDevice				device;
		VkhDeviceQueues			deviceQueues;
		VkhSwapChain			swapChain;
		VkCommandPool			gfxCommandPool;
		VkCommandPool			transferCommandPool;
		VkCommandPool			presentCommandPool;
		std::vector<VkFence>	frameFences;
		VkSemaphore				imageAvailableSemaphore;
		VkSemaphore				renderFinishedSemaphore;
		AllocatorInterface		allocator;
		VkDescriptorPool		descriptorPool;

		//hate this being here, but if material can create itself
		//this is where it has to live, otherwise rendering has to return
		//a vulkan type from a function and that means adding vkh.h to renderer.h
		VkRenderPass		mainRenderPass;
	};