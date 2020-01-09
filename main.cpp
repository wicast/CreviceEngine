#include "HelloTriangleApplication.h"

int main() {
    HelloTriangleApplication app = {};

    try {
        app.setEventHandling();
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}