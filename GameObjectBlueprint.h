#pragma once
#include <vector>
#include <string>
#include <map>


class GameObjectBlueprint{
    private:
        std::string m_Name = "";
        std::vector<std::string> m_ComponentList;
        std::string m_BitmapName = "";
        // Initialised, because a level file that omits a tag would otherwise
        // leave these indeterminate and the object would be built from garbage.
        float m_Width = 0.f;
        float m_Height = 0.f;
        float m_LocationX = 0.f;
        float m_LocationY = 0.f;
        float m_Speed = 0.f;
        bool m_EncompassingRectCollider = false;
        std::string m_EncompassingRectColliderLabel = "";

    public:
        float getWidth();
        void setWidth(float width);
        float getHeight();
        void setHeight(float height);
        float getLocationX();
        void setLocationX(float locationX);
        float getLocationY();
        void setLocationY(float locationY);
        std::string getName();
        void setName(std::string name);
        std::vector<std::string>& getComponentList();
        void addToComponentList(std::string newComponent);
        std::string getBitmapName();
        void setBitmapName(std::string bitmapName);
        float getSpeed();
        void setSpeed(float speed);
        std::string getEncompassingRectColliderLabel();
        bool getEncompassingRectCollider();
        void setEncompassingRectCollider(std::string label);
};