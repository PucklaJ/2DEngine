#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "GUI_Element.h"
#include <string>
    #include <SDL2/SDL.h>
#ifdef __linux__
    #include <SDL2/SDL_ttf.h>
#else
#ifdef _WIN32
    #include <TTF/SDL_ttf.h>
#else
    #include <SDL2_ttf/SDL_ttf.h>
#endif
#endif


namespace SDL
{
    class GUI;

    class TextBox : public GUI_Element
    {
        public:
            TextBox(TTF_Font* font,const Vector2& pos = Vector2(),SDL_Color tColor = {0,0,0,255});
            virtual ~TextBox();

            virtual bool render() override;
            virtual bool update() override;
            virtual void quit() override;
            virtual void insertText(const std::string& str) override;

            const std::string& getText() const {return m_text;}
            void setText(const std::string& text);
            void setBackground(SDL_Texture*);
            void setTextColor(const SDL_Color& col);
            void setTextScale(float f){m_textScale = f;m_needsTextUpdate=true;}

            const Vector2 customCameraScalingBeforeRendering() override;
            void customCameraScalingAfterRendering(const Vector2&) override;

        protected:
            Vector2 m_renderedTextPos;

        private:
            static int* m_currentActive;

            void generateText();

            std::string m_text;
            SDL_Color m_textColor;
            SDL_Surface* m_textSurface;
            SDL_Texture* m_textTexture;
            TTF_Font* m_font;
            Vector2 m_textPos;
            Vector2 m_textSize;
            SDL_Rect m_textSrc;
            SDL_Rect m_textDst;

            SDL_Texture* m_backGround;

            bool m_needsTextUpdate;
            float m_textScale = 1.0f;
    };
}

#endif // TEXTBOX_H
