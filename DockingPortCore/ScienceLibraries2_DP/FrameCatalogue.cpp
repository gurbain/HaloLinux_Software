/*
 * FrameCatalogue.cpp
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#include "FrameCatalogue.h"

FrameCatalogue::FrameCatalogue() {
}

void FrameCatalogue::extractFeatures(Frame & frame) {
	// Detect keypoints
	logger->log("Detect keypoints");
	std::vector<cv::KeyPoint> leftKeypoints, rightKeypoints;
	leftKeypoints.clear();
	rightKeypoints.clear();
	stopWatch->start("Detect Features");
	featureDetector->detect(frame.leftImage, leftKeypoints);
	featureDetector->detect(frame.rightImage, rightKeypoints);
	stopWatch->stop("Detect Features");

	// Compute feature descriptors
	logger->log("Compute feature descriptors");
	cv::Mat leftDescriptors, rightDescriptors;
	stopWatch->start("Extract Descriptors");
	descriptorExtractor->compute(frame.leftImage, leftKeypoints, leftDescriptors);
	descriptorExtractor->compute(frame.rightImage, rightKeypoints, rightDescriptors);
	stopWatch->stop("Extract Descriptors");

	// Match descriptors
	logger->log("Match descriptors");
	std::vector<cv::DMatch> leftRightMatches, rightLeftMatches;
	stopWatch->start("Match Features");
	featureMatcher->match(leftDescriptors, rightDescriptors, leftRightMatches);
	featureMatcher->match(rightDescriptors, leftDescriptors, rightLeftMatches);
	stopWatch->stop("Match Features");

	// Check matches
	logger->log("Check matches");
	std::vector<cv::DMatch> stereoMatches;
	stopWatch->start("Check Matches");
	featureMatcher->viceVersaCheck(leftRightMatches, rightLeftMatches, stereoMatches);
	featureMatcher->stereoSanityCheck(leftKeypoints, rightKeypoints, stereoMatches,
			triangulator->maxVerticalOffset, triangulator->minDisparity, triangulator->maxDisparity);
	stopWatch->stop("Check Matches");

	// Store good matches in frame
	logger->log("Store good matches in frame");
	stopWatch->start("Copy Good Features");
	frame.addFeatures(leftKeypoints, leftDescriptors, rightKeypoints, rightDescriptors, stereoMatches);
	stopWatch->stop("Copy Good Features");

	// Triangulate to obtain 3D points
	logger->log("Triangulate to obtain 3D points");
	stopWatch->start("Triangulate Features");
	Eigen::Vector3d* p;
	for (int i=0; i < (int)frame.leftKeypoints.size(); i++) {
		p = new Eigen::Vector3d(triangulator->triangulate(frame.leftKeypoints[i].pt, frame.rightKeypoints[i].pt));
		frame.keypoints3D.push_back( *p );
	}
	stopWatch->stop("Triangulate Features");
}

void FrameCatalogue::extractFeaturesOfAllFrames() {
	logger->log("Extracting features");
	for (int i=0; i < (int)frames.size(); i++) {
		extractFeatures(*frames[i]);
		logger->log(".", false);
	}
	logger->newLine();
	stopWatch->stats(std::cout);
}

void FrameCatalogue::getLinkedSets(std::vector< std::vector<int> > & linkedSets) {
	linkedSets.clear();

	std::vector< std::vector<int> > sets;
	std::vector<bool> visited (frames.size(), false);

	for (int i=0; i < (int)visited.size(); i++) {
		if (!visited[i]) {
			std::vector<int> set;
			searchLinkedFrames(i, visited, set);
			linkedSets.push_back(set);
		}
	}
}

void FrameCatalogue::searchLinkedFrames(int startFrameId, std::vector<bool> &visited, std::vector<int> &set) {
	visited[startFrameId] = true;
	set.push_back(startFrameId);

	for (int i=0; i < (int)frames[startFrameId]->linkedFrames.size(); i++) {
		if (!visited[frames[startFrameId]->linkedFrames[i]]) {
			searchLinkedFrames(frames[startFrameId]->linkedFrames[i], visited, set);
		}
	}
}

void FrameCatalogue::addNewFrame(const cv::Mat & leftImage, const cv::Mat & rightImage, bool doExtractFeatures) {
	// Add images to frame
	Frame* newFrame = new Frame();

	newFrame->leftImage = leftImage.clone();
	newFrame->rightImage = rightImage.clone();

	cv::equalizeHist(newFrame->leftImage, newFrame->leftImage);
	cv::equalizeHist(newFrame->rightImage, newFrame->rightImage);


	// Extract features if requested
	if (doExtractFeatures) {
		extractFeatures(*newFrame);
	}

	// Save frame
	frames.push_back(newFrame);

	std::stringstream logSS;
	logSS << "Total Frames: " << getNumberOfFrames() << std::endl;
	logger->log(logSS.str(), false);
}

void FrameCatalogue::addNewFrame() {
	Frame* newFrame = new Frame();
	frames.push_back(newFrame);
}

void FrameCatalogue::addImagesToFrame(int frameId, const cv::Mat & leftImage, const cv::Mat & rightImage) {
	frames[frameId]->leftImage = leftImage.clone();
	frames[frameId]->rightImage = rightImage.clone();
}

Frame FrameCatalogue::getFrame(const cv::Mat & leftImage, const cv::Mat & rightImage) {
	Frame currentFrame;
	currentFrame.leftImage = leftImage;
	currentFrame.rightImage = rightImage;
	extractFeatures(currentFrame);
	return currentFrame;
}

void FrameCatalogue::save(const std::string & dirname, bool withImages) {

	std::stringstream filename;
	filename << dirname << "frame-catalogue.yml";
	std::cout << "filename: " << filename.str() << std::endl;

	cv::FileStorage fs(filename.str(), cv::FileStorage::WRITE);

	if (frames.size() > 0) {
		bool useFeatures = (frames[0]->leftDescriptors.rows > 0);

		if (useFeatures) {
			fs << "descriptor_type" <<  frames[0]->leftDescriptors.row(0).type();
			fs << "descriptor_length" <<  frames[0]->leftDescriptors.cols;
		}

		std::stringstream leftImgName, rightImgName;
		fs << "frames" << "[";
		for (int i=0; i < (int)frames.size(); i++) {
			fs << "{";
			if (withImages) {
				leftImgName.str(""); rightImgName.str("");

				leftImgName << dirname << "leftFrameImg" << i << ".bmp";
				rightImgName << dirname << "rightFrameImg" << i << ".bmp";

				cv::imwrite(leftImgName.str(), frames[i]->leftImage);
				cv::imwrite(rightImgName.str(), frames[i]->rightImage);
			}

			// Convert Eigen::Matrix4d to cv::Mat
			cv::Mat pose = cv::Mat(4, 4, CV_64F);
			for (int k=0; k<4; k++) {
				for (int l=0; l<4; l++) {
					pose.at<double>(k, l) = frames[i]->pose(k, l);
				}
			}
			fs << "pose" << pose;

			if (useFeatures) {
				// points
				fs << "keypoints" << "[";
				for (int j=0; j < (int)frames[i]->leftKeypoints.size(); j++) {
					fs << "{";
					fs << "left_keypoint" << frames[i]->leftKeypoints[j].pt;
					fs << "right_keypoint" << frames[i]->rightKeypoints[j].pt;
					fs << "left_descriptor" <<  frames[i]->leftDescriptors.row(j);
					fs << "right_descriptor" <<  frames[i]->rightDescriptors.row(j);
					// Convert Eigen::Vector3d to cv::Point3d
					cv::Point3d keypoint3D(frames[i]->keypoints3D[j](0), frames[i]->keypoints3D[j](1), frames[i]->keypoints3D[j](2)) ;
					fs << "point_3d" << keypoint3D;
					fs << "}";
				}
				fs << "]";
			}
			fs << "}";
		}
		fs << "]";

		// Close file
		fs.release();
	}
}

void FrameCatalogue::load(const std::string & dirname) {
	frames.clear();
	std::stringstream filename;
	filename << dirname << "frame-catalogue.yml";

	cv::FileStorage fs(filename.str(), cv::FileStorage::READ);

	int descriptorLength = fs["descriptor_length"];
	int descriptor_type = fs["descriptor_type"];

	cv::FileNode framesEntries = fs["frames"];
	cv::FileNodeIterator it = framesEntries.begin(), it_end = framesEntries.end();
	int idx = 0;

	Frame* frame;

	cv::Mat *image;
	cv::Mat temp;
	cv::Mat *descriptor;
	cv::KeyPoint *keypoint;

	logger->log("Start loading Frame Catalogue...");

	int imgCnt = 0;
	std::stringstream leftImgName, rightImgName;
	for( ; it != it_end; ++it, idx++ ) {
		frame = new Frame();

		leftImgName.str(""); rightImgName.str("");

		leftImgName << dirname << "leftFrameImg" << imgCnt << ".bmp";
		rightImgName << dirname << "rightFrameImg" << imgCnt << ".bmp";
		imgCnt++;

		frame->leftImage = cv::imread(leftImgName.str(),CV_LOAD_IMAGE_GRAYSCALE);
		frame->rightImage = cv::imread(rightImgName.str(),CV_LOAD_IMAGE_GRAYSCALE);

/*		cv::imshow("left", frame->leftImage);
		cv::imshow("right", frame->rightImage);
		cv::waitKey(200);

		std::cout << "Read: \n" << leftImgName.str() << std::endl << rightImgName.str() << std::endl;
		std::cout << "Size Left: [" << frame->leftImage.size().height << ", " << frame->leftImage.size().width << ", " << frame->leftImage.depth() << "]\n";
		std::cout << "Size Right: [" << frame->rightImage.size().height << ", " << frame->rightImage.size().width << ", " << frame->rightImage.depth() << "]\n";
*/
		cv::Mat pose;
		(*it)["pose"] >> pose;
		// Convert from cv::Mat to Eigen::Matrix4d
		for (int i=0; i<4; i++) {
			for (int j=0; j<4; j++) {
				frame->pose(i, j) = pose.at<double>(i, j);
			}
		}

		cv::FileNode keypoints = (*it)["keypoints"];
		cv::FileNodeIterator it2 = keypoints.begin(), it2_end = keypoints.end();
		int idx2=0;

		frame->leftDescriptors = cv::Mat(keypoints.size(), descriptorLength, descriptor_type);
		frame->rightDescriptors = cv::Mat(keypoints.size(), descriptorLength, descriptor_type);

		std::vector<double> vals;

		for( ; it2 != it2_end; ++it2, idx2++ ) {

			vals.clear();
			(*it2)["left_keypoint"] >> vals;
			keypoint = new cv::KeyPoint;
			keypoint->pt.x = vals[0];
			keypoint->pt.y = vals[1];
			frame->leftKeypoints.push_back( *keypoint );

			vals.clear();
			(*it2)["right_keypoint"] >> vals;
			keypoint = new cv::KeyPoint;
			keypoint->pt.x = vals[0];
			keypoint->pt.y = vals[1];
			frame->rightKeypoints.push_back( *keypoint );

			Eigen::Vector3d p3;
			vals.clear();
			(*it2)["point_3d"] >> vals;
			p3(0) = vals[0];
			p3(1) = vals[1];
			p3(2) = vals[2];
			frame->keypoints3D.push_back( p3 );

			descriptor = new cv::Mat;
			(*it2)["left_descriptor"] >> *descriptor;
			temp = frame->leftDescriptors.row(idx2);
			descriptor->copyTo(temp);

			descriptor = new cv::Mat;
			(*it2)["right_descriptor"] >> *descriptor;
			temp = frame->rightDescriptors.row(idx2);
			descriptor->copyTo(temp);
		}

		frames.push_back(frame);
	}
	fs.release();

	std::stringstream logstringstream;
	logstringstream << "Loaded " << frames.size() << " frames.";
	logger->log(logstringstream.str());
}



Frame* FrameCatalogue::getCurrentFrame() const {
	if (frames.size() >= 1)
		return frames.back();
	else
		return NULL;
}

Frame* FrameCatalogue::getFrame(int frameId) const {
	if (frameId >= 0 && frameId < (int)frames.size())
		return frames[frameId];
	else
		return NULL;
}

int FrameCatalogue::getNumberOfFrames() const {
	return frames.size();
}

void FrameCatalogue::setFeatureDetector(cv::FeatureDetector & _featureDetector) {
	featureDetector = &_featureDetector;
}
void FrameCatalogue::setDescriptorExtractor(cv::DescriptorExtractor & _descriptorExtractor) {
	descriptorExtractor = &_descriptorExtractor;
}
void FrameCatalogue::setFeatureMatcher(FeatureMatcher & _featureMatcher) {
	featureMatcher = &_featureMatcher;
}

void FrameCatalogue::setTriangulator(Triangulator & _triangulator) {
	triangulator = &_triangulator;
}

void FrameCatalogue::setStopWatch(StopWatch & _stopWatch) {
	stopWatch = &_stopWatch;
}

void FrameCatalogue::setLogger(Logger & _logger) {
	logger = &_logger;
}
