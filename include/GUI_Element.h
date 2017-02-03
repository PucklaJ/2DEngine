#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H
#include <map>
#include <vector>
#include <SDL2/SDL_rect.h>
#include <string>
#include "Actor.h"
#define MS_COORDS m_gui->getMainClass()->getInputManager()->getMouseCoords()
#define MOUSE_HOVERS_OVER (MS_COORDS.x > (m_scaledPos.getX()) && MS_COORDS.x < (m_scaledPos.getX())+m_scaledSize.getX() && MS_COORDS.y > (m_scaledPos.getY()) && MS_COORDS.y < (m_scaledPos.getY())+m_scaledSize.getY())


class SDL_Renderer;

namespace SDL
{
    enum GUI_Event
    {
        CLICK_DOWN,CLICK_UP,HOVER_ENTER,HOVER_LEAVE
    };

    class GUI;

    class GUI_Element : public Actor
    {
        public:
            GUI_Element(const Vector2& pos = Vector2(),const char* name = "GUI_Element");
            virtual ~GUI_Element();

            virtual bool render()=0;
            virtual bool update()=0;
            virtual void quit()=0;
            virtual void insertText(const std::string& str);

            void addEvent(GUI_Event,bool(*)(GUI_Element*));

            const int getID() const {return m_id;}
            GUI* getGUI();
            void setGUI(GUI*);

        protected:
            GUI* m_gui;
            bool hover;
            bool clicked;
            int m_id;

            bool callEvent(GUI_Event);
        private:
            static std::vector<int> m_ids;

            std::map<GUI_Event,bool(*)(GUI_Element*)> m_eventFunctions;


    };

    typedef bool(*EventFunction)(GUI_Element*);
}
#endif // GUI_ELEMENT_H
