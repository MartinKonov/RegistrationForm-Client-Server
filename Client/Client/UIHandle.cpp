#include "UIHandle.h"



UIHandle::UIHandle()
{
	window = nullptr;
}

void UIHandle::init(int width, int height)
{
	if (!glfwInit())
		return;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


	GLFWwindow* window = glfwCreateWindow(width, height, "Registration", NULL, NULL);
	if (window == NULL)
		return;
	this->window = window;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw("Unable to context to OpenGL");

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16.0f);
	io.FontDefault = io.Fonts->Fonts.back();
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::StyleColorsLight();

	
	ImVec4* colors = style.Colors;

	colors[ImGuiCol_WindowBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f); // background
	colors[ImGuiCol_Text] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);     // text 
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); //border shadow

}

bool UIHandle::run()
{
	bool toReturn = false;
	bool shouldClose = false;
	while (!glfwWindowShouldClose(window) && !shouldClose)
	{
		glfwPollEvents();
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		newFrame();

		if (!update())
		{
			shouldClose = true;
			toReturn = true;
		}

		render();
		glfwSwapBuffers(window);
	}
	shutDown();
	return toReturn;
}

void UIHandle::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

bool UIHandle::update()
{
	static bool openLoginWindow = true;

	ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
	ImGui::SetNextWindowSize(viewportSize);
	ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
	if (openLoginWindow)
	{
		ImGui::Begin("Login window", &openLoginWindow, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
		titleText("Login");
		static char usernameBuffer[1000];
		userPassInput(usernameBuffer,"Username: ", sizeof(usernameBuffer), 1);
		static char passwordBuffer[1000];
		userPassInput(passwordBuffer, "Password:  ", sizeof(passwordBuffer), 0);

		if (ImGui::Button("Enter", ImVec2(100, 40)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
			loginWindow = true;
			information.push_back(std::string(usernameBuffer));
			information.push_back(std::string(passwordBuffer));
			ImGui::End();
			return false;
		}

		ImGui::SameLine();
		if ((ImGui::Button("Register", ImVec2(225, 40)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))))
		{
			openLoginWindow = false;
		}

		ImGui::End();
	}
	else {
		ImGui::Begin("Register window", &openLoginWindow, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
		titleText("Register");
		static char usernameRegisterBuffer[1000];
		userPassInput(usernameRegisterBuffer, "Username: ", sizeof(usernameRegisterBuffer), 1);
		static char passwordRegisterBuffer[1000];
		userPassInput(passwordRegisterBuffer, "Password:  ", sizeof(passwordRegisterBuffer), 0);
		static char EmailRegisterBuffer[1000];
		userPassInput(EmailRegisterBuffer, "Email:         ", sizeof(EmailRegisterBuffer), 0, 1);

		if (ImGui::Button("Enter", ImVec2(100, 40)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
			loginWindow = false;
			information.push_back(std::string(usernameRegisterBuffer));
			information.push_back(std::string(passwordRegisterBuffer));
			information.push_back(std::string(EmailRegisterBuffer));
			ImGui::End();
			return false;
		}
		ImGui::SameLine();
		if ((ImGui::Button("Back", ImVec2(225, 40)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))))
		{
			openLoginWindow = true;
		}
		ImGui::End();

	}

}

void UIHandle::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIHandle::shutDown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

std::vector<std::string>* UIHandle::getInfo()
{
	return &information;
}

void UIHandle::titleText(const char* title)
{
	ImVec2 textWindowSize = ImGui::GetWindowSize();
	ImVec2 textSize = ImGui::CalcTextSize(title);
	ImVec2 textPosition((textWindowSize.x / 2) - textSize.x, 20);

	ImGui::SetCursorPos(textPosition);
	ImGui::SetWindowFontScale(2.0f);
	ImGui::Text(title);
}

void UIHandle::userPassInput(char* buffer,const char* text, int bufferLength, bool username, bool email)
{
	ImGui::SetWindowFontScale(1.0f);
	ImGui::Text("%s", text);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(250);

	switch (username)
	{
	case true:
		ImGui::InputText("##Username", buffer, bufferLength);
		break;
	case false:
		if(!email)
			ImGui::InputText("##Password", buffer, bufferLength, ImGuiInputTextFlags_Password);
	}


	if (email)
		ImGui::InputText("##Email", buffer, bufferLength);
}

void UIHandle::result(const char* result)
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		newFrame();

		ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
		ImGui::SetNextWindowSize(viewportSize);
		ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
		static bool success = true;
		ImGui::Begin("Login result", &success, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

		ImVec2 textWindowSize = ImGui::GetWindowSize();
		ImVec2 textSize = ImGui::CalcTextSize(result);
		ImVec2 textPosition(textWindowSize.x / 2 - 50, textWindowSize.y/2 - 10);

		ImGui::SetCursorPos(textPosition);
		ImGui::SetWindowFontScale(1.0f);
		ImGui::Text(result);

		ImGui::End();

		render();
		glfwSwapBuffers(window);
	}

}