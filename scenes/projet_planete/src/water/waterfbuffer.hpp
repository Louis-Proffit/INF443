#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "helpers/scene_helper.hpp"

using namespace vcl;

class WaterFrameBuffers
{
public:
    int REFLECTION_WIDTH = 1280;
    int REFLECTION_HEIGHT = 720;

    int REFRACTION_WIDTH = 1280;
    int REFRACTION_HEIGHT = 720;

    GLuint reflectionFrameBuffer = 0;

    GLuint reflectionTexture = 0;

    GLuint reflectionDepthBuffer = 0;

    GLuint refractionFrameBuffer = 0;

    GLuint refractionTexture = 0;

    GLuint refractionDepthTexture = 0;

    float movefactor = 0;

    void initWaterFrameBuffers()
    { //call when loading the game
        initialiseReflectionFrameBuffer();
        initialiseRefractionFrameBuffer();
        opengl_check;
    }

    void cleanUp()
    { //call when closing the game
        glDeleteFramebuffers(1, &reflectionFrameBuffer);
        glDeleteTextures(1, &reflectionTexture);
        glDeleteRenderbuffers(1, &reflectionDepthBuffer);
        glDeleteFramebuffers(1, &refractionFrameBuffer);
        glDeleteTextures(1, &refractionTexture);
        glDeleteTextures(1, &refractionDepthTexture);
        opengl_check;
    }

    void bindReflectionFrameBuffer()
    { //call before rendering to this FBO
        bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
        opengl_check;
    }

    void bindRefractionFrameBuffer()
    { //call before rendering to this FBO
        bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
        opengl_check;
    }

    void unbindCurrentFrameBuffer()
    { //call to switch to default frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, scene_visual::display_w, scene_visual::display_h);
        // glViewport(0, 0, 2580, 2048);
        opengl_check;
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