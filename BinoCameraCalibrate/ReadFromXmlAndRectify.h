#ifndef READ_FROM_XML_RECTIFY
#define READ_FROM_XML_RECTIFY

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "utils.h"
#include "tinyxml2.h"


using namespace cv;
using namespace tinyxml2;
using namespace std;


// Read parameters from xml file
int readParamsFromXml(const string& xml_path, const string& elem_name, vector<float>& params);

// Distortion coefficients 
int readLeftDistXml(const string& xml_path, vector<float>& l_dists);
int readRightDistXml(const string& xml_path, vector<float>& r_dists);

// Rotation matrix
int readLeftRotateXml(const string& xml_path, vector<float>& l_rotate);


#endif