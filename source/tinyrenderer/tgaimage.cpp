#include <iostream>
#include <cstring>
#include "tgaimage.h"

TGAImage::TGAImage(const int w, const int h, const int bpp)
	: w(w), h(h), bpp(bpp), data(w* h* bpp, 0)
{
}
