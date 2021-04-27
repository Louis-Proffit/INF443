#pragma once

#include "vcl/vcl.hpp"

#include "/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/src/scene_helper.hpp"

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
    const int MaxParticles = 10000;
    particle ParticlesContainer[10000];
    int LastUsedParticle = 0;
    GLuint VertexArrayID = 0;
    GLfloat *g_particule_position_size_data = new GLfloat[10000 * 4];
    GLubyte *g_particule_color_data = new GLubyte[10000 * 4];
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

    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;

    int ParticlesCount;

    int FindUnusedParticle();
    void SortParticles();

    void initVaoVbo();

    void updateParticles(vec3 CameraPosition);

    template <typename SCENE>
    void updateShadVbos(SCENE const &scene);

    void cleanUp();
};