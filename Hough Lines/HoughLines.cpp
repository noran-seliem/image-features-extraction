#include "HoughLines.hpp"

//Creating Empty Accumulator
int HoughTransform(unsigned char* img_data, int w, int h)
{
	ImageWidth = w;
	ImageHight = h;

	//Creating Empty Accumulator
	double HoughHight = ((sqrt(2.0) * (double)(h > w ? h : w)) / 2.0);
	AccuHight = HoughHight * 2.0; // -r -> +r
	AccuWidth = 180;

	Accu = (unsigned int*)calloc(AccuHight * AccuWidth, sizeof(unsigned int));

	double center_x = w / 2;
	double center_y = h / 2;

	// Calculating r for every theta in the acc
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			if (img_data[(y * w) + x] > 250)
			{
				for (int t = 0; t < 180; t++)
				{
					double r = (((double)x - center_x) * cos((double)t * DEG2RAD)) + (((double)y - center_y) * sin((double)t * DEG2RAD));
					Accu[(int)((round(r + HoughHight) * 180.0)) + t]++;
				}
			}
		}
	}

	return 0;
}


std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > GetLines(int threshold)
{
	std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > lines;

	if (Accu == 0)
		return lines;

	for (int r = 0; r < AccuHight; r++)
	{
		for (int t = 0; t < AccuWidth; t++)
		{
			if ((int)Accu[(r * AccuWidth) + t] >= threshold)
			{
				//Is this point a local maxima (9x9)
				int max = Accu[(r * AccuWidth) + t];
				for (int ly = -4; ly <= 4; ly++)
				{
					for (int lx = -4; lx <= 4; lx++)
					{
						if ((ly + r >= 0 && ly + r < AccuHight) && (lx + t >= 0 && lx + t < AccuWidth))
						{
							if ((int)Accu[((r + ly) * AccuWidth) + (t + lx)] > max)
							{
								max = Accu[((r + ly) * AccuWidth) + (t + lx)];
								ly = lx = 5;
							}
						}
					}
				}
				if (max > (int)Accu[(r * AccuWidth) + t])
					continue;


				int x1, y1, x2, y2;
				x1 = y1 = x2 = y2 = 0;

				if (t >= 45 && t <= 135)
				{
					//y = (r - x cos(t)) / sin(t)
					x1 = 0;
					y1 = ((double)(r - (AccuHight / 2)) - ((x1 - (ImageWidth / 2)) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (ImageHight / 2);
					x2 = ImageWidth - 0;
					y2 = ((double)(r - (AccuHight / 2)) - ((x2 - (ImageWidth / 2)) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (ImageHight / 2);
				}
				else
				{
					//x = (r - y sin(t)) / cos(t);
					y1 = 0;
					x1 = ((double)(r - (AccuHight / 2)) - ((y1 - (ImageHight / 2)) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (ImageWidth / 2);
					y2 = ImageHight - 0;
					x2 = ((double)(r - (AccuHight / 2)) - ((y2 - (ImageHight / 2)) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (ImageWidth / 2);
				}

				lines.push_back(std::pair< std::pair<int, int>, std::pair<int, int> >(std::pair<int, int>(x1, y1), std::pair<int, int>(x2, y2)));

			}
		}
	}

	std::cout << "lines: " << lines.size() << " " << threshold << std::endl;
	return lines;
}

const unsigned int* GetAccu(int* w, int* h)
{
	*w = AccuWidth;
	*h = AccuHight;

	return Accu;
}

