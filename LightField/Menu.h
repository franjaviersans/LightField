//**********************************************
//Menu class
//**********************************************

#ifndef MENU_H
#define MENU_H


#include "Definitions.h"

class Menu
{
public:
	Menu();
	~Menu();

	void destroy();
	void init(GLFWwindow * glfwWindow);
	void captureIO();
	void render();
	void createGUI();
	void show();
	void loadVolume();

private:
	bool m_show = true;
	bool m_show_test_window = false;
};

#endif
