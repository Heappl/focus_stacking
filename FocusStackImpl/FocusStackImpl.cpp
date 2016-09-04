// FocusStackImpl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FocusStackImpl.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <tuple>

struct Pixel
{
	uint32_t red, green, blue;

	Pixel(uint32_t value = 0)
		: red((value & 0xff0000) >> 16)
		, green((value & 0xff00) >> 8)
		, blue((value & 0xff))
	{
	}

	Pixel(uint32_t red, uint32_t green, uint32_t blue)
		: red(red)
		, green(green)
		, blue(blue)
	{
	}

	Pixel& operator+=(const Pixel& other)
	{
		red += other.red;
		green += other.green;
		blue += other.blue;
		return *this;
	}

	Pixel operator+(const Pixel& other) const
	{
		Pixel ret = *this;
		ret += other;
		return ret;
	}

	Pixel operator*(const uint32_t mult) const
	{
		return Pixel(red * mult, green * mult, blue * mult);
	}

	Pixel& operator/=(const uint32_t div)
	{
		red /= div;
		green /= div;
		blue /= div;
		return *this;
	}

	Pixel operator/(const uint32_t div) const
	{
		Pixel ret = *this;
		ret /= div;
		return ret;
	}

	uint32_t argb()
	{
		return ((red & 0xff) << 16) + ((green & 0xff) << 8) + ((blue & 0xff));
	}
};

template <
	typename AccType,
	typename InputType,
	typename RetType,
	typename FilterType,
	typename AccFunc,
	typename StoreFunc>
void applyFilter(const std::vector<InputType>& image,
				 const std::vector<FilterType>& filter,
			     std::vector<RetType>& ret,
				 int width,
				 AccFunc acc,
				 StoreFunc store)
{
	ret.resize(image.size());
	int height = image.size() / width;
	int filterSize = (int)sqrt(filter.size());
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			AccType accValue;
			for (int ky = 0; ky < filterSize; ++ky)
			{
				if (y + ky >= height) break;
				for (int kx = 0; kx < filterSize; ++kx)
				{
					if (x + kx >= width) break;
					acc(accValue, image[(y + ky) * width + x + kx], filter[ky * filterSize + kx]);
				}
			}
			ret[y * width + x] = store(accValue);
		}
	}
}

struct FocusMap
{
	std::vector<uint32_t> map;

	FocusMap(const std::vector<uint32_t>& image, int width)
		: map(createFocusMap(image, width, image.size() / width))
	{

	}

	std::vector<uint32_t> createFocusMap(const std::vector<uint32_t>& image, int width, int height)
	{
		//apply gaussian blurring
		std::vector<Pixel> blurred;
		std::vector<uint32_t> gaussianFilter = {
			1, 2, 1,
			2, 4, 2,
			1, 2, 1 };
		uint32_t total = std::accumulate(gaussianFilter.begin(), gaussianFilter.end(), 0);
		applyFilter<Pixel>(image, gaussianFilter, blurred, width,
			[](Pixel& acc, uint32_t pixel, uint32_t weight) { acc += Pixel(pixel) * weight;  },
			[total](Pixel val) { return val / total; });

		//generate laplacian
		std::vector<uint32_t> focusMap(width * height);
		std::vector<int32_t> laplacianFilter = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
		applyFilter<std::tuple<int, int, int>>(blurred, laplacianFilter, focusMap, width,
			[&](std::tuple<int, int, int>& acc, Pixel pixel, uint32_t weight) {
				std::get<0>(acc) += pixel.red * weight;
				std::get<1>(acc) += pixel.green * weight;
				std::get<2>(acc) += pixel.blue * weight; },
			[&](std::tuple<int, int, int> acc) {
				return (uint32_t)(abs(std::get<0>(acc)) + abs(std::get<1>(acc)) + abs(std::get<2>(acc))); });

		return focusMap;
	}

	std::vector<uint32_t> toImage() const
	{
		auto maxValue = *std::max_element(map.begin(), map.end()) + 1;
		std::vector<uint32_t> ret(map.size());
		for (unsigned i = 0; i < map.size(); ++i)
			ret[i] = Pixel(min(map[i], 255), min(map[i], 255), min(map[i], 255)).argb();
		return ret;
	}
};

struct FocusStackCtx
{
	int width, height;
	std::vector<std::vector<uint32_t>> images;
	std::vector<FocusMap> focus;

	FocusStackCtx(int32_t imgWidth, int32_t imgHeight)
		: width(imgWidth)
		, height(imgHeight)
	{
	}

	void addImage(int32_t* image)
	{
		images.emplace_back(width * height);
		std::memcpy(&images.back().front(), image, images.back().size() * sizeof(int32_t));

		focus.emplace_back(images.back(), width);
	}

	void createDepthOfField(int32_t* dest)
	{
		int diff = (255 - 64) * 2 / focus.size();
		for (int i = 0; i < width * height; ++i)
		{
			uint32_t totalFocus = 0;
			uint32_t dist = 0;
			uint32_t bestFocus = focus[0].map[i];
			for (unsigned j = 0; j < focus.size(); ++j)
			{
				if (bestFocus < focus[j].map[i])
				{
					dist = diff * j;
					bestFocus = focus[j].map[i];
				}
			}
			auto red = (dist < (255 - 64)) ? (255 - dist) : 64;
			auto blue = (dist >= (255 - 64)) ? (dist - 128) : 64;
			dest[i] = Pixel(red, 0, blue).argb();
		}
	}

	void createInFocusImg(int32_t* dest)
	{
		for (int i = 0; i < width * height; ++i)
		{
			dest[i] = images.front()[i];
			uint32_t bestFocus = focus.front().map[i];
			for (unsigned j = 0; j < focus.size(); ++j)
			{
				if (bestFocus < focus[j].map[i])
				{
					bestFocus = focus[j].map[i];
					dest[i] = images[j][i];
				}
			}
		}
	}
	
	int size()
	{
		return width * height * sizeof(int32_t);
	}
};


FOCUSSTACKIMPL_API void* createFocusStack(int32_t imgWidth, int32_t imgHeight)
{
	return (void*)new FocusStackCtx(imgWidth, imgHeight);
}

FOCUSSTACKIMPL_API int addImage(void* focusStack, int32_t* img)
{
	auto ctx = (FocusStackCtx*)focusStack;
	ctx->addImage(img);
	return 1;
}

FOCUSSTACKIMPL_API int createDepthOfField(void* ctx, int32_t* dest)
{
	auto focusStack = (FocusStackCtx*)ctx;
	focusStack->createDepthOfField(dest);
	return 1;
}

FOCUSSTACKIMPL_API int createInFocusImg(void* ctx, int32_t* dest)
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

