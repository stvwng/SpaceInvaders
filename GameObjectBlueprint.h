#pragma once
#include <vector>
#include <string>
#include <map>

using namespace std;

class GameObjectBlueprint{
    private:
        string m_Name = "";
        vector<string> m_ComponentList;
        string m_BitmapName = "";
        // Initialised, because a level file that omits a tag would otherwise
        // leave these indeterminate and the object would be built from garbage.
        float m_Width = 0.f;
        float m_Height = 0.f;
        float m_LocationX = 0.f;
        float m_LocationY = 0.f;
        float m_Speed = 0.f;
        bool m_EncompassingRectCollider = false;
        string m_EncompassingRectColliderLabel = "";

    public:
        float getWidth();
        void setWidth(float width);
        float getHeight();
        void setHeight(float height);
        float getLocationX();
        void setLocationX(float locationX);
        float getLocationY();
        void setLocationY(float locationY);
        string getName();
        void setName(string name);
        vector<string>& getComponentList();
        void addToComponentList(string newComponent);
        string getBitmapName();
        void setBitmapName(string bitmapName);
        float getSpeed();
        void setSpeed(float speed);
        string getEncompassingRectColliderLabel();
        bool getEncompassingRectCollider();
        void setEncompassingRectCollider(string label);
};