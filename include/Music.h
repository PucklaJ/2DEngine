#ifndef MUSIC_H
#define MUSIC_H

#ifndef _APPLE_
#include <SDL2/SDL_mixer.h>
#else
#include <SDL2_mixer/SDL_mixer.h>
#endif

#define LOOP -1

namespace SDL
{
	class Music
	{
	public:
		Music(Mix_Music*);
		~Music();

		static int setVolume(int);
		static void pause();
		static void resume();
		static void rewind();
		static int stop();
		static int fadeOut(int);
		static bool isPlaying();
		static bool isPaused();
		static Mix_Fading getFade();
		static Mix_MusicType getCurrentMusicType();

		int fadeIn(int,double pos = 0.0,int loops = LOOP);
		int play(int loops = LOOP);
		Mix_Music* getMusic() {return m_music;}
		Mix_MusicType getType();

	private:
		Mix_Music* m_music = nullptr;
	};
}

#endif
