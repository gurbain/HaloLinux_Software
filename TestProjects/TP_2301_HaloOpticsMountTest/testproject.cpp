#include "testproject.h"

#include <iostream>

using namespace std;

testproject *test = NULL;

void terminateHandlerHelper(int sig)
{
	test->terminateHandler(sig);
}

void preInit()
{
	test = new testproject;

	signal(SIGTERM, terminateHandlerHelper);
	signal(SIGABRT, terminateHandlerHelper);
	signal(SIGINT, terminateHandlerHelper);

}

int main (int argc, char *argv[])
{
	preInit();

	test->runMain(argc, argv);

	delete test;
	return 0;

}

void testproject::GSrunMain()
{
	this->captureAndRectifyImages(this->leftImage, this->rightImage);

	this->GSprocessImages(this->leftImage, this->rightImage);

}


double testproject::timeDiff(struct timespec *start, struct timespec *end)
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

double testproject::round (double value)
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


testproject::testproject () {
	maneuverNumber = 1;
	averageTime = 0;
	time_samples = 0;
	max_depth = 100.0;
	min_depth = 0.01;
	SubSample = false;
	equalize = false;
	pthread_mutex_init(&keymutex, NULL);
}

void testproject::GSsetup(){
	
	opticsmountGSP::init();

	videostreaming.createNew_MatVideoBuffer(stereoVideoBuffer, "Stereo Disparity");
	videostreaming.createNew_MatVideoBuffer(blobVideoBuffer, "Blob Tracking");
	videostreaming.setAsDefaultVideoMode(blobVideoBuffer);

	this->rectifier.getCameraParameters(Q);

	previous_meanvec[0] = 0;
	previous_meanvec[1] = 0;
	previous_meanvec[2] = 0;

	previous2_meanvec[0] = 0;
	previous2_meanvec[1] = 0;
	previous2_meanvec[2] = 0;

	previous_meanvel[0] = 0;
	previous_meanvel[1] = 0;
	previous_meanvel[2] = 0;

	previous2_meanvel[0] = 0;
	previous2_meanvel[1] = 0;
	previous2_meanvel[2] = 0;

	camera_delta_T = 1/this->cameras.getFrameRate();	//seconds
	measurement_delta_T = 0.0;


	save_frame_number = 0;
	/////////////////////////////////////////////////////////////////////////
	// Setup: Load camera calibration
	/////////////////////////////////////////////////////////////////////////

	// TODO: Easier access to key parameters (f, cx, cy, tx) from GSP core
	cv::Mat Qin, Rin, Tin, R1in, P1in, R2in, P2in, M1in, D1in, M2in, D2in;
	double Txin, Tyin, Tzin, fin, cxin, cyin;
	rectifier.getCameraParameters(Qin, Rin, Tin, R1in, P1in, R2in, P2in, M1in, D1in, M2in, D2in,
			Txin, Tyin, Tzin, fin, cxin, cyin);

	// Store camera settings
	camera.f = M1in.at<double>(0,0);
	camera.cx = M1in.at<double>(0,2);
	camera.cy = M1in.at<double>(1,2);
	// P2(0,3)=Tx*f, see:
	// http://opencv.willowgarage.com/documentation/camera_calibration_and_3d_reconstruction.html#stereorectify
	//camera.tx = - P2in.at<double>(0,3) / P1in.at<double>(0,0) * 2.54/100.; // inch to meter conversion;
	camera.tx = - Tin.at<double>(0,0)*0.0254; // inch to meter conversion;
	camera.width =  cameras.getImageWidth();
	camera.height = cameras.getImageHeight();

//	std::cout << "Min, Max Depth: " << min_depth << " , " << max_depth << std::endl;

    // Calculate disparity range
	minDisparity = camera.tx * camera.f / max_depth;
	maxDisparity = camera.tx * camera.f / min_depth;


	//TRIANGULATOR
	triangulator = new SimpleTriangulator(camera);
	triangulator->minDisparity = minDisparity;
	triangulator->maxDisparity = maxDisparity;
	triangulator->maxVerticalOffset = 1.0;

	denseStereo = new DenseStereo(*triangulator, this->SubSample);

	string imu_log_filename = this->datastorage.getGSdatastoragePath() + "imu_data.txt";
	imu_log_file.open(imu_log_filename.c_str());

	string forceTorque_log_filename = this->datastorage.getGSdatastoragePath() + "forceTorque_data.txt";
	forceTorque_log_file.open(forceTorque_log_filename.c_str());

	string posVel_log_filename = this->datastorage.getGSdatastoragePath() + "posVel_data.txt";
	posVel_log_file.open(posVel_log_filename.c_str());

	string inst_posVel_log_filename = this->datastorage.getGSdatastoragePath() + "inst_posVel_data.txt";
	inst_posVel_log_file.open(inst_posVel_log_filename.c_str());

	this->initVideoStreaming();
}

void testproject::GScleanup() {
	imu_log_file.close();
	forceTorque_log_file.close();
	posVel_log_file.close();
	inst_posVel_log_file.close();

	opticsmountGSP::shutdown();
		
}

void testproject::blobTrack(Mat& leftImage, Mat& rightImage) {
	Mat disparityImage, outImg, outImgStereo, disp, newDisp, elasDispImage, elasDispMap, elasDisp;
	Mat nonthresholded_img_small, nonthresholded_img;
	Mat threeD;
	  double Q03, Q13, Q23, Q32, Q33;
	  msg_body_tgt_location targetMessage;

	  Q03 = Q.at<double>(0,3);
	  Q13 = Q.at<double>(1,3);
	  Q23 = Q.at<double>(2,3);
	  Q32 = Q.at<double>(3,2);
	  Q33 = Q.at<double>(3,3);

	  measurement_delta_T += camera_delta_T;

	  if (equalize) {
		  equalizeHist(leftImage, leftImage);
		  equalizeHist(rightImage, rightImage);
	  }

	  try {

		 if (denseStereo->getSubsample()) {
			 elasDispImage = denseStereo->calculate(leftImage, rightImage);
			 cv::resize(elasDispImage, elasDisp,cv::Size(2.0*elasDispImage.cols, 2.0*elasDispImage.rows),0, 0, cv::INTER_NEAREST);
			 nonthresholded_img_small = denseStereo->getPreThreshDisp();

			 cv::resize(nonthresholded_img_small, nonthresholded_img,cv::Size(2.0*nonthresholded_img_small.cols, 2.0*nonthresholded_img_small.rows),0, 0, cv::INTER_NEAREST);
//			 std::cout << "rows/cols small: " << nonthresholded_img_small.rows << ", " <<  nonthresholded_img_small.cols << std::endl;
//			 std::cout << "rows/cols: " << nonthresholded_img.rows << ", " <<  nonthresholded_img.cols << std::endl;
		 } else {
			 elasDisp = denseStereo->calculate(leftImage, rightImage);
			 nonthresholded_img = denseStereo->getPreThreshDisp();
		 }


			Vec3d meanvec(0,0,0);
			Vec3d meanvel(0,0,0);
			Vec3d fir_meanvec(0.0, 0.0, 0.0);
			Vec3d fir_meanvel(0.0, 0.0, 0.0);


			meanvec[0] = denseStereo->meanPoints(0);
			meanvec[1] = denseStereo->meanPoints(1);
			meanvec[2] = denseStereo->meanPoints(2);

			//compute velocity
			meanvel[0] = (meanvec[0] - previous_meanvec[0])/measurement_delta_T;
			meanvel[1] = (meanvec[1] - previous_meanvec[1])/measurement_delta_T;
			meanvel[2] = (meanvec[2] - previous_meanvec[2])/measurement_delta_T;
/*
			printf("previous2_meanvec: %f, %f, %f\n", previous2_meanvec[0], previous2_meanvec[1], previous2_meanvec[2]);
			printf("previous_meanvec: %f, %f, %f\n", previous_meanvec[0], previous_meanvec[1], previous_meanvec[2]);
			printf("meanvec: %f, %f, %f\n", meanvec[0], meanvec[1], meanvec[2]);
			printf("previous2_meanvel: %f, %f, %f\n", previous2_meanvel[0], previous2_meanvel[1], previous2_meanvel[2]);
			printf("previous_meanvel: %f, %f, %f\n", previous_meanvel[0], previous_meanvel[1], previous_meanvel[2]);
			printf("meanvel: %f, %f, %f\n", meanvel[0], meanvel[1], meanvel[2]);
*/
			fir_meanvec[0] = (meanvec[0] + previous_meanvec[0] + previous2_meanvec[0]) / 3.0f;
			fir_meanvec[1] = (meanvec[1] + previous_meanvec[1] + previous2_meanvec[1]) / 3.0f;
			fir_meanvec[2] = (meanvec[2] + previous_meanvec[2] + previous2_meanvec[2]) / 3.0f;

			fir_meanvel[0] = (meanvel[0] + previous_meanvel[0] + previous2_meanvel[0]) / 3.0f;
			fir_meanvel[1] = (meanvel[1] + previous_meanvel[1] + previous2_meanvel[1]) / 3.0f;
			fir_meanvel[2] = (meanvel[2] + previous_meanvel[2] + previous2_meanvel[2]) / 3.0f;
/*
			printf("fir_meanvec: %f, %f, %f\n", fir_meanvec[0], fir_meanvec[1], fir_meanvec[2]);
			printf("fir_meanvel: %f, %f, %f\n", fir_meanvel[0], fir_meanvel[1], fir_meanvel[2]);
*/
			previous2_meanvec[0] = previous_meanvec[0];
			previous2_meanvec[1] = previous_meanvec[1];
			previous2_meanvec[2] = previous_meanvec[2];

			previous2_meanvel[0] = previous_meanvel[0];
			previous2_meanvel[1] = previous_meanvel[1];
			previous2_meanvel[2] = previous_meanvel[2];

			previous_meanvec[0] = meanvec[0];
			previous_meanvec[1] = meanvec[1];
			previous_meanvec[2] = meanvec[2];

			previous_meanvel[0] = meanvel[0];
			previous_meanvel[1] = meanvel[1];
			previous_meanvel[2] = meanvel[2];

			targetMessage.xyzPos[0] = (float) fir_meanvec[0];
			targetMessage.xyzPos[1] = (float) fir_meanvec[1];
			targetMessage.xyzPos[2] = (float) fir_meanvec[2];
			targetMessage.xyzVel[0] = (float) fir_meanvel[0];
			targetMessage.xyzVel[1] = (float) fir_meanvel[1];
			targetMessage.xyzVel[2] = (float) fir_meanvel[2];

			if (this->spheres.useSpheres) {
				if (!isfinite(targetMessage.xyzPos[0]) || !isfinite(targetMessage.xyzPos[1]) || !isfinite(targetMessage.xyzPos[2]) || !isfinite(targetMessage.xyzVel[0]) || !isfinite(targetMessage.xyzVel[1]) || !isfinite(targetMessage.xyzVel[2])) {
					printf("Non-Finite Pos/Vel Detected - no message sent to spheres.\n");
				} else {
					this->spheres.sendUserMessage(TGT_POSITION, (void *) &targetMessage, sizeof(targetMessage));
					std::cout << "SEND TGT MSG: [" << targetMessage.xyzPos[0] << "," << targetMessage.xyzPos[1] << "," << targetMessage.xyzPos[2] << "," << targetMessage.xyzVel[0] << "," << targetMessage.xyzVel[1] << "," << targetMessage.xyzVel[2] << "]\n";
				}
			}

			double curr_test_time = updateTime();

			posVel_log_file << curr_test_time << ","  << targetMessage.xyzPos[0] << "," << targetMessage.xyzPos[1] << "," << targetMessage.xyzPos[2] << "," << targetMessage.xyzVel[0] << "," << targetMessage.xyzVel[1] << "," << targetMessage.xyzVel[2] << std::endl;
			inst_posVel_log_file << curr_test_time << ","  << meanvec[0] << "," << meanvec[1] << "," << meanvec[2] << "," << meanvel[0] << "," << meanvel[1] << "," << meanvel[2] << std::endl;

				Size size( leftImage.cols + rightImage.cols, MAX(leftImage.rows, rightImage.rows) );

				//place two images side by side
				outImg.create( size, CV_MAKETYPE(leftImage.depth(), 3) );
				Mat outImgLeft = outImg( Rect(0, 0, leftImage.cols, leftImage.rows) );
				Mat outImgRight = outImg( Rect(leftImage.cols, 0, rightImage.cols, rightImage.rows) );

				outImgStereo.create( size, CV_MAKETYPE(leftImage.depth(), 3) );
				Mat outImgStereoLeft = outImgStereo( Rect(0, 0, leftImage.cols, leftImage.rows) );
				Mat outImgStereoRight = outImgStereo( Rect(leftImage.cols, 0, rightImage.cols, rightImage.rows) );


				cvtColor( leftImage, outImgLeft, CV_GRAY2BGR );
				cvtColor( leftImage, outImgStereoLeft, CV_GRAY2BGR );
				cvtColor( elasDisp, outImgRight, CV_GRAY2BGR );
				cvtColor( nonthresholded_img, outImgStereoRight, CV_GRAY2BGR );



				circle(outImgRight, Point(targetMessage.xyzPos[0]/targetMessage.xyzPos[2]*camera.f+camera.cx,targetMessage.xyzPos[1]/targetMessage.xyzPos[2]*camera.f+camera.cy), 7,CV_RGB(0x00,0xFF,0x00),-1, CV_AA);

				stringstream waitText;
				if (isnan(targetMessage.xyzPos[0]) || isnan(targetMessage.xyzPos[1]) || isnan(targetMessage.xyzPos[2]) || isnan(targetMessage.xyzVel[0]) || isnan(targetMessage.xyzVel[1]) || isnan(targetMessage.xyzVel[2])) {
					waitText.str("");
					waitText << "NaN Detected - No Message Sent to SPHERE";
					putText(outImgLeft, waitText.str(), Point(20,440), FONT_HERSHEY_SIMPLEX,0.5, CV_RGB(0,255,0),1.5,CV_AA,false);
				} else {
					waitText.str("");
					waitText << std::fixed << setprecision(3) << showpos << "Position (m):   [" << targetMessage.xyzPos[0] << ", " << targetMessage.xyzPos[1] << ", " << targetMessage.xyzPos[2] << "]";
					putText(outImgLeft, waitText.str(), Point(20,420), FONT_HERSHEY_SIMPLEX,0.5, CV_RGB(0,255,0),1.5,CV_AA,false);

					waitText.str("");
					waitText << std::fixed << setprecision(3) << showpos << "Velocity (m/s): [" << targetMessage.xyzVel[0] << ", " << targetMessage.xyzVel[1] << ", " << targetMessage.xyzVel[2] << "]";
					putText(outImgLeft, waitText.str(), Point(20,440), FONT_HERSHEY_SIMPLEX,0.5, CV_RGB(0,255,0),1.5,CV_AA,false);
				}

				if (this->datastorage.autoImageStorage) {
					stringstream filename;
					filename << this->datastorage.getGSdatastoragePath() << "BlobTrackImg" << save_frame_number << ".bmp";
					cv::imwrite(filename.str(), outImg);
					filename.clear();

					stringstream filename2;
					filename2 << this->datastorage.getGSdatastoragePath() << "StereoImg" << save_frame_number << ".bmp";
					cv::imwrite(filename2.str(), nonthresholded_img);

					save_frame_number++;
				}

			if (videostreaming.videoStreamingOn) {
				videostreaming.update_MatVideoBuffer(blobVideoBuffer, outImg);

				cvtColor( nonthresholded_img, outImgRight, CV_GRAY2BGR );
				videostreaming.update_MatVideoBuffer(stereoVideoBuffer, outImgStereo);
			}

		//output data

	//    printf("Mean: (%f, %f, %f)\n", targetMessage.xyzPos[0], targetMessage.xyzPos[1], targetMessage.xyzPos[2]);
	//    printf("Velocity: (%f, %f, %f)\n", targetMessage.xyzVel[0], targetMessage.xyzVel[1], targetMessage.xyzVel[2]);

			measurement_delta_T = 0.0;
	  } catch (DenseStereoException& e) {
		  std::cout << e.what() << std::endl;
		  std::cout << "Measurement Delta T: " << measurement_delta_T << std::endl;
	  } catch (...) {
		  std::cout << "Default exception handler" << std::endl;
		  std::cout << "Measurement Delta T: " << measurement_delta_T << std::endl;
	  }

	pthread_mutex_lock(&keymutex);
	if (charKey == 0x1B) { // ESC
		printf("Quitting...\n");
		shutdownCriterion = true;
	} else if (charKey == 0x31) {
		stereosad.numberOfDisparities +=16;
		std::cout << "Num Disparities: " << stereosad.numberOfDisparities << std::endl;
	} else if (charKey == 0x32) {
		stereosad.numberOfDisparities -=16;
		std::cout << "Num Disparities: " << stereosad.numberOfDisparities << std::endl;
	} else if (charKey == 0x35) {
		stereosad.minDisparity += 5;
		std::cout << "Min Disparities: " << stereosad.minDisparity << std::endl;
	} else if (charKey == 0x36) {
		stereosad.minDisparity -= 5;
		std::cout << "Min Disparities: " << stereosad.minDisparity << std::endl;
	}
	charKey = 0x00;
	pthread_mutex_unlock(&keymutex);
}

void testproject::GSprocessImages(Mat& leftImage, Mat& rightImage) {

	clock_gettime(CLOCK_REALTIME, &time1);

	blobTrack(leftImage, rightImage);

	clock_gettime(CLOCK_REALTIME, &time2);
	delT = timeDiff(&time1, &time2);

	time_samples++;
	if (time_samples % 10 != 0) {
		averageTime += delT;
	} else {
		averageTime /= 10;
		printf("Average time to complete blob track: %f ms\n", averageTime);
		averageTime = 0;
		time_samples = 0;
	}

}

void testproject::GSprocessGuiKeyPress(unsigned char networkkey) {
	pthread_mutex_lock(&keymutex);
	charKey = networkkey;
	pthread_mutex_unlock(&keymutex);
}

void testproject::GSparseCommandlineArgs(int argc, char *argv[]) {

// Read in path from environment variable
	if (argc>3) {

		sprintf(this->calibParamSetName, "%s", argv[3]);

	}
	else {

		cout << endl << endl << "Not enough command line arguments" << endl << "Please specify path to Camera Calibration Parameters!" << endl;

		cout << "Usage: <name of camera calibration set>" << endl << endl;

		exit(1);

	}


}

void testproject::GSparseParameterFile(string line) {
	  string searchString;
	  string foundString;
	  size_t found;
	  
	  opticsmountGSP::parseParameterFile(line);

	  searchString = "SUBSAMPLE";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true") {
			this->SubSample = true;
		}
		cout << "SUBSAMPLE:  " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  searchString = "EQUALIZE";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true") {
			this->equalize = true;
		}
		cout << "EQUALIZE:  " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  searchString = "MIN_DEPTH";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  min_depth =  atof(foundString.c_str());
		  cout << "MIN_DEPTH: " << min_depth << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  searchString = "MAX_DEPTH";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  max_depth =  atof(foundString.c_str());
		  cout << "MAX_DEPTH: " << max_depth << endl;
	  }
	  searchString.clear();
	  found = string::npos;

}

void testproject::GSprocessIMU(double timestamp, msg_imu_data imu_data) {
/*
	cout << "Time: " << timestamp << ", ";
	cout << "Gyro, Accel: [" << imu_data.gyro[0] << "," << imu_data.gyro[1] << "," << imu_data.gyro[2] << "],";
	cout << "Accel: [" << imu_data.accel[0] << "," << imu_data.accel[1] << "," << imu_data.accel[2] << "]" << endl;
*/

	imu_log_file << timestamp << ","  << imu_data.gyro[0] << "," << imu_data.gyro[1] << "," << imu_data.gyro[2] << "," << imu_data.accel[0] << "," << imu_data.accel[1] << "," << imu_data.accel[2] << std::endl;
}

void testproject::GSprocessForceTorque(double timestamp, msg_body_forces_torques FT_data) {
/*
	cout << "Time: " << timestamp << ", ";
	cout << "Gyro, Accel: [" << imu_data.gyro[0] << "," << imu_data.gyro[1] << "," << imu_data.gyro[2] << "],";
	cout << "Accel: [" << imu_data.accel[0] << "," << imu_data.accel[1] << "," << imu_data.accel[2] << "]" << endl;
*/

	forceTorque_log_file << timestamp << ","  << FT_data.forces[0] << "," << FT_data.forces[1] << "," << FT_data.forces[2] << "," << FT_data.torques[0] << "," << FT_data.torques[1] << "," << FT_data.torques[2] << std::endl;
}
