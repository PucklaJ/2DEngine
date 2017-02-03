#ifndef GUI_H
#define GUI_H
#include <vector>
#include "InputManager.h"
#include <string>
#include "Actor.h"



namespace SDL
{
    class GUI_Element;

    class GUI : public Actor
    {
        public:
            GUI(int order = GUI_ORDER);
            GUI(const std::vector<GUI_Element*>&,int order = GUI_ORDER);
            virtual ~GUI();

            bool update() override;
            bool render() override;
            void quit() override;

            void insertText(const std::string& str);

            void addGUIElement(GUI_Element*);
            void removeGUIElement(GUI_Element*,bool del = true);

        protected:

        private:
            std::vector<GUI_Element*> m_guiElements;
    };
}


#endif // GUI_H
