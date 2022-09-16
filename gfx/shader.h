#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
	unsigned int id;
} shader;

void detectGLShaderError(char*, unsigned int);
void create_shader(shader*, char *, char *);
void copy_file(char **, const char*);
void detectGLShaderError();
void useShader(shader*);
void setBool(shader *, char *, bool);
void setInt(shader *, char *, int);
void setFloat(shader *, char *, float);
#endif