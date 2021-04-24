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
    
    // clear color
    glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};

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
    uint32_t m_indexCount {0};



    const float PI=3.141592f;

    int donut_segment {40};        //  donut_element
    int circle_segment {40};       //  
    float donut_radius {5.0f};     //
    float circle_radius {2.0f};    //

    float cylinder_top_radius {5.0f};        //  cylinder_element
    float cylinder_bottom_radius {5.0f};     //
    float cylinder_height {4.0f};            //
    int segment {10};                        //

    float user_radius {5.0f};        //  sphere_element
    int width_segment {10};          //
    int height_segment {10};         //
    
};

#endif // __CONTEXT_H__