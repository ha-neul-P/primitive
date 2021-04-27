#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"

// ... context 클래스 선언
CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
    void Create_Cube();
    void Create_Cylinder(float upper_radius,float lower_radius,float height_cylinder,int segment);
    void Create_Sphere(float radius, int lati, int longi_seg);
    void Create_Donut(float inner_radius, float outer_radius, int circle_segment, int donut_segment);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;

    int m_indexCount { 6 };

    TextureUPtr m_texture;

    	
    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.5f, 0.2f, 0.0f) };
  
    // camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch{0.0f};
    float m_cameraYaw{0.0f};
    glm::vec3 m_cameraPos{glm::vec3(0.0f, 0.0f, 3.0f)};
    glm::vec3 m_cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 m_cameraUp{glm::vec3(0.0f, 1.0f, 0.0f)};

    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};

    //cylinder
    float m_upper_radius=0.5, m_lower_radius=0.5, m_height_cylinder=1;
    int m_cylinder_segment=16;

    //sphere
    float m_phrase_radius=1.0f;
    int m_lati_segment=8, m_longi_segment=4;

    //donut
    float m_inner_radius = 0.5f, m_outer_radius=1.0f;
    int m_circle_segment=4, m_donut_segment=4;
    int m_donut_draw_sup_num = 0;


    //animation check box
    bool check = false;

    int m_vertices_count = 0;
    int m_triangles_count = 0;
    char counter1 = (char)m_vertices_count;
    char counter2 = (char)m_vertices_count;
    int checked_shape = 0;
    int checked_texture = 0;

};

#endif // __CONTEXT_H__