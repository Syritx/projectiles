#ifndef quad_h
#define quad_h



class Renderable {
public:
    Shader shader;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    unsigned int texture;
    
    float vertices[24];
    unsigned int indices[6];
    
    float size = 10.5f;
    glm::vec3 color;
    
    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        shader.use_shader();
        
        glBindVertexArray(vao);
        
        shader.set_mat4(glGetUniformLocation(shader.program, "projection"), projection);
        shader.set_mat4(glGetUniformLocation(shader.program, "view"), view);
        shader.set_mat4(glGetUniformLocation(shader.program, "model"), model);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/sizeof(vertices[0]) * sizeof(float), vertices, GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    
    void setPosition(glm::vec3 position, glm::vec3 rotation) {
        float v[] = {
            -size*rotation.x+position.x, size+position.y, -size*rotation.z+position.z, color.x, color.y, color.z,
             size*rotation.x+position.x, size+position.y,  size*rotation.z+position.z, color.x, color.y, color.z,
            -size*rotation.x+position.x,-size+position.y, -size*rotation.z+position.z, color.x, color.y, color.z,
             size*rotation.x+position.x,-size+position.y,  size*rotation.z+position.z, color.x, color.y, color.z
        };
        
        for (int i = 0; i < 24; i++) {
            vertices[i] = v[i];
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/sizeof(vertices[0]) * sizeof(float), vertices, GL_STATIC_DRAW);
    }
};



class Quad : public Renderable {

    
public:
    
    Quad(const char* VSS, const char* FSS, glm::vec3 position, glm::vec3 dir, glm::vec3 up, float r, float g, float b) {
        shader = Shader(VSS, FSS);
        
        glm::vec3 right = normalize(glm::cross(dir, up));
        
        std::cout << right.x << " " << right.y << " " << right.z << std::endl;
        
        float v[] = {
            -size+position.x, size+position.y, size+position.z, r, g, b,
             size+position.x, size+position.y, size+position.z, r, g, b,
            -size+position.x,-size+position.y, size+position.z, r, g, b,
             size+position.x,-size+position.y, size+position.z, r, g, b
        };
        
        color = vec3(r,g,b);
        
        unsigned int _in[] = {
            0, 1, 2,
            1, 3, 2
        };
        
        for (int i = 0; i < 24; i++) {
            vertices[i] = v[i];
        }
        for (int i = 0; i < 6; i++) {
            indices[i] = _in[i];
        }
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/sizeof(vertices[0]) * sizeof(float), vertices, GL_STATIC_DRAW);
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices)/sizeof(indices[0]) * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }
};

#endif /* triangle_h */
