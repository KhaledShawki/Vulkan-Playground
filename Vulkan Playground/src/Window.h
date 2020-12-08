#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

class Window
{
public: 
	Window(std::string title, const unsigned int width, const unsigned int height);
	~Window() = default;
	inline void* GetNativeWindow() const { return m_Window; }

	static std::shared_ptr<Window> GetWindow();

	bool ShouldClose();

	void OnUpdate();
	void Shutdown();

private:
	GLFWwindow* m_Window;

	inline static std::shared_ptr<Window> s_Window;
};