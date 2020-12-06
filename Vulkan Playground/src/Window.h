#pragma once
#include <GLFW/glfw3.h>
#include <string>


class Window
{
public: 
	Window(std::string title, const unsigned int width, const unsigned int height);
	~Window() = default;
	inline void* GetNativeWindow() const { return m_Window; }

	bool ShouldClose();

	void OnUpdate();
	void Shutdown();

private:
	GLFWwindow* m_Window;

};