#ifndef _VIDEO_DISPLAY_H
#define _VIDEO_DISPLAY_H

// OpenCV
//#include "cv.h"
//#include "highgui.h"
#include <opencv2/opencv.hpp>
// OpenGL
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

// C++ standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;
using namespace cv;

#define IMAGE_WIDTH (640+20)
#define IMAGE_HEIGHT (480+20)
#define IMAGE_PIX_SIZE 3
#define IMAGE_BUFFER_SIZE IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_PIX_SIZE

#define DISPLAY_WIN	"Video Display"

using namespace std;
using namespace cv;

extern GLint imgHeight, imgWidth;

extern vector<videoModeDef> videoModes;
extern int menu;

//opengl callbacks and globals
void display(void);
void selectVideoMode(int value);
void reshape(int w, int h);
void keypress(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

class VideoDisplayClass {
	pthread_t VideoDisplayThread;

public:
	VideoDisplayClass() {

	};

	void init();
	int displayImage(unsigned char imgbuf[IMAGE_BUFFER_SIZE]);
	void initOpenGLUT();
	void refreshPopUpMenu();
	void clearPopUpMenu();

private:

    static void * videoDisplayThreadHelper(void * This)
    {
    	((VideoDisplayClass *)This)->videoDisplayThread();
    	return NULL;
    }

	void videoDisplayThread() {
		initOpenGLUT();
	}

};

#endif
