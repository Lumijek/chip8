#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gfx/shader.h"
#include "src/chip8.h"

uint8_t keys[16] = {
    GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4,
    GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_R,
    GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F,
    GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_V
};
void ErrorCallback(int i,
    const char * err_str) {
    printf("GLFW Error: %s\n", err_str);
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static bool wireframe = false;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if(!wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        wireframe = !wireframe;
    }
}
void load_rom(chip8* c8, const char *rom_filename) {
    long rom_length;
    uint8_t *rom_buffer;

    FILE *rom = fopen(rom_filename, "rb");
    fseek(rom, 0, SEEK_END);
    rom_length = ftell(rom); 
    rewind(rom);

    rom_buffer = (uint8_t*) malloc(sizeof(uint8_t) * rom_length);
    fread(rom_buffer, sizeof(uint8_t), rom_length, rom); 
    if ((0xFFF - 0x200) >= rom_length) {
        for(int i = 0; i < rom_length; i++) {
            c8->memory[i + 0x200] = rom_buffer[i];
        }
    }
    fclose(rom);
    free(rom_buffer);
}
int main() {
    chip8 c8;
    setup_chip8(&c8);
    load_rom(&c8, "../ROMS/testrom");
    // Image
    int image_width = 64;
    int image_height = 32;
    glfwSetErrorCallback(ErrorCallback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    GLFWwindow * window = glfwCreateWindow(image_width, image_height, "CHIP 8", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD!\n");
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    float vertices[] = {
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  // top right
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom left
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    shader *s = malloc(sizeof(shader));
    char *vertexShaderFile = "../shaders/shader.vs";
    char *fragmentShaderFile = "../shaders/shader.fs";
    create_shader(s, vertexShaderFile, fragmentShaderFile);
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, & VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    GLubyte *checkImage = malloc(image_width * image_height * 3 * sizeof(GLubyte));
    int cp = 0;
    int c = 0;
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            int ind = (i * image_width + j) * 3;
            checkImage[ind++] = c;
            checkImage[ind++] = c;
            checkImage[ind++] = c;
        }
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
    glGenerateMipmap(GL_TEXTURE_2D);


    useShader(s);


    double dt;
    double lastTime = glfwGetTime();

    glfwSwapInterval(0); // disable v-sync

    while (!glfwWindowShouldClose(window)) {
        dt = glfwGetTime() - lastTime;
        //printf("%f\n", 1.0 / dt);
        lastTime = glfwGetTime();
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // CHIP 8 INSTRUCTIONS
        fetch_opcode(&c8);
        execute(&c8);
        
        for(int i = 0; i < 16; i++) {
            c8.keypad[i] = glfwGetKey(window, keys[i]) == GLFW_PRESS;
        }

        // CHANGE PIXEL BUFFER ACCORDINGLY
        for(int i = 0; i < image_height;i++) {
            for(int j = 0; j < image_width; j++) {
                if(c8.display[i * image_width + j]) {
                    int ind = (i * image_width + j) * 3;
                    checkImage[ind++] = 255;
                    checkImage[ind++] = 255;
                    checkImage[ind++] = 255;
                }
            }
        }
        
        
        // GRAPHICS RENDERING
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    };
    glfwTerminate();
    return 0;
}
