/*
 * FrameCatalogue.h
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#ifndef FRAMECATALOGUE_H_
#define FRAMECATALOGUE_H_

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include "Frame.h"
#include "Triangulator.h"
#include "FeatureMatcher.h"
#include "StopWatch.h"

//#include "HelperFunctions.h"
#include "Logger.h"


class FrameCatalogue {
public:
	FrameCatalogue();

	void extractFeatures(Frame & frame);
	void extractFeaturesOfAllFrames();
	void addNewFrame(const cv::Mat & leftImage, const cv::Mat & rightImage, bool doExtractFeatures);
	void addNewFrame();
	void addImagesToFrame(int frameId, const cv::Mat & leftImage, const cv::Mat & rightImage);
	Frame getFrame(const cv::Mat & leftImage, const cv::Mat & rightImage);

	void getLinkedSets(std::vector< std::vector<int> > & linkedSets);

	Frame* getCurrentFrame() const;
	Frame* getFrame(int frameId) const;
	int getNumberOfFrames() const;

	void save(const std::string & dirname, bool withImages=true);
	void load(const std::string & dirname);

	void setFeatureDetector(cv::FeatureDetector & _featureDetector);
	void setDescriptorExtractor(cv::DescriptorExtractor & _descriptorExtractor);
	void setFeatureMatcher(FeatureMatcher & _featureMatcher);
	void setTriangulator(Triangulator & _triangulator);

	void setStopWatch(StopWatch & _stopWatch);
	void setLogger(Logger & _logger);

private:
	void searchLinkedFrames(int startFrameId, std::vector<bool> &visited, std::vector<int> &set);

	std::vector<Frame*> frames;

	// Feature detection
	cv::FeatureDetector* featureDetector;
	cv::DescriptorExtractor* descriptorExtractor;
	FeatureMatcher* featureMatcher;
	Triangulator* triangulator;

	StopWatch* stopWatch;
	Logger* logger;
};

#endif /* FRAMECATALOGUE_H_ */
