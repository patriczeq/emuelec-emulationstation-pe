#pragma once
#ifndef ES_CORE_AUDIO_MANAGER_H
#define ES_CORE_AUDIO_MANAGER_H

#include <SDL_audio.h>
#include <memory>
#include <vector>
#include "SDL_mixer.h"
#include <string>
#include <iostream>
#include <deque>
#include <math.h>
#include "utils/VectorEx.h"

class Sound;
class ThemeData;

class AudioManager
{
private:
	AudioManager();

	static std::vector<std::shared_ptr<Sound>> sSoundVector;
	static AudioManager* sInstance;

	Mix_Music* mCurrentMusic;
	void getMusicIn(const std::string &path, std::vector<std::string>& all_matching_files);

	static void musicEnd_callback();
	static void musicPlaylistEnd_callback();

	std::string mSystemName;			// per system music folder
	std::string mCurrentSong;			// pop-up for SongName.cpp
	std::string mCurrentThemeMusicDirectory;
	std::string mCurrentMusicPath;
	std::deque<std::string> mLastPlayed;    // batocera

	bool		mInitialized;
	std::string	mPlayingSystemThemeSong;

public:
	static AudioManager* getInstance();
	static bool isInitialized();

	void init();
	void deinit();

	void registerSound(std::shared_ptr<Sound> & sound);
	void unregisterSound(std::shared_ptr<Sound> & sound);

	void play();
	void stop();
	void pause();

	bool isPaused();

	void addToPlaylist(std::string path);
	void clearPlaylist();
	void playDir(std::string path);
	void playMySong(std::string song);
	void playNext();
	bool isPlaying(std::string song);

	std::vector<std::string> myPlaylist;

	// video
	int VideoGetCurrTime();
	void VideoSetCurrTime(int t);
	int VideoGetTotalTime();
	void VideoSetTotalTime(int t);
	bool VideoGetPaused();
	void VideoSetPaused(bool p);
	void VideoSetShowOSD(bool show);
	bool VideoShowOSD();



	//std::vector<std::string> getID3(std::string song);

	void playRandomMusic(bool continueIfPlaying = true);
	void stopMusic(bool fadeOut=true);

	void playSong(const std::string& song);
	void playMusic(std::string path);


	inline const std::string getSongName() const { return mCurrentSong; }

	bool songNameChanged() { return mSongNameChanged; }
	void resetSongNameChangedFlag() { mSongNameChanged = false; }

	inline bool isSongPlaying() { return (mCurrentMusic != NULL); }

	void changePlaylist(const std::shared_ptr<ThemeData>& theme, bool force = false);

	virtual ~AudioManager();

	float mMusicVolume;
	int mVideoPlaying;

	static void setVideoPlaying(bool state);
	static void setVideoMoviePlaying(bool state);
	static bool getVideoPlaying();
	static bool getVideoMoviePlaying();
	static void update(int deltaTime);

	static int getMaxMusicVolume();

private:
	//void playSong(const std::string& song);
	void setSongName(const std::string& song);
	void addLastPlayed(const std::string& newSong, int totalMusic);
	bool songWasPlayedRecently(const std::string& song);

	bool mSongNameChanged;
	bool VideoPlay;
	bool VideoMoviePlay;

	int VideoCurrTime;
	int VideoTotalTime;
	bool VideoIsPaused;
	bool VideoShow_OSD;
};

#endif // ES_CORE_AUDIO_MANAGER_H
