#pragma once

#include "libs.hpp"
#include "engine.hpp"
#include "scene.hpp"

class App {
    private:
        ASH::Engine* m_engine;
        GLFWwindow *m_window;
        Scene* m_scene;

        double m_lastTime, m_currentTime;
        int m_frameCount = 0;
        double m_frameTime;

        void makeGlfwWindow(int width, int height);

        void calculateFrameRate();

    public:
        App(int width, int height);
        ~App();

        void run();
};