#define _USE_MATH_DEFINES	// for math constants in C++

#include <opencv2/imgproc/imgproc.hpp>
#include "Util.h"
#include "NormalDistribution.h"
#include "ImageRenderer3.h"


ImageRenderer3::ImageRenderer3(void)
{
}


ImageRenderer3::~ImageRenderer3(void)
{
}


Mat ImageRenderer3::getImage()
{
	int x0 = this->getPinholePosition()[0]; // TODO kann au�erhalb des ML-Bilds liegen
	int y0 = this->getPinholePosition()[1];

	float standardDeviation1 = this->lightfield.ANGULAR_RESOLUTION.width / 2.0;
	float standardDeviation2 = this->lightfield.ANGULAR_RESOLUTION.height / 2.0;
	NormalDistribution apertureFunction = NormalDistribution(x0, y0, standardDeviation1, standardDeviation2);

	const double F		= this->lightfield.getRawFocalLength();	// focal length of the raw image
	const double alpha	= focalLength / F;

	const Vec2f uvScale = Vec2f(1.0, 1.0 / cos(M_PI / 6.0));

	const double weight = 1.0 - 1.0 / alpha;
	const Size dilatedSize = Size(this->lightfield.SPARTIAL_RESOLUTION.width * alpha,
		this->lightfield.SPARTIAL_RESOLUTION.height * alpha);
	const Size imageSize = Size(dilatedSize.width + this->lightfield.ANGULAR_RESOLUTION.width * uvScale[0] * weight,
		dilatedSize.height + this->lightfield.ANGULAR_RESOLUTION.height * uvScale[1] * weight);
	const int imageType = CV_MAKETYPE(CV_32F, this->lightfield.getRawImage().channels());
	Mat image = Mat::zeros(imageSize, imageType);

	Mat subapertureImage, resizedSAImage, dstROI;
	Vec2d translation, dstCorner;
	const Vec2d angularCorrection = Vec2d(this->lightfield.ANGULAR_RESOLUTION.width,
		this->lightfield.ANGULAR_RESOLUTION.height) * 0.5;
	const Vec2d dstCenter = Vec2d(image.size().width, image.size().height) * 0.5;
	Rect dstRect;
	const Vec2d fromCenterToCorner = Vec2d(dilatedSize.width, dilatedSize.height) * -0.5;
	const int interpolationMethod = (alpha < 0.0) ? CV_INTER_AREA : CV_INTER_CUBIC;

	float w;
	for(int u = 0; u < this->lightfield.ANGULAR_RESOLUTION.width; u++)
	{
		for(int v = 0; v < this->lightfield.ANGULAR_RESOLUTION.height; v++)
		{
			subapertureImage = this->lightfield.getSubapertureImage(u, v);

			resize(subapertureImage, resizedSAImage, dilatedSize, 0, 0, interpolationMethod);

			w = apertureFunction.f(u * uvScale[0] - angularCorrection[0],
				v * uvScale[1] - angularCorrection[1]);
			resizedSAImage *= w;

			translation	= (Vec2d(u * uvScale[0], v * uvScale[1]) - angularCorrection) * weight;
			dstCorner	= dstCenter + translation + fromCenterToCorner;
			dstRect		= Rect(Point(round(dstCorner[0]), round(dstCorner[1])), dilatedSize);
			dstROI		= Mat(image, dstRect);

			add(resizedSAImage, dstROI, dstROI, noArray(), dstROI.type());
		}
	}

	// scale luminance/color values to fit inside [0, 1]
	// better: scale by theoretical value space instead of actual values
	image = adjustLuminanceSpace(image);

	return image;
}