#include "Render.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"

extern Camera camera;

Setting setting;

extern unsigned int checkerBoardTexture;
extern unsigned int waveTexture;



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



void FBO::Clear()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ID);

    // clear buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    // model matrix for river plane
    model = glm::scale(model, glm::vec3(2.0f));
    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));
}




#pragma region fluid helpers

// Note: Bind the advect shader first.
void Render::AdvectHelper(Quad &quad, FBO* velocity, FBO* obstacles, FBO* src, FBO* dst, float dissipation)
{
    assert(src != dst);
    glBindFramebuffer(GL_FRAMEBUFFER, dst->ID);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //constexpr float DELTATIME = 0.1f; // TODO! !!!!

    //flowAdvect.setVec2("uInverseSize", fluidInvScale);
    flowAdvect.setFloat("uDeltaTime", FIXED_DELTA_TIME);
    flowAdvect.setFloat("uDissipation", dissipation);


    // The source texture.
    //flowAdvect.setInt("uSoureTexture", src->ColorBuffer1);
    flowAdvect.setTexture("uSourceTexture", src->ColorBuffer1);

    flowAdvect.setTexture("uObstacleMap", obstacles->ColorBuffer1);
    flowAdvect.setTexture("uVelocity", velocity->ColorBuffer1);

    flowAdvect.Bind();
    glBindVertexArray(quad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    flowAdvect.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Render::JacobiHelper(Quad& quad, FBO* pressure, FBO* divergence, FBO* obstacles, FBO* dst)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dst->ID);
    
    const float ALPHA = -cellSize*cellSize;
    constexpr float INVBETA = 0.25f; 

    flowJacobi.setFloat("uAlpha", ALPHA);
    flowJacobi.setFloat("uInverseBeta", INVBETA);

    flowAdvect.setTexture("uObstacleMap", obstacles->ColorBuffer1);
    flowAdvect.setTexture("uPressure", pressure->ColorBuffer1);
    flowAdvect.setTexture("uDivergence", divergence->ColorBuffer1);

    flowJacobi.Bind();
    glBindVertexArray(quad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    flowJacobi.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Render::SubtractGradientHelper(Quad& quad, FBO* velocity, FBO* pressure, FBO* obstacles, FBO* dst)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dst->ID);

    flowSubtractGradient.setFloat("uGradientScale", gradientScale);

    flowAdvect.setTexture("uObstacleMap", obstacles->ColorBuffer1);
    flowAdvect.setTexture("uVelocity", velocity->ColorBuffer1);
    flowAdvect.setTexture("uPressure", pressure->ColorBuffer1);

    flowSubtractGradient.Bind();
    glBindVertexArray(quad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    flowSubtractGradient.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Render::ComputeDivergenceHelper(Quad& quad, FBO* velocity, FBO* obstacles, FBO* dst)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dst->ID);

    const float HALFINVCELLSIZE = 0.5f/cellSize;

    flowComputeDivergence.setFloat("uHalfInvCellSize", HALFINVCELLSIZE);

    flowAdvect.setTexture("uObstacleMap", obstacles->ColorBuffer1);
    flowAdvect.setTexture("uVelocity", velocity->ColorBuffer1);

    flowComputeDivergence.Bind();
    glBindVertexArray(quad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    flowComputeDivergence.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Render::ApplyExternalFlow(Quad& quad, FBO* velocity, unsigned int srcTex, float multiplier)
{
    glBindFramebuffer(GL_FRAMEBUFFER, velocity->ID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    //constexpr float DELTATIME = 0.1f; // TODO!!
    multiplier *= FIXED_DELTA_TIME;

    flowAdder.setTexture("uSrc", srcTex);
    flowAdder.setFloat("uMultiplier", multiplier);
    //flowAdder.setVec2("uDstScale", glm::vec2(fluidGridScale.x, fluidGridScale.y));

    flowAdder.Bind();
    glBindVertexArray(quad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    flowAdder.unBind();

    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#pragma endregion

void Render::UpdateFlowMap(Quad& quad, FBO* obstacleFBO, PingPong* velocity, PingPong* pressure, FBO* divergence)
{
    glDisable(GL_DEPTH_TEST);

    constexpr float DISSIPATION_VELOCITY = 0.99f;
    
    // TODO: Where do I put vorticity step?

    // FORCE APPLICATION //
    if (impulseMapTexture > 0 && impulseFieldEnabled)
    {
        // Apply impulse.
        ApplyExternalFlow(quad, velocity->ping, this->impulseMapTexture, 1.5f);
    }


    // STEP 1: ADVECTION STEP //


    // Perform advection on the velocity:
    //flowAdvect.Bind();
    AdvectHelper(quad, velocity->ping, obstacleFBO, velocity->ping, velocity->pong, DISSIPATION_VELOCITY);
    velocity->Swap();

    // Advection would be performed on density here if it was relevant to water.
    // ...

    // Perform advection on the density:

    // STEP 3: COMPUTE DIVERGENCE //
    ComputeDivergenceHelper(quad, velocity->ping, obstacleFBO, divergence);
    // Clear the pressure reading fbo, don't need to clear the writing one.
    pressure->ping->Clear();

    // STEP 4: PERFORM JACOBI ITERATIONS //
    constexpr int ITR = 40;
    for (int i = 0; i < ITR; ++i)
    {
        JacobiHelper(quad, pressure->ping, divergence, obstacleFBO, pressure->pong);
        pressure->Swap();
    }

    // STEP 5: GRADIENT SUBTRACTION //
    SubtractGradientHelper(quad, velocity->ping, pressure->ping, obstacleFBO, velocity->pong);
    velocity->Swap();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
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



void Render::ObstacleBlur(Quad& quad, unsigned int ObstaclePosMap, unsigned int fbo)
{
    // horizontal blur
    glBindFramebuffer(GL_FRAMEBUFFER, obstacleBlurFBO.ID);

    // clear buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    obstacleBlurHShader.setTexture("obstaclePosMap", ObstaclePosMap);

    quad.Draw(obstacleBlurHShader);

    // vertical blur
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // clear buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // pass horizontal blured result
    obstacleBlurVShader.setTexture("horiBlurMap", obstacleBlurFBO.ColorBuffer1);

    quad.Draw(obstacleBlurVShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Render::HorizontalBlur(Quad& quad, unsigned int inputTexture, unsigned int fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // pass input texture
    horozontalBlur.setTexture("inputTexture", inputTexture);

    // blur radius
    horozontalBlur.setInt("blurRadius", setting.blurSize);

    quad.Draw(horozontalBlur);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Render::VerticalBlur(Quad& quad, unsigned int f123, unsigned int f45v, unsigned int fbo)
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

    quad.Draw(verticalBlur);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Render::RenderWaveMesh(WaterMesh& waterMesh, unsigned int irradianceMap, unsigned int skybox, unsigned int deviation, unsigned int gradient, unsigned int fbo)
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

    waterMesh.Draw(waveMeshShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Render::RenderObstacleHeightMap(Quad& quad, unsigned int fbo)
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

        quad.Draw(createObstacleShader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Render::RenderObstacles(WaterMesh& waterMesh, unsigned int heightMap, unsigned int fbo)
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

    //waveMeshShader.setTexture("IrradianceMap", irradianceMap, GL_TEXTURE_CUBE_MAP);

    waterMesh.Draw(renderObstacleShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Render::DebugDraw(
    Quad& quad,
    unsigned int particleMap,
    unsigned int f123,
    unsigned int f45v,
    unsigned int deviation,
    unsigned int gradient,
    unsigned int waveMesh,
    unsigned int obstacleMap,
    unsigned int blurredObstacleMap,
    unsigned int flowDivergence,
    unsigned int flowVelocity,
    unsigned int flowPressure
)
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
    quadShader.setTexture("obstacleMap", obstacleMap);
    quadShader.setTexture("blurredObstacleMap", blurredObstacleMap);
    quadShader.setTexture("flowDivergence", flowDivergence);
    quadShader.setTexture("flowVelocity", flowVelocity);
    quadShader.setTexture("flowPressure", flowPressure);

    quad.Draw(quadShader);

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
