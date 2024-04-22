#pragma once
#include "UIPanel.h"

class SelectUIPanel : public UIPanel
{
private:
    void initializeButtons();

public:
    SelectUIPanel(Vector2i res);
    void virtual draw(RenderWindow& window);
};
