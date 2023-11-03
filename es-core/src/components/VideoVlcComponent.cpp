#include "components/VideoVlcComponent.h"

#include "renderers/Renderer.h"
#include "resources/TextureResource.h"
#include "utils/StringUtil.h"
#include "PowerSaver.h"
#include "Settings.h"
#include <vlc/vlc.h>
#include <SDL_mutex.h>
#include <cmath>
#include "SystemConf.h"
#include "ThemeData.h"
#include <SDL_timer.h>
#include "AudioManager.h"
#include "Log.h"

/*

libvlc_media_player_get_length( libvlc_media_player_t *p_mi ); // vraci ms
libvlc_media_player_get_time( libvlc_media_player_t *p_mi ); // ms
libvlc_media_player_set_time( libvlc_media_player_t *p_mi,
                                             libvlc_time_t i_time, bool b_fast );
libvlc_media_player_get_position( libvlc_media_player_t *p_mi ); // vraci procenta

*/

#ifdef WIN32
#include <codecvt>
#endif

#include "ImageIO.h"

#define MATHPI          3.141592653589793238462643383279502884L

libvlc_instance_t* VideoVlcComponent::mVLC = NULL;

// VLC prepares to render a video frame.
static void *lock(void *data, void **p_pixels)
{
	struct VideoContext *c = (struct VideoContext *)data;

	int frame = (c->surfaceId ^ 1);

	c->mutexes[frame].lock();
	c->hasFrame[frame] = false;
	*p_pixels = c->surfaces[frame];
	return NULL; // Picture identifier, not needed here.
}

// VLC just rendered a video frame.
static void unlock(void *data, void* /*id*/, void *const* /*p_pixels*/)
{
	struct VideoContext *c = (struct VideoContext *)data;

	int frame = (c->surfaceId ^ 1);

	c->surfaceId = frame;
	c->hasFrame[frame] = true;
	c->mutexes[frame].unlock();
}

// VLC wants to display a video frame.
static void display(void* data, void* id)
{
	if (data == NULL)
		return;

	struct VideoContext *c = (struct VideoContext *)data;
	if (c->valid && c->component != NULL && !c->component->isPlaying() && c->component->isWaitingForVideoToStart())
		c->component->onVideoStarted();
}

VideoVlcComponent::VideoVlcComponent(Window* window, bool isMovie) :
	VideoComponent(window, isMovie),
	mMediaPlayer(nullptr),
	mMedia(nullptr)
{
	mSaturation = 1.0f;
	mElapsed = 0;
	mColorShift = 0xFFFFFFFF;
	mLinearSmooth = false;

	mLoops = -1;
	mCurrentLoop = 0;

	// Get an empty texture for rendering the video
	mTexture = nullptr;// TextureResource::get("");
	mEffect = VideoVlcFlags::VideoVlcEffect::BUMP;
	subIndex = -1;
	// Make sure VLC has been initialised
	mIsMovie = isMovie;
	init();

}

VideoVlcComponent::~VideoVlcComponent()
{
	stopVideo();
}

void VideoVlcComponent::setResize(float width, float height)
{
	if (mSize.x() != 0 && mSize.y() != 0 && !mTargetIsMax && !mTargetIsMin && mTargetSize.x() == width && mTargetSize.y() == height)
		return;

	mTargetSize = Vector2f(width, height);
	mTargetIsMax = false;
	mTargetIsMin = false;
	mStaticImage.setResize(width, height);
	resize();
}

void VideoVlcComponent::setMaxSize(float width, float height)
{
	if (mSize.x() != 0 && mSize.y() != 0 && mTargetIsMax && !mTargetIsMin && mTargetSize.x() == width && mTargetSize.y() == height)
		return;

	mTargetSize = Vector2f(width, height);
	mTargetIsMax = true;
	mTargetIsMin = false;
	mStaticImage.setMaxSize(width, height);
	resize();
}

void VideoVlcComponent::setMinSize(float width, float height)
{
	if (mSize.x() != 0 && mSize.y() != 0 && mTargetIsMin && !mTargetIsMax && mTargetSize.x() == width && mTargetSize.y() == height)
		return;

	mTargetSize = Vector2f(width, height);
	mTargetIsMax = false;
	mTargetIsMin = true;
	mStaticImage.setMinSize(width, height);
	resize();
}

void VideoVlcComponent::onVideoStarted()
{
	VideoComponent::onVideoStarted();
	resize();
}

void VideoVlcComponent::resize()
{
	if(!mTexture)
		return;

	const Vector2f textureSize((float)mVideoWidth, (float)mVideoHeight);

	if(textureSize == Vector2f::Zero())
		return;

		// SVG rasterization is determined by height (see SVGResource.cpp), and rasterization is done in terms of pixels
		// if rounding is off enough in the rasterization step (for images with extreme aspect ratios), it can cause cutoff when the aspect ratio breaks
		// so, we always make sure the resultant height is an integer to make sure cutoff doesn't happen, and scale width from that
		// (you'll see this scattered throughout the function)
		// this is probably not the best way, so if you're familiar with this problem and have a better solution, please make a pull request!

		if(mTargetIsMax)
		{

			mSize = textureSize;

			Vector2f resizeScale((mTargetSize.x() / mSize.x()), (mTargetSize.y() / mSize.y()));

			if(resizeScale.x() < resizeScale.y())
			{
				mSize[0] *= resizeScale.x();
				mSize[1] *= resizeScale.x();
			}else{
				mSize[0] *= resizeScale.y();
				mSize[1] *= resizeScale.y();
			}

			// for SVG rasterization, always calculate width from rounded height (see comment above)
			mSize[1] = Math::round(mSize[1]);
			mSize[0] = (mSize[1] / textureSize.y()) * textureSize.x();

		}
		else if (mTargetIsMin)
		{
			mSize = ImageIO::getPictureMinSize(textureSize, mTargetSize);
		}
		else {
			// if both components are set, we just stretch
			// if no components are set, we don't resize at all
			mSize = mTargetSize == Vector2f::Zero() ? textureSize : mTargetSize;

			// if only one component is set, we resize in a way that maintains aspect ratio
			// for SVG rasterization, we always calculate width from rounded height (see comment above)
			if(!mTargetSize.x() && mTargetSize.y())
			{
				mSize[1] = Math::round(mTargetSize.y());
				mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
			}else if(mTargetSize.x() && !mTargetSize.y())
			{
				mSize[1] = Math::round((mTargetSize.x() / textureSize.x()) * textureSize.y());
				mSize[0] = (mSize.y() / textureSize.y()) * textureSize.x();
			}
		}

	// mSize.y() should already be rounded
	mTexture->rasterizeAt((size_t)Math::round(mSize.x()), (size_t)Math::round(mSize.y()));

	onSizeChanged();
}

void VideoVlcComponent::setColorShift(unsigned int color)
{
	mColorShift = color;
}

void VideoVlcComponent::render(const Transform4x4f& parentTrans)
{
	if (!isShowing() || !isVisible())
		return;
	if(mIsMovie)
		{
			Renderer::drawRect(0.0f, 0.0f, Renderer::getScreenWidth(), Renderer::getScreenHeight(), 0x000000FF);
		}
	VideoComponent::render(parentTrans);

	bool initFromPixels = true;

	if (!mIsPlaying || !mContext.valid)
	{
		// If video is still attached to the path & texture is initialized, we suppose it had just been stopped (onhide, ondisable, screensaver...)
		// still render the last frame
		if (mTexture != nullptr && !mVideoPath.empty() && mPlayingVideoPath == mVideoPath && mTexture->isLoaded())
			initFromPixels = false;
		else
			return;
	}

	float t = mFadeIn;
	if (mFadeIn < 1.0)
	{
		t = 1.0 - mFadeIn;
		t -= 1; // cubic ease in
		t = Math::lerp(0, 1, t*t*t + 1);
		t = 1.0 - t;
	}

	if (t == 0.0)
		return;

	Transform4x4f trans = parentTrans * getTransform();

	if (mRotation == 0 && !mTargetIsMin)
	{
		auto rect = Renderer::getScreenRect(trans, mSize);
		if (!Renderer::isVisibleOnScreen(rect))
			return;
	}

	Renderer::setMatrix(trans);

	// Build a texture for the video frame
	if (initFromPixels)
	{
		int frame = mContext.surfaceId;
		if (mContext.hasFrame[frame])
		{
			if (mTexture == nullptr)
			{
				mTexture = TextureResource::get("", false, mLinearSmooth);

				resize();
				trans = parentTrans * getTransform();
				Renderer::setMatrix(trans);
			}

#ifdef _RPI_
			// Rpi : A lot of videos are encoded in 60fps on screenscraper
			// Try to limit transfert to opengl textures to 30fps to save CPU
			if (!Settings::getInstance()->getBool("OptimizeVideo") || mElapsed >= 40) // 40ms = 25fps, 33.33 = 30 fps
#endif
			{
				mContext.mutexes[frame].lock();
				mTexture->updateFromExternalPixels(mContext.surfaces[frame], mVideoWidth, mVideoHeight);
				mContext.hasFrame[frame] = false;
				mContext.mutexes[frame].unlock();

				mElapsed = 0;
			}
		}
	}

	if (mTexture == nullptr)
		return;

	float opacity = (mOpacity / 255.0f) * t;

	if (hasStoryBoard())
		opacity = (mOpacity / 255.0f);

	unsigned int color = Renderer::convertColor(mColorShift & 0xFFFFFF00 | (unsigned char)((mColorShift & 0xFF) * opacity));

	Renderer::Vertex   vertices[4];

	if (mEffect == VideoVlcFlags::VideoVlcEffect::SLIDERIGHT && mFadeIn > 0.0 && mFadeIn < 1.0 && mConfig.startDelay > 0 && !hasStoryBoard())
	{
		float t = 1.0 - mFadeIn;
		t -= 1; // cubic ease in
		t = Math::lerp(0, 1, t*t*t + 1);
		//t = 1.0 - t;

		vertices[0] = { { 0.0f     , 0.0f      }, { t, 0.0f }, color };
		vertices[1] = { { 0.0f     , mSize.y() }, { t, 1.0f }, color };
		vertices[2] = { { mSize.x(), 0.0f      }, { t + 1.0f, 0.0f }, color };
		vertices[3] = { { mSize.x(), mSize.y() }, { t + 1.0f, 1.0f }, color };
	}
	else
	if (mEffect == VideoVlcFlags::VideoVlcEffect::SIZE && mFadeIn > 0.0 && mFadeIn < 1.0 && mConfig.startDelay > 0 && !hasStoryBoard())
	{
		float t = 1.0 - mFadeIn;
		t -= 1; // cubic ease in
		t = Math::lerp(0, 1, t*t*t + 1);
		t = 1.0 - t;

		float w = mSize.x() * t;
		float h = mSize.y() * t;
		float centerX = mSize.x() / 2.0f;
		float centerY = mSize.y() / 2.0f;

		Vector2f topLeft(Math::round(centerX - w / 2.0f), Math::round(centerY - h / 2.0f));
		Vector2f bottomRight(Math::round(centerX + w / 2.0f), Math::round(centerY + h / 2.0f));

		vertices[0] = { { topLeft.x()		, topLeft.y()	  }, { 0.0f, 0.0f }, color };
		vertices[1] = { { topLeft.x()		, bottomRight.y() }, { 0.0f, 1.0f }, color };
		vertices[2] = { { bottomRight.x()	, topLeft.y()     }, { 1.0f, 0.0f }, color };
		vertices[3] = { { bottomRight.x()	, bottomRight.y() }, { 1.0f, 1.0f }, color };
	}
	else if (mEffect == VideoVlcFlags::VideoVlcEffect::BUMP && mFadeIn > 0.0 && mFadeIn < 1.0 && mConfig.startDelay > 0 && !hasStoryBoard())
	{
		// Bump Effect
		float bump = sin((MATHPI / 2.0) * mFadeIn) + sin(MATHPI * mFadeIn) / 2.0;

		float w = mSize.x() * bump;
		float h = mSize.y() * bump;
		float centerX = mSize.x() / 2.0f;
		float centerY = mSize.y() / 2.0f;

		Vector2f topLeft(Math::round(centerX - w / 2.0f), Math::round(centerY - h / 2.0f));
		Vector2f bottomRight(Math::round(centerX + w / 2.0f), Math::round(centerY + h / 2.0f));

		vertices[0] = { { topLeft.x()		, topLeft.y()	  }, { 0.0f, 0.0f }, color };
		vertices[1] = { { topLeft.x()		, bottomRight.y() }, { 0.0f, 1.0f }, color };
		vertices[2] = { { bottomRight.x()	, topLeft.y()     }, { 1.0f, 0.0f }, color };
		vertices[3] = { { bottomRight.x()	, bottomRight.y() }, { 1.0f, 1.0f }, color };
	}
	else
	{
		vertices[0] = { { 0.0f     , 0.0f      }, { 0.0f, 0.0f }, color };
		vertices[1] = { { 0.0f     , mSize.y() }, { 0.0f, 1.0f }, color };
		vertices[2] = { { mSize.x(), 0.0f      }, { 1.0f, 0.0f }, color };
		vertices[3] = { { mSize.x(), mSize.y() }, { 1.0f, 1.0f }, color };
	}

	// round vertices
	for(int i = 0; i < 4; ++i)
		vertices[i].pos.round();

	if (mTexture->bind())
	{
		beginCustomClipRect();

		Vector2f targetSizePos = (mTargetSize - mSize) * mOrigin * -1;

		if (mTargetIsMin)
		{
			Vector2i pos(trans.translation().x() + (int)targetSizePos.x(), trans.translation().y() + (int)targetSizePos.y());
			Vector2i size((int)(mTargetSize.x() * trans.r0().x()), (int)(mTargetSize.y() * trans.r1().y()));
			Renderer::pushClipRect(pos, size);
		}

		if (mRoundCorners > 0)
		{
			float x = 0;
			float y = 0;
			float size_x = mSize.x();
			float size_y = mSize.y();

			if (mTargetIsMin)
			{
				x = targetSizePos.x();
				y = targetSizePos.y();
				size_x = mTargetSize.x();
				size_y = mTargetSize.y();
			}

			float radius = Math::max(size_x, size_y) * mRoundCorners;
			Renderer::enableRoundCornerStencil(x, y, size_x, size_y, radius);

			mTexture->bind();
		}

		// Render it
		vertices->saturation = mSaturation;
		vertices->customShader = mCustomShader.empty() ? nullptr : (char*)mCustomShader.c_str();

		Renderer::drawTriangleStrips(&vertices[0], 4);

		if (mRoundCorners > 0)
			Renderer::disableStencil();

		if (mTargetIsMin)
			Renderer::popClipRect();

		endCustomClipRect();

		Renderer::bindTexture(0);
	}
}

void VideoVlcComponent::setupContext()
{
	if (mContext.valid)
		return;

	// Create an RGBA surface to render the video into
	mContext.surfaces[0] = new unsigned char[mVideoWidth * mVideoHeight * 4];
	mContext.surfaces[1] = new unsigned char[mVideoWidth * mVideoHeight * 4];
	mContext.hasFrame[0] = false;
	mContext.hasFrame[1] = false;
	mContext.component = this;
	mContext.valid = true;
	resize();
}

void VideoVlcComponent::freeContext()
{
	if (!mContext.valid)
		return;

	if (!mDisable)
	{
		// Release texture memory -> except if mDisable by topWindow ( ex: menu was poped )
		mTexture = nullptr;
	}

	delete[] mContext.surfaces[0];
	delete[] mContext.surfaces[1];
	mContext.surfaces[0] = nullptr;
	mContext.surfaces[1] = nullptr;
	mContext.hasFrame[0] = false;
	mContext.hasFrame[1] = false;
	mContext.component = NULL;
	mContext.valid = false;
}

#if WIN32
#include <Windows.h>

// If Vlc2 dlls have been upgraded with vlc3 dlls, libqt4_plugin.dll is not compatible, so check if libvlc is 3.x then delete obsolete libqt4_plugin.dll
void _checkUpgradedVlcVersion()
{
	char str[1024] = { 0 };
	if (GetModuleFileNameA(NULL, str, 1024) == 0)
		return;

	auto dir = Utils::FileSystem::getParent(str);
	auto path = Utils::FileSystem::getPreferredPath(Utils::FileSystem::combine(dir, "libvlc.dll"));
	if (Utils::FileSystem::exists(path))
	{
		// Get the version information for the file requested
		DWORD dwSize = GetFileVersionInfoSize(path.c_str(), NULL);
		if (dwSize == 0)
		{
			printf("Error in GetFileVersionInfoSize: %d\n", GetLastError());
			return;
		}

		BYTE                *pbVersionInfo = NULL;
		VS_FIXEDFILEINFO    *pFileInfo = NULL;
		UINT                puLenFileInfo = 0;

		pbVersionInfo = new BYTE[dwSize];

		if (!GetFileVersionInfo(path.c_str(), 0, dwSize, pbVersionInfo))
		{
			printf("Error in GetFileVersionInfo: %d\n", GetLastError());
			delete[] pbVersionInfo;
			return;
		}

		if (!VerQueryValue(pbVersionInfo, TEXT("\\"), (LPVOID*)&pFileInfo, &puLenFileInfo))
		{
			printf("Error in VerQueryValue: %d\n", GetLastError());
			delete[] pbVersionInfo;
			return;
		}

		// FileVersion for libvlc.dll is >= 3.x.x.x ???
		if (HIWORD(pFileInfo->dwFileVersionMS) >= 3)
		{
			auto badV2PluginPath = Utils::FileSystem::getPreferredPath(Utils::FileSystem::combine(dir, "plugins/gui/libqt4_plugin.dll"));
			if (Utils::FileSystem::exists(badV2PluginPath))
				Utils::FileSystem::removeFile(badV2PluginPath);
		}
	}
}
#endif

void VideoVlcComponent::init()
{
	if (mVLC != nullptr)
		return;

	std::vector<std::string> cmdline;
	//cmdline.push_back("--quiet");
	//cmdline.push_back("--no-video-title-show");
	//cmdline.push_back("--spu");

	std::string commandLine = SystemConf::getInstance()->get("vlc.commandline");
	if (!commandLine.empty())
	{
		std::vector<std::string> tokens = Utils::String::split(commandLine, ' ');
		for (auto token : tokens)
			cmdline.push_back(token);
	}

	const char* *theArgs = new const char*[cmdline.size()];

	for (int i = 0 ; i < cmdline.size() ; i++)
		theArgs[i] = cmdline[i].c_str();

#if WIN32
	_checkUpgradedVlcVersion();
#endif

	mVLC = libvlc_new(cmdline.size(), theArgs);

	delete[] theArgs;
}

void VideoVlcComponent::handleLooping()
{
	if (mIsPlaying && mMediaPlayer)
	{
		libvlc_state_t state = libvlc_media_player_get_state(mMediaPlayer);
		if (state == libvlc_Ended)
		{
			if (mLoops >= 0)
			{
				mCurrentLoop++;
				if (mCurrentLoop > mLoops)
				{
					stopVideo();

					mFadeIn = 0.0;
					mPlayingVideoPath = "";
					mVideoPath = "";
					return;
				}
			}

			if (mPlaylist != nullptr)
			{
				auto nextVideo = mPlaylist->getNextItem();
				if (!nextVideo.empty())
				{
					stopVideo();
					setVideo(nextVideo);
					return;
				}
				else
					mPlaylist = nullptr;
			}

			if (mVideoEnded != nullptr)
			{
				bool cont = mVideoEnded();
				if (!cont)
				{
					stopVideo();
					return;
				}
			}

			if (!mIsMovie && (!getPlayAudio() || (!mScreensaverMode && !Settings::getInstance()->getBool("VideoAudio")) || (Settings::getInstance()->getBool("ScreenSaverVideoMute") && mScreensaverMode)))
				libvlc_audio_set_mute(mMediaPlayer, 1);

			//libvlc_media_player_set_position(mMediaPlayer, 0.0f);
			if (mMedia)
				libvlc_media_player_set_media(mMediaPlayer, mMedia);
			//	libvlc_audio_set_mute(mMediaPlayer, 0);
			loadSubtitles();
			libvlc_media_player_play(mMediaPlayer);


			if(mIsMovie)
				{
					AudioManager::getInstance()->VideoSetTotalTime(libvlc_media_player_get_length(mMediaPlayer));
					AudioManager::setVideoMoviePlaying(true);
					AudioManager::getInstance()->VideoSetTotalTime(libvlc_media_player_get_length(mMediaPlayer));
				}
			else{
				AudioManager::setVideoPlaying(true);
			}
		}
	}
}

void VideoVlcComponent::startVideo()
{
	LOG(LogDebug) << "VLCVideoComponent start: " << mVideoPath;
	if (mIsPlaying)
		return;

	if (hasStoryBoard("", true) && mConfig.startDelay > 0)
		startStoryboard();

	mTexture = nullptr;
	mCurrentLoop = 0;
	mVideoWidth = 0;
	mVideoHeight = 0;

#ifdef WIN32
	std::string path(Utils::String::replace(mVideoPath, "/", "\\"));
#else
	std::string path(mVideoPath);
#endif
	// Make sure we have a video path
	if (mVLC && (path.size() > 0))
	{
		// Set the video that we are going to be playing so we don't attempt to restart it
		mPlayingVideoPath = mVideoPath;

		// Open the media
		mMedia = libvlc_media_new_path(mVLC, path.c_str());
		if (mMedia)
		{

			// use : vlc �long-help
			// WIN32 ? libvlc_media_add_option(mMedia, ":avcodec-hw=dxva2");
			// RPI/OMX ? libvlc_media_add_option(mMedia, ":codec=mediacodec,iomx,all"); .

			std::string options = SystemConf::getInstance()->get("vlc.options");
			if (!options.empty())
			{
				std::vector<std::string> tokens = Utils::String::split(options, ' ');
				for (auto token : tokens)
					libvlc_media_add_option(mMedia, token.c_str());
			}

			// If we have a playlist : most videos have a fader, skip it 1 second
			/*if (mPlaylist != nullptr && mConfig.startDelay == 0 && !mConfig.showSnapshotDelay && !mConfig.showSnapshotNoVideo)
				libvlc_media_add_option(mMedia, ":start-time=0.7");	*/

			bool hasAudioTrack = false;

			unsigned track_count;
			// Get the media metadata so we can find the aspect ratio
			libvlc_media_parse(mMedia);
			libvlc_media_track_t** tracks;
			track_count = libvlc_media_tracks_get(mMedia, &tracks);
			for (unsigned track = 0; track < track_count; ++track)
			{
				if (tracks[track]->i_type == libvlc_track_audio)
					hasAudioTrack = true;
				else if (tracks[track]->i_type == libvlc_track_video)
				{
					mVideoWidth = tracks[track]->video->i_width;
					mVideoHeight = tracks[track]->video->i_height;

					if (hasAudioTrack)
						break;
				}
			}


			libvlc_media_tracks_release(tracks, track_count);

			if (mVideoWidth == 0 && mVideoHeight == 0 && Utils::FileSystem::isAudio(path))
			{
				if (getPlayAudio() && !mScreensaverMode && Settings::getInstance()->getBool("VideoAudio"))
				{
					// Make fake dimension to play audio files
					mVideoWidth = 1;
					mVideoHeight = 1;
				}
			}

			// Make sure we found a valid video track
			if ((mVideoWidth > 0) && (mVideoHeight > 0))
			{
				if (mVideoWidth > 1 && Settings::getInstance()->getBool("OptimizeVideo"))
				{
					// Avoid videos bigger than resolution
					Vector2f maxSize(Renderer::getScreenWidth(), Renderer::getScreenHeight());

#ifdef _RPI_
					// Temporary -> RPI -> Try to limit videos to 400x300 for performance benchmark
					if (!Renderer::isSmallScreen())
						maxSize = Vector2f(400, 300);
#endif

					if (!mTargetSize.empty() && (mTargetSize.x() < maxSize.x() || mTargetSize.y() < maxSize.y()))
						maxSize = mTargetSize;



					// If video is bigger than display, ask VLC for a smaller image
					auto sz = ImageIO::adjustPictureSize(Vector2i(mVideoWidth, mVideoHeight), Vector2i(maxSize.x(), maxSize.y()), mTargetIsMin);
					if (sz.x() < mVideoWidth || sz.y() < mVideoHeight)
					{
						mVideoWidth = sz.x();
						mVideoHeight = sz.y();
					}
				}

				PowerSaver::pause();
				setupContext();

				// Setup the media player
				mMediaPlayer = libvlc_media_player_new_from_media(mMedia);

				if (hasAudioTrack && !mIsMovie)
				{
					if (!getPlayAudio() || (!mScreensaverMode && !Settings::getInstance()->getBool("VideoAudio")) || (Settings::getInstance()->getBool("ScreenSaverVideoMute") && mScreensaverMode))
						libvlc_audio_set_mute(mMediaPlayer, 1);
					else
						AudioManager::setVideoPlaying(true);
				}

				if(mIsMovie){
					AudioManager::getInstance()->VideoSetTotalTime(libvlc_media_player_get_length(mMediaPlayer));
					AudioManager::setVideoMoviePlaying(true);
				}

				libvlc_media_player_play(mMediaPlayer);


				if (mVideoWidth > 1)
				{
					libvlc_video_set_callbacks(mMediaPlayer, lock, unlock, display, (void*)&mContext);
					libvlc_video_set_format(mMediaPlayer, "RGBA", (int)mVideoWidth, (int)mVideoHeight, (int)mVideoWidth * 4);
				}
			}
		}
	}
}

void VideoVlcComponent::seek(int s)
	{
		if(mMediaPlayer == NULL){
			return;
		}
		int total 	= libvlc_media_player_get_length(mMediaPlayer);
		int current = libvlc_media_player_get_time(mMediaPlayer);
		int newTime	= current + s;
		if(newTime > total)
		{
			newTime = total;
		}
		if(newTime < 0)
		{
			newTime = 0;
		}
		libvlc_media_player_set_time(mMediaPlayer, newTime);

		//AudioManager::getInstance()->VideoSetCurrTime(newTime);
		AudioManager::getInstance()->VideoSetOSD(newTime);

	}

void VideoVlcComponent::pauseResume()
	{
		libvlc_media_player_pause(mMediaPlayer);

		if(!libvlc_media_player_is_playing(mMediaPlayer))
		{
			PowerSaver::resume();
		}
		else{
			PowerSaver::pause();
		}
	}

void VideoVlcComponent::toggleSubs(){
	subIndex++;
	if(libvlc_video_set_spu(mMediaPlayer, subIndex) != 0){
		subIndex = -1;
	}
}

void VideoVlcComponent::loadSubtitles()
	{
		// internal subs
			//libvlc_video_set_spu(mMediaPlayer,int index) // vrátí 0 když ok -1 fail;
		//mSubtitlePath

		// find subtitles file...
		int lastindexExt = mVideoPath.find_last_of(".");
		std::string videoRemExt = mVideoPath.substr(0, lastindexExt);

		if (FILE *file = fopen((videoRemExt + ".srt").c_str(), "r")) {
        mSubtitlePath = videoRemExt + ".srt";
    }else if (FILE *file = fopen((videoRemExt + ".sub").c_str(), "r")) {
        mSubtitlePath = videoRemExt + ".sub";
    }
		else{
			LOG(LogInfo) << "libVLC subtitles not found: " << videoRemExt + ".(srt/sub)";
		}

		if(!mSubtitlePath.empty()){
			LOG(LogInfo) << "libVLC external subtitles found: " << mSubtitlePath;
			if(
				//libvlc_video_set_subtitle_file(mMediaPlayer, mSubtitlePath.c_str())
				libvlc_media_player_add_slave(mMediaPlayer, libvlc_media_slave_type_subtitle, ("file://" + mSubtitlePath).c_str(), false) == 0
			){
				//libvlc_video_set_spu(mMediaPlayer, 0);
				LOG(LogInfo) << "libVLC external subtitles loaded";
			}else{
				LOG(LogError) << "libVLC ERROR loading external subtitles";
			}
		}

		LOG(LogInfo) << "libVLC subtitles count: " << libvlc_video_get_spu_count(mMediaPlayer);
		//LOG(LogInfo) << "libVLC subtitles scale: " << libvlc_video_get_spu_text_scale(mMediaPlayer);

	}

void VideoVlcComponent::stopVideo()
{
	mIsPlaying = false;
	mIsWaitingForVideoToStart = false;
	mStartDelayed = false;

	// Release the media player so it stops calling back to us
	if (mMediaPlayer)
	{
		libvlc_media_player_stop(mMediaPlayer);
		libvlc_media_player_release(mMediaPlayer);
		mMediaPlayer = NULL;
	}

	// Release the media
	if (mMedia)
	{
		libvlc_media_release(mMedia);
		mMedia = NULL;
	}

	freeContext();
	PowerSaver::resume();
	if(mIsMovie){
		AudioManager::getInstance()->VideoReset();
	}
}

void VideoVlcComponent::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties)
{
	VideoComponent::applyTheme(theme, view, element, properties);

	using namespace ThemeFlags;

	const ThemeData::ThemeElement* elem = theme->getElement(view, element, "video");

	if (elem && elem->has("effect"))
	{
		if (!(elem->get<std::string>("effect").compare("slideRight")))
			mEffect = VideoVlcFlags::VideoVlcEffect::SLIDERIGHT;
		else if (!(elem->get<std::string>("effect").compare("size")))
			mEffect = VideoVlcFlags::VideoVlcEffect::SIZE;
		else if (!(elem->get<std::string>("effect").compare("bump")))
			mEffect = VideoVlcFlags::VideoVlcEffect::BUMP;
		else
			mEffect = VideoVlcFlags::VideoVlcEffect::NONE;
	}

	if (elem && elem->has("roundCorners"))
		setRoundCorners(elem->get<float>("roundCorners"));

	if (properties & COLOR)
	{
		if (elem && elem->has("color"))
			setColorShift(elem->get<unsigned int>("color"));

		if (elem->has("opacity"))
			setOpacity((unsigned char)(elem->get<float>("opacity") * 255.0));

		if (elem->has("saturation"))
			setSaturation(Math::clamp(elem->get<float>("saturation"), 0.0f, 1.0f));

		if (elem->has("shader"))
		{
			mCustomShader = elem->get<std::string>("shader");
			mStaticImage.setCustomShader(mCustomShader);
		}
	}

	if (elem && elem->has("loops"))
		mLoops = (int)elem->get<float>("loops");
	else
		mLoops = -1;

	if (elem->has("linearSmooth"))
		mLinearSmooth = elem->get<bool>("linearSmooth");

	applyStoryboard(elem);
	mStaticImage.applyStoryboard(elem, "snapshot");
}

void VideoVlcComponent::update(int deltaTime)
{
	mElapsed += deltaTime;
	if(mIsMovie)
	{
		AudioManager::getInstance()->VideoSetCurrTime((mMediaPlayer == NULL) ? 0 : libvlc_media_player_get_time(mMediaPlayer));
		AudioManager::getInstance()->VideoSetTotalTime((mMediaPlayer == NULL) ? 0 : libvlc_media_player_get_length(mMediaPlayer));
		AudioManager::getInstance()->VideoSetPaused((mMediaPlayer == NULL) ? false : !libvlc_media_player_is_playing(mMediaPlayer));
	}

	if (mConfig.showSnapshotNoVideo || mConfig.showSnapshotDelay)
		mStaticImage.update(deltaTime);

	VideoComponent::update(deltaTime);
}

void VideoVlcComponent::onShow()
{
	VideoComponent::onShow();
	mStaticImage.onShow();

	if (hasStoryBoard("", true) && mConfig.startDelay > 0)
		pauseStoryboard();
}

ThemeData::ThemeElement::Property VideoVlcComponent::getProperty(const std::string name)
{
	Vector2f scale = getParent() ? getParent()->getSize() : Vector2f((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());

	if (name == "size" || name == "maxSize" || name == "minSize")
		return mSize / scale;

	if (name == "color")
		return mColorShift;

	if (name == "roundCorners")
		return mRoundCorners;

	if (name == "saturation")
		return mSaturation;

	return VideoComponent::getProperty(name);
}

void VideoVlcComponent::setProperty(const std::string name, const ThemeData::ThemeElement::Property& value)
{
	Vector2f scale = getParent() ? getParent()->getSize() : Vector2f((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());

	if ((name == "maxSize" || name == "minSize") && value.type == ThemeData::ThemeElement::Property::PropertyType::Pair)
	{
		mTargetSize = Vector2f(value.v.x() * scale.x(), value.v.y() * scale.y());
		resize();
	}
	else if (name == "color" && value.type == ThemeData::ThemeElement::Property::PropertyType::Int)
		setColorShift(value.i);
	else if (name == "roundCorners" && value.type == ThemeData::ThemeElement::Property::PropertyType::Float)
		setRoundCorners(value.f);
	else if (name == "saturation" && value.type == ThemeData::ThemeElement::Property::PropertyType::Float)
		setSaturation(value.f);
	else
		VideoComponent::setProperty(name, value);
}


void VideoVlcComponent::pauseVideo()
{
	if (!mIsPlaying && !mIsWaitingForVideoToStart && !mStartDelayed)
		return;

	mIsPlaying = false;
	mIsWaitingForVideoToStart = false;
	mStartDelayed = false;

	if (mMediaPlayer == NULL)
		stopVideo();
	else
	{
		libvlc_media_player_pause(mMediaPlayer);

		PowerSaver::resume();
		AudioManager::setVideoPlaying(false);
		if(mIsMovie){
			AudioManager::setVideoMoviePlaying(true);
		}
	}
}

void VideoVlcComponent::resumeVideo()
{
	if (mIsPlaying)
		return;

	if (mMediaPlayer == NULL)
	{
		startVideoWithDelay();
		return;
	}

	mIsPlaying = true;
	libvlc_media_player_play(mMediaPlayer);
	PowerSaver::pause();
	AudioManager::setVideoPlaying(true);
	if(mIsMovie){
		AudioManager::setVideoMoviePlaying(true);
	}
}

bool VideoVlcComponent::isPaused()
{
	return !mIsPlaying && !mIsWaitingForVideoToStart && !mStartDelayed && mMediaPlayer != NULL;
}

void VideoVlcComponent::setSaturation(float saturation)
{
	mSaturation = saturation;
}
