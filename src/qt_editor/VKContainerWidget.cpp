#include "VKContainerWidget.hpp"

std::array<const char*, 2> r_ext_a = {"VK_KHR_surface", "VK_KHR_win32_surface"};

VKContainerWidget::VKContainerWidget(QWidget* parent) : QWidget(parent) {}
VKContainerWidget::~VKContainerWidget() {
}

const char** VKContainerWidget::getVkRequiredInstanceExtensions(
    uint32_t* count) {
  *count = r_ext_a.size();
  return r_ext_a.data();
}

QPaintEngine* VKContainerWidget::paintEngine() const { return nullptr; }

QSize VKContainerWidget::sizeHint() const { return QSize(400, 400); }