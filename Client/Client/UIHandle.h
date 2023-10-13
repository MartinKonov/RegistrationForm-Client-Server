#pragma once
#include<iostream>
#include <imgui.h>
#include<Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>
#include<map>
#include<string>
#include<vector>



class UIHandle
{
	GLFWwindow* window;
	const char* glsl_version = "#version 130";

public:
	std::vector<std::string> information;
	bool loginWindow;
	UIHandle();
	void init(int width = 800, int height = 600);
	bool run();
	void newFrame();
	bool update();
	void render();
	void shutDown();
	void result(const char* result);
	std::vector<std::string>* getInfo();

private:
	void titleText(const char* title);
	void userPassInput(char *buffer, const char* text, int bufferLength, bool username, bool email = false);
};