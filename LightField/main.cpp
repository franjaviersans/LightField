#include "Definitions.h"
#include "GLSLProgram.h"
#include "TextureManager.h"
#include "VBOQuad.h" 
#include "Menu.h"
#include "LightField.h"



namespace glfwFunc
{
	GLFWwindow* glfwWindow;
	Menu menu;
	int WINDOW_WIDTH = 1024;
	int WINDOW_HEIGHT = 768;
	std::string strNameWindow = "Light Fields";

	LightField mLF;

	const float NCP = 1.0f;
	const float FCP = 10.0f;
	const float fAngle = 45.f * (3.14f / 180.0f); //In radians

	glm::mat4 scale = glm::mat4();
	
	float color[]={1,1,1};
	bool pintar = false;

	GLSLProgram m_program;
	glm::mat4x4 mProjMatrix, mModelViewMatrix, mMVP;

	//Variables to do rotation
	glm::quat quater = glm::quat(), q2; //begin with a diagonal view
	glm::mat4x4 RotationMat = glm::mat4x4();
	float angle = 0;
	float *vector=(float*)malloc(sizeof(float)*3);
	double lastx, lasty;
	bool pres = false;

	int corteX = 0, corteY = 0;

	///< Callback function used by GLFW to capture some possible error.
	void errorCB(int error, const char* description)
	{
		printf("%s\n",description );
	}


	///
	/// The keyboard function call back
	/// @param window id of the window that received the event
	/// @param iKey the key pressed or released
	/// @param iScancode the system-specific scancode of the key.
	/// @param iAction can be GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
	/// @param iMods Bit field describing which modifier keys were held down (Shift, Alt, & so on)
	///
	void keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods)
	{
		if (iAction == GLFW_PRESS)
		{
			switch (iKey)
			{
				case GLFW_KEY_ESCAPE:
				case GLFW_KEY_Q:
					glfwSetWindowShouldClose(window, GL_TRUE);
					break;
				case GLFW_KEY_A:
					corteX -= 1;
					if (corteX < 0) corteX = 0;
					break;
				case GLFW_KEY_D:
					corteX += 1;
					if (corteX > 15) corteX = 15;
					break;
				case GLFW_KEY_W:
					corteY += 1;
					if (corteY > 15) corteY = 15;
					break;
				case GLFW_KEY_S:
					corteY -= 1;
					if (corteY < 0) corteY = 0;
					break;
				case GLFW_KEY_SPACE:
				{
					
				}
					break;
				case GLFW_KEY_M:
					menu.show();
					break;
			}
		}
	}

	inline int TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos)
	{ 
	
		if (pres){
			//Rotation
			float dx = float(xpos - lastx);
			float dy = float(ypos - lasty);

			if (!(dx == 0 && dy == 0)){
				//Calculate angle and rotation axis
				float angle = sqrtf(dx*dx + dy*dy) / 50.0f;

				//Acumulate rotation with quaternion multiplication
				q2 = glm::angleAxis(angle, glm::normalize(glm::vec3(dy, dx, 0.0f)));
				quater = glm::cross(q2, quater);

				lastx = xpos;
				lasty = ypos;
			}
		}
		return false;
	}

	int TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
	{ 
		pres = false;

		double x, y;   
		glfwGetCursorPos(window, &x, &y);  

			
		if(button == GLFW_MOUSE_BUTTON_LEFT){
			if(action == GLFW_PRESS){
				lastx = x;
				lasty = y;
				pres = true;
			}

			return true;
		}else{
			if(action == GLFW_PRESS){			
			}
				
		}

		return true;
	}
	
	///< The resizing function
	void resizeCB(GLFWwindow* window, int iWidth, int iHeight)
	{

		WINDOW_WIDTH = iWidth;
		WINDOW_HEIGHT = iHeight;

		if(iHeight == 0) iHeight = 1;
		float ratio = iWidth / float(iHeight);
		glViewport(0, 0, iWidth, iHeight);

		mProjMatrix = glm::perspective(float(fAngle), ratio, NCP, FCP);
	//	mProjMatrix = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,5.0f);

		m_program.use();
		{
			m_program.setUniform("mProjection", mProjMatrix);
		}	
	}

	///< The main rendering function.
	void draw()
	{
		//create the menus to display!!!
		menu.createGUI();

		glClearColor(0.15f, 0.15f, 0.15f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RotationMat = glm::mat4_cast(glm::normalize(quater));

		mModelViewMatrix =  glm::translate(glm::mat4(), glm::vec3(0.0f,0.0f,-2.0f)) * 
							RotationMat * scale;
		mat4 aux = RotationMat * scale;

		mMVP = mProjMatrix * mModelViewMatrix;

		//Draw a Cube
		m_program.use();
		{
			//bind texture
			glActiveTexture(GL_TEXTURE0);
			TextureManager::Inst().BindTexture(LIGHT_FILED_TEXTURE);


			m_program.setUniform("mModelView", mModelViewMatrix);
			m_program.setUniform("corteX", corteX);
			m_program.setUniform("corteY", corteY);
			printf("%d %d\n ", corteX, corteY);
			VBOQuad::Instance().Draw();
		}



		menu.render();

		glfwSwapBuffers(glfwWindow);
	}


	///
	/// Init all data and variables.
	/// @return true if everything is ok, false otherwise
	///
	bool initialize()
	{
		//Init GLEW
		//load with glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			printf("Something went wrong!\n");
			exit(EXIT_FAILURE);
		}
		printf("OpenGL version: %s\n", glGetString(GL_VERSION));
		printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));

		//check if the images inside the directory have the correct format
		mLF.setPath("./images");
		if (!mLF.checkDirectory())
		{
			printf("Problem with the images\n");
			exit(EXIT_FAILURE);
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);


		// send window size events to AntTweakBar
		glfwSetWindowSizeCallback(glfwWindow, resizeCB);
		glfwSetMouseButtonCallback(glfwWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
		glfwSetCursorPosCallback(glfwWindow, (GLFWcursorposfun)TwEventMousePosGLFW3);
		glfwSetKeyCallback(glfwWindow, (GLFWkeyfun)keyboardCB);


		//load the shaders
		try{
			m_program.compileShader("shaders/basic.vert", GLSLShader::VERTEX);
			m_program.compileShader("shaders/basic.frag", GLSLShader::FRAGMENT);

			m_program.link();
		}
		catch (GLSLProgramException & e) {
			printf("%s\n", e.what());
			char s[1000];
			scanf("%s",s);
			exit(EXIT_FAILURE);
		}

		//read all the images from the directory
		if (!mLF.readLightField()) {
			printf("Problem reading an image\n");
			char s[1000];
			scanf("%s", s);
			exit(EXIT_FAILURE);
		}

		//init menu
		menu.init(glfwWindow);

		return true;
	}


	/// Here all data must be destroyed + glfwTerminate
	void destroy()
	{
		TextureManager::Inst().UnloadAllTextures();
		// Cleanup
		menu.destroy();
		glfwTerminate();
		glfwDestroyWindow(glfwWindow);
	}
}



int main(int argc, char** argv)
{

	glfwSetErrorCallback(glfwFunc::errorCB);
	if (!glfwInit())	exit(EXIT_FAILURE);
	glfwFunc::glfwWindow = glfwCreateWindow(glfwFunc::WINDOW_WIDTH, glfwFunc::WINDOW_HEIGHT, glfwFunc::strNameWindow.c_str(), NULL, NULL);
	if (!glfwFunc::glfwWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(glfwFunc::glfwWindow);
	if(!glfwFunc::initialize()) exit(EXIT_FAILURE);
	glfwFunc::resizeCB(glfwFunc::glfwWindow, glfwFunc::WINDOW_WIDTH, glfwFunc::WINDOW_HEIGHT);	//just the 1st time

	


	// main loop!
	while (!glfwWindowShouldClose(glfwFunc::glfwWindow))
	{

		//capture events with menu
		glfwFunc::menu.captureIO();

		glfwPollEvents();	//or glfwWaitEvents()


		glfwFunc::draw();
		
	}

	glfwFunc::destroy();
	return EXIT_SUCCESS;
}
