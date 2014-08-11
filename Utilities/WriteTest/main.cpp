#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define REPEAT 10

#ifdef _DEBUG
    #pragma comment(lib,"cxcore200d.lib")
    #pragma comment(lib,"cv200d.lib")
    #pragma comment(lib,"highgui200d.lib")
#else
    #pragma comment(lib,"cxcore200.lib")
    #pragma comment(lib,"cv200.lib")
    #pragma comment(lib,"highgui200.lib")
#endif

using namespace cv;

double timeDiff(struct timespec *start, struct timespec *end)
{
	double milliseconds;
	double tv_sec, tv_nsec;
	if ((end->tv_nsec-start->tv_nsec)<0) {
		tv_sec = end->tv_sec-start->tv_sec-1;
		tv_nsec = 1.0e9+end->tv_nsec-start->tv_nsec;
	} else {
		tv_sec = end->tv_sec-start->tv_sec;
		tv_nsec = end->tv_nsec-start->tv_nsec;
	}
	milliseconds = tv_sec*1.0e3 + (tv_nsec / 1.0e6);
	return milliseconds;
}

double round (double value)
{

	if (value < 0)
	{
		return -(floor(-value+0.5));
	}
	else
	{
		return floor(value+0.5);
	}
}

int main(int argc, char* argv[])
{

	if (argc < 2) {
		fprintf(stderr, "No file specified\n");
		return -1;
	}
	std::string filename = argv[1];

        Mat img = imread(filename);
        //cvtColor( img, img, CV_BGR2GRAY );
/*
    Mat imgDbl;
    Mat img2;
    Size size( img.cols*2, img.rows);
    imgDbl.create( size, CV_MAKETYPE(img.depth(), 1) );
        Mat imgDblLeft = imgDbl( Rect(0, 0, img.cols, img.rows) );
        Mat imgDblRight = imgDbl( Rect(img.cols, 0, img.cols, img.rows) );

        cvtColor( img, imgDblLeft, CV_BGR2GRAY );
		cvtColor( img, imgDblRight, CV_BGR2GRAY );

		img = imgDbl;
*/
        cvtColor( img, img, CV_BGR2GRAY );

    if(img.empty())
    {
        fprintf(stderr, "Can not load image\n");
        return -1;
    }

    printf("Height: %d, Width: %d, Depth: %d\n", img.rows, img.cols, img.elemSize());

        vector<int> param;

        param.push_back(CV_IMWRITE_PNG_COMPRESSION);
        param.push_back(0);	 // MAX_MEM_LEVEL = 9

        float t1, t2, t_save;
        for(int k = 0; k <= 9; k++)
        {
                char strpng[256];
            	struct timespec time1, time2;
            	double delT[REPEAT];

                param[1] = k;
                t1 = (float)clock();

                for(int n = 0; n < REPEAT; n++) {
                	sprintf(strpng, "output_c%d-%d.bmp", k, 0);
                	clock_gettime(CLOCK_REALTIME, &time1);
                	imwrite(strpng, img, param);
                	//img.copyTo(img2);

/*
                	FILE *fp = fopen(strpng, "r");
                    fseek(fp, 0, SEEK_END);
*/
                	clock_gettime(CLOCK_REALTIME, &time2);
                	delT[n] = timeDiff(&time1, &time2);
                }
                t2 = (float)clock();
                t_save = (t2 - t1) * 0.001 / 10;

                printf("Compression = %d\n", k);
                for(int n=0; n < REPEAT; n++) {
                	printf("%4.2f ms, ", delT[n]);
                }
                printf("\n");

        }

    return 0;
}
