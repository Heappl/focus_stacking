// FocusStackImpl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FocusStackImpl.h"
#include <vector>


struct FocusStackCtx
{
	int width, height;
	std::vector<std::vector<int>> images;

	FocusStackCtx(int imgWidth, int imgHeight)
		: width(imgWidth)
		, height(imgHeight)
	{
	}

	void addImage(int* image)
	{
		images.emplace_back(width * height);
		std::memcpy(&images.back().front(), image, images.back().size() * sizeof(int));

	}

	void createDepthOfField(int* dest)
	{
		std::memcpy(dest, &images.back().front(), size());
	}

	void createInFocusImg(int* dest)
	{
		std::memcpy(dest, &images.front().front(), size());
	}
	

	int size()
	{
		return width * height * sizeof(int);
	}
};


FOCUSSTACKIMPL_API void* createFocusStack(int imgWidth, int imgHeight)
{
	return (void*)new FocusStackCtx(imgWidth, imgHeight);
}

FOCUSSTACKIMPL_API int addImage(void* focusStack, int* img)
{
	auto ctx = (FocusStackCtx*)focusStack;
	ctx->addImage(img);
	return 1;
}

FOCUSSTACKIMPL_API int createDepthOfField(void* ctx, int* dest)
{
	auto focusStack = (FocusStackCtx*)ctx;
	focusStack->createDepthOfField(dest);
	return 1;
}

FOCUSSTACKIMPL_API int createInFocusImg(void* ctx, int* dest)
{
	auto focusStack = (FocusStackCtx*)ctx;
	focusStack->createInFocusImg(dest);
	return 1;
}

FOCUSSTACKIMPL_API int releaseFocusStack(void* focusStack)
{
	delete (FocusStackCtx*)focusStack;
	return 1;
}

