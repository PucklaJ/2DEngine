#include <Music.h>

namespace SDL
{
	Music::Music(Mix_Music* mus) :
			m_music(mus)
	{

	}

	Music::~Music()
	{
		Mix_FreeMusic(m_music);
	}

	int Music::setVolume(int volume)
	{
		return Mix_VolumeMusic(volume);
	}

	int Music::play(int loops)
	{
		return Mix_PlayMusic(m_music,loops);
	}

	void Music::pause()
	{
		Mix_PauseMusic();
	}
	void Music::resume()
	{
		Mix_ResumeMusic();
	}
	void Music::rewind()
	{
		Mix_RewindMusic();
	}
	int Music::stop()
	{
		return Mix_HaltMusic();
	}
	int Music::fadeOut(int ms)
	{
		return Mix_FadeOutMusic(ms);
	}
	bool Music::isPlaying()
	{
		return Mix_PlayingMusic() == 1;
	}
	bool Music::isPaused()
	{
		return Mix_PausedMusic() == 1;
	}
	Mix_Fading Music::getFade()
	{
		return Mix_FadingMusic();
	}
	Mix_MusicType Music::getCurrentMusicType()
	{
		return Mix_GetMusicType(nullptr);
	}
	Mix_MusicType Music::getType()
	{
		return Mix_GetMusicType(m_music);
	}
	int Music::fadeIn(int ms,double pos,int loops)
	{
		if(pos == 0.0)
		{
			return Mix_FadeInMusic(m_music,loops,ms);
		}
		else
		{
			return Mix_FadeInMusicPos(m_music,loops,ms,pos);
		}
	}
}


