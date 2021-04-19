#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

using namespace vcl;

class WaterFrameBuffers
{
public:
    int REFLECTION_WIDTH = 320;
    int REFLECTION_HEIGHT = 180;

    int REFRACTION_WIDTH = 1280;
    int REFRACTION_HEIGHT = 720;

    GLuint reflectionFrameBuffer;

    GLuint reflectionTexture;

    GLuint reflectionDepthBuffer;

    GLuint refractionFrameBuffer;

    GLuint refractionTexture;

    GLuint refractionDepthTexture;

    WaterFrameBuffers()
    { //call when loading the game
        initialiseReflectionFrameBuffer();
        initialiseRefractionFrameBuffer();
    }

    /*void cleanUp()
    { //call when closing the game
        glDeleteFramebuffers(reflectionFrameBuffer);
        glDeleteTextures(reflectionTexture);
        glDeleteRenderbuffers(reflectionDepthBuffer);
        glDeleteFramebuffers(refractionFrameBuffer);
        glDeleteTextures(refractionTexture);
        glDeleteTextures(refractionDepthTexture);
    }*/

    void bindReflectionFrameBuffer()
    { //call before rendering to this FBO
        bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
    }

    void bindRefractionFrameBuffer()
    { //call before rendering to this FBO
        bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
    }

    void unbindCurrentFrameBuffer(int width, int height)
    { //call to switch to default frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
    }

    GLuint getReflectionTexture()
    { //get the resulting texture
        return reflectionTexture;
    }

    GLuint getRefractionTexture()
    { //get the resulting texture
        return refractionTexture;
    }

    GLuint getRefractionDepthTexture()
    { //get the resulting depth texture
        return refractionDepthTexture;
    }

    void initialiseReflectionFrameBuffer();

    void initialiseRefractionFrameBuffer();

    void bindFrameBuffer(GLuint frameBuffer, int width, int height);

    GLuint createFrameBuffer();

    GLuint createTextureAttachment(int width, int height);

    GLuint createDepthTextureAttachment(int width, int height);

    GLuint createDepthBufferAttachment(int width, int height);
};