//
// Created by tropi on 12/7/2023.
//

#include "waterBufferStuff.h"

namespace tsa {
    WaterBuffers::WaterBuffers() {
        initReflectionFrameBuffer();
        initRefractionFrameBuffer();
    }

    void tsa::WaterBuffers::WaterBuffers::initReflectionFrameBuffer(){
        reflectionFrameBuffer = createFrameBuffer();
        reflectionTexture = createTextureAttribute(REFLECTION_WIDTH, REFLECTION_HEIGHT);
        reflectionDepthBuffer = createDepthBufferAttribute(REFLECTION_WIDTH, REFLECTION_HEIGHT);
        unbindCurrFrameBuffers();
    }

    void tsa::WaterBuffers::WaterBuffers::initRefractionFrameBuffer() {
        refractionFrameBuffer = createFrameBuffer();
        refractionTexture = createTextureAttribute(REFRACTION_WIDTH, REFRACTION_HEIGHT);
        refractionDepthTexture = createDepthTextureAttribute(REFRACTION_WIDTH, REFRACTION_HEIGHT);
        unbindCurrFrameBuffers();
    }

    int tsa::WaterBuffers::createFrameBuffer(){
        GLuint frameBuffer;

        //Generates the frame buffer
        glGenFramebuffers(1, &frameBuffer);

        //Generate name for frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        //Create the framebuffer
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        return frameBuffer;
    }

    int tsa::WaterBuffers::createTextureAttribute(int width, int height) {
        GLuint texture;

        //Generates texture
        glGenTextures(1, &texture);

        //Binds the texture
        glBindTexture(GL_TEXTURE_2D, texture);

        //Sets texture image info and stuff
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                texture, 0);

        return texture;
    }

    int tsa::WaterBuffers::createDepthBufferAttribute(int width, int height) {
        GLuint depthBuffer;

        //Generates depth render buffer
        glGenRenderbuffers(1, &depthBuffer);

        //Binds render buffer
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

        //Creates render buffer with info
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width,
                height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                GL_RENDERBUFFER, depthBuffer);
        return depthBuffer;
    }

    int tsa::WaterBuffers::createDepthTextureAttribute(int width, int height) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
                0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                texture, 0);
        return texture;
    }

    void tsa::WaterBuffers::bindReflectionFrameBuffer() {
        glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
        //glViewport(0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT);
    }

    void tsa::WaterBuffers::bindRefractionFrameBuffer() {
        glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
        glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
        //glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
    }

    void tsa::WaterBuffers::unbindCurrFrameBuffers() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint tsa::WaterBuffers::getReflectionText() {
        return reflectionTexture;
    }

    void tsa::WaterBuffers::cleanUpTime() {
        glDeleteFramebuffers(1, &reflectionFrameBuffer);
        glDeleteTextures(1, &reflectionTexture);
        glDeleteRenderbuffers(1, &reflectionDepthBuffer);
        glDeleteFramebuffers(1, &refractionFrameBuffer);
        glDeleteTextures(1, &refractionTexture);
        glDeleteTextures(1, &refractionDepthTexture);
    }
}