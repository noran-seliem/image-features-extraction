#include <cmath>
#include <iostream>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#ifndef DEG2RAD
#define DEG2RAD 0.017453293f
#endif

std::string img_path = "E:\\forth year\\2nd sim\\cv\\task1\\assignment-1-cv-2022-sbe-404-team_12-main\\images\\10.jpg";

unsigned int* _accu;
int _accu_w;
int _accu_h;
int _img_w;
int _img_h;
int _r;

class HoughCircle {

private:
	int Transform(unsigned char* img_data, int w, int h, int r)
	{
		_r = r;
		_img_w = w;
		_img_h = h;

		//Create the accu
		_accu_h = h;
		_accu_w = w;

		if (_accu)
			free(_accu);
		_accu = (unsigned int*)calloc(_accu_h * _accu_w, sizeof(unsigned int));

		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				if (img_data[(y * w) + x] > 250)
				{
					for (int t = 1; t <= 360; t++)
					{
						int a = ((double)x - ((double)_r * cos((double)t * DEG2RAD)));
						int b = ((double)y - ((double)_r * sin((double)t * DEG2RAD)));

						if ((b >= 0 && b < _accu_h) && (a >= 0 && a < _accu_w))
							_accu[(b * _accu_w) + a]++;
					}
				}
			}
		}

		return 0;
	}

	int GetCircles(int threshold, vector<pair<pair<int, int>, int> >& result)
	{
		int found = 0;

		if (_accu == 0)
			return found;

		for (int b = 0; b < _accu_h; b++)
		{
			for (int a = 0; a < _accu_w; a++)
			{
				if ((int)_accu[(b * _accu_w) + a] >= threshold)
				{
					//Is this point a local maxima (9x9)
					int max = _accu[(b * _accu_w) + a];
					for (int ly = -4; ly <= 4; ly++)
					{
						for (int lx = -4; lx <= 4; lx++)
						{
							if ((ly + b >= 0 && ly + b < _accu_h) && (lx + a >= 0 && lx + a < _accu_w))
							{
								if ((int)_accu[((b + ly) * _accu_w) + (a + lx)] > max)
								{
									max = _accu[((b + ly) * _accu_w) + (a + lx)];
									ly = lx = 5;
								}
							}
						}
					}
					if (max > (int)_accu[(b * _accu_w) + a])
						continue;

					result.push_back(pair<pair<int, int>, int>(pair<int, int>(a, b), _r));
					found++;

				}
			}
		}

		cout << "result: " << found << " " << threshold << endl;

		return found;
	}

public:
	void detect_circles(string file_path, int threshold)
	{
		Mat img_edge;
		Mat img_blur;

		Mat img_ori = cv::imread(file_path, 1);
		blur(img_ori, img_blur, cv::Size(5, 5));
		Canny(img_blur, img_edge, 175, 200, 3);

		int w = img_edge.cols;
		int h = img_edge.rows;

		//Transform
		vector<pair<pair<int, int>, int> > circles;

		for (int r = 20; r < 100; r = r + 5)
		{
			Transform(img_edge.data, w, h, r);

			cout << r << " / " << h / 2;

			if (threshold == 0)
				threshold = 0.95 * (2.0 * (double)r * 3.14);

			{
				//Search the accumulator
				GetCircles(threshold, circles);
				imshow("original", img_ori);
				imshow("Canny Edge Detection", img_edge);
				waitKey(1);
			}
		}
		//Filter the results
		{
			int a, b, r;
			a = b = r = 0;
			vector<pair<pair<int, int>, int> > result;
			vector<pair<pair<int, int>, int> >::iterator it;
			for (it = circles.begin(); it != circles.end(); it++)
			{
				int d = sqrt(pow(abs(it->first.first - a), 2) + pow(abs(it->first.second - b), 2));
				if (d > it->second + r)
				{
					result.push_back(*it);
		
					a = it->first.first;
					b = it->first.second;
					r = it->second;
				}
			}
			//Visualize all
			Mat img_res = img_ori.clone();
			for (it = result.begin(); it != result.end(); it++)
			{
				cout << it->first.first << ", " << it->first.second << std::endl;
				circle(img_res, cv::Point(it->first.first, it->first.second), it->second, cv::Scalar(0, 0, 255), 2, 8);
			}
			imshow("Result", img_res);
			imwrite("result3.jpg", img_res);

			waitKey(360000);
		}
	}
};


//int main(int argc, char** argv) {
//
//	HoughCircle h;
//	int threshold = 100;
//
//	cv::namedWindow("Result", WINDOW_AUTOSIZE);
//	cv::namedWindow("Canny Edge Detection", WINDOW_AUTOSIZE);
//	cv::namedWindow("Accumulator", WINDOW_AUTOSIZE);
//
//	cv::moveWindow("Result", 10, 10);
//	cv::moveWindow("Canny Edge Detection", 680, 10);
//	cv::moveWindow("Accumulator", 1350, 10);
//
//	h.detect_circles(img_path, threshold);
//	return 0;
//}



