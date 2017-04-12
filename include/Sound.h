#ifndef SOUND_H
#define SOUND_H

#ifndef _APPLE_
#include <SDL2/SDL_mixer.h>
#else
#include <SDL2_mixer/SDL_mixer.h>
#endif

#define ANY_CHANNEL -1
#define LOOP -1

namespace SDL
{
	class Sound
	{
	public:
		Sound(Mix_Chunk*);
		~Sound();

		int play(int loops = 0,int channel = ANY_CHANNEL);
		int setVolume(int);

		Mix_Chunk* getChunk() {return m_chunk;}
		int getChannel() const {return m_channel;}

	private:
		Mix_Chunk* m_chunk;
		int m_channel = -1;
	};
}

#endif
