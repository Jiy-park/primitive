#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"

CLASS_PTR(Context)
class Context{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

private:
    Context() {}
    bool Create_Cube();
    bool Create_Sphere(); 
    bool Create_Cylinder(); 
    bool Create_Donut();
    ProgramUPtr m_program;
    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;
    TextureUPtr m_texture3;

    // clear color
    glm::vec4 m_clearColor{glm::vec4(0.5f,1.0f,0.8f,0.5f)};

    // camera parameter

    bool m_cameraControl{false};
    glm::vec2 m_prevMousePos{glm::vec2(0.0f)};
    float m_cameraPitch{0.0f};
    float m_cameraYaw{0.0f};
    glm::vec3 m_cameraPos{glm::vec3(0.0f, 0.0f, 3.0f)};
    glm::vec3 m_cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 m_cameraUp{glm::vec3(0.0f, 1.0f, 0.0f)};

    int m_width{WINDOW_WIDTH};
    int m_height{WINDOW_HEIGHT};


    uint32_t m_indexCount {0};      //indices_count
    uint32_t m_vertices_count;      //vertices_count
    uint32_t m_triangle_count {0};  //triangle_count


    const float PI=3.141592f;
    bool for_call_Create_func_once=false; 

    int donut_segment {8};        //  donut_elements
    int circle_segment {8};       //  
    float donut_radius {2.0f};     //  (0,0,0) ~ circle_center
    float circle_radius {1.0f};    //

    float cylinder_top_radius {1.0f};        //  cylinder_elements
    float cylinder_bottom_radius {1.0f};     //
    float cylinder_height {1.0f};            //
    int segment {10};                        //

    float user_radius {1.0f};        //  sphere_elements
    int width_segment {10};          //
    int height_segment {10};         //

    float time_for_autorotation=0.0f;
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotate_speed=glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 scale=glm::vec3(1.0f,1.0f,1.0f);
};

#endif // __CONTEXT_H__