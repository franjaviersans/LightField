# LightField #

A little project for rendering a light field with OpenGL

## Project Configuration ##
- Need GLFW, stb_image, dear imgui, GLM, and GLAD. 
- Add include folder with all .h files, lib folder with all .lib files and the .dll files in the root folder.


## External Libraries and Compilation

The following extern libraries/ projects were used in the develop of this program:

* [Dear Imgui](https://github.com/ocornut/imgui), for user interface.
* [GLFW 3.2](http://www.glfw.org/), for OpenGL context and window management.
* [glad](https://github.com/Dav1dde/glad), for OpenGL 4.5 extension loader.
* [OpenGL Mathematics (GLM)](http://glm.g-truc.net/), as a mathematic library.
* [STB Image 2.12](https://github.com/nothings/stb), for image loading.

To compile in Visual Studio, the header files must be put in a "include" folder as follows:

    - LightField/include/glad/ (glad files)
    - LightField/include/GLFW/ (GLFW files)
    - LightField/include/glm/ (GLM files)
	- LightField/include/KHR/ (file included with glad)
	- LightField/include/imgui/ (Dear Imgui files)
	- LightField/include/ (stb_image file)

The GLFW libs libraries must be set on a folder "lib" that must be organized as follows:

    - LightField/lib/x86/ (x86 libs)
	- LightField/lib/x64/ (x64 libs)

Add all the imgui cpp files and the glad.c to the project.