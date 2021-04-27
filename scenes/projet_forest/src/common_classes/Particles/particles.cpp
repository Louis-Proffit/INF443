#include "particles.hpp"
#include "vcl/vcl.hpp"
#include <iostream>

#include "/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/src/scene_helper.hpp"

using namespace vcl;

int ParticleS::FindUnusedParticle()
{
    for (int i = LastUsedParticle; i < MaxParticles; i++)
    {
        if (ParticlesContainer[i].life < 0)
        {
            LastUsedParticle = i;
            return i;
        }
    }

    for (int i = 0; i < LastUsedParticle; i++)
    {
        if (ParticlesContainer[i].life < 0)
        {
            LastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void ParticleS::SortParticles()
{
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

void ParticleS::initVaoVbo()
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    for (int i = 0; i < MaxParticles; i++)
    {
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }
    // shad = opengl_create_shader_program(openShader("partic_vert"), openShader("partic_frag"));

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void ParticleS::updateParticles(vec3 CameraPosition)
{
    float delta = 0.02f;
    int newparticles = (int)(0.02f * 10000.0);

    for (int i = 0; i < newparticles; i++)
    {
        int particleIndex = FindUnusedParticle();
        ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
        ParticlesContainer[particleIndex].pos = vec3(0, 0, -20.0f);

        float spread = 1.5f;
        vec3 maindir = vec3(0.0f, 10.0f, 0.0f);
        // Very bad way to generate a random direction;
        // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
        // combined with some user-controlled parameters (main direction, spread, etc)
        vec3 randomdir = vec3(
            (rand() % 2000 - 1000.0f) / 1000.0f,
            (rand() % 2000 - 1000.0f) / 1000.0f,
            (rand() % 2000 - 1000.0f) / 1000.0f);

        ParticlesContainer[particleIndex].speed = maindir + randomdir * spread;

        // Very bad way to generate a random color
        ParticlesContainer[particleIndex].color.x = rand() % 256;
        ParticlesContainer[particleIndex].color.y = rand() % 256;
        ParticlesContainer[particleIndex].color.z = rand() % 256;
        ParticlesContainer[particleIndex].color[3] = (rand() % 256) / 3;

        ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
    }

    // Simulate all particles
    ParticlesCount = 0;
    for (int i = 0; i < MaxParticles; i++)
    {

        particle &p = ParticlesContainer[i]; // shortcut

        if (p.life > 0.0f)
        {

            // Decrease life
            p.life -= delta;
            if (p.life > 0.0f)
            {

                // Simulate simple physics : gravity only, no collisions
                p.speed += vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
                p.pos += p.speed * (float)delta;
                p.cameradistance = norm(p.pos - CameraPosition);
                //ParticlesContainer[i].pos += vec3(0.0f,10.0f, 0.0f) * (float)delta;

                // Fill the GPU buffer
                g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

                g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

                g_particule_color_data[4 * ParticlesCount + 0] = p.color.x;
                g_particule_color_data[4 * ParticlesCount + 1] = p.color.y;
                g_particule_color_data[4 * ParticlesCount + 2] = p.color.z;
                g_particule_color_data[4 * ParticlesCount + 3] = p.color[3];
            }
            else
            {
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
            }

            ParticlesCount++;
        }
    }

    SortParticles();
}
template <typename SCENE>
void ParticleS::updateShadVbos(SCENE const &scene)
{
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use our shader
    glUseProgram(shad);

    opengl_uniform(shad, scene);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
        0,        // attribute. No particular reason for 0, but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
        1,        // attribute. No particular reason for 1, but must match the layout in the shader.
        4,        // size : x + y + z + size => 4
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
        2,                // attribute. No particular reason for 1, but must match the layout in the shader.
        4,                // size : r + g + b + a => 4
        GL_UNSIGNED_BYTE, // type
        GL_TRUE,          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
        0,                // stride
        (void *)0         // array buffer offset
    );

    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    ParticlesCount = 0;
}

void ParticleS::cleanUp()
{
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(shad);
    glDeleteVertexArrays(1, &VertexArrayID);
}