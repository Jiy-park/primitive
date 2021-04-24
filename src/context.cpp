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
    else if (action == GLFW_RELEASE) {
        m_cameraControl = false;
    }
  }
}

bool Context::Create_Cube(){

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // int donut_segment=40;
    // int circle_segment=40;
    // float donut_radius=5;
    // float circle_radius=2;

    for(int i=0;i<=donut_segment;i++){
        float donut_angle=2*PI/donut_segment*i;
        float donut_x=donut_radius*cosf(donut_angle);
        float donut_y=donut_radius*sinf(donut_angle);
        for(int j=0;j<=circle_segment;j++){
            float circle_angle=2*PI/circle_segment*j;
            float circle_x = donut_x + circle_radius * cosf(donut_angle) * cosf(circle_angle);
            float circle_y = donut_y + circle_radius * sinf(donut_angle) * cosf(circle_angle);
            float circle_z = circle_radius * sinf(circle_angle);
            vertices.push_back(circle_x);//circle_x
            vertices.push_back(circle_y);//circle_y
            vertices.push_back(circle_z);//circle_z
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
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    //m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);//3~5//
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

    glClearColor(0.5f, 1.0f, 0.8f, 0.5f);

    auto image = Image::Load("./image/container.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);

    m_indexCount = (uint32_t)indices.size();

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
            float circle_x = donut_x + circle_radius * cosf(donut_angle) * cosf(circle_angle);
            float circle_y = donut_y + circle_radius * sinf(donut_angle) * cosf(circle_angle);
            float circle_z = circle_radius * sinf(circle_angle);
            vertices.push_back(circle_x);//circle_x
            vertices.push_back(circle_y);//circle_y
            vertices.push_back(circle_z);//circle_z
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
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    //m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);//3~5//
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
    glClearColor(0.5f, 1.0f, 0.8f, 0.5f);
    auto image = Image::Load("./image/container.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture = Texture::CreateFromImage(image.get());
    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);
    auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);
    auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    m_indexCount = (uint32_t)indices.size();
    return true;
}

bool Context::Create_Sphere(){

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    vertices.push_back(0);           //sphere_start_poit
    vertices.push_back(0);           //
    vertices.push_back(user_radius); //
    for(int i=1;i<width_segment;i++){
        float height_angle=PI*i/(float)width_segment;
        if(i==width_segment) height_angle=0;
        float radius=user_radius*sinf(height_angle);
        for(int j=0;j<=height_segment;j++){
            float width_angle=(2.0f/(float)height_segment*j)*PI;
            float x=cosf(width_angle)*radius;
            float y=sinf(width_angle)*radius;
            float z=cosf(height_angle)*user_radius;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z); 
        }
    }
    vertices.push_back(0);            //sphere_end_poit
    vertices.push_back(0);            //
    vertices.push_back(-user_radius); //
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
        m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        //m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);//3~5//
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
        glClearColor(0.5f, 1.0f, 0.8f, 0.5f);
        auto image = Image::Load("./image/container.jpg");
        if (!image)
            return false;
        SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
        m_texture = Texture::CreateFromImage(image.get());
        auto image2 = Image::Load("./image/awesomeface.png");
        m_texture2 = Texture::CreateFromImage(image2.get());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture->Get());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
        m_program->Use();
        m_program->SetUniform("tex", 0);
        m_program->SetUniform("tex2", 1);
        auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        m_indexCount = (uint32_t)indices.size();
        return true;
}

bool Context::Create_Cylinder(){
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    vertices.push_back(0);           //top_circle_center_point
    vertices.push_back(0);           //
    vertices.push_back(cylinder_height/2.0f); //
    for(int i=0;i<=segment;i++){//top_circle
        float angle=2.0f*PI/segment*i;
        float x=cylinder_top_radius*cosf(angle);
        float y=cylinder_top_radius*sinf(angle);
        float z=cylinder_height/2.0f;
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }
    for(int i=0;i<=segment;i++){//bottom_circle
        float angle=2.0f*PI/segment*i;
        float x=cylinder_bottom_radius*cosf(angle);
        float y=cylinder_bottom_radius*sinf(angle);
        float z=-cylinder_height/2.0f;
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }
    vertices.push_back(0);            //bottom_circle_center_point
    vertices.push_back(0);            //
    vertices.push_back(-cylinder_height/2.0f); //
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
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    //m_vertexLayout->SetAttrib(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*5,sizeof(float)*3);//3~5//
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
    glClearColor(0.5f, 1.0f, 0.8f, 0.5f);
    auto image = Image::Load("./image/container.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    m_texture = Texture::CreateFromImage(image.get());
    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);
    auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);
    auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    m_indexCount = (uint32_t)indices.size();
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
     
        const char *solid_figure[] = {"CUBE","SPHERE","DONUT","CYLINDER"};
        static const char *current_figure = "CUBE";

        if (ImGui::BeginCombo("##combo", current_figure)){
            for (int n = 0; n < IM_ARRAYSIZE(solid_figure); n++){
                bool is_selected = (current_figure == solid_figure[n]);
                if (ImGui::Selectable(solid_figure[n], is_selected)){
                    current_figure = solid_figure[n];
                    if(n==1) Create_Sphere();

                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
       // if (items[0] && is_selected)  Create_Sphere();    
       // else if (items[1] && is_selected)  Init();
       // else if (items[2] && is_selected)  Create_Cylinder();              
        if(current_figure == solid_figure[0]){
            if (ImGui::DragFloat("user_radius", &user_radius, 0.5f, 1.0f, 50.0f) ||
                    ImGui::DragInt("width_segment", &width_segment, 0.5f, 3, 50) ||
                    ImGui::DragInt("height_segment", &height_segment, 0.5f, 3, 50))
                Create_Cube();/////////////////////////////////큐브 코드 작성할것
        }
        if(current_figure == solid_figure[1]){
            if (ImGui::DragFloat("user_radius", &user_radius, 0.5f, 1.0f, 50.0f) ||
                    ImGui::DragInt("width_segment", &width_segment, 0.5f, 3, 50) ||
                    ImGui::DragInt("height_segment", &height_segment, 0.5f, 3, 50))
                Create_Sphere();
        }
        if(current_figure == solid_figure[2]){
            if (ImGui::DragFloat("donut_radius", &donut_radius, 0.5f, 1.0f, 50.0f) ||
                    ImGui::DragFloat("circle_radius", &circle_radius, 0.5f, 1.0f, 50.0f) ||
                    ImGui::DragInt("donut_segment", &donut_segment, 0.5f, 3, 50) ||
                    ImGui::DragInt("circle_segment", &circle_segment, 0.5f, 3, 50))           
                Create_Donut();
        }
        if(current_figure == solid_figure[3]){;
            if (ImGui::DragFloat("cylinder_top_radius", &cylinder_top_radius, 0.5f, 1.0f, 50.0f) ||
                    ImGui::DragFloat("cylinder_bottom_radius", &cylinder_bottom_radius, 0.5f, 1.0f, 50.0f) ||
                    ImGui::DragFloat("cylinder_height", &cylinder_height, 0.5f, 1.0f, 50.0f) ||
                    ImGui::DragInt("segment", &segment, 0.5f, 3, 50))
                Create_Cylinder();
        }
        static bool check=true;
        ImGui::Checkbox("rotate",&check);
        


    }
    ImGui::End();

    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f)
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront =
        glm::rotate(glm::mat4(1.0f),glm::radians(m_cameraYaw),glm::vec3(0.0f, 1.0f, 0.0f)) *              
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch),glm::vec3(1.0f, 0.0f, 0.0f)) *              
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f),
    (float)m_width / (float)m_height, 0.01f, 30.0f);

    auto view = glm::lookAt( m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    for (size_t i = 0; i < cubePositions.size(); i++){
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * 0.00001f + 20.0f * (float)i),
                            glm::vec3(1.0f, 0.5f, 0.0f));     
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
    } 

    //LINE_STRIP
    glDrawElements(GL_LINE_STRIP, m_indexCount, GL_UNSIGNED_INT, 0);
    //GL_TRIANGLES
}