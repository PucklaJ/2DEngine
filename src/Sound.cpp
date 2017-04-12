#include <Sound.h>

namespace SDL
{
	Sound::Sound(Mix_Chunk* c) :
			m_chunk(c)
	{

	}

	Sound::~Sound()
	{
		Mix_FreeChunk(m_chunk);
	}

	int Sound::play(int loops,int channel)
	{
		m_channel = Mix_PlayChannel(channel,m_chunk,loops);
		return m_channel;
	}

	int Sound::setVolume(int volume)
	{
		return Mix_VolumeChunk(m_chunk,volume);
	}
}
