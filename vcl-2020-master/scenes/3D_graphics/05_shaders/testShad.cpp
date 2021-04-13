
#include "testShad.hpp"

#include <random>

#ifdef SCENE_TEST_SHADERS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string, GLuint> &shaders, scene_structure &scene, gui_structure &)
{
    // Create mesh structure (stored on CPU)
    // *************************************** //
    mesh quadrangle;
    // Fill position buffer to model a unit quadrangle
    quadrangle.position = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
    // Set the connectivity (quadrangle made of two triangles)
    quadrangle.connectivity = {{0, 1, 2}, {0, 2, 3}};

    // Transfert mesh onto GPU (ready to be displayed)
    // *************************************** //

    // Convert a mesh to mesh_drawable structure
    //  = mesh_drawable stores VBO and VAO, and allows to set uniform parameters easily
    surface = mesh_drawable(quadrangle);

    // Can attach a default shader to the mesh_drawable element
    surface.shader = shaders["mesh"];

    // Example of uniform parameter setting: color of the shape (used in the shader)
    surface.uniform.color = {1.0f, 1.0f, 0.6f};

    /*mesh position_cpu;
    position_cpu.position.resize(N * N);

    for (int x = 0; x < N; ++x)
    {
        for (int z = 0; z < N; ++z)
        {
            position_cpu.position[x + N * z] = vec3(x, z, 0);
        }
    }

    for (size_t ku = 0; ku < N - 1; ++ku)
    {
        for (size_t kv = 0; kv < N - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx + 1 + N, idx + 1};
            const uint3 triangle_2 = {idx, idx + N, idx + 1 + N};

            position_cpu.connectivity.push_back(triangle_1);
            position_cpu.connectivity.push_back(triangle_2);
        }
    }

    position_cpu.fill_empty_fields();
    poing = mesh_drawable(position_cpu, shaders["grass"]);*/

    uniform(shaders["grass"], "perspective", scene.camera.perspective.matrix());
    opengl_debug();
    uniform(shaders["grass"], "view", scene.camera.view_matrix());
    opengl_debug();

    for (float x = -10.0f; x < 10.0f; x += 0.2f)
    {
        for (float z = -10.0f; z < 10.0f; z += 0.2f)
        {
            positions.push_back(vec3(x, 0, z));
        }
    }
    glGenVertexArrays(1, &vaop);
    glGenBuffers(1, &vbop);
    glBindVertexArray(vaop);

    glBindBuffer(GL_ARRAY_BUFFER, vbop);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPointSize(5.0f);
}

/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string, GLuint> &shaders, scene_structure &scene, gui_structure &)
{
    // Drawing call: need to provide the camera information (use the default shader if it has been set previously)
    // draw(surface, scene.camera);

    // draw2(poing, scene.camera);
    uniform(shaders["grass"], "view", scene.camera.view_matrix());
    opengl_debug();
    glUseProgram(shaders["grass"]);
    glBindVertexArray(vaop);
    glDrawArrays(GL_POINTS, 0, positions.size());
}

#endif
