#pragma once
//Standard headers
#include <iostream>
#include <queue>
#include <ctime>
#include <string>

//OpenCV headers
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

//Local headers
#include "MayaConnector.h"
#include "Landmarks.h"
#include "render_face.hpp"

//Dlib headers
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

//Namespaces
using namespace dlib;
using namespace std;

//Constants
#define FACE_DOWNSAMPLE_RATIO 4
#define SKIP_FRAMES 2

//Attributes
MayaConnector connector;

//Code substitution for GUI and face orientation (optional)
#include "FaceAnimatorGui.h"
#include "FaceAnimatorOrientation.h"