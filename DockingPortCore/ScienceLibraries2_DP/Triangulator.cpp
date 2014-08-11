/*
 * Triangulator.cpp
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#include "Triangulator.h"

Triangulator::Triangulator(Camera & _camera) {
	camera = &_camera;
	minDisparity = 0.;
	maxDisparity = 256.;
	maxVerticalOffset = 1.;
}
