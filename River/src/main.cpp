
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
#include "IBL.h"

// These 2 lines should only be defined in this file
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Camera camera;

// default screen size
const int window_width = 1280;
const int window_height = 960;

unsigned int checkerBoardTexture;
unsigned int waveTexture;


extern Setting setting;

static const char* RenderPassList[]{ 
                                     "Particle Velocity", 
                                     "Particle Amplitude", 
                                     "Velocity after Horizontal Pass",
                                     "Wave Height Map(Deviation)",
                                     "Wave Height Map(Gradient)",
                                     "Wave Mesh",
                                     "Obstacle Pos Map",
                                     "Obstacle Map",
                                     "Flow Velocity",
                                     "Flow Pressure"};

ObstacleMesh obstacleMesh;

bool rightMouseClick = false;

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


void RenderUI()
{
    // UI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Combo("Render Pass", &setting.seletectedRenderPass, RenderPassList, IM_ARRAYSIZE(RenderPassList));

    if (ImGui::TreeNode("Obstacle Creatation"))
    {
        ImGui::SliderInt("Obstacle Particle Size", &setting.obstacleParticleSize, 1, 10);
        ImGui::SliderFloat("Brush Size", &setting.brushSize, 1, 10.0);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Wave Particle"))
    {
        ImGui::SliderInt("Particle Size", &setting.particleSize, 1, 10);
        ImGui::SliderFloat("Time scale", &setting.timeScale, 0.0, 2.0);
        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Two Pass Wave Construction"))
    {
        ImGui::SliderInt("Blur Size", &setting.blurSize, 20, 80);

        ImGui::SliderFloat("dx scale", &setting.dx, 0.0f, 1.0f);
        ImGui::SliderFloat("dz scale", &setting.dz, 0.0f, 1.0f);
        ImGui::SliderFloat("Height Factor", &setting.heightFactor, 0.001, 1.0);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Tessellation"))
    {
        ImGui::SliderInt("Tessellation Factor", &setting.tessellationFactor, 1, 50);
        ImGui::Checkbox("Wireframe Mode", &setting.enableWireframeMode);
        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Lighting"))
    {
        ImGui::SliderFloat("water depth", &setting.waterDepth, 1, 20);
        ImGui::SliderFloat("extinction Coeff", &setting.extinctionCoeff, 0, 1);
        ImGui::SliderFloat("FoamTurbulance", &setting.FoamTurbulance, -0.1, 0.1);
        ImGui::Checkbox("Normal Map", &setting.enableNormalMap);
        ImGui::TreePop();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Rendering UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera.setCameraKey(window);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //camera.updateCameraDirection((float)xpos, (float)ypos);
    camera.updataRayDir((float)xpos, (float)ypos, window_width, window_height);
}


void mouseButton_callback(GLFWwindow* window, int button, int action, int mods)
{
    camera.SetMouseStatus(button, action);

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if      (GLFW_PRESS   == action) rightMouseClick = true;
            else if (GLFW_RELEASE == action) rightMouseClick = false;
        }
    }  
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
    glDepthFunc(GL_LEQUAL);

    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);

    ////////////////////////////////////////////////////////////
    // imgui setup

    setupGUI(window);

    checkerBoardTexture = loadTexture("res/checkerboard.jpg", false);

    waveTexture = loadTexture("res/wave.jpg", false);

    Skybox skybox;
    skybox.GenerateCubemap(LoadHDR("res/Arches_E_PineTree/Arches_E_PineTree_3k.hdr"));

    Skybox irradianceMap;
    irradianceMap.GenerateCubemap(LoadHDR("res/Arches_E_PineTree/Arches_E_PineTree_Env.hdr"));

    Render renderer;

    WaveParticleMesh waveParticleMesh{ 600 };


    // FBO's:

    FBO waveParticleFBO{ window_width , window_height};

    // used as output for horizontal blur, multi target rendering
    FBO f12345v{ window_width , window_height };
    f12345v.AddTarget(window_width, window_height);

    // used as output for vertical blur, multi target rendering
    FBO deviationGradient{ window_width , window_height };
    deviationGradient.AddTarget(window_width, window_height);

    FBO waveMesh{ window_width , window_height };

    FBO createObstacleFBO{ window_width , window_height };  // I'm using this as the obstacles fbo right now, no idea if that's right. --jarred
    FBO obstacleFBO{ window_width , window_height };

    constexpr glm::ivec2 flowMapScale = { 512, 512 };
    renderer.fluidGridScale = flowMapScale;
    renderer.fluidInvScale = {1.0f/(float)flowMapScale.x, 1.0f/(float)flowMapScale.y};

    //FBO* fboVelPress1 = new FBO(flowMapScale.x, flowMapScale.y);
    //fboVelPress1->AddTarget(flowMapScale.x, flowMapScale.y);
    PingPong flowVelocityPressure(flowMapScale.x, flowMapScale.y );
    flowVelocityPressure.AddTargetToBoth(flowMapScale.x, flowMapScale.y);
    PingPong flowDivergence(flowMapScale.x, flowMapScale.y);

    // renderer.InitFlowMapBindings(createObstacleFBO, flowVelocityPressure, flowDivergence);


    glViewport(0, 0, window_width, window_height);

    while (!glfwWindowShouldClose(window))
    {
        // UPDATING //

        processInput(window);

        camera.cameraUpdateFrameTime();

        if (rightMouseClick) {

            float t = -1;

            RayPlaneIntersection(glm::vec3(0, 1, 0), glm::vec3(0), camera.worldRayDir, camera.worldRayOrigin, t);

            if (t > 0) obstacleMesh.AddObstacle(camera.worldRayOrigin + camera.worldRayDir * t);
        }

        // Flow map updates:
        // ...
        

        // step 1:
        // obstacle map creation
        renderer.RenderObstacleHeightMap(createObstacleFBO.ID);

        //std::cout << "t: " << t << std::endl;

        // RENDERING //

        renderer.RenderObstacles(createObstacleFBO.ID);
        
        renderer.ObstacleBlur(createObstacleFBO.ColorBuffer1, obstacleFBO.ID);

        // step 2:
        // wave map creation
        renderer.RenderWaveParticle(waveParticleMesh, waveParticleFBO.ID);

        renderer.HorizontalBlur(waveParticleFBO.ColorBuffer1, f12345v.ID);

        renderer.VerticalBlur(f12345v.ColorBuffer1, f12345v.ColorBuffer2, deviationGradient.ID);

        // step 3:
        // render obstacles
        glEnable(GL_CULL_FACE);
        renderer.RenderObstacles(obstacleFBO.ColorBuffer1, waveMesh.ID);

        // step 4: 
        // render wave mesh

        renderer.RenderWaveMesh(irradianceMap.ID(), skybox.ID(), 
            deviationGradient.ColorBuffer1, deviationGradient.ColorBuffer2, waveMesh.ID);

        glDisable(GL_CULL_FACE);

        // render skybox into the same FBO contains wave mesh
        skybox.Render(waveMesh.ID);

     
        
        renderer.DebugDraw(
            waveParticleFBO.ColorBuffer1,
            f12345v.ColorBuffer1, f12345v.ColorBuffer2, 
            deviationGradient.ColorBuffer1, deviationGradient.ColorBuffer2,
            waveMesh.ColorBuffer1, 
            createObstacleFBO.ColorBuffer1,
            obstacleFBO.ColorBuffer1);
            

        ////////////////////////////////////////////////////
        RenderUI();

        glfwSwapBuffers(window);

        glfwPollEvents();   /* Poll for and process events */
    }

    destroyGUI();

    return 1;
}
