
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

#include <iostream>

Camera camera;


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera.setCameraKey(window);
}





int main()
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "GLFW failed to init." << std::endl;
        return false;
    }

    // default screen size
    int window_width = 1280;
    int window_height = 960;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "River", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cout << "GLFW window failed to init." << std::endl;
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init error" << std::endl;
        return false;
    }



    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        camera.cameraUpdateFrameTime();



         /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();


    }




    return 1;

}