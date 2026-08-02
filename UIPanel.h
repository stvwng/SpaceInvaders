#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"
#include <memory>


class UIPanel
{
    private:
        sf::RectangleShape m_UIPanel;
        bool m_Hidden = false;
        std::vector<std::shared_ptr<Button>> m_Buttons;

    protected:
        // The panel's own size in pixels. Panel-local coordinates are 1:1 with
        // pixels because m_View is sized to the panel, so a subclass can lay
        // text out against these directly.
        float m_Width = 0;
        float m_Height = 0;

        float m_ButtonWidth = 0;
        float m_ButtonHeight = 0;
        float m_ButtonPadding = 0;

        sf::Text m_Text;

        // The panel-local x that horizontally centres text within the panel.
        // Size the text with TextFit::fitToWidth first: every panel dimension is
        // a fraction of a resolution we do not control, and SFML clips drawing
        // to the panel's viewport, so text too large for its panel is silently
        // cut off rather than overflowing visibly.
        float centredTextX(const sf::Text& text) const;

        void addButton(
            float x,
            float y,
            int width,
            int height,
            int red,
            int green,
            int blue,
            std::string label
        );

    public:
        // Owned as unique_ptr<UIPanel> in Screen::m_Panels and deleted through
        // that base pointer.
        virtual ~UIPanel() = default;

        sf::View m_View;
        UIPanel(
            sf::Vector2i res,
            int x,
            int y,
            float width,
            float height,
            int alpha,
            int red,
            int green,
            int blue
        );
        std::vector<std::shared_ptr<Button>> getButtons();
        virtual void draw(sf::RenderWindow& window);
        void show();
        void hide();
};