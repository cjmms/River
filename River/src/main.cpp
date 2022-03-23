
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Shader.h"

#include <iostream>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include "Mesh.h"
#include "Render.h"

// These 2 lines should only be defined in this file
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Camera camera;
int tessellationFactor = 37;
bool enableWireframeMode = false;
float heightFactor = 1.0f;

// default screen size
const int window_width = 1280;
const int window_height = 960;



unsigned int loadTexture(char const* path, bool gamma)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << path << " loaded successfully." << std::endl;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}




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


void setupGUI(GLFWwindow* window)
{
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
}


void destroyGUI()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

    glEnable(GL_DEPTH_TEST);

    /*
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    */
    ////////////////////////////////////////////////////////////
    // imgui setup

    setupGUI(window);

    /*
    /////////////////////////////////////////////////////////////

    //Shader cubeShader("res/Shaders/basic.vs", "res/Shaders/basic.fs");
    Shader planeShader( "res/Shaders/basic.vs",
                        "res/Shaders/basic.tcs",
                        "res/Shaders/basic.tes",
                        "res/Shaders/basic.fs"  );

    unsigned int waveParticleTexture = loadTexture("res/wave.jpg", false);
    planeShader.setTexture("waveParticle", waveParticleTexture);

    //////////////////////////////////////////////

    float vertices[] = {
        -0.5f,  0.f, -0.5f,  0, 1,
         0.5f,  0.f, -0.5f,  1, 1,
         0.5f,  0.f,  0.5f,  1, 0,

         0.5f,  0.f,  0.5f,  1, 0,
        -0.5f,  0.f,  0.5f,  0, 0,
        -0.5f,  0.f, -0.5f,  0, 1
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // tex coord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //glPatchParameteri(GL_PATCH_VERTICES, 3);
    */
    ///////////////////////////////////////////////

    Render renderer;

    WaveParticleMesh waveParticleMesh{ 200 };

    FBO waveParticleFBO{ window_width , window_height};


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //camera.cameraUpdateFrameTime();

        //if (enableWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ////////////////////////////////////////////////////

        /*
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
        planeShader.setFloat("heightFactor", heightFactor);



        planeShader.Bind();
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_PATCHES, 0, 6);
        planeShader.unBind();
        */

        renderer.RenderWaveParticle(waveParticleMesh, 1, waveParticleFBO.ID);

        renderer.HorizontalBlur(waveParticleFBO.ColorBuffer, 0);

        /////////////////////////////////////////////////////

        ////////////////////////////////////////////////////
        // UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::SliderInt("Tessellation Factor", &tessellationFactor, 1, 50);
        ImGui::SliderFloat("Height Factor", &heightFactor, 0.0, 2.0);
        ImGui::Checkbox("Wireframe Mode", &enableWireframeMode);

        // Rendering UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /////////////////////////////////////////////////////



         /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    destroyGUI();

    return 1;
}