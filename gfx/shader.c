#include "shader.h"

void detectGLShaderError(char *name, unsigned int shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error with %s shader: %s!\n", name, infoLog);
    }
}
void create_shader(shader *s, char *vertexFile, char *fragmentFile) {
	char* vertexShaderSource;
	char* fragmentShaderSource;
	copy_file(&vertexShaderSource, vertexFile);
	copy_file(&fragmentShaderSource, fragmentFile);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char *const *)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    detectGLShaderError("vertex", vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char *const *)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    detectGLShaderError("fragment", fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    detectGLShaderError("shader program", shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    s->id = shaderProgram;
}

void useShader(shader *s) {
    glUseProgram(s->id);
}

void setBool(shader *s, char *name, bool value) {         
    glUniform1i(glGetUniformLocation(s->id, name), value); 
}
void setInt(shader *s, char *name, int value) { 
    glUniform1i(glGetUniformLocation(s->id, name), value); 
}
void setFloat(shader *s, char *name, float value) { 
    glUniform1f(glGetUniformLocation(s->id, name), value); 
}

void copy_file(char **c, const char* filename) {

    FILE* f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    string[fsize] = 0;
    *c = malloc(strlen(string) + 1);
    memcpy(*c, string, strlen(string) + 1);
}