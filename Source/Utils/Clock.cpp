#include "Clock.h"

#include <Windows.h>
#include <imgui.h>

Clock::Clock()
{
    // Initialise variables
    paused_ = false;
    timeScale_ = 1.0f;
    time_ = 0.0f;
    deltaTime_ = 0.0f;
    realTime_ = 0.0f;
    realDeltaTime_ = 0.0f;
    
    // Get clock frequency and initial time stamp
    QueryPerformanceFrequency((LARGE_INTEGER*)&clockFrequency_);
    prevFrameTimestamp_ = getTimestamp();
}

// Return game pause state
bool Clock::paused() const
{
    return paused_;
}

// Set game pause state
void Clock::setPaused(bool paused)
{
    paused_ = paused;
}

// Return timescale
float Clock::timeScale() const
{
    return timeScale_;
}

// Set timescale
void Clock::setTimeScale(float timeScale)
{
    timeScale_ = timeScale;
}

// Return elapsed game time
float Clock::time() const
{
    return time_;
}

// Return time since last frame
float Clock::deltaTime() const
{
    return deltaTime_;
}

// Return elapsed real time
float Clock::realTime() const
{
    return realTime_;
}

// Return real time since last frame
float Clock::realDeltaTime() const
{
    return realDeltaTime_;
}

//On every frame start
void Clock::frameStart()
{
    // Get change in timestamp since last frame
    uint64_t timeStamp = getTimestamp();
    uint64_t deltaTimeStamp = timeStamp - prevFrameTimestamp_;
    prevFrameTimestamp_ = timeStamp;  

    // Update deltaTime values
    realDeltaTime_ = (float)deltaTimeStamp / (float)clockFrequency_;
    deltaTime_ = realDeltaTime_ * timeScale_ * (paused_ ? 0.0f : 1.0f);

    // Update time values
    realTime_ += realDeltaTime_;
    time_ += deltaTime_;
}

void Clock::drawDebugMenu()
{
    ImGui::Text("Time: %g", time_);
    ImGui::Text("Real Time: %g", realTime_);
    ImGui::Text("Delta Time: %g", deltaTime_);
    ImGui::Text("Real Delta Time: %g", realDeltaTime_);

    ImGui::SliderFloat("Time Scale", &timeScale_, 0.0f, 2.0f);

    if(ImGui::Button(paused() ? "Unpause" : "Pause"))
    {
        setPaused(!paused());
    }
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
}

// Return current time stamp using WINAPI
uint64_t Clock::getTimestamp() const
{
    uint64_t timeStamp;
    QueryPerformanceCounter((LARGE_INTEGER*)&timeStamp);

    return timeStamp;
}