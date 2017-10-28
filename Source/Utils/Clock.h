#pragma once

#include <cstdint>

#include "Application.h"

class Clock : public ApplicationModule
{
public:
    Clock();
    
    // Return and set game pause state
    bool paused() const;
    void setPaused(bool paused);

    // Return and set timescale
    float timeScale() const;
    void setTimeScale(float timeScale);

    // Return values for game running time and game time since last frame
    float time() const;
    float deltaTime() const;

    // Return values for real running time and real time since last frame
    float realTime() const;
    float realDeltaTime() const;

    //Called on every frame
    void frameStart();

    // Draws the clock section of the debug menu
    void drawDebugMenu() override;

private:
    // Paused flag
    bool paused_;

    // Time scale value
    float timeScale_;

    // Game running time
    float time_;

    // Time since last frame
    float deltaTime_;

    // Real running time
    float realTime_;

    //Real time since last frame
    float realDeltaTime_;
    
    // Timestamp of previous frame
    uint64_t prevFrameTimestamp_;

    // Clock frequency
    uint64_t clockFrequency_;

    // Returns current time stamp
    uint64_t getTimestamp() const;
};