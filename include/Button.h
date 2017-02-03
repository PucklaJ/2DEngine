#ifndef BUTTON_H
#define BUTTON_H
#include "GUI_Element.h"
#include <SDL2/SDL.h>

namespace SDL
{
    class Button : public GUI_Element
    {
        public:
            Button(const Vector2& pos = Vector2());
            virtual ~Button();

            virtual bool render() override;
            virtual bool update() override;
            virtual void quit() override;

            void setBackground(SDL_Texture*);
            SDL_Texture* getBackground() {return m_backGround;}

            static bool defaultHoverEnter(GUI_Element*);
            static bool defaultHoverLeave(GUI_Element*);

        protected:

        private:
            SDL_Texture* m_backGround;

    };
}

#endif // BUTTON_H
