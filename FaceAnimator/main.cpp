#include <iostream>
#include <queue>
#include <ctime>
#include <string>

//OpenCV headers
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"


#include "MayaConnector.h"
#include "Landmarks.h"

//Dlib headers
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

using namespace dlib;
using namespace std;

int main(int argc, char* argv[])
{
	MayaConnector connector;
	
	try
	{
		cv::VideoCapture cap(0);
		if (!cap.isOpened())
		{
			cerr << "Unable to connect to camera" << endl;
			return 1;
		}
		cap.set(CV_CAP_PROP_FRAME_WIDTH, 800);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, 800);

		image_window win;
		cv::namedWindow("Webcam", CV_WINDOW_AUTOSIZE);

		// Setup FPS calculation
		int framecount = 0;
		std::clock_t start;
		double duration;
		start = std::clock();

		bool record = false;

		float time = 0;

		// Load face detection and pose estimation models.
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

		// Grab and process frames until the main window is closed by the user.
		while (!win.is_closed())
		{
			//handle input
			char key = cv::waitKey(1);
			if (key == 27) {
				cout << "esc key is pressed by user" << endl;
				break;
			}
			else if (key == 'p') {
				record = !record;
				if (record)
					cout << "recording ON" << endl;
				else
					cout << "recording OFF" << endl;
			}
			// Grab a frame
			cv::Mat temp;
			cap >> temp;
			flip(temp, temp, 1);
			// Turn OpenCV's Mat into something dlib can deal with.  Note that this just
			// wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
			// long as temp is valid.  Also don't do anything to temp that would cause it
			// to reallocate the memory which stores the image as that will make cimg
			// contain dangling pointers.  This basically means you shouldn't modify temp
			// while using cimg.
			cv_image<bgr_pixel> cimg(temp);

			// Detect faces 
			std::vector<rectangle> faces = detector(cimg);
			// Find the pose of each face.
			std::vector<full_object_detection> shapes;
			for (unsigned long i = 0; i < faces.size(); ++i) {
				shapes.push_back(pose_model(cimg, faces[i]));
			}
			
			Landmarks &data = Landmarks();

			for (int l = 0; l <= N_LANDMARKS; ++l) {
				if (!shapes.empty() && record) {
					//cout << shapes[0].part(l);
					int x = shapes[0].part(l).x();
					int y = shapes[0].part(l).y();
					data.landmarks[l] = cv::Point2f(x,y);
					float currTime = std::clock() / (float)CLOCKS_PER_SEC;
					data.time = currTime - time;
					time = currTime;
				}
			}
			if (!shapes.empty() && record) {
				connector.send(data);
			}
			if (record) {
				record = !record;
			}
				
			// Display it all on the screen
			win.clear_overlay();
			win.set_image(cimg);
			win.add_overlay(render_face_detections(shapes));

			// Calculate FPS
			framecount++;
			duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
			if (duration > 1.0) {
				printf("FPS: %i\n", framecount);
				start = std::clock();
				framecount = 0;
			}
		}
	}
	catch (serialization_error& e)
	{
		cout << "You need dlib's default face landmarking model file to run this example." << endl;
		cout << "You can get it from the following URL: " << endl;
		cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
		cout << endl << e.what() << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}