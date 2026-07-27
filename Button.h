#pragma once
#include <SFML/Graphics.hpp>


class Button
{
    private:
        sf::RectangleShape m_Button;
        sf::Text m_ButtonText;

    public:
        std::string m_Text;
        sf::FloatRect m_Collider;
        
        Button(
            sf::Vector2f position,
            float width,
            float height,
            int red,
            int green,
            int blue,
            std::string text
        );
        void draw(sf::RenderWindow& window);
};