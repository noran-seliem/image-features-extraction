#include "HoughLines.hpp"

String ImagePath = "\\Chess.jpg";

int main()
{
	doTransform(ImagePath, 175);

	imshow("Result", ResultImage);
	//imshow("Canny Edge Detection", EdgesImage);
	//imshow("Accumulator", AccumulatorImage);
	waitKey(0);
}


void doTransform(string ImagePath, int threshold)
{

	OriginalImage = imread(ImagePath, 1);
	cvtColor(OriginalImage, GrayImage, COLOR_RGB2GRAY);
	blur(GrayImage, BlurredImage, Size(5, 5));
	Canny(BlurredImage, EdgesImage, 100, 150, 3);

	int w = EdgesImage.cols;
	int h = EdgesImage.rows;

	HoughTransform(EdgesImage.data, w, h);

	if (threshold == 0)
		threshold = w > h ? w / 4 : h / 4;


	{
		ResultImage = OriginalImage.clone();

		//Search the accumulator
		std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > lines = GetLines(threshold);

		//Draw the results
		std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > >::iterator it;
		for (it = lines.begin(); it != lines.end(); it++)
		{
			line(ResultImage, Point(it->first.first, it->first.second), Point(it->second.first, it->second.second), Scalar(0, 255, 0), 3, 0);
		}

		//Display all line on the original Image
		int aw, ah, maxa;
		aw = ah = maxa = 0;
		const unsigned int* accu = GetAccu(&aw, &ah);

		for (int p = 0; p < (ah * aw); p++)
		{
			if ((int)accu[p] > maxa)
				maxa = accu[p];
		}
		double contrast = 1.0;
		double coef = 255.0 / (double)maxa * contrast;

		AccumulatorImage = Mat(ah, aw, CV_8UC3);
		for (int p = 0; p < (ah * aw); p++)
		{
			unsigned char c = (double)accu[p] * coef < 255.0 ? (double)accu[p] * coef : 255.0;
			AccumulatorImage.data[(p * 3) + 0] = 255;
			AccumulatorImage.data[(p * 3) + 1] = 255 - c;
			AccumulatorImage.data[(p * 3) + 2] = 255 - c;
		}
	}
}