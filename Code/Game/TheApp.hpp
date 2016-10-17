#pragma once

class TheApp
{
public:
    TheApp();
    TheApp(unsigned int width, unsigned int height);
    void AdvanceFrameNumber();
    void SetWindowWidth(unsigned int width);
    void SetWindowHeight(unsigned int height);
    unsigned int GetWindowWidth();
    unsigned int GetWindowHeight();

    static TheApp* instance;

private:
    unsigned int m_frameNumber;
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
};
