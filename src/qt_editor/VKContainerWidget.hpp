#pragma once

#include <QtWidgets>
#include <array>

#include "engine_api/api.h"
#include "CoreLoader.hpp"
#include "volk_imp.h"
class VKContainerWidget : public QWidget, public crevice::Container {
  Q_OBJECT
 private:
  /* data */
 public:
   VKContainerWidget(QWidget *parent = nullptr);
   ~VKContainerWidget();

  void createWindow(int width, int height, const char* title) {}
  void createSurface(VkInstance instance, VkSurfaceKHR* surface,
                     const VkAllocationCallbacks* allocator = nullptr) {
     volkInitialize();
     volkLoadInstance(instance);
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hinstance = GetModuleHandle(NULL);
    createInfo.hwnd = (HWND)this->winId();

    vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, surface);
    
#endif
  }
  void getFramebufferSize(int* width, int* height) {
    *width = this->width();
    *height = this->height();
  }
  const char** getVkRequiredInstanceExtensions(uint32_t* count);

  QPaintEngine *paintEngine() const override;
  QSize	sizeHint() const override;
};

