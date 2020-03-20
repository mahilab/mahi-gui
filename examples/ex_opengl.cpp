#include <Mahi/Gui.hpp>
#include <mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

const char* vertex_shader = "#version 150\n"
                            "in vec3 vp;"
                            "void main() {"
                            "  gl_Position = vec4(vp, 1.0);"
                            "}";

const char* fragment_shader = "#version 150\n"
                              "out vec4 frag_colour;"
                              "void main() {"
                              "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
                              "}";

class OpenGlDemo : public Application {
public:
    OpenGlDemo() : Application(640, 480, "OpenGL Demo") {

        background_color = Grays::Black;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_shader, NULL);
        glCompileShader(vs);

        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_shader, NULL);
        glCompileShader(fs);

        shader = glCreateProgram();
        glAttachShader(shader, fs);
        glAttachShader(shader, vs);
        glLinkProgram(shader);
    }

    void update() {
        glUseProgram(shader);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    GLuint vbo = 0, vao = 0, vs, fs;
    GLuint shader;
    float  points[9] = {0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};
};

int main(int argc, char const* argv[]) {
    OpenGlDemo demo;
    demo.run();
    return 0;
}
