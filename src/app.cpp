#include "app.hpp"

App::App(int width, int height) {
    makeGlfwWindow(width, height);
    m_engine = new ASH::Engine(width, height, m_window);
    m_scene = new Scene();
}

App::~App() {
    delete m_engine;
    delete m_scene;
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void App::makeGlfwWindow(int width, int height) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if (!(m_window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr))) {
        std::cerr << "Failed to create window" << std::endl;
        exit(1);
    }
}

void App::run() {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        m_engine->render(m_scene);
        calculateFrameRate();
    }
}

void App::calculateFrameRate() {
    m_currentTime = glfwGetTime();
    double delta = m_currentTime - m_lastTime;

    if (delta >= 1.0) {
        int framerate = std::max(1, int(m_frameCount / delta));
        std::stringstream title;
        title << "Vulkan (" << framerate << " fps)";
        glfwSetWindowTitle(m_window, title.str().c_str());
        m_lastTime = m_currentTime;
        m_frameCount = -1;
        m_frameTime = double(1000.0 / framerate);
    }

    ++m_frameCount;
}

