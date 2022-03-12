
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Shader.h"

#include <iostream>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

Camera camera;
int tessellationFactor = 1;


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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

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


   // glfwSetCursorPosCallback(window, mouse_callback);
   // glfwSetMouseButtonCallback(window, mouseButton_callback);

   // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ////////////////////////////////////////////////////////////
    // imgui setup

    // imgui
    const char* glsl_version = "#version 450";

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    /////////////////////////////////////////////////////////////

    //Shader cubeShader("res/Shaders/basic.vs", "res/Shaders/basic.fs");
    Shader planeShader( "res/Shaders/basic.vs",
                        "res/Shaders/basic.tcs",
                        "res/Shaders/basic.tes",
                        "res/Shaders/basic.fs"  );


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

    glPatchParameteri(GL_PATCH_VERTICES, 3);

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
        planeShader.setMat4("projection", camera.getProjectionMatrix());
        planeShader.setMat4("view", camera.getViewMatrix());

        glm::mat4 model(1.0f);

        // scale by 2
        planeShader.setMat4("model", glm::scale(model, glm::vec3(2.0f)));

        planeShader.setInt("tessellationFactor", tessellationFactor);

        planeShader.Bind();
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_PATCHES, 0, 6);
        planeShader.unBind();

        /////////////////////////////////////////////////////

        ////////////////////////////////////////////////////
        // UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float f;

        ImGui::Text("Hello, world %d", 123);
        //if (ImGui::Button("Save"))
          //  MySaveFunction();
        //ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
        ImGui::SliderInt("Tessellation Factor", &tessellationFactor, 1, 20);
        

        // Rendering UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /////////////////////////////////////////////////////



         /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }



    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return 1;
}