#pragma once

#include "GuiComponent.h"
#include "Window.h"
#include "components/ImageGridComponent.h"
#include "utils/ThreadPool.h"

class ThemeData;
class VideoComponent;

class GuiImageViewer : public GuiComponent
{
public:
	static void showPdf(Window* window, const std::string imagePath);
	static void showCbz(Window* window, const std::string imagePath);
	static void showImage(Window* window, const std::string imagePath, bool zoomSingleFile = false);
	static void showImages(Window* window, std::vector<std::string>& images);

	GuiImageViewer(Window* window, bool linearSmooth = false);
	~GuiImageViewer();

	bool input(InputConfig* config, Input input) override;
	virtual std::vector<HelpPrompt> getHelpPrompts() override;

	void add(const std::string imagePath);
	void setCursor(const std::string imagePath);

protected:
	void loadPdf(const std::string& imagePath);
	void loadCbz(const std::string& imagePath);
	void loadImages(std::vector<std::string>& images);

	ImageGridComponent<std::string> mGrid;
	std::shared_ptr<ThemeData> mTheme;
	std::string mPdf;

	Utils::ThreadPool* mPdfThreads;
};

class GuiVideoViewer : public GuiComponent
{
public:
	static void playVideo(Window* window, const std::string videoPath, bool movie = false);

	GuiVideoViewer(Window* window, const std::string& path, bool movie = false);
	~GuiVideoViewer();

	bool input(InputConfig* config, Input input) override;

protected:
	bool isMovie;
	VideoComponent*		mVideo;
	Window* vWindow;
};
