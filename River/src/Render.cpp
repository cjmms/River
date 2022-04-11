#include "Render.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"

extern Camera camera;

Setting setting;

extern unsigned int checkerBoardTexture;
extern unsigned int waveTexture;

const unsigned int NUM_PATCH_PTS = 4;

extern ObstacleMesh obstacleMesh;




// model matrix for plane
glm::mat4 model(1.0f);

FBO::FBO(unsigned int width, unsigned int height)
{
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);

    // generate FBO color attachment, bind to current FBO
    glGenTextures(1, &ColorBuffer1);
    glBindTexture(GL_TEXTURE_2D, ColorBuffer1);  // bind texture

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);        // unbind texture

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer1, 0);

    // depth
    glGenTextures(1, &DepthBuffer);
    glBindTexture(GL_TEXTURE_2D, DepthBuffer);  // bind texture

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);        // unbind texture

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);


    // check if framebuffer created successfullly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void FBO::AddTarget(unsigned int width, unsigned int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, ID);

    // generate FBO color attachment, bind to current FBO
    glGenTextures(1, &ColorBuffer2);
    glBindTexture(GL_TEXTURE_2D, ColorBuffer2);  // bind texture

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);        // unbind texture

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ColorBuffer2, 0);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    // check if framebuffer created successfullly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



Render::Render()
{
    
    // init quad
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    // init quad patch
    float quadPatchVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords    
        -1.0f, -1.0f,     0.0f, 0.0f,  
        1.0f, -1.0f,     1.0f, 0.0f,
        -1.0f,  1.0f,     0.0f, 1.0f,    
         1.0f,  1.0f,     1.0f, 1.0f
    };

    unsigned int quadPatchVBO;

    glGenBuffers(1, &quadPatchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadPatchVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadPatchVertices), &quadPatchVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quadPatchVAO);
    glBindVertexArray(quadPatchVAO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

    glBindVertexArray(0);

    // model matrix for river plane
    model = glm::scale(model, glm::vec3(2.0f));
    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));


}



void Render::RenderWaveParticle(WaveParticleMesh& waveParticleMesh, unsigned int fbo)
{
	glPointSize(setting.particleSize);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    waveParticleShader.setFloat("time", glfwGetTime());
    waveParticleShader.setFloat("timeScale", setting.timeScale);
    waveParticleShader.setFloat("heightScale", setting.heightFactor);

	waveParticleShader.Bind();

	glBindVertexArray(waveParticleMesh.VAO);
	glDrawArrays(GL_POINTS, 0, waveParticleMesh.size);

	waveParticleShader.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set point size to default
    glPointSize(1);
}



void Render::ObstacleBlur(unsigned int ObstaclePosMap, unsigned int fbo)
{
    // horizontal blur
    glBindFramebuffer(GL_FRAMEBUFFER, obstacleBlurFBO.ID);

    // clear buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    obstacleBlurHShader.setTexture("obstaclePosMap", ObstaclePosMap);

    obstacleBlurHShader.Bind();
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    obstacleBlurHShader.unBind();

    // vertical blur
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // clear buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // pass horizontal blured result
    obstacleBlurVShader.setTexture("horiBlurMap", obstacleBlurFBO.ColorBuffer1);

    obstacleBlurVShader.Bind();
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    obstacleBlurVShader.unBind();

    // vertical blur
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Render::HorizontalBlur(unsigned int inputTexture, unsigned int fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // pass input texture
    horozontalBlur.setTexture("inputTexture", inputTexture);

    // blur radius
    horozontalBlur.setInt("blurRadius", setting.blurSize);

    // bind shader
    horozontalBlur.Bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    // unbind shader
    horozontalBlur.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Render::VerticalBlur(unsigned int f123, unsigned int f45v, unsigned int fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // pass input texture
    verticalBlur.setTexture("tex1", f123);
    verticalBlur.setTexture("tex2", f45v);

    // blur radius
    verticalBlur.setInt("blurRadius", setting.blurSize);

    verticalBlur.setFloat("dxScale", setting.dx);
    verticalBlur.setFloat("dzScale", setting.dz);

    verticalBlur.setFloat("heightFactor", setting.heightFactor);

    verticalBlur.Bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    verticalBlur.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Render::DrawQuad(unsigned int inputTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // pass input texture
    verticalBlur.setTexture("tex", inputTexture);

    verticalBlur.Bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    verticalBlur.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}






void Render::RenderWaveMesh(unsigned int irradianceMap, unsigned int skybox, unsigned int deviation, unsigned int gradient, unsigned int fbo)
{
    if (setting.enableWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // also draw the lamp object
    waveMeshShader.setMat4("projection", camera.getProjectionMatrix());
    waveMeshShader.setMat4("view", camera.getViewMatrix());
    waveMeshShader.setMat4("model", model);
    

    waveMeshShader.setInt("tessellationFactor", setting.tessellationFactor);

    waveMeshShader.setTexture("gradientMap", gradient);
    waveMeshShader.setTexture("deviationMap", deviation);

    waveMeshShader.setTexture("checkerBoard", checkerBoardTexture);

    waveMeshShader.setFloat("waterDepth", setting.waterDepth);
    waveMeshShader.setFloat("extinctionCoeff", setting.extinctionCoeff);
    waveMeshShader.setVec3("waterBedColor", setting.waterBedColor);
    waveMeshShader.setFloat("FoamTurbulance", setting.FoamTurbulance);

    waveMeshShader.setInt("enableNormalMap", setting.enableNormalMap);

    waveMeshShader.setTexture("IrradianceMap", irradianceMap, GL_TEXTURE_CUBE_MAP);
    waveMeshShader.setTexture("skybox", skybox, GL_TEXTURE_CUBE_MAP);

    waveMeshShader.setVec3("ViewPos", camera.getCameraPos());

    waveMeshShader.Bind();
    glBindVertexArray(quadPatchVAO);
    glDrawArrays(GL_PATCHES, 0, 4);
    glBindVertexArray(0);
    waveMeshShader.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



void Render::RenderObstacleHeightMap(unsigned int fbo)
{
    // check if obstacle exists
    if (obstacleMesh.size() == 0) return;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    createObstacleShader.setTexture("checkerboard", checkerBoardTexture);
    createObstacleShader.setTexture("roundObstacle", waveTexture);

    for (Obstacle& obstacle : obstacleMesh.obstacleList)
    {
        glm::vec4 translation = glm::inverse(model) * glm::vec4(obstacle.pos, 1);

        glm::mat4 transformationMat = glm::translate(glm::mat4(1.0), glm::vec3(translation));  // translation
        transformationMat = glm::scale(transformationMat, glm::vec3(0.1 * setting.brushSize));       // scale

        createObstacleShader.setMat4("transformationMatrix", transformationMat);

        createObstacleShader.Bind();

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        createObstacleShader.unBind();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




void Render::RenderObstacles(unsigned int heightMap, unsigned int fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderObstacleShader.setMat4("projection", camera.getProjectionMatrix());
    renderObstacleShader.setMat4("view", camera.getViewMatrix());
    renderObstacleShader.setMat4("model", model);

    renderObstacleShader.setInt("tessellationFactor", setting.tessellationFactor);
    renderObstacleShader.setVec3("ViewPos", camera.getCameraPos());

    renderObstacleShader.setTexture("ObstacleHeightMap", heightMap);
    renderObstacleShader.setFloat("obstacleHeightFactor", setting.obstacleHeightFactor * 0.2f);

    renderObstacleShader.Bind();
    glBindVertexArray(quadPatchVAO);
    glDrawArrays(GL_PATCHES, 0, 4);
    glBindVertexArray(0);
    renderObstacleShader.unBind();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




Render::~Render()
{
    // delete quad
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}


void Render::DebugDraw(
    unsigned int particleMap,
    unsigned int f123,
    unsigned int f45v,
    unsigned int deviation,
    unsigned int gradient,
    unsigned int waveMesh,
    unsigned int obstaclePosMap,
    unsigned int obstacleMap)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quadShader.setInt("selectedRenderPass", setting.seletectedRenderPass);

    // pass input texture
    quadShader.setTexture("particleMap", particleMap);
    quadShader.setTexture("f123", f123);
    quadShader.setTexture("f45v", f45v);
    quadShader.setTexture("deviation", deviation);
    quadShader.setTexture("gradient", gradient);
    quadShader.setTexture("waveMesh", waveMesh);
    quadShader.setTexture("obstaclePosMap", obstaclePosMap);
    quadShader.setTexture("obstacleMap", obstacleMap);

    quadShader.Bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    quadShader.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}




// normal: normal of plane
// center: any point on the plane
// rayDir: direction of ray
// rayOrigin: ray starting position
// https://stackoverflow.com/questions/23975555/how-to-do-ray-plane-intersection
bool RayPlaneIntersection(glm::vec3 normal, glm::vec3 center, glm::vec3 rayDir, glm::vec3 rayOrigin, float& t)
{
    float denom = glm::dot(normal, rayDir);
    if (abs(denom) > 0.0001f) // your favorite epsilon
    {
        t = glm::dot(center - rayOrigin, normal) / denom;
        if (t >= 0) return true; // you might want to allow an epsilon here too
    }
    return false;
}