#include "networkServer.h"

//opengl globals
GLubyte glImage[IMAGE_BUFFER_SIZE];

GLint imgHeight, imgWidth, winHeight, winWidth;
GLfloat zoomX, zoomY, zoom;

//menu items
vector<videoModeDef> videoModes;
int menu;

pthread_mutex_t mutex;

void display(void)
{
	pthread_mutex_lock(&mutex);

	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);

	//pick zooms to fit to window size and keep aspect ratio
	zoomX = ((GLfloat) winWidth) / ((GLfloat) imgWidth);
	zoomY = ((GLfloat) winHeight) / ((GLfloat) imgHeight);
	if (zoomX > zoomY)
		zoom = zoomY;
	else
		zoom = zoomX;

//	printf("Zoom value: %f\n", zoom);


//	glPixelZoom(zoom,-zoom);
	glPixelZoom(zoom,zoom);
	glDrawPixels(imgWidth, imgHeight, GL_BGR,
                GL_UNSIGNED_BYTE, glImage);

	glutSwapBuffers();
	glutPostRedisplay();

	pthread_mutex_unlock(&mutex);
	usleep(50000);
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   winHeight = (GLint) h;
   winWidth = (GLint) w;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
//   gluOrtho2D(0.0, (GLdouble) w, (GLdouble) h, 0.0);
   gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void selectVideoMode(int value) {

	int retVal;
	unsigned int msgModeType = NEW_MODE_SEL;

	printf("Menu Selection: %d\n", value);

	//send data
	retVal = send(newsockfd, &msgModeType, sizeof(msgModeType), 0);
	if (retVal == -1 || retVal != sizeof(msgModeType))
	{
		printf("Send error: %s\n", strerror(errno));
	}

	//send data
	retVal = send(newsockfd, &value, sizeof(value), 0);
	if (retVal == -1 || retVal != sizeof(value))
	{
		printf("Send error: %s\n", strerror(errno));
	}

}

void keypress(unsigned char charkey, int x, int y) {
	unsigned int msgModeType = KEY_PRESS;
	pressedKey key;
	int retVal;

	//code to adjust for zooming
//	int blackBar = winHeight - imgHeight * zoom;
//	key.x = (int) (x / zoom);
//	key.y = (int) ((y - blackBar) / zoom);
	key.key = (unsigned int) charkey;

	printf("Keypress: %X", key.key);

	//send data
	retVal = send(newsockfd, &msgModeType, sizeof(msgModeType), 0);
	if (retVal == -1 || retVal != sizeof(msgModeType))
	{
		printf("Send error: %s\n", strerror(errno));
	}

	//send data
	retVal = send(newsockfd, &key, sizeof(key), 0);
	if (retVal == -1 || retVal != sizeof(key))
	{
		printf("Send error: %s\n", strerror(errno));
	}
}

void mouse(int button, int state, int x, int y) {
	unsigned int msgModeType = MOUSE_CLICK;
	int retVal;
	mouseClick click;

	int keyModifier = glutGetModifiers();

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && keyModifier == GLUT_ACTIVE_SHIFT) {
		//code to adjust for zooming
		int blackBar = winHeight - imgHeight * zoom;
		click.x = (int) (x / zoom);
		click.y = (int) ((y - blackBar) / zoom);

		if (click.x < 0 || click.x > imgWidth || click.y < 0 || click.y > imgHeight) {
			printf("Click out of bounds\n");
		} else {
			//send data
			retVal = send(newsockfd, &msgModeType, sizeof(msgModeType), 0);
			if (retVal == -1 || retVal != sizeof(msgModeType))
			{
				printf("Send error: %s\n", strerror(errno));
			}

			//send data
			retVal = send(newsockfd, &click, sizeof(click), 0);
			if (retVal == -1 || retVal != sizeof(click))
			{
				printf("Send error: %s\n", strerror(errno));
			}
		}
	}
}

void VideoDisplayClass::initOpenGLUT() {
	int argc = 0;
	char *argv;

	//Glut code
	glutInit(&argc, &argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("OpenGL");

	//OpenGL Code
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_MULTISAMPLE_ARB);
//	glutSetOption(GL_MULTISAMPLE);

	//Glut code
	menu = glutCreateMenu(selectVideoMode);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keypress);

	GLint buf[10], sbuf[10];
	cout << "GLUT_WINDOW_NUM_SAMPLES: " << glutGet(GLUT_WINDOW_NUM_SAMPLES) << endl;
	cout << "GLUT_VERSION: " << glutGet(0x01FC) << endl;
	glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, buf);
	printf ("number of sample buffers is %d\n", buf[0]);
	glGetIntegerv(GL_SAMPLES_ARB, sbuf);
	printf ("number of samples is %d\n", sbuf[0]);


	glutMainLoop();

}

void VideoDisplayClass::refreshPopUpMenu() {
	videoModeDef currentMode;

	glutAddMenuEntry("No Video", NO_VIDMODE);
	glutAddMenuEntry("Default Mode", DEFAULT_VIDMODE);

	vector<videoModeDef>::iterator it;
	for (it = videoModes.begin(); it < videoModes.end(); it++) {
		currentMode = *it;

	glutAddMenuEntry(currentMode.name, currentMode.modeNumber);

	}

}

void VideoDisplayClass::clearPopUpMenu() {
	int numItems = glutGet(GLUT_MENU_NUM_ITEMS);
	for (int i = 1; i <= numItems; i++) {
		glutRemoveMenuItem(1);
	}
	videoModes.clear();
}


int VideoDisplayClass::displayImage(unsigned char imgbuf[IMAGE_BUFFER_SIZE]) {
	int row, col, color;

	pthread_mutex_lock(&mutex);

	//stored in Blue, Green, Red column order for opencv matricies
	for (row = 0; row < imgHeight; row++) {
		for (col = 0; col < imgWidth; col++) {
			for (color = 0; color < 3; color ++) {
				glImage[row*imgWidth*3 + col*3 + color] = (GLubyte) imgbuf[(imgHeight-row-1)*imgWidth*3 + col*3 + color];
			}
		}
	}

	pthread_mutex_unlock(&mutex);
}

void VideoDisplayClass::init() {
	winHeight = IMAGE_HEIGHT;
	winWidth = IMAGE_WIDTH;

	pthread_mutex_init(&mutex, NULL);

	int retVal = pthread_create(&VideoDisplayThread, NULL, this->videoDisplayThreadHelper, this);
	if (retVal != 0)
	{
		printf("pthread_create VideoStreamingThread failed\n");
		exit(1);
	}

}
