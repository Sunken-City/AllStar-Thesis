#include "Game/TheApp.hpp"

TheApp* TheApp::instance = nullptr;

//-----------------------------------------------------------------------------------
void TheApp::SetWindowWidth(unsigned int width)
{
	m_windowWidth = width;
}

//-----------------------------------------------------------------------------------
void TheApp::SetWindowHeight(unsigned int height)
{
	m_windowHeight = height;
}

//-----------------------------------------------------------------------------------
unsigned int TheApp::GetWindowWidth()
{
	return m_windowWidth;
}

//-----------------------------------------------------------------------------------
unsigned int TheApp::GetWindowHeight()
{
	return m_windowHeight;
}

//-----------------------------------------------------------------------------------
TheApp::TheApp() : m_frameNumber(0)
{
}

//-----------------------------------------------------------------------------------
TheApp::TheApp(unsigned int width, unsigned int height) : m_windowWidth(width), m_windowHeight(height) , m_frameNumber(0)
{
}

//-----------------------------------------------------------------------------------
void TheApp::AdvanceFrameNumber()
{
	++m_frameNumber;
}