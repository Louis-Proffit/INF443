#pragma once

#include "vcl/vcl.hpp"

#include "scene_helper.hpp"

using namespace vcl;
struct particle
{
    vec3 pos;
    vec3 speed;
    vec4 color;
    float size, angle, weight;
    float life;
    float cameradistance;

    bool operator<(const particle &that) const
    {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

class ParticleS
{
public:
    int MaxParticles;
    particle *ParticlesContainer;
    int LastUsedParticle = 0;
    GLuint VertexArrayID = 0;
    GLfloat *g_particule_position_size_data;
    GLubyte *g_particule_color_data;
    GLuint shad = 0;
    GLfloat g_vertex_buffer_data[12] = {
        -0.5f,
        -0.5f,
        0.0f,
        0.5f,
        -0.5f,
        0.0f,
        -0.5f,
        0.5f,
        0.0f,
        0.5f,
        0.5f,
        0.0f,
    };

    GLuint billboard_vertex_buffer = 0;
    GLuint particles_position_buffer = 0;
    GLuint particles_color_buffer = 0;

    GLuint text = 0;
    GLuint textID = 0;
    int nbRows;
    std::string type;

    int ParticlesCount;

    float x_min;
    float y_min;
    float x_max;
    float y_max;

    ParticleS(int nmax = 3000, std::string nametext = "snowflakes", float xmin = -10.0f, float xmax = 10.0f, float ymin = -10.0f, float ymax = 10.0f);
    ~ParticleS();

    void setTexture(GLuint text_);

    int FindUnusedParticle();
    void SortParticles();

    void initVaoVbo();

    void updateParticles(vec3 CameraPosition);

    template <typename SCENE>
    void updateShadVbos(SCENE const &scene)
    {
        opengl_check;
        glClearColor(0.215f, 0.215f, 0.215f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VertexArrayID);
        opengl_check;
        //std::cout << particles_position_buffer << std::endl;

        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        opengl_check;
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
        opengl_check;
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);
        opengl_check;

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
        opengl_check;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use our shader
        glUseProgram(shad);

        // Coder le fait de mettre de la neige avec differents flocons facile
        glActiveTexture(GL_TEXTURE1);
        opengl_check;
        glBindTexture(GL_TEXTURE_2D, text);
        opengl_check;
        opengl_uniform(shad, "myTexture", 1);
        opengl_check;

        opengl_uniform(shad, "nbRows", nbRows);
        opengl_check;

        glBindVertexArray(VertexArrayID);
        opengl_check;
        glEnableVertexAttribArray(0);
        opengl_check;
        // 1rst attribute buffer : vertices
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        opengl_check;
        glVertexAttribPointer(
            0,        // attribute. No particular reason for 0, but must match the layout in the shader.
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            nullptr   // array buffer offset
        );
        opengl_check;

        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        opengl_check;

        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        opengl_check;
        glVertexAttribPointer(
            1,        // attribute. No particular reason for 1, but must match the layout in the shader.
            4,        // size : x + y + z + size => 4
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            nullptr   // array buffer offset
        );
        opengl_check;

        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        opengl_check;
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        opengl_check;
        glVertexAttribPointer(
            2,                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                // size : r + g + b + a => 4
            GL_UNSIGNED_BYTE, // type
            GL_FALSE,         // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
            0,                // stride
            nullptr           // array buffer offset
        );
        opengl_check;

        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
        opengl_check;
        // Draw the particules !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
        // but faster.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

        opengl_check;
        glDisableVertexAttribArray(0);
        opengl_check;
        glDisableVertexAttribArray(1);
        opengl_check;
        glDisableVertexAttribArray(2);
        opengl_check;

        ParticlesCount = 0;

        glBindVertexArray(0);
        opengl_check;
    };

    void cleanUp();
};