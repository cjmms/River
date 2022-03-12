
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Shader.h"

#include <iostream>

Camera camera;


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera.setCameraKey(window);
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.updateCameraDirection((float)xpos, (float)ypos);
}

void mouseButton_callback(GLFWwindow* window, int button, int action, int mods)
{

    camera.SetMouseStatus(button, action);
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


    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    //Shader cubeShader("res/Shaders/basic.shader");
    Shader cubeShader("res/Shaders/basic.vs", "res/Shaders/basic.fs");


    //////////////////////////////////////////////

    float vertices[] = {
        -0.5f,  0.f, -0.5f,
         0.5f,  0.f, -0.5f,
         0.5f,  0.f,  0.5f,
         0.5f,  0.f,  0.5f,
        -0.5f,  0.f,  0.5f,
        -0.5f,  0.f, -0.5f,
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ///////////////////////////////////////////////




    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        camera.cameraUpdateFrameTime();


        ////////////////////////////////////////////////////

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // also draw the lamp object
        cubeShader.setMat4("projection", camera.getProjectionMatrix());
        cubeShader.setMat4("view", camera.getViewMatrix());

        glm::mat4 model(1.0f);

        // scale by 2
        cubeShader.setMat4("model", glm::scale(model, glm::vec3(2.0f)));

        cubeShader.Bind();
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        cubeShader.unBind();

        /////////////////////////////////////////////////////


         /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();


    }




    return 1;

}