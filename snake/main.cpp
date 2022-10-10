#include<iostream>
#include<string>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#define CONTOUR   std::vector<std::pair<int,int>>
void gradTheta(Mat src, Mat& dst, Mat& xDir, Mat& yDir, Mat& theta);
Mat padding(Mat img, int k_width, int k_height);
void convolve(Mat src, Mat& convolved, Mat kernel);
Mat sobelX(Mat src);
Mat sobelY(Mat src);
void sobel(Mat src, Mat& dst, Mat& theta);
void imageSqr(Mat src, Mat& result);

class snakes{
  public:
    Mat image,lineE;
    double alpha,beta,gamma;
    int iterations;
    CONTOUR contour;
  public:
    snakes();
    ~snakes();
    void loadImage(const Mat&);
    void setParametars(double,double,double,int);
    void makePts(CONTOUR selectedPts);
    void gradientMap();
    double extEnergy(CONTOUR points,int lineWeight, int edgeWeight);
    std::pair<double,double> innerEnergy(CONTOUR contour);
    void iterateSnake();
};

int main(void){
    snakes snake;
    Mat image =cv::imread("C:\\Users\\moamen\\Desktop\\snakes\\apple.jpg", IMREAD_GRAYSCALE);
    snake.loadImage(image);
    snake.setParametars(1,1,1,100);
    vector<pair<int,int>> points;
    points.push_back(make_pair(28,192));
    points.push_back(make_pair(96,83));
    points.push_back(make_pair(182,31));
    points.push_back(make_pair(329,58));
    points.push_back(make_pair(377,173));
    points.push_back(make_pair(361,326));
    points.push_back(make_pair(266,387));
    points.push_back(make_pair(105,383));
    snake.makePts(points);
    snake.iterateSnake();
    return 0;
}

void gradTheta(Mat src, Mat& dst, Mat& xDir, Mat& yDir, Mat& theta) {
    theta = Mat::zeros(src.size(), CV_64FC1);
    for (int i = 0; i < dst.cols; i++) {
        for (int j = 0; j < dst.rows; j++) {
            // calculate sobel pixel value
            dst.at<uchar>(j, i) = sqrt(pow(xDir.at<uchar>(j, i), 2) + pow(yDir.at<uchar>(j, i), 2));

            //std::cout << "grad";
            // calculate orientation
            if (yDir.at<uchar>(j, i) == 0) {
                theta.at<double>(j, i) = 90;

            }
            else {
                theta.at<double>(j, i) = atan((xDir.at<uchar>(j, i) / yDir.at<uchar>(j, i))) * 180 / 3.14;
            }
        }
    }
}
void imageSqr(Mat src, Mat& result) {
    result = Mat::zeros(src.size(), CV_64FC1);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0;j < src.cols; j++) {
            result.at<double>(i, j) = pow(src.at<uchar>(i, j), 2);
        }
    }
    result.convertTo(result, CV_8UC1);
}
Mat padding(Mat img, int k_width, int k_height)
{
    Mat scr;
    img.convertTo(scr, CV_64FC1);

    int p_rows, p_cols;
    p_rows = (k_height - 1) / 2;
    p_cols = (k_width - 1) / 2;
    // zero padding
    Mat p_image(Size(scr.cols + 2 * p_cols, scr.rows + 2 * p_rows), CV_64FC1, Scalar(0));
    scr.copyTo(p_image(Rect(p_cols, p_rows, scr.cols, scr.rows)));

    return p_image;
}
void convolve(Mat src, Mat& convolved, Mat kernel) {
    Mat p_image;
    p_image = padding(src, 3, 3);


    Mat output = Mat::zeros(src.size(), CV_64FC1);

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            output.at<double>(i, j) = sum(kernel.mul(p_image(Rect(j, i, 3, 3)))).val[0];
        }
    }

    output.convertTo(convolved, CV_8UC1);

};
Mat sobelX(Mat src) {

    Mat_<double> sobelX(3, 3);
    sobelX << -1, -2, -1,
        0, 0, 0,
        1, 2, 1;
    Mat soblXResult;
    //convolve(src, sobelX, dst);
    convolve(src, soblXResult, sobelX);
    return soblXResult;
}
Mat sobelY(Mat src) {


    Mat_<double> sobelY(3, 3);

    sobelY << -1, 0, 1,
        -2, 0, 2,
        -1, 0, -1;

    Mat soblYResult;

    convolve(src, soblYResult, sobelY);
    return soblYResult;}
void sobel(Mat src, Mat& dst, Mat& theta) {
    // apply gauessian blur first
    //apply sobel x
    Mat soblXResult, soblYResult;
    soblXResult = sobelX(src);
    //apply sobel y
    soblYResult = sobelY(src);
    dst = Mat::zeros(src.size(), CV_8UC1);
    gradTheta(src, dst, soblXResult, soblYResult, theta);

}
snakes::snakes(){}
snakes::~snakes(){}
void snakes::loadImage(const Mat& image){
    this->image = image;
    //cv::cvtColor(this->image,this->image,COLOR_BGR2GRAY);
}
void snakes::setParametars(double alpha, double beta, double gamma, int iterations){
    this->alpha      = alpha      ;
    this->beta       = beta       ;
    this->gamma      = gamma      ;
    this->iterations = iterations ;
}
void snakes::gradientMap(){
    //apply gauessian filter
    Mat  grad, gradSqr, edgeE, theta;
    // get the gradient

    sobel(this->image, edgeE, theta); //the edge energy
    imageSqr(edgeE,gradSqr);
    GaussianBlur(gradSqr, this->lineE, cv::Size(7, 7), 0); // the line energy
}
std::pair<double,double> snakes::innerEnergy(CONTOUR contour){
    double elasticEnergy =0,smoothEnergy=0;
    for(int i =0;i<this->contour.size();i++){
        double tempElastic = 0, tempSmooth = 0;
        if(i == 0){
            tempElastic = pow((contour[i+1].first-contour[i].first),2) + pow((contour[i+1].second-contour[i].second),2);
            tempSmooth =  pow((contour[i+1].first - 2*contour[i].first + contour[contour.size()-1].first),2)+
                          pow((contour[i+1].second - 2*contour[i].second + contour[contour.size()-1].second),2);
        }
        else if (i == contour.size()-1){
            tempElastic = pow((contour[0].first-contour[i].first),2) + pow((contour[0].second-contour[i].second),2);
            tempSmooth =  pow((contour[0].first - 2*contour[i].first + contour[i-1].first),2)+
                          pow((contour[0].second - 2*contour[i].second + contour[i-1].second),2);
        }
        else{
            tempElastic = pow((contour[i+1].first-contour[i].first),2) + pow((contour[i+1].second-contour[i].second),2);
            tempSmooth =  pow((contour[i+1].first - 2*contour[i].first + contour[i-1].first),2)+
                          pow((contour[i+1].second - 2*contour[i].second + contour[i-1].second),2);
        }
        elasticEnergy +=tempElastic;
        smoothEnergy  +=tempSmooth;
    }
    std::pair<double,double>inEnergy = std::make_pair(this->alpha*elasticEnergy,this->beta*smoothEnergy);
    return(inEnergy);
}
double snakes::extEnergy(CONTOUR points,int lineWeight, int edgeWeight) {

    double externalEnergy=0;
    for(int i =0;i<points.size();i++){
        externalEnergy += this->lineE.at<uchar>(points[i].second,points[i].first);
    }
    return this->gamma*externalEnergy;
}
void snakes::makePts(CONTOUR selectedPts){
    for(int i =0;i<selectedPts.size();i++){
        if(i !=(selectedPts.size()-1)){
            LineIterator it (this->image,Point(selectedPts[i].first,selectedPts[i].second)
                             ,Point(selectedPts[i+1].first,selectedPts[i+1].second),8);
            for(int i =0;i<it.count;i++){
                this->contour.push_back(std::make_pair(it.pos().x,it.pos().y));
                it++;
            }
        }
        else{
            LineIterator it (this->image,Point(selectedPts[i].first,selectedPts[i].second)
                             ,Point(selectedPts[0].first,selectedPts[0].second),8);
            for(int i =0;i<it.count;i++){
                this->contour.push_back(std::make_pair(it.pos().x,it.pos().y));
                it++;

            }
        }
    }
//    for(auto i=this->contour.begin();i != this->contour.end();i=i+5){
//        this->contour.erase(i);
//    }
}

void snakes::iterateSnake(){
    std::vector<std::pair<int,int>> window;
    window.push_back(make_pair(-1,-1));
    window.push_back(make_pair(-1,0));
    window.push_back(make_pair(-1,1));
    window.push_back(make_pair(0,-1));
    window.push_back(make_pair(0,0));
    window.push_back(make_pair(0,1));
    window.push_back(make_pair(1,-1));
    window.push_back(make_pair(1,0));
    window.push_back(make_pair(1,1));
    snakes::gradientMap();
    auto tempPts = this->contour;
    cout<<"contour size is "<<this->contour.size()<<endl;
    for(int k=0;k<this->iterations;k++){

        for(int i =0;i<this->contour.size();i++){
            int minx=0,miny=0;
            double minEnergy =1000000000;
            for(int j =0 ;j<window.size();j++){
                auto windowPts = tempPts;
                windowPts[i].first += window[j].first;
                windowPts[i].second += window[j].second;
                auto inner = innerEnergy(windowPts);
                auto external = snakes::extEnergy(windowPts,1,8);
                double totalEnergy =10000000;
                if(windowPts[i].second<=0 || windowPts[i].second>=this->image.rows || windowPts[i].first<=0 || windowPts[i].first>=this->image.cols)
                    totalEnergy =10000000;
                else
                    totalEnergy = -1*external+inner.first+inner.second;
                if(totalEnergy<minEnergy){
                    minx = windowPts[i].first;
                    miny = windowPts[i].second;
                }
            }
            tempPts[i].first= minx;
            tempPts[i].second= miny;
        }
        cout<<"iteration is "<<k<<endl;
        }
    Mat contour (this->image.size(),  CV_8UC1, Scalar(0));
        for (int i = 0; i <tempPts.size(); i++) {
            if(tempPts[i].second<=0 || tempPts[i].second>=this->image.rows || tempPts[i].first<=0 || tempPts[i].first>=this->image.cols)
                continue;
            else
                contour.at<uchar>(tempPts[i].second, tempPts[i].first) =  255;
        }
        imshow("cont", contour);
        cv::waitKey(0);

    this->contour = tempPts;
}





//    Mat frame;
//    Point pt(-1, -1);
//    bool newCoords = false;

//    void mouse_callback(int  event, int  x, int  y, int  flag, void* param)
//    {
//        if (event == EVENT_LBUTTONDOWN)
//        {
//            // Store point coordinates
//            pt.x = x;
//            pt.y = y;
//            newCoords = true;
//        }
//    }

//    int main(int){
//        std::string image_path = samples::findFile("C:\\Users\\moamen\\Desktop\\snakes\\apple.jpg");
//        Mat image = imread(image_path, IMREAD_GRAYSCALE);

//        namedWindow("img", 1);

//        // Set callback
//        setMouseCallback("img", mouse_callback);

//        for (;;)
//        {
//           // cap >> frame; // get a new frame from camera

//            // Show last point clicked, if valid
//            if (pt.x != -1 && pt.y != -1)
//            {
//                circle(image, pt, 3, Scalar(0, 0, 255));

//                if (newCoords)
//                {
//                    std::cout << "Clicked coordinates: " << pt << std::endl;
//                    newCoords = false;
//                }
//            }

//            imshow("img", image);

//            // Exit if 'q' is pressed
//            if ((waitKey(1) & 0xFF) == 'q') break;
//        }
//        // the camera will be deinitialized automatically in VideoCapture destructor
//        return 0;
//    }












