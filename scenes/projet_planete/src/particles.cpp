#include "particles.hpp"
#include "vcl/vcl.hpp"
#include <iostream>

#include "scene_helper.hpp"

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
    setTexture(opengl_texture_to_gpu(image_load_png("../../assets/particles/snowflake_3.png"), GL_REPEAT /**GL_TEXTURE_WRAP_S*/, GL_REPEAT /**GL_TEXTURE_WRAP_T*/));

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    for (int i = 0; i < MaxParticles; i++)
    {
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }
    //shad = opengl_create_shader_program(openShader("partic_vert"), openShader("partic_frag"));
    opengl_check;

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);
    opengl_check;

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    opengl_check;

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    opengl_check;

    glBindVertexArray(0);
    opengl_check;
}

void ParticleS::updateParticles(vec3 CameraPosition)
{
    float delta = 0.016f;
    int newparticles = (int)(0.016f * 10000.0);

    for (int i = 0; i < newparticles; i++)
    {
        int particleIndex = FindUnusedParticle();
        ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
        vec3 randompos = vec3(
            20 * ((rand() / (float)RAND_MAX) - 0.5f),
            20 * ((rand() / (float)RAND_MAX) - 0.5f),
            10.0f);

        ParticlesContainer[particleIndex].pos = randompos;

        float spread = 1.5f;
        vec3 maindir = vec3(0.0f, 0.0, -1.0f);
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
                p.speed += vec3(0.0f, 0, -9.81f) * (float)delta * 0.5f;
                p.pos += p.speed * (float)delta;
                p.cameradistance = norm(p.pos - CameraPosition);
                //ParticlesContainer[i].pos += vec3(0.0f,10.0f, 0.0f) * (float)delta;

                //std::cout << p.pos << std::endl;

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

void ParticleS::cleanUp()
{
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(shad);
    glDeleteVertexArrays(1, &VertexArrayID);
}

void ParticleS::setTexture(GLuint text_)
{
    text = text_;
}