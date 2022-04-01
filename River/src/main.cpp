
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

// default screen size
const int window_width = 1280;
const int window_height = 960;

unsigned int checkerBoardTexture;


extern Setting setting;

static const char* RenderPassList[]{ "Particle Velocity", 
                                     "Particle Amplitude", 
                                     "Velocity after Horizontal Pass",
                                     "Wave Height Map(Deviation)",
                                     "Wave Height Map(Gradient)",
                                     "Wave Mesh"};


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

    checkerBoardTexture = loadTexture("res/checkerboard.jpg", false);


    Render renderer;

    WaveParticleMesh waveParticleMesh{ 200 };

    FBO waveParticleFBO{ window_width , window_height};

    // used as output for horizontal blur, multi target rendering
    FBO f12345v{ window_width , window_height };
    f12345v.AddTarget(window_width, window_height);

    // used as output for vertical blur, multi target rendering
    FBO deviationGradient{ window_width , window_height };
    deviationGradient.AddTarget(window_width, window_height);

    FBO waveMesh{ window_width , window_height };

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        camera.cameraUpdateFrameTime();

        renderer.RenderWaveParticle(waveParticleMesh, waveParticleFBO.ID);

        renderer.HorizontalBlur(waveParticleFBO.ColorBuffer1, f12345v.ID);

        renderer.VerticalBlur(f12345v.ColorBuffer1, f12345v.ColorBuffer2, deviationGradient.ID);

        renderer.RenderWaveMesh(deviationGradient.ColorBuffer1, deviationGradient.ColorBuffer2, waveMesh.ID);

        //renderer.DrawQuad(deviationGradient.ColorBuffer2);

        renderer.DebugDraw(
            waveParticleFBO.ColorBuffer1,
            f12345v.ColorBuffer1, f12345v.ColorBuffer2, 
            deviationGradient.ColorBuffer1, deviationGradient.ColorBuffer2,
            waveMesh.ColorBuffer1);


        ////////////////////////////////////////////////////
        // UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Combo("Render Pass", &setting.seletectedRenderPass, RenderPassList, IM_ARRAYSIZE(RenderPassList));

        ImGui::SliderInt("Particle Size", &setting.particleSize, 1, 10);

        ImGui::SliderInt("Blur Size", &setting.blurSize, 20, 80);

        ImGui::SliderFloat("dx scale", &setting.dx, 0.0f, 1.0f);
        ImGui::SliderFloat("dz scale", &setting.dz, 0.0f, 1.0f);

        ImGui::SliderInt("Tessellation Factor", &setting.tessellationFactor, 1, 50);
        ImGui::SliderFloat("Height Factor", &setting.heightFactor, 0.001, 1.0);
        ImGui::Checkbox("Wireframe Mode", &setting.enableWireframeMode);



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