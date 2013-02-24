#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "color_table.h"
#include "gl3.h"
#include "utils.h"

#include "hexmesh.hpp"

SDL_Window *g_window;
SDL_GLContext g_glContext;

bool g_quit;

void lookup_color(size_t index, float &r, float &g, float &b) {
    index = index % color_table_num_colors;
    r = 1.0f * color_table[index + 0] / 255.0f;
    g = 1.0f * color_table[index + 1] / 255.0f;
    b = 1.0f * color_table[index + 2] / 255.0f;
}

void setup(int w, int h) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32 );

    g_window = SDL_CreateWindow(
       "<_>",
       SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
       w, h,
       SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
   );

    if (not g_window) {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GLContext g_glContext = SDL_GL_CreateContext(g_window);
    if (not g_glContext) {
        fprintf(stderr, "Unable to create opengl context: %s\n", SDL_GetError());
        exit(1);
    }

    glViewport(0, 0, w, h);
}

void foo() {
    char *shader_src;
    size_t shader_len;

    if ((shader_src = (char *) file_contents("shader-src/blah.vert", &shader_len))) {
        std::cout << "shader source (" << shader_len << "):" << std::endl << shader_src << std::endl;
        free(shader_src);
    }
}

GLuint make_shader(const char * path, GLenum shader_type) {
    size_t source_len;
    GLchar *source = (GLchar *) file_contents(path, &source_len);
    if (not source)  return 0;

    GLuint shader = glCreateShader(shader_type);
    if (not shader)  return 0;

    GLint tmp_len = source_len;
    glShaderSource(shader, 1, (const GLchar **) &source, &tmp_len);
    free(source);

    glCompileShader(shader);
    GLint shader_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);

    if (not shader_ok) {
        fprintf(stderr, "failed to compile shader %s\n", path);

        GLint log_length;
        GLchar *log;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        log = (GLchar *) malloc(log_length);
        glGetShaderInfoLog(shader, log_length, NULL, log);
        fprintf(stderr, "%s", log);
        free(log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int main (int argc, char **argv) {
    setup(640, 480);
    foo();

    glClearColor(0, 0, 0, 0);

    g_quit = false;

    HexMesh hexmesh(5, 1.0f);
    gl_assert_ok();

    GLuint vshader = make_shader("shader-src/blah.vert", GL_VERTEX_SHADER);
    if (not vshader) exit(1);

    GLuint fshader = make_shader("shader-src/blah.frag", GL_FRAGMENT_SHADER);
    if (not fshader) exit(1);

    GLint program_ok;
    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "color");
    glLinkProgram(program);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (not program_ok) {
        fprintf(stderr, "failed to link\n");

        GLint log_length;
        char *log;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        log = (char *) malloc(log_length);
        glGetProgramInfoLog(program, log_length, NULL, log);
        fprintf(stderr, "%s", log);
        free(log);

        glDeleteProgram(program);
        exit(1);
    }

    glUseProgram(program);

    GLint loc_modelview, loc_projection;

    loc_modelview = glGetUniformLocation(program, "modelview");
    loc_projection = glGetUniformLocation(program, "projection");

    int w, h;
    SDL_GetWindowSize(g_window, &w, &h);

    glm::vec3 camera_offset(0.0f, -1.0f, 12.0f);
    glm::vec3 camera_focus(hexmesh.get_centre());
    glm::vec3 camera_pos(camera_focus + camera_offset);

    glm::mat4 modelview = glm::lookAt(camera_pos, camera_focus, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(80.0f, (float) w / (float) h, 0.1f, 100.0f);

    glUniformMatrix4fv(loc_modelview, 1, GL_FALSE, glm::value_ptr(modelview));
    glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projection));

    while(not g_quit) {
        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                case SDL_KEYDOWN:
                case SDL_MOUSEBUTTONDOWN:
                    g_quit = true;
                    break;

                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        fprintf(stderr, "received window resize event\n");
                        w = e.window.data1;
                        h = e.window.data2;
                        glViewport(0, 0, w, h);
                        projection = glm::perspective(80.0f, (float) w / (float) h, 0.1f, 100.0f);
                        glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projection));
                    }
                    break;

                default:
                    // do nothing for now
                    break;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hexmesh.draw();

        gl_assert_ok();
        
        SDL_GL_SwapWindow(g_window);
    }

    std::cout << "received event, exiting..." << std::endl;

















    exit(0);
}
