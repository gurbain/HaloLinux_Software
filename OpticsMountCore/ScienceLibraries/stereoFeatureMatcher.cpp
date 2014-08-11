#include "stereoFeatureMatcher.h"


void StereoMatcher::initialStereoMatch(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
		cv::Mat& leftDescriptors, cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::KeyPoint>& leftFeaturesPreMatch,
		vector<cv::KeyPoint>& rightFeaturesPreMatch) {

	if (leftFeatures.size() > 0)
	{

		int found;
		int i, j = 0;
		float ythresh = 5; // pixel difference allowed vertically for stereo match
		stereoFeatureMatches.resize(leftFeatures.size());
		if (rightFeatures.size() > leftFeatures.size())
			stereoFeatureMatches.resize(rightFeatures.size());

		cv::BruteForceMatcher<cv::L2<float> > matcher;
	//	FlannBasedMatcher matcher;
		vector<cv::DMatch> stereoFeatureMatchesRL;

		matcher.match(leftDescriptors, rightDescriptors, stereoFeatureMatches);
		matcher.match(rightDescriptors, leftDescriptors, stereoFeatureMatchesRL);

//printf("feature matches number before doing anything: %d\n", stereoFeatureMatches.size());

		vector<cv::DMatch> goodstereoFeatureMatches;
		goodstereoFeatureMatches.resize(stereoFeatureMatches.size());

		// Go through matches (stereoFeatureMatches) and eliminate wrong matches with too large y-displacement
		found = 0;
		int qindx=0, tindx=0;
		while (j < stereoFeatureMatches.size()) {

			tindx = stereoFeatureMatches[j].trainIdx;
			qindx = stereoFeatureMatches[j].queryIdx;

			if (abs(rightFeatures[tindx].pt.y - leftFeatures[qindx].pt.y) < ythresh) // same epiline
			{

				// acceptable match
				goodstereoFeatureMatches[found].trainIdx = tindx;
				goodstereoFeatureMatches[found].queryIdx = qindx;
				goodstereoFeatureMatches[found].distance = stereoFeatureMatches[j].distance;

				found++;
			}
			j++;
		}

		stereoFeatureMatches.resize(found);
		for (int i=0; i<stereoFeatureMatches.size(); i++)
		{
			stereoFeatureMatches[i] = goodstereoFeatureMatches[i];
		}

//printf("stereoFeatureMatches before leftright check %d \n", stereoFeatureMatches.size());

		goodstereoFeatureMatches.resize(0); // use the same temp storage variable as above
		// do cross-matching left-right and right-left
		int iterMax = stereoFeatureMatches.size();
		if (stereoFeatureMatchesRL.size() < iterMax)
			iterMax = stereoFeatureMatchesRL.size();
		for (int i=0; i<iterMax; i++)
		{
			cv::DMatch matchForward;
			matchForward = stereoFeatureMatches[i];
			cv::DMatch matchBackwards;
			matchBackwards = stereoFeatureMatchesRL[matchForward.trainIdx];

			if (matchBackwards.trainIdx == matchForward.queryIdx)
				goodstereoFeatureMatches.push_back(matchForward);

		}

		stereoFeatureMatches.resize(goodstereoFeatureMatches.size());

		for (int i=0; i<goodstereoFeatureMatches.size(); i++)
		{
			stereoFeatureMatches[i] = goodstereoFeatureMatches[i];
		}


//printf("stereoFeatureMatches %d \n", stereoFeatureMatches.size());


		/*
		if (stereoFeatureMatches.size() > 0)
		{

			double max_dist = 0;
			double min_dist = 100;

			//-- Quick calculation of max and min distances between keypoints
			for (int i = 0; i < stereoFeatureMatches.size(); i++) {
				double dist = stereoFeatureMatches[i].distance;
				if (dist < min_dist)
					min_dist = dist;
				if (dist > max_dist)
					max_dist = dist;
			}

			//	  printf("-- Max dist : %f \n", max_dist );
			//	  printf("-- Min dist : %f \n", min_dist );

			//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
			//-- PS.- radiusMatch can also be used here.
			vector<DMatch> goodStereoFeatureMatches;

			for (int i = 0; i < stereoFeatureMatches.size(); i++) {
				if (stereoFeatureMatches[i].distance < (min_dist + 0.7*(max_dist-min_dist)) ) {
					goodStereoFeatureMatches.push_back(stereoFeatureMatches[i]);
				}
			}

			stereoFeatureMatches = goodStereoFeatureMatches;
		}
		*/

		// now copy the good features into leftFeaturesPreMatch and rightFeaturesPreMatch vectors
		leftFeaturesPreMatch.resize( stereoFeatureMatches.size() );
		rightFeaturesPreMatch.resize( stereoFeatureMatches.size() );
		for (int i=0; i<stereoFeatureMatches.size(); i++)
		{
			leftFeaturesPreMatch[i] = leftFeatures[ (stereoFeatureMatches[i]).queryIdx ];

			rightFeaturesPreMatch[i] = rightFeatures[ (stereoFeatureMatches[i]).trainIdx ];
		}

	}

}



void StereoMatcher::getStereoMatches(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
		vector<cv::Point3f>& stereoPreMatched3Dcoord, cv::Mat& P1, vector<cv::KeyPoint>& leftFeaturesRANSAC,
		vector<cv::KeyPoint>& rightFeaturesRANSAC, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC)
{

	cv::Point3f leftProjection; // 3D point projected on rectified left image plane
	leftProjection.x = 0;
	leftProjection.y = 0;
	leftProjection.z = 0; // Homogeneous coordinates

	double distance = 0;

	stereoFeatureMatchesRANSAC.erase (stereoFeatureMatchesRANSAC.begin(), stereoFeatureMatchesRANSAC.end());
	leftFeaturesRANSAC.erase (leftFeaturesRANSAC.begin(), leftFeaturesRANSAC.end());
	rightFeaturesRANSAC.erase (rightFeaturesRANSAC.begin(), rightFeaturesRANSAC.end());


	for (int k=0; k<leftFeatures.size(); k++)
	{

	///	leftProjection = P1*stereoPreMatched3Dcoord;
		// custom Matrix multiplication
		leftProjection.x += P1.at<double> (0, 0) * stereoPreMatched3Dcoord[k].x;
		leftProjection.x += P1.at<double> (0, 1) * stereoPreMatched3Dcoord[k].y;
		leftProjection.x += P1.at<double> (0, 2) * stereoPreMatched3Dcoord[k].z;
		leftProjection.x += P1.at<double> (0, 3) * 1;

		leftProjection.y += P1.at<double> (1, 0) * stereoPreMatched3Dcoord[k].x;
		leftProjection.y += P1.at<double> (1, 1) * stereoPreMatched3Dcoord[k].y;
		leftProjection.y += P1.at<double> (1, 2) * stereoPreMatched3Dcoord[k].z;
		leftProjection.y += P1.at<double> (1, 3) * 1;

		leftProjection.z += P1.at<double> (2, 0) * stereoPreMatched3Dcoord[k].x;
		leftProjection.z += P1.at<double> (2, 1) * stereoPreMatched3Dcoord[k].y;
		leftProjection.z += P1.at<double> (2, 2) * stereoPreMatched3Dcoord[k].z;
		leftProjection.z += P1.at<double> (2, 3) * 1;

		leftProjection.x = leftProjection.x / leftProjection.z;
		leftProjection.y = leftProjection.y / leftProjection.z;

		printf("stereoPreMatched3Dcoord = ( %f | %f | %f ) \n", stereoPreMatched3Dcoord[k].x, stereoPreMatched3Dcoord[k].y, stereoPreMatched3Dcoord[k].z);

		printf("leftProjection x and y = ( %f | %f )   leftFeatures[k].pt. x and y ( %f | %f ) \n", leftProjection.x , leftProjection.y, leftFeatures[k].pt.x, leftFeatures[k].pt.y);
		printf("x'-x and y'-y = ( %f | %f ) \n", leftProjection.x - leftFeatures[k].pt.x, leftProjection.y - leftFeatures[k].pt.y);


	/// see if euclidian distance between projected point leftProjection and measured point leftFeature.pt is larger/smaller than threshold
		// Euclidian distance
		distance = (leftProjection.x - leftFeatures[k].pt.x) * (leftProjection.x - leftFeatures[k].pt.x) + (leftProjection.y - leftFeatures[k].pt.y) * (leftProjection.y - leftFeatures[k].pt.y);
		distance = sqrt(distance);

		cout << "distance = " << distance << endl;

		if (distance < this->SDthreshold)
		{
			/// good stereo match
			stereoFeatureMatchesRANSAC.push_back( stereoFeatureMatches[k] );

			leftFeaturesRANSAC.push_back( leftFeatures[k] );

			rightFeaturesRANSAC.push_back( rightFeatures[k] );

		}

	}

}


void StereoMatcher::getStereoInliersRANSAC_Homography(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
		vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors,
		cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC)
{

	this->RANSACmatching_Homography(leftFeatures, rightFeatures, leftFeaturesRANSAC, rightFeaturesRANSAC,
			leftDescriptors, rightDescriptors, stereoFeatureMatches, stereoFeatureMatchesRANSAC, this->SDthreshold,
			this->maxiterRANSAC, this->numberOfSufficientMatches);

}

void StereoMatcher::getStereoInliersRANSAC_MultiHomography(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
		vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors, cv::Mat& rightDescriptors,
		vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC)
{

	this->RANSACmatching_MultiHomography(leftFeatures, rightFeatures, leftFeaturesRANSAC, rightFeaturesRANSAC, leftDescriptors,
			rightDescriptors, stereoFeatureMatches, stereoFeatureMatchesRANSAC, this->SDthreshold, this->numOfMinMatchesForGoodHom,
			this->maxiterRANSAC, this->numberOfSufficientMatches);

}

void StereoMatcher::getStereoInliersRANSAC_Fundamental(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
		vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors,
		cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC)
{

	this->RANSACmatching_Fundamental(leftFeatures, rightFeatures, leftFeaturesRANSAC, rightFeaturesRANSAC, leftDescriptors,
			rightDescriptors, stereoFeatureMatches, stereoFeatureMatchesRANSAC, this->SDthreshold, this->maxiterRANSAC,
			this->numberOfSufficientMatches);

}


void RANSACmatcherStereo::randSelectDescriptorMatches(vector<cv::KeyPoint>& leftFeatures,
		vector<cv::KeyPoint>& rightFeatures, vector<cv::KeyPoint>& KeypointsSelect1,
		vector<cv::KeyPoint>& KeypointsSelect2, vector<cv::DMatch>& allMatches, vector<cv::DMatch>& matchesSel,
		int numRandomSelections, int numAvMatches) // numRandomSelections should be 4
{
	vector<int> randIndexVec;
	randIndexVec.resize(allMatches.size());

	vector<cv::DMatch> allMatchesDummy = allMatches;

	for (int j = 0; j < randIndexVec.size(); j++) {
		randIndexVec[j] = j;
	}

	int randIndex;
	int numAvMatchesDummy = numAvMatches - 1; // -1 because 0...(size()-1) elements

	for (int i = 0; i < numRandomSelections; i++) {
		numAvMatchesDummy = numAvMatchesDummy - 1;
/*
		// own random number generator
		double randmax = double(RAND_MAX);

		randIndex = int(numAvMatchesDummy * (rand() / randmax));
		//		printf("randindex: %d\n", randIndex);
*/

		cv::randShuffle(randIndexVec, 20., 0);
		randIndex = randIndexVec[0];

		(matchesSel)[i] = allMatchesDummy[randIndex];

		KeypointsSelect1[i] = leftFeatures[allMatchesDummy[randIndex].queryIdx];

		KeypointsSelect2[i]
				= rightFeatures[allMatchesDummy[randIndex].trainIdx];

		allMatchesDummy.erase(allMatchesDummy.begin() + randIndex); // erase that chosen element, so that it won't be chosen again

	}

	(matchesSel).resize(numRandomSelections);
}

double RANSACmatcherStereo::calcSquaredDifference_Homography(const cv::KeyPoint& oneLeftFeature,
		const cv::KeyPoint& oneRightFeature, cv::Mat& Homography) // Sum of Squared Differences
{
	cv::Mat sdMat;
	cv::Mat diff, diff2;
	diff2.create(2, 1, CV_64FC1);
	cv::Mat HxKpt1;
	const cv::Mat zeroMatrix = cv::Mat(1, 3, CV_64FC1, NULL);
	double sdFloat;
	int sdInt;

	cv::Point2f oneLeftFeaturept = oneLeftFeature.pt;
	cv::Point2f oneRightFeaturept = oneRightFeature.pt;
	vector<float> keypointCoordinates1;
	keypointCoordinates1.resize(3);
	keypointCoordinates1[0] = oneLeftFeaturept.x;
	keypointCoordinates1[1] = oneLeftFeaturept.y;
	keypointCoordinates1[2] = 1.0;
	vector<float> keypointCoordinates2;
	keypointCoordinates2.resize(3);
	keypointCoordinates2[0] = oneRightFeaturept.x;
	keypointCoordinates2[1] = oneRightFeaturept.y;
	keypointCoordinates2[2] = 1.0;

	cv::Mat keypointCoordinatesMat1 = cv::Mat(keypointCoordinates1);
	cv::Mat keypointCoordinatesMat2 = cv::Mat(keypointCoordinates2);
	keypointCoordinatesMat1.convertTo(keypointCoordinatesMat1,
			Homography.type(), 1.0, 0.0); //mat.convertTo(Mat& m, int rtype, double alpha=1, double beta=0)
	keypointCoordinatesMat2.convertTo(keypointCoordinatesMat2,
			Homography.type(), 1.0, 0.0);

	gemm(Homography, keypointCoordinatesMat1, 1.0, zeroMatrix, 0.0, HxKpt1, 0); // this should be correct

	subtract(HxKpt1, keypointCoordinatesMat2, diff); // this should be correct

	diff2.at<double> (0, 0) = diff.at<double> (0, 0);
	diff2.at<double> (1, 0) = diff.at<double> (1, 0);

	mulTransposed(diff2, sdMat, true, cv::Mat(), 1, -1); // Calculates the product of a matrix and its transposition. Diff is a 3D vector

	sdFloat = sqrt(sdMat.at<double> (0, 0));

	sdInt = int(sdFloat);

	return sdFloat; // this is the squared difference
}


double RANSACmatcherStereo::calcSquaredDifference_Fundamental(const cv::KeyPoint& oneLeftFeature,
		const cv::KeyPoint& oneRightFeature, cv::Mat& Fundamental) // Sum of Squared Differences
{

//	err = p_rightT * F * p_left;

	double error;
	cv::Mat tempMat(3, 3, CV_64F); // tempMat = E*x_right
	tempMat.at<double> (0, 0) = 0;
	tempMat.at<double> (1, 0) = 0;
	tempMat.at<double> (2, 0) = 0;

	error = 0.0;
	tempMat.at<double> (0, 0) = 0.0;
	tempMat.at<double> (1, 0) = 0.0;
	tempMat.at<double> (2, 0) = 0.0;

	// custom Matrix multiplication
	tempMat.at<double> (0, 0) = Fundamental.at<double> (0, 0) * oneLeftFeature.pt.x;
	tempMat.at<double> (0, 0) += Fundamental.at<double> (0, 1) * oneLeftFeature.pt.y;
	tempMat.at<double> (0, 0) += Fundamental.at<double> (0, 2) * 1;


	tempMat.at<double> (1, 0) = Fundamental.at<double> (1, 0) * oneLeftFeature.pt.x;
	tempMat.at<double> (1, 0) += Fundamental.at<double> (1, 1) * oneLeftFeature.pt.y;
	tempMat.at<double> (1, 0) += Fundamental.at<double> (1, 2) * 1;


	tempMat.at<double> (2, 0) = Fundamental.at<double> (2, 0) * oneLeftFeature.pt.x;
	tempMat.at<double> (2, 0) += Fundamental.at<double> (2, 1) * oneLeftFeature.pt.y;
	tempMat.at<double> (2, 0) += Fundamental.at<double> (2, 2) * 1;


	error = oneRightFeature.pt.x * tempMat.at<double> (0, 0);
	error += oneRightFeature.pt.y * tempMat.at<double> (1, 0);
	error += 1 * tempMat.at<double> (2, 0);

	error = abs(error);
	error = sqrt(error); // just to reduce the number of zeros after the comma

//	cout << "error = " << error << endl;

	return error; // this is the error

}


void RANSACmatcherStereo::RANSACmatching_Homography(vector<cv::KeyPoint>& leftFeatures,
		vector<cv::KeyPoint>& rightFeatures, vector<cv::KeyPoint>& leftFeaturesRANSAC,
		vector<cv::KeyPoint>& rightFeaturesRANSAC,  cv::Mat& leftDescriptors, cv::Mat& rightDescriptors,
		vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC,
		double SDthreshold, int maxiterRANSAC, int numberOfSufficientMatches) {

	int numRandomSelections = 4; // since model equation for RANSAC needs four matches

	int numberInliers = 0;
	int numberInliersNew = 0;

	cv::Mat H; // homography matrix
	int randSelection[4];

	vector<cv::KeyPoint> KeypointsSelected1;
	KeypointsSelected1.resize(numRandomSelections);
	vector<cv::KeyPoint> KeypointsSelected2;
	KeypointsSelected2.resize(numRandomSelections);

	vector<cv::DMatch> matchesSelection;
	matchesSelection.resize(numRandomSelections);

	int numberOfAvailableMatches;
	numberOfAvailableMatches = stereoFeatureMatches.size();

	int maxNeededInlierNumber = numberOfAvailableMatches;

	vector<cv::Point2f> KeypointsCoord1, KeypointsCoord2;
	KeypointsCoord1.resize(numRandomSelections);
	KeypointsCoord2.resize(numRandomSelections);

	vector<unsigned char> zeroMask;
	zeroMask.assign(4, 0);

	int keypointIdx1;
	int keypointIdx2;

	vector<int> matchesRANSACindex;
	matchesRANSACindex.resize(numberOfAvailableMatches + 1);

	vector<int> bestMatchesRANSACindex;

	int loop = 0;
//	while (numberInliers < (numberOfAvailableMatches*0.6) && loop < this->maxiterRANSAC && numberInliers < this->numberOfSufficientMatches) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
//	while (numberInliers < (numberOfAvailableMatches*0.6) && loop < maxiterRANSAC) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
	while (loop < maxiterRANSAC) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
	{
		loop = loop++;
		//printf("\n newloop\n");
		////// randomly select 4 descriptor matches and return the corresponding 4 element vector<DMatch> matchesSelection + the corresponding 2x4 keypoints
		randSelectDescriptorMatches(leftFeatures, rightFeatures,
				KeypointsSelected1, KeypointsSelected2, stereoFeatureMatches,
				matchesSelection, numRandomSelections, numberOfAvailableMatches);
		/////////////////////////////////

		////// Homography for 4 randomly chosen points

		for (int k = 0; k < 4; k++) {
			KeypointsCoord1[k] = KeypointsSelected1[k].pt;
			KeypointsCoord2[k] = KeypointsSelected2[k].pt;
		}

		cv::Mat KeypointsCoord1Mat = cv::Mat(KeypointsCoord1);
		cv::Mat KeypointsCoord2Mat = cv::Mat(KeypointsCoord2);

		// compute Homography matrix H for those 4 matches
		H = findHomography(KeypointsCoord1Mat, KeypointsCoord2Mat, zeroMask, 0,
				0); // INSTEAD: use findFundamentalMat(const Mat& points1, const Mat& points2, int method=FM_RANSAC, double param1=3., double param2=0.99)

		//		printf("H = \n   %f \t %f \t %f \n   %f \t %f \t %f \n   %f \t %f \t %f \n\n", H.at<double>(0,0), H.at<double>(0,1), H.at<double>(0,2), H.at<double>(1,0), H.at<double>(1,1), H.at<double>(1,2), H.at<double>(2,0), H.at<double>(2,1), H.at<double>(2,2));

		/////////////////////////////////

		////// find inliers that fit to that homography
		numberInliersNew = 0;
		double SD = 0;
		int iter = 0;
		// compute number of inliers for computed Homography matrix H
		for (int iter = 0; iter < numberOfAvailableMatches; iter++) {
			keypointIdx1 = stereoFeatureMatches[iter].queryIdx;
			keypointIdx2 = stereoFeatureMatches[iter].trainIdx;

			SD = calcSquaredDifference_Homography(leftFeatures[keypointIdx1],
					rightFeatures[keypointIdx2], H); // should be correct

			if (SD < SDthreshold) {

				numberInliersNew++;
				matchesRANSACindex.resize(numberInliersNew);
				matchesRANSACindex[numberInliersNew - 1] = iter;

			}

		}

		/////////////////////////////////

		////// check if this consensus set is better than the previous one
		// and if it is better, store it (resp. the indices)
		if (numberInliersNew > numberInliers) {
			numberInliers = numberInliersNew;
			bestMatchesRANSACindex.resize(matchesRANSACindex.size());

			for (int iter = 0; iter < matchesRANSACindex.size(); iter++) {
				bestMatchesRANSACindex[iter] = matchesRANSACindex[iter];
			}

		}
		/////////////////////////////////
	}

//	printf( "\nnumber of available matches: %d \t number of RANSAC iteration: %d \n", numberOfAvailableMatches, loop);

	////// after having found the best consensus set, put together vector<DMatch> stereoFeatureMatchesRANSAC with the valid matches found by RANSAC

	(stereoFeatureMatchesRANSAC).resize(bestMatchesRANSACindex.size());
	leftFeaturesRANSAC.resize(bestMatchesRANSACindex.size());
	rightFeaturesRANSAC.resize(bestMatchesRANSACindex.size());

	for (int iter = 0; iter < bestMatchesRANSACindex.size(); iter++) {
		(stereoFeatureMatchesRANSAC)[iter]
				= stereoFeatureMatches[bestMatchesRANSACindex[iter]];

		leftFeaturesRANSAC[iter]
				= leftFeatures[(stereoFeatureMatchesRANSAC)[iter].queryIdx]; // should be correct

		rightFeaturesRANSAC[iter]
				= rightFeatures[(stereoFeatureMatchesRANSAC)[iter].trainIdx]; // should be correct
		//		printf("queryIdx = %d\t trainIdx = %d\n", (stereoFeatureMatchesRANSAC)[iter].queryIdx, stereoFeatureMatchesRANSAC[iter].trainIdx);

	}

	// "return value" is matchesRANSAC and the corresponding subset of matched RANSACfeatures

	if ((stereoFeatureMatchesRANSAC).size() > 0) {
	/*	printf("initial matches: %d\tRANSAC matches: %d\t difference: %d\n",
				stereoFeatureMatches.size(),
				(stereoFeatureMatchesRANSAC).size(),
				(stereoFeatureMatches.size()
						- stereoFeatureMatchesRANSAC.size()));
						*/
	}

}

void RANSACmatcherStereo::RANSACmatching_MultiHomography(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
		vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors,
		cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC,
		double SDthreshold, int numOfMinMatchesForGoodHom, int maxiterRANSAC, int numberOfSufficientMatches)
{
	int numRandomSelections = 4; // since model equation for RANSAC needs four matches

	int numberInliers = 0;
	int numberInliersNew = 0;

	cv::Mat H; // homography matrix
	int randSelection[4];

	vector<cv::KeyPoint> KeypointsSelected1;
	KeypointsSelected1.resize(numRandomSelections);
	vector<cv::KeyPoint> KeypointsSelected2;
	KeypointsSelected2.resize(numRandomSelections);

	vector<cv::DMatch> matchesSelection;
	matchesSelection.resize(numRandomSelections);

	int numberOfAvailableMatches;
	numberOfAvailableMatches = stereoFeatureMatches.size();

	int maxNeededInlierNumber = numberOfAvailableMatches;

	vector<cv::Point2f> KeypointsCoord1, KeypointsCoord2;
	KeypointsCoord1.resize(numRandomSelections);
	KeypointsCoord2.resize(numRandomSelections);

	vector<unsigned char> zeroMask;
	zeroMask.assign(4, 0);

	int keypointIdx1;
	int keypointIdx2;

	vector<int> matchesRANSACindex;
	matchesRANSACindex.resize(numberOfAvailableMatches + 1);

	vector<vector<int> > bestMatchesRANSACindex;

	bool featureIsAlreadyInASet = false;
	int numberOfTotalInliers = 0;
	int loop = 0;
//	while (numberInliers < (numberOfAvailableMatches*0.6) && loop < this->maxiterRANSAC && numberInliers < this->numberOfSufficientMatches) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
//	while (numberInliers < (numberOfAvailableMatches*0.6) && loop < maxiterRANSAC) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
	while (loop < maxiterRANSAC) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
	{
		loop = loop++;
		//printf("\n newloop\n");
		////// randomly select 4 descriptor matches and return the corresponding 4 element vector<DMatch> matchesSelection + the corresponding 2x4 keypoints
		randSelectDescriptorMatches(leftFeatures, rightFeatures,
				KeypointsSelected1, KeypointsSelected2, stereoFeatureMatches,
				matchesSelection, numRandomSelections, numberOfAvailableMatches);
		/////////////////////////////////

		////// Homography for 4 randomly chosen points

		for (int k = 0; k < 4; k++) {
			KeypointsCoord1[k] = KeypointsSelected1[k].pt;
			KeypointsCoord2[k] = KeypointsSelected2[k].pt;
		}

		cv::Mat KeypointsCoord1Mat = cv::Mat(KeypointsCoord1);
		cv::Mat KeypointsCoord2Mat = cv::Mat(KeypointsCoord2);

		// compute Homography matrix H for those 4 matches
		H = findHomography(KeypointsCoord1Mat, KeypointsCoord2Mat, zeroMask, 0,
				0); // INSTEAD: use findFundamentalMat(const Mat& points1, const Mat& points2, int method=FM_RANSAC, double param1=3., double param2=0.99)

		//		printf("H = \n   %f \t %f \t %f \n   %f \t %f \t %f \n   %f \t %f \t %f \n\n", H.at<double>(0,0), H.at<double>(0,1), H.at<double>(0,2), H.at<double>(1,0), H.at<double>(1,1), H.at<double>(1,2), H.at<double>(2,0), H.at<double>(2,1), H.at<double>(2,2));

		/////////////////////////////////

		////// find inliers that fit to that homography
		numberInliersNew = 0;
		double SD = 0;
		int iter = 0;
		// compute number of inliers for computed Homography matrix H
		for (int iter = 0; iter < numberOfAvailableMatches; iter++)
		{
			// first check if this feature is already part of a consensus set
			featureIsAlreadyInASet = false;
			if (bestMatchesRANSACindex.size() > 0)
			{
				for (int l=0; l < bestMatchesRANSACindex.size(); l++)
				{
					for (int q=0; q < bestMatchesRANSACindex[l].size(); q++)
					{
						if (bestMatchesRANSACindex[l][q] == iter)
						{
							featureIsAlreadyInASet = true;
							break;
						}
					}
					if (featureIsAlreadyInASet)
						break;
				}
			}

			if (!featureIsAlreadyInASet)
			{

				keypointIdx1 = stereoFeatureMatches[iter].queryIdx;
				keypointIdx2 = stereoFeatureMatches[iter].trainIdx;

				SD = calcSquaredDifference_Homography(leftFeatures[keypointIdx1],
						rightFeatures[keypointIdx2], H); // should be correct

				if (SD < SDthreshold)
				{
					numberOfTotalInliers++;
					numberInliersNew++;
					matchesRANSACindex.resize(numberInliersNew);
					matchesRANSACindex[numberInliersNew - 1] = iter;
				}
			}

		}

		/////////////////////////////////

		if (bestMatchesRANSACindex.size() > 0)
		{
			////// check if this consensus set is good enough
			// and if it is better, store it (resp. the indices)
			if (numberInliersNew > numOfMinMatchesForGoodHom)
			{

				bestMatchesRANSACindex.resize(bestMatchesRANSACindex.size() + 1);
				bestMatchesRANSACindex[bestMatchesRANSACindex.size() - 1].resize(matchesRANSACindex.size());

				for (int iter = 0; iter < matchesRANSACindex.size(); iter++)
				{
					bestMatchesRANSACindex[bestMatchesRANSACindex.size() - 1][iter] = matchesRANSACindex[iter];
				}

			}
		}
		////////////////////////////////
		else
		{
			////// check if this consensus set is better than the previous one
			// and if it is better, store it (resp. the indices)
			if (numberInliersNew > numberInliers)
			{
				numberInliers = numberInliersNew;
				bestMatchesRANSACindex.resize(bestMatchesRANSACindex.size() + 1);
				bestMatchesRANSACindex[bestMatchesRANSACindex.size() - 1].resize(matchesRANSACindex.size());

				for (int iter = 0; iter < matchesRANSACindex.size(); iter++)
				{
					bestMatchesRANSACindex[bestMatchesRANSACindex.size() - 1][iter] = matchesRANSACindex[iter];
				}

			}
			/////////////////////////////////
		}


	}

//	printf( "\nnumber of available matches: %d \t number of RANSAC iteration: %d \n", numberOfAvailableMatches, loop);

	////// after having found the best consensus set, put together vector<DMatch> stereoFeatureMatchesRANSAC with the valid matches found by RANSAC

	(stereoFeatureMatchesRANSAC).resize(numberOfTotalInliers);
	leftFeaturesRANSAC.resize(numberOfTotalInliers);
	rightFeaturesRANSAC.resize(numberOfTotalInliers);

	for (int iter1 = 0; iter1 < bestMatchesRANSACindex.size(); iter1++)
	{
		for (int iter2 = 0; iter2 < bestMatchesRANSACindex[iter1].size(); iter2++)
		{
			(stereoFeatureMatchesRANSAC)[iter2]
					= stereoFeatureMatches[bestMatchesRANSACindex[iter1][iter2]];

			leftFeaturesRANSAC[iter2]
					= leftFeatures[(stereoFeatureMatchesRANSAC)[iter2].queryIdx]; // should be correct

			rightFeaturesRANSAC[iter2]
					= rightFeatures[(stereoFeatureMatchesRANSAC)[iter2].trainIdx]; // should be correct
			//		printf("queryIdx = %d\t trainIdx = %d\n", (stereoFeatureMatchesRANSAC)[iter].queryIdx, stereoFeatureMatchesRANSAC[iter].trainIdx);
		}
	}

	// "return value" is matchesRANSAC and the corresponding subset of matched RANSACfeatures

	if ((stereoFeatureMatchesRANSAC).size() > 0) {
	/*	printf("initial matches: %d\tRANSAC matches: %d\t difference: %d\n",
				stereoFeatureMatches.size(),
				(stereoFeatureMatchesRANSAC).size(),
				(stereoFeatureMatches.size()
						- stereoFeatureMatchesRANSAC.size()));
						*/
	}

}


void RANSACmatcherStereo::RANSACmatching_Fundamental(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
		vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors,
		cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches,
		vector<cv::DMatch>& stereoFeatureMatchesRANSAC, double SDthreshold, int maxiterRANSAC, int numberOfSufficientMatches) {

	int numRandomSelections = 8; // since model equation for RANSAC needs four matches

	int numberInliers = 0;
	int numberInliersNew = 0;

	cv::Mat F; // Fundamental matrix
	int randSelection[numRandomSelections];

	vector<cv::KeyPoint> KeypointsSelected1;
	KeypointsSelected1.resize(numRandomSelections);
	vector<cv::KeyPoint> KeypointsSelected2;
	KeypointsSelected2.resize(numRandomSelections);

	vector<cv::DMatch> matchesSelection;
	matchesSelection.resize(numRandomSelections);

	int numberOfAvailableMatches;
	numberOfAvailableMatches = stereoFeatureMatches.size();

	int maxNeededInlierNumber = numberOfAvailableMatches;

	vector<cv::Point2f> KeypointsCoord1, KeypointsCoord2;
	KeypointsCoord1.resize(numRandomSelections);
	KeypointsCoord2.resize(numRandomSelections);

	vector<unsigned char> zeroMask;
	zeroMask.assign(4, 0);

	int keypointIdx1;
	int keypointIdx2;

	vector<int> matchesRANSACindex;
	matchesRANSACindex.resize(numberOfAvailableMatches + 1);

	vector<int> bestMatchesRANSACindex;

	numberOfSufficientMatches = int( numberOfAvailableMatches*0.8 );
	if (numberOfAvailableMatches < 20)
	{
		numberOfSufficientMatches = 15;
	}

	int loop = 0;
	while (numberInliers < (numberOfAvailableMatches*0.95) && loop < maxiterRANSAC) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
//	while (numberInliers < numberOfSufficientMatches && loop < maxiterRANSAC) //numberOfAvailableMatches-3 because we need 4 matches to compute homography. In case of 4 matches we just need to perform the loop ONCE (and so on), since there are not more combinations available
//	while (loop < maxiterRANSAC)
	{
		loop = loop++;
		//printf("\n newloop\n");
		////// randomly select 4 descriptor matches and return the corresponding 4 element vector<DMatch> matchesSelection + the corresponding 2x4 keypoints
		randSelectDescriptorMatches(leftFeatures, rightFeatures,
				KeypointsSelected1, KeypointsSelected2, stereoFeatureMatches,
				matchesSelection, numRandomSelections, numberOfAvailableMatches);
		/////////////////////////////////

		////// Fundamental Matrix for numRandomSelections randomly chosen points

		for (int k = 0; k < numRandomSelections; k++) {
			KeypointsCoord1[k] = KeypointsSelected1[k].pt;
			KeypointsCoord2[k] = KeypointsSelected2[k].pt;
		}

		cv::Mat KeypointsCoord1Mat = cv::Mat(KeypointsCoord1);
		cv::Mat KeypointsCoord2Mat = cv::Mat(KeypointsCoord2);

		// compute Fundamental matrix F for those numRandomSelections matches
        F = findFundamentalMat(KeypointsCoord1Mat, KeypointsCoord2Mat, CV_FM_8POINT, 1, 1);

  //      printf("Fundamental Mat:\n");
  //      printf("%f %f %f \n%f %f %f\n%f %f %f \n", F.at<double>(0, 0), F.at<double>(0, 1), F.at<double>(0, 2),  F.at<double>(1, 0),  F.at<double>(1, 1),  F.at<double>(1, 2),  F.at<double>(2, 0),  F.at<double>(2, 1),  F.at<double>(2, 2) );

		/////////////////////////////////

		////// find inliers that fit to that Fundamental
		numberInliersNew = 0;
		double SD = 0;
		int iter = 0;
		// compute number of inliers for computed Fundamental matrix F
		int sumOfFund = F.at<double>(0, 0) + F.at<double>(0, 1) + F.at<double>(0, 2);
		sumOfFund += F.at<double>(1, 0) + F.at<double>(1, 1) + F.at<double>(1, 2);
		sumOfFund += F.at<double>(2, 0) + F.at<double>(2, 1) + F.at<double>(2, 2);

		if (sumOfFund > 0) // make sure that F is valid
		{
			for (int iter = 0; iter < numberOfAvailableMatches; iter++)
			{
				keypointIdx1 = stereoFeatureMatches[iter].queryIdx;
				keypointIdx2 = stereoFeatureMatches[iter].trainIdx;

				SD = calcSquaredDifference_Fundamental(leftFeatures[keypointIdx1], rightFeatures[keypointIdx2], F);

		//		printf("SD: %f", SD);

				if (SD < SDthreshold) {

	//				printf("GOOD SD: %f\n", SD);
			//	    printf("Fundamental Mat:\n");
			//		printf("%f %f %f \n%f %f %f\n%f %f %f \n", F.at<double>(0, 0), F.at<double>(0, 1), F.at<double>(0, 2),  F.at<double>(1, 0),  F.at<double>(1, 1),  F.at<double>(1, 2),  F.at<double>(2, 0),  F.at<double>(2, 1),  F.at<double>(2, 2) );


					numberInliersNew++;
					matchesRANSACindex.resize(numberInliersNew);
					matchesRANSACindex[numberInliersNew - 1] = iter;

				}
	/*			else
				{
					printf("BAD SD: %f\n", SD);

				}
	*/
			}
		}

		/////////////////////////////////

		////// check if this consensus set is better than the previous one
		// and if it is better, store it (resp. the indices)
		if (numberInliersNew > numberInliers) {
			numberInliers = numberInliersNew;
			bestMatchesRANSACindex.resize(matchesRANSACindex.size());

			for (int iter = 0; iter < matchesRANSACindex.size(); iter++) {
				bestMatchesRANSACindex[iter] = matchesRANSACindex[iter];
			}

		}
		/////////////////////////////////
	}

//	printf( "number of available matches: %d \t number of RANSAC iteration: %d \n", numberOfAvailableMatches, loop);

	////// after having found the best consensus set, put together vector<DMatch> stereoFeatureMatchesRANSAC with the valid matches found by RANSAC

	(stereoFeatureMatchesRANSAC).resize(bestMatchesRANSACindex.size());
	leftFeaturesRANSAC.resize(bestMatchesRANSACindex.size());
	rightFeaturesRANSAC.resize(bestMatchesRANSACindex.size());

	for (int iter = 0; iter < bestMatchesRANSACindex.size(); iter++) {
		(stereoFeatureMatchesRANSAC)[iter]
				= stereoFeatureMatches[bestMatchesRANSACindex[iter]];

		leftFeaturesRANSAC[iter]
				= leftFeatures[(stereoFeatureMatchesRANSAC)[iter].queryIdx]; // should be correct

		rightFeaturesRANSAC[iter]
				= rightFeatures[(stereoFeatureMatchesRANSAC)[iter].trainIdx]; // should be correct
		//		printf("queryIdx = %d\t trainIdx = %d\n", (stereoFeatureMatchesRANSAC)[iter].queryIdx, stereoFeatureMatchesRANSAC[iter].trainIdx);

	}

	// "return value" is matchesRANSAC and the corresponding subset of matched RANSACfeatures


}
