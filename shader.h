#ifndef shader_h
#define shader_h

class Shader {
private:
    int vertexShader;
    int fragmentShader;
    
public:
    int program;
    
    Shader(const char* vertexShaderSource = nullptr, const char* fragmentShaderSource = nullptr) {
        
        if (vertexShaderSource && fragmentShaderSource) {
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(vertexShader);
            glCompileShader(fragmentShader);
            
            program = glCreateProgram();
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);
            
            std::cout << program << std::endl;
        }
    }
    
    void set_mat4(int location, glm::mat4 &matrix) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
    }
    
    void use_shader() {
        glUseProgram(program);
    }
};

#endif /* shader_h */
