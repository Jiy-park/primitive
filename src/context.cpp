#include "context.h"
#include "image.h"
#include <imgui.h>
#include <cmath>
ContextUPtr Context::Create(){
    auto context = ContextUPtr(new Context());
    if (!context->Create_Cube())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (!m_cameraControl)
        return;
    const float cameraSpeed = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;
    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;
    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {   
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.3f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)      m_cameraYaw += 360.0f;     
    if (m_cameraYaw > 360.0f)      m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)    m_cameraPitch = 89.0f;  
    if (m_cameraPitch < -89.0f)    m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS){
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
    else if (action == GLFW_RELEASE)
        m_cameraControl = false;
  }
}

bool Context::Create_Cube(){
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
};

    uint32_t indices[] = {
         0,  2,  1,  2,  0,  3,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
};
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer=Buffer::CreateWithData(GL_ARRAY_BUFFER,GL_STATIC_DRAW,vertices,sizeof(float)*120);
    m_vertexLayout->SetAttrib(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*5,0);                       
    m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);                    
    m_indexBuffer=Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,indices,sizeof(float)*36);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);

    auto image = Image::Load("./image/wood.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture= Texture::CreateFromImage(image.get());
    
    auto image2=Image::Load("./image/metal.jpg");
    m_texture2=Texture::CreateFromImage(image2.get());

    auto image3=Image::Load("./image/earth.png");
    m_texture3=Texture::CreateFromImage(image3.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture3->Get());

    m_program->Use();

    m_indexCount=36;
    m_vertices_count=120;
    m_triangle_count=12;
    return true;
}

bool Context::Create_Donut(){
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    for(int i=0;i<=donut_segment;i++){
        float donut_angle=2*PI/donut_segment*i;
        float donut_x=donut_radius*cosf(donut_angle);
        float donut_y=donut_radius*sinf(donut_angle);
        for(int j=0;j<=circle_segment;j++){
            float circle_angle=2*PI/circle_segment*j;
            float circle_x=donut_x+circle_radius*cosf(donut_angle)*cosf(circle_angle);
            float circle_y=donut_y+circle_radius*sinf(donut_angle)*cosf(circle_angle);
            float circle_z=circle_radius*sinf(circle_angle);
            vertices.push_back(circle_x*scale.x);//circle_x
            vertices.push_back(circle_y*scale.y);//circle_y
            vertices.push_back(circle_z*scale.z);//circle_z
            vertices.push_back(i/(float)donut_segment);
            vertices.push_back(j/(float)circle_segment);
        }
    }

    for(int i=0;i<donut_segment;i++){
        int donut_piece=(circle_segment+1)*i;
        for(int j=0;j<circle_segment;j++){
            indices.push_back(donut_piece+j);
            indices.push_back(donut_piece+j+1);
            indices.push_back(donut_piece+j+1+circle_segment);

            indices.push_back(donut_piece+j+1+circle_segment);
            indices.push_back(donut_piece+j+2+circle_segment);
            indices.push_back(donut_piece+j+1);
        }
    }

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);//3~5//
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(), sizeof(float) * indices.size());
    
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());
    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    auto image = Image::Load("./image/wood.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture= Texture::CreateFromImage(image.get());
    
    auto image2=Image::Load("./image/metal.jpg");
    m_texture2=Texture::CreateFromImage(image2.get());

    auto image3=Image::Load("./image/earth.png");
    m_texture3=Texture::CreateFromImage(image3.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture3->Get());

    m_program->Use();
    m_indexCount = (uint32_t)indices.size();
    m_vertices_count = (uint32_t)vertices.size();
    m_triangle_count = 2*circle_segment*donut_segment;
    return true;
}

bool Context::Create_Sphere(){
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    vertices.push_back(0);           //sphere_start_poit
    vertices.push_back(0);           //
    vertices.push_back(user_radius*scale.z); //
    vertices.push_back(0.5f);
    vertices.push_back(0);
    for(int i=1;i<width_segment;i++){
        float height_angle=PI*i/(float)width_segment;
        if(i==width_segment) height_angle=0;
        float radius=user_radius*sinf(height_angle);
        for(int j=0;j<=height_segment;j++){
            float width_angle=(2.0f/(float)height_segment*j)*PI;
            float x=cosf(width_angle)*radius;
            float y=sinf(width_angle)*radius;
            float z=cosf(height_angle)*user_radius;
            vertices.push_back(x*scale.x);
            vertices.push_back(y*scale.y);
            vertices.push_back(z*scale.z); 
            vertices.push_back(j/(float)height_segment);
            vertices.push_back(i/(float)width_segment); 
        }
    }
    vertices.push_back(0);            //sphere_end_poit
    vertices.push_back(0);            //
    vertices.push_back(-user_radius*scale.z); //
    vertices.push_back(0.5f);
    vertices.push_back(1.0f);
    for(int i=0;i<(height_segment+1)*(width_segment-1);i++){
        if(i==0){
            for(int j=0;j<height_segment;j++){
                indices.push_back(0);
                indices.push_back(j+1);
                indices.push_back(j+2);
            }
        }
        else if(i>=1+(height_segment+1)*(width_segment-2)){
            indices.push_back(1+(height_segment+1)*(width_segment-1));
            indices.push_back(i);
            indices.push_back(i+1);
        }
        else{
            indices.push_back(i);
            indices.push_back(i+1);
            indices.push_back(i+1+height_segment);

            indices.push_back(i+1+height_segment);
            indices.push_back(i+2+height_segment);
            indices.push_back(i+1);
        }
    }

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);//3~5//
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(), sizeof(float) * indices.size());

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

        auto image = Image::Load("./image/wood.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture= Texture::CreateFromImage(image.get());
    
    auto image2=Image::Load("./image/metal.jpg");
    m_texture2=Texture::CreateFromImage(image2.get());

    auto image3=Image::Load("./image/earth.png");
    m_texture3=Texture::CreateFromImage(image3.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture3->Get());

    m_program->Use();
    m_indexCount = (uint32_t)indices.size();
    m_vertices_count = (uint32_t)vertices.size();
    m_triangle_count = 2*height_segment*(width_segment-1);
    return true;
}

bool Context::Create_Cylinder(){
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    vertices.push_back(0);                    //top_circle_center_point
    vertices.push_back(0);                    //
    vertices.push_back(cylinder_height/2.0f*scale.z); //
    vertices.push_back(1.0f/2.0f);            
    vertices.push_back(1.0f);                 
    for(int i=0;i<=segment;i++){              //top_circle
        float angle=2.0f*PI/segment*i;
        float x=cylinder_top_radius*cosf(angle);
        float y=cylinder_top_radius*sinf(angle);
        float z=cylinder_height/2.0f;
        vertices.push_back(x*scale.x);
        vertices.push_back(y*scale.y);
        vertices.push_back(z*scale.z);
        vertices.push_back(i/(float)segment); /////texture 
        vertices.push_back(1.0f);             /////vertices
    }
    for(int i=0;i<=segment;i++){              //bottom_circle
        float angle=2.0f*PI/segment*i;
        float x=cylinder_bottom_radius*cosf(angle);
        float y=cylinder_bottom_radius*sinf(angle);
        float z=-cylinder_height/2.0f;
        vertices.push_back(x*scale.x);
        vertices.push_back(y*scale.y);
        vertices.push_back(z*scale.z);
        vertices.push_back(i/(float)segment);  /////texture 
        vertices.push_back(0);                 /////vertices
    }
    vertices.push_back(0);                     //bottom_circle_center_point
    vertices.push_back(0);                     //
    vertices.push_back(-cylinder_height/2.0f*scale.z); //
    vertices.push_back(1.0f/2.0f);
    vertices.push_back(0);

    for(int i=0;i<segment;i++){
        indices.push_back(i+1);
        indices.push_back(i+2);
        indices.push_back(0);
    }
    for(int i=0;i<segment;i++){
        indices.push_back(1+2*(segment+1));
        indices.push_back(i+2+segment);
        indices.push_back(i+3+segment);
    }
    for(int i=1;i<=segment;i++){
        indices.push_back(i);
        indices.push_back(i+1);
        indices.push_back(i+1+segment);

        indices.push_back(i+1+segment);
        indices.push_back(i+2+segment);
        indices.push_back(i+1);
    }

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);//3~5//

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(), sizeof(float) * indices.size());
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());
    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    auto image = Image::Load("./image/wood.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture= Texture::CreateFromImage(image.get());
    
    auto image2=Image::Load("./image/metal.jpg");
    m_texture2=Texture::CreateFromImage(image2.get());

    auto image3=Image::Load("./image/earth.png");
    m_texture3=Texture::CreateFromImage(image3.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture3->Get());

    m_program->Use();
    m_indexCount = (uint32_t)indices.size();
    m_vertices_count = (uint32_t)vertices.size();
    m_triangle_count = 4*segment;
    return true;
} 

void Context::Render(){ 
    if (ImGui::Begin("UI_WINDOW")){
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")){
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }

        ImGui::Separator();

        ImGui::LabelText("vertices","%d",m_vertices_count);
        ImGui::LabelText("triangle","%d",m_triangle_count);

        const char *solid_figure[] = {"CUBE", "SPHERE", "DONUT", "CYLINDER"};
        static const char *current_figure = "CUBE";
        if (ImGui::BeginCombo("figure", current_figure)){
            for (int n = 0; n < IM_ARRAYSIZE(solid_figure); n++){
                bool is_selected = (current_figure == solid_figure[n]);
                if (ImGui::Selectable(solid_figure[n], is_selected)){
                    current_figure = solid_figure[n];
                    for_call_Create_func_once=false; 
                    scale.x=1;  scale.y=1;  scale.z=1;
                }
            }
            ImGui::EndCombo();
        }
        if (current_figure == solid_figure[0]){//selected_cube
            if (!for_call_Create_func_once){
                for_call_Create_func_once = true;
                Create_Cube();
            }
        }
        else if (current_figure == solid_figure[1]){ //selected_Sphere
            if (!for_call_Create_func_once) {
                for_call_Create_func_once = true;
                Create_Sphere();
            }
            if (ImGui::DragFloat("radius", &user_radius, 0.5f, 1.0f, 50.0f) ||
                ImGui::DragInt("width_segment", &width_segment, 0.5f, 3, 50) ||
                ImGui::DragInt("height_segment", &height_segment, 0.5f, 3, 50))
                Create_Sphere();
            if(ImGui::DragFloat3("scale",glm::value_ptr(scale),0.05f,1.0f)) 
                Create_Sphere();
        }
        else if (current_figure == solid_figure[2]){ //selected_Donut
            if (!for_call_Create_func_once) {
                for_call_Create_func_once = true;
                Create_Donut();
            }
            if (ImGui::DragFloat("donut_radius", &donut_radius, 0.5f, 1.0f, 50.0f) ||
                ImGui::DragFloat("circle_radius", &circle_radius, 0.5f, 1.0f, 50.0f) ||
                ImGui::DragInt("donut_segment", &donut_segment, 0.5f, 3, 50) ||
                ImGui::DragInt("circle_segment", &circle_segment, 0.5f, 3, 50))
                Create_Donut();
            if(ImGui::DragFloat3("scale",glm::value_ptr(scale),0.05f,1.0f)) 
                Create_Donut();    
        }
        else if (current_figure == solid_figure[3]){ //selected_Cylinder
            if (!for_call_Create_func_once){
                for_call_Create_func_once = true;
                Create_Cylinder();
            }
            if (ImGui::DragFloat("top_radius", &cylinder_top_radius, 0.5f, 1.0f, 50.0f) ||
                ImGui::DragFloat("bottom_radius", &cylinder_bottom_radius, 0.5f, 1.0f, 50.0f) ||
                ImGui::DragFloat("height", &cylinder_height, 0.5f, 1.0f, 50.0f) ||
                ImGui::DragInt("segment", &segment, 0.5f, 3, 50))
                Create_Cylinder();
            if(ImGui::DragFloat3("scale",glm::value_ptr(scale),0.05f,1.0f))
                Create_Cylinder();
        }

        const char *texture[] = {"wood","metal","earth"};
        static const char *current_texture = "wood";

        if (ImGui::BeginCombo("texture", current_texture)){
            for (int n = 0; n < IM_ARRAYSIZE(texture); n++){
                bool is_selected = (current_texture == texture[n]);
                if (ImGui::Selectable(texture[n], is_selected))
                    current_texture = texture[n];
            }
            ImGui::EndCombo();
        }
        if (current_texture == texture[0])
            m_program->SetUniform("tex", 0);
        else if (current_texture == texture[1])
            m_program->SetUniform("tex", 1);
        else if (current_texture == texture[2])
            m_program->SetUniform("tex", 2);
        
        m_cameraFront =
            glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 30.0f);
        auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);        
        auto& pos =  glm::vec3(0.0f, 0.0f, 0.0f);
        auto model = glm::translate(glm::mat4(1.0f), pos);
       
        static bool check = false;       
        ImGui::Checkbox("rotate", &check);
        if(check){
            time_for_autorotation+=0.01f;
            rotation.x=time_for_autorotation*rotate_speed.x;
            rotation.y=time_for_autorotation*rotate_speed.y;
            rotation.z=time_for_autorotation*rotate_speed.z;
        } 
        ImGui::DragFloat3("rotation",glm::value_ptr(rotation),0.1f);

        model=glm::rotate(model, glm::radians(rotation.x),glm::vec3(1.0f, 0.0f, 0.0f));
        model=glm::rotate(model, glm::radians(rotation.y),glm::vec3(0.0f, 1.0f, 0.0f));
        model=glm::rotate(model, glm::radians(rotation.z),glm::vec3(0.0f, 0.0f, 1.0f));
        
        ImGui::DragFloat3("rotate_speed",glm::value_ptr(rotate_speed),0.01f);
        if(ImGui::Button("reset")){
            rotation.x=0;   rotation.y=0;   rotation.z=0;
            time_for_autorotation = 0;
            scale.x=1;  scale.y=1;  scale.z=1;   
            for_call_Create_func_once=false;
            check=false;
        }
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
    }
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //LINE_STRIP
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    //GL_TRIANGLES
}