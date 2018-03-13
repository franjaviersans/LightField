#ifndef Definitions_H
#define Definitions_H

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
#define WORLD_COORD_LOCATION	0
#define COLOR_COORD_LOCATION	1
#define NORMAL_COORD_LOCATION	2
#define TEXTURE_COORD_LOCATION	3


#pragma comment (lib, "opengl32.lib")
#define PROJECT_DIR "./"
#ifdef WIN64_
	#if (_DEBUG)
		#pragma comment (lib,"./lib/x64/glfw3d.lib")
	#else
		#pragma comment (lib,"./lib/x64/glfw3.lib")
	#endif
#else
	#if (_DEBUG)
		#pragma comment (lib,"./lib/x86/glfw3d.lib")
	#else
		#pragma comment (lib,"./lib/x86/glfw3.lib")
	#endif	
#endif


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <imgui/imgui_impl_glfw_gl3.h>


enum textureIDS {
	TEXTURE_T =0
};

#endif