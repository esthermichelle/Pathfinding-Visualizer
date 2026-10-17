#pragma once

#include "VisualizerEngine.hpp"
#include "raylib.h"
#include <string>

class UIManager
{
public:
    UIManager(int panelX, int panelY, int panelWidth, int panelHeight);

    // Draw the UI panel; returns true if any button was clicked
    void draw(const VisualizerEngine &engine) const;

    // Handle clicks and return whether the engine state should change
    void handleInput(VisualizerEngine &engine);

private:
    int panelX;
    int panelY;
    int panelWidth;
    int panelHeight;

    // Button helper
    struct Button
    {
        Rectangle rect;
        const char *label;
        Color bgColor;
        Color textColor;
        bool enabled;
    };

    bool drawButton(const Button &btn) const;
    bool isButtonClicked(const Button &btn) const;
};
