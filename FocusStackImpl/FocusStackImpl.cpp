// FocusStackImpl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FocusStackImpl.h"
#include <vector>

struct FocusStackCtx
{
	int width, height;
	std::vector<std::vector<char>> images;

	FocusStackCtx(char** imgs, int numOfImages, int imgWidth, int imgHeight)
		: width(imgWidth)
		, height(imgHeight)
	{
		for (int i = 0; i < numOfImages; ++i)
		{
			images.emplace_back(imgWidth * imgHeight * 4);
			std::memcpy(&images.back().front(), imgs[i], images.back().size());
		}
	}

	int size()
	{
		return width * height * 4;
	}
};

FOCUSSTACKIMPL_API void* createFocusStack(char** images, int numOfImages, int imgWidth, int imgHeight)
{
	return (void*)new FocusStackCtx(images, numOfImages, imgWidth, imgHeight);
}

FOCUSSTACKIMPL_API int createDepthOfField(void* focusStack, char* dest)
{
	auto ctx = (FocusStackCtx*)focusStack;
	std::memcpy(dest, &ctx->images.front().front(), ctx->size());
	return 1;
}

FOCUSSTACKIMPL_API int createInFocusImg(void* focusStack, char* dest)
{
	auto ctx = (FocusStackCtx*)focusStack;
	std::memcpy(dest, &ctx->images.front().front(), ctx->size());
	return 1;
}

FOCUSSTACKIMPL_API int releaseFocusStack(void* focusStack)
{
	delete (FocusStackCtx*)focusStack;
	return 1;
}

