#include "particles.hpp"
#include "vcl/vcl.hpp"
#include <iostream>

#include "scene_helper.hpp"

using namespace vcl;

Particles::Particles(int nmax, std::string nametext, float xmin, float xmax, float ymin, float ymax)
{
    MaxParticles = nmax;
    ParticlesContainer = new particle[nmax];
    g_particule_position_size_data = new GLfloat[nmax * 4];
    g_particule_color_data = new GLubyte[nmax * 4];
    type = nametext;
    if (nametext == "snowflakes")
    {

        nbRows = 1;
    }
    else if (nametext == "fire")
    {
        nbRows = 8;
    }
    else if (nametext == "grass")
    {
        nbRows = 8;
    }
    else
        nbRows = 0;
    x_min = xmin;
    y_min = ymin;
    x_max = xmax;
    y_max = ymax;
}

Particles::~Particles() {}

int Particles::FindUnusedParticle()
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

void Particles::SortParticles()
{
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

void Particles::initVaoVbo()
{
    if (type == "snowflakes") text = scene_visual::get_texture(texture_type::SNOWFLAKE);
    else if (type == "fire") text = scene_visual::get_texture(texture_type::FIRE);
    else if (type == "grass") text = scene_visual::get_texture(texture_type::GRASS_ATLAS);
    else
        error_vcl("la texture n'existe pas");

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    opengl_check;

    for (int i = 0; i < MaxParticles; i++)
    {
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }
    if (type == "grass")
    {
        for (int i = 0; i < MaxParticles; i++)
        {

            ParticlesContainer[i].life = 15.0f; // This particle will live 5 seconds.
            vec3 randompos = vec3(
                (x_max - x_min) * ((rand() / (float)RAND_MAX) - 0.5f),
                (x_max - x_min) * ((rand() / (float)RAND_MAX) - 0.5f),
                0.15f);

            ParticlesContainer[i].pos = randompos;
            ParticlesContainer[i].speed = vec3(0, 0, 0);

            // Very bad way to generate a random color
            ParticlesContainer[i].color.x = 0;

            ParticlesContainer[i].color.y = 0;

            ParticlesContainer[i].color.z = rand() % 64;

            ParticlesContainer[i].color[3] = 1;

            ParticlesContainer[i].size = z_min + rand_interval() * (z_max - z_min);
        }
    }
    shad = scene_visual::get_shader(shader_type::PARTICLE);
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

void Particles::updateParticles(vec3 CameraPosition)
{
    if (type == "snowflakes")
    {
        float delta = 0.016f;
        int newparticles = (int)(0.016f * MaxParticles);

        for (int i = 0; i < newparticles; i++)
        {
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
            vec3 randompos = vec3(
                4 * ((rand() / (float)RAND_MAX) - 0.5f),
                4 * ((rand() / (float)RAND_MAX) - 0.5f),
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
            ParticlesContainer[particleIndex].color.x = 0;

            ParticlesContainer[particleIndex].color.y = 0;

            ParticlesContainer[particleIndex].color.z = 0;

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

                    std::cout << p.pos << std::endl;

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
    }
    else if (type == "fire")
    {
        float delta = 0.016f;
        int newparticles = (int)(delta * MaxParticles);

        for (int i = 0; i < newparticles; i++)
        {
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = 1.5f; // This particle will live 1.5 seconds.
            vec3 spawnpos = vec3(0, 0, 0);

            ParticlesContainer[particleIndex].pos = spawnpos;

            float spread = 0.5f;
            vec3 maindir = vec3(0.0f, 0.0, 1.0f);
            // Very bad way to generate a random direction;
            // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
            // combined with some user-controlled parameters (main direction, spread, etc)
            vec3 randomdir = vec3(
                (rand() % 2000 - 1000.0f) / 1000.0f,
                (rand() % 2000 - 1000.0f) / 1000.0f,
                0);

            ParticlesContainer[particleIndex].speed = maindir + randomdir * spread;

            // Very bad way to generate a random color
            ParticlesContainer[particleIndex].color.x = (rand() % 256) / 3;
            ParticlesContainer[particleIndex].color.y = (rand() % 256) / 3;
            ParticlesContainer[particleIndex].color.z = (rand() % 256) / 3;
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
                    // Pour le feu: on envoie le type dans l'offset

                    // Simulate simple physics : gravity only, no collisions
                    //p.speed += vec3(0.0f, 0, -9.81f) * (float)delta * 0.5f;
                    p.pos += p.speed * (float)delta;
                    p.cameradistance = norm(p.pos - CameraPosition);
                    //ParticlesContainer[i].pos += vec3(0.0f,10.0f, 0.0f) * (float)delta;

                    //std::cout << p.pos << std::endl;

                    float lifefactor = 1 - p.life / 1.5f;
                    int progression = floor(lifefactor * nbRows * nbRows);
                    int column = progression % nbRows;
                    int row = progression / nbRows;
                    std::cout << vec2((row) / (float)nbRows, (column) / (float)nbRows) << std::endl;

                    vec2 decalcolor = vec2((row) / (float)nbRows, (column) / (float)nbRows);
                    std::cout << decalcolor << std::endl;

                    // Fill the GPU buffer
                    g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
                    g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
                    g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

                    g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

                    g_particule_color_data[4 * ParticlesCount + 0] = decalcolor.x; // Le pb vient du type
                    g_particule_color_data[4 * ParticlesCount + 1] = decalcolor.y;
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
    }
    else if (type == "grass")
    {
        // Simulate all particles
        ParticlesCount = 0;
        for (int i = 0; i < MaxParticles; i++)
        {

            particle &p = ParticlesContainer[i]; // shortcut

            if (p.life > 0.0f)
            {

                // Simulate simple physics : gravity only, no collisions
                //p.speed += vec3(0.0f, 0, -9.81f) * (float)delta * 0.5f;
                //p.pos += p.speed * (float)delta;
                p.cameradistance = norm(p.pos);
                //ParticlesContainer[i].pos += vec3(0.0f,10.0f, 0.0f) * (float)delta;

                // Fill the GPU buffer
                g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

                g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

                g_particule_color_data[4 * ParticlesCount + 0] = p.color.x;
                g_particule_color_data[4 * ParticlesCount + 1] = 5;
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
void Particles::cleanUp()
{
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(shad);
    glDeleteVertexArrays(1, &VertexArrayID);
}

void Particles::setTexture(GLuint text_)
{
    text = text_;
}