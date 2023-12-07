#pragma once
#include<glad/glad.h>
#include<glfw/glfw3.h>

#include <iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"
typedef unsigned int uint;

struct ffRGBA
{
	unsigned char mb;
	unsigned char mg;
	unsigned char mr;
	unsigned char ma;

	ffRGBA(unsigned char r = 255, unsigned char g = 255,
		unsigned char b = 255, unsigned char a = 255)
	{
		mr = r;
		mg = g;
		mb = b;
		ma = a;
	}
};