#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create(){
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow *window){
    if (!m_cameraControl)
        return;

    static float lastTime = 0.0f;
    float currentTime = glfwGetTime();
    if (lastTime == 0.0f)
        lastTime = currentTime;
    float deltaTime = currentTime - lastTime;
    float cameraSpeed = 4.0 * deltaTime;

    // update code
    lastTime = currentTime;
    //const float cameraSpeed = 0.005f;
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

void Context::Reshape(int width, int height){
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y){
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.4f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f) m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f) m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y){
    if (button == GLFW_MOUSE_BUTTON_RIGHT){
        if (action == GLFW_PRESS){
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE){
            m_cameraControl = false;
        }
    }
}

bool Context::Init(){
    Context::Create_Cube();

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

    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);

    auto image = Image::Load("./image/Wood.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(),
                image->GetHeight(), image->GetChannelCount());

    m_texture = Texture::CreateFromImage(image.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);

    return true;
}

void Context::Render(){

    if (ImGui::Begin("my first ImGui window")){
        if(ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor))){
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.1f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.1f, -89.0f, 89.0f);
        ImGui::Separator();
        if(ImGui::Button("reset camera")){
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f,0.0f,3.0f);
        }
        ImGui::Separator();
        ImGui::LabelText("#vertices",  &counter1);
        ImGui::LabelText("#triangles",  &counter2);
        const char* shape[] = { "Cube", "Cylinder", "Sphere", "Donut"};
        static int shape_item_current = 0; // static을 지우면 0으로 계속 초기화돼서 다른 것을 선택할수 없다.
        ImGui::Combo("Primitive", &shape_item_current, shape, IM_ARRAYSIZE(shape));
        checked_shape = shape_item_current;
        const char* texture[] = { "Wood", "Earth", "Metal"};
        static int texture_item_current = 0;
        ImGui::Combo("Texture", &texture_item_current, texture, IM_ARRAYSIZE(texture));
        checked_texture = texture_item_current;
        if(checked_shape==1){
            ImGui::DragInt("# segment", &m_cylinder_segment, 16,4,100);
            ImGui::DragFloat("upper radius", &m_upper_radius, 0.5f, 0.1f, 1.0f);
            ImGui::DragFloat("lower radius", &m_lower_radius, 0.5f, 0.1f, 1.0f);
            ImGui::DragFloat("height", &m_height_cylinder, 0.75f,0.5f,1.0f);
        }
        if(checked_shape==2){
            ImGui::DragInt("# lati.seg", &m_lati_segment, 16,4,100);
            ImGui::DragInt("# iongi.seg", &m_longi_segment, 16,4,100);
        }
        if(checked_shape==3){
            ImGui::DragInt("# outer segment", &m_donut_segment, 16,3,100);
            ImGui::DragInt("# inner segment", &m_circle_segment, 16,3,100);
            ImGui::DragFloat("outer radius", &m_outer_radius, 1.0f, 1.0f, 2.0f);
            ImGui::DragFloat("inner redius", &m_inner_radius, 0.5f,0.1f,0.9f);
        }
        ImGui::Separator();
        ImGui::DragFloat3("rotation", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat3("scale", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::Checkbox("animation", &check);
        ImGui::DragFloat3("rot speed", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::Button("reset transform");
    }
    ImGui::End();

    //shape
    if(checked_shape==0)
        Context::Create_Cube();
    if(checked_shape==1)
        Context::Create_Cylinder(m_upper_radius,m_lower_radius,m_height_cylinder,m_cylinder_segment);
    if(checked_shape==2)
        Context::Create_Sphere(m_phrase_radius,m_lati_segment,m_longi_segment);
    if(checked_shape==3)
        Context::Create_Donut(m_inner_radius,m_outer_radius,m_circle_segment,m_donut_segment);

    //texture
    auto image = Image::Load("./image/Wood.jpg");
    if(checked_texture == 0)
        image = Image::Load("./image/Wood.jpg");
    if(checked_texture == 1)
        image = Image::Load("./image/Earth.jpg");
    if(checked_texture == 2)
        image = Image::Load("./image/Metal.png");

    m_texture = Texture::CreateFromImage(image.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)m_width / (float)m_height, 0.01f, 20.0f);

    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f, 0.0f, 0.0f));
    if(check==true){
    model = glm::rotate(model,
        glm::radians((float)glfwGetTime() * 120.0f),
        glm::vec3(1.0f, 0.5f, 0.0f));
    }
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);
    glDrawElements(GL_LINE_STRIP, m_indexCount, GL_UNSIGNED_INT, 0);
    //GL_LINE_STRIP  GL_TRIANGLES
    
}

void Context::Create_Cube(){
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
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,GL_STATIC_DRAW, vertices, sizeof(float)*120);
    // GL_STATIC_DRAW = GL_ +  STATIC,DYNAMIC,STREAM,DRAW,COPY의 조합

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) *5, sizeof(float) * 3);

    m_indexBuffer=Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices, sizeof(uint32_t)*36);

    m_vertices_count = 24;
    m_triangles_count = 12;

    m_indexCount = 36;

}

void Context::Create_Cylinder(float upper_radius,float lower_radius,float height_cylinder,int segment = 0){
    std::vector<float> vertices_cylinder;
    std::vector<uint32_t> indices;
    
    const float pi = 3.141592f;

    vertices_cylinder.push_back(0.0f);
    vertices_cylinder.push_back(height_cylinder/2);
    vertices_cylinder.push_back(0.0f);
    


    for(int i=0; i<= segment; i++){
        float angle = (360.0f / segment * i) * pi / 180.0f;
        float x = cosf(angle) * upper_radius;
        float z = sinf(angle) * upper_radius;
        vertices_cylinder.push_back(x);
        vertices_cylinder.push_back(height_cylinder/2);
        vertices_cylinder.push_back(z);
        x = cosf(angle) * lower_radius;
        z = sinf(angle) * lower_radius;
        vertices_cylinder.push_back(x);
        vertices_cylinder.push_back(-height_cylinder/2);
        vertices_cylinder.push_back(z);
        
        
    }

    vertices_cylinder.push_back(0.0f);
    vertices_cylinder.push_back(-height_cylinder/2);
    vertices_cylinder.push_back(0.0f);
    
    
    //upper_circle
    for(int i=0; i<segment; i++){
        indices.push_back(0);
        indices.push_back(i*2+1);
        indices.push_back(i*2+3);
    }

    //wall
    for(int i=0; i<segment*2; i++){
        indices.push_back(i+1);
        indices.push_back(i+2);
        indices.push_back(i+3);
    }

    //lower_circle
    for(int i=0; i<segment; i++){
        indices.push_back(segment*2+3);
        indices.push_back(i*2+2);
        indices.push_back(i*2+4);
    }


    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices_cylinder.data(), sizeof(float) * vertices_cylinder.size());
    // GL_STATIC_DRAW = GL_ +  STATIC,DYNAMIC,STREAM,DRAW,COPY의 조합

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
    //m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, 3);
   
    m_indexBuffer=Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, 
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());

    m_indexCount = (int)indices.size();
    m_vertices_count = vertices_cylinder.size()/5;
    m_triangles_count = indices.size()/3;    
}

void Context::Create_Sphere(float radius, int lati_seg, int longi_seg){
    std::vector<float> vertices_sphere;
    std::vector<uint32_t> indices;
    
    const float pi = 3.141592f;
    float circle_radius = 0.0f;
    int floor_count = 0;
    int end_floor = (longi_seg+1)*(lati_seg-2);

    //indices[0]
    vertices_sphere.push_back(0.0f);
    vertices_sphere.push_back(1.0f);
    vertices_sphere.push_back(0.0f);
    //indices[1]
    vertices_sphere.push_back(0.0f);
    vertices_sphere.push_back(-1.0f);
    vertices_sphere.push_back(0.0f);
    


    for(int i=1; i< lati_seg; i++){
        float angle_a = (180.0f / lati_seg * i + 90.0f) * pi / 180.0f;
        float y = sinf(angle_a);
        circle_radius = cosf(angle_a);
        for(int j=0; j<=longi_seg; j++){
            float angle_b = (360.0f / longi_seg * j) * pi / 180.0f;
            float x =circle_radius * cosf(angle_b);
            float z =circle_radius * sinf(angle_b);
            vertices_sphere.push_back(x);
            vertices_sphere.push_back(y);
            vertices_sphere.push_back(z);
        }  
    }
    
    for(int i=1;i<=lati_seg; i++){
        if(i==1){
            for(int j=0;j<longi_seg;j++){
                indices.push_back(0);
                indices.push_back(j+2);
                indices.push_back(j+3);
            }
        }
        else if(i==lati_seg){
            for(int j=0;j<longi_seg;j++){
                indices.push_back(1);
                indices.push_back(j+end_floor+2);
                indices.push_back(j+end_floor+3);
            }
        }
        else{
            for(int j=0; j<longi_seg; j++){
                indices.push_back(j+2+floor_count);
                indices.push_back(j+3+floor_count);
                indices.push_back(j+longi_seg+3+floor_count);

                indices.push_back(j+longi_seg+3+floor_count);
                indices.push_back(j+longi_seg+4+floor_count);
                indices.push_back(j+3+floor_count);
            }
        }

        if(i>1)
        floor_count+=(longi_seg+1);        
    }
    

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices_sphere.data(),sizeof(float) * vertices_sphere.size());
    // GL_STATIC_DRAW = GL_ +  STATIC,DYNAMIC,STREAM,DRAW,COPY의 조합

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
    //m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, 3);
   
    m_indexBuffer=Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, 
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());

    m_indexCount = (int)indices.size();
    m_vertices_count = vertices_sphere.size()/5;
    m_triangles_count = indices.size()/3;
}

void Context::Create_Donut(float inner_radius, float outer_radius, int circle_segment, int donut_segment){
std::vector<float> vertices_donut;
    std::vector<uint32_t> indices;
    const float pi = 3.141592f;
    float circle_radius = (outer_radius-inner_radius)/2;
    float circle_center = (outer_radius+inner_radius)/2;



    for(int i=0; i<= donut_segment; i++){
        float angle_a = (360.0f / donut_segment * i) * pi / 180.0f;
        for(int j=0; j<=circle_segment; j++){
            float angle_b = (360.0f / circle_segment * j) * pi / 180.0f;
            float x = circle_center * cosf(angle_a) - circle_radius * cosf(angle_b) * cosf(angle_a);
            float y = circle_radius * sinf(angle_b);
            float z = circle_center * sinf(angle_a) - circle_radius * cosf(angle_b) * sinf(angle_a);
            //float x_texture=i/(float)donut_segment;
            //float y_texture=j/(float)circle_segment;
            vertices_donut.push_back(x);
            vertices_donut.push_back(y);
            vertices_donut.push_back(z);
            vertices_donut.push_back(i/(float)donut_segment);
            vertices_donut.push_back(j/(float)circle_segment);
        } 
    }
    
    for(int i=0,num=0; i< donut_segment; i++){
        for(int j=0; j<circle_segment; j++){
            indices.push_back(j+num);
            indices.push_back(j+1+num);
            indices.push_back(j+circle_segment+1+num);

            indices.push_back(j+circle_segment+1+num);
            indices.push_back(j+circle_segment+2+num);
            indices.push_back(j+1+num);
        }
        num+=(circle_segment+1);
    }
    
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices_donut.data() ,sizeof(float) * vertices_donut.size());
    // GL_STATIC_DRAW = GL_ +  STATIC,DYNAMIC,STREAM,DRAW,COPY의 조합

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, sizeof(float)*3);
   
    m_indexBuffer=Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, 
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());

    m_indexCount = (int)indices.size();
    
    

}