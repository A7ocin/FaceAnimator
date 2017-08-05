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

std::vector<cv::Point3d> get_3d_model_points()
{
	std::vector<cv::Point3d> modelPoints;

	modelPoints.push_back(cv::Point3d(0.0f, 0.0f, 0.0f)); //The first must be (0,0,0) while using POSIT
	modelPoints.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));
	modelPoints.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));
	modelPoints.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));
	modelPoints.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));
	modelPoints.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));

	return modelPoints;

}

std::vector<cv::Point2d> get_2d_image_points(Landmarks data)
{
	std::vector<cv::Point2d> imagePoints;

	imagePoints.push_back(data.landmarks[30]);    // Nose tip
	imagePoints.push_back(data.landmarks[8]);    // Chin
	imagePoints.push_back(data.landmarks[36]);     // Left eye left corner
	imagePoints.push_back(data.landmarks[45]);    // Right eye right corner
	imagePoints.push_back(data.landmarks[48]);    // Left Mouth corner
	imagePoints.push_back(data.landmarks[54]);    // Right mouth corner

	return imagePoints;

}

class win : public drawable_window
{
	/*
	Here I am going to define our window.  In general, you can define as
	many window types as you like and make as many instances of them as you want.
	In this example I am only making one though.
	*/
public:
	win(
	) : // All widgets take their parent window as an argument to their constructor.
		c(*this),
		b1(*this),
		b2(*this),
		mbar(*this),
		img()
	{
		img.set_title("Face Animator landmark detector");

		b1.set_pos(10, 60);
		b1.set_name("Send current frame");

		b2.set_pos(10, 100);
		b2.set_name("Record");

		// let's put the label 5 pixels below the button
		c.set_pos(b2.left(), b2.bottom() + 5);
		c.set_text("Press \"Send current frame\" to map the current face landmarks to the 3D Maya model.\nPress \"Record\" to keep sending frames to Maya (can be slow)");

		b1.set_click_handler(*this, &win::sendToMaya);
		b2.set_click_handler(*this, &win::startRecording);

		// Let's also make a simple menu bar.  
		// First we say how many menus we want in our menu bar.  In this example we only want 1.
		mbar.set_number_of_menus(1);
		// Now we set the name of our menu.  The 'M' means that the M in Menu will be underlined
		// and the user will be able to select it by hitting alt+M
		mbar.set_menu_name(0, "Menu", 'M');

		// Now we add some items to the menu.  Note that items in a menu are listed in the
		// order in which they were added.

		// First let's make a menu item that does the same thing as our button does when it is clicked.
		// Again, the 'C' means the C in Click is underlined in the menu. 
		mbar.menu(0).add_menu_item(menu_item_text("Send current frame", *this, &win::sendToMaya, 'C'));
		mbar.menu(0).add_menu_item(menu_item_text("Record", *this, &win::startRecording, 'C'));
		// let's add a separator (i.e. a horizontal separating line) to the menu
		mbar.menu(0).add_menu_item(menu_item_separator());
		// Now let's make a menu item that calls show_about when the user selects it.  
		mbar.menu(0).add_menu_item(menu_item_text("About", *this, &win::show_about, 'A'));


		// set the size of this window
		set_size(450, 250);

		counter = 0;

		set_title("Face Animator controller");
		show();
	}

	~win(
	)
	{
		// You should always call close_window() in the destructor of window
		// objects to ensure that no events will be sent to this window while 
		// it is being destructed.  
		close_window();
	}

	void display_frame(cv_image<bgr_pixel> cimg, std::vector<full_object_detection> shapes)
	{
		img.clear_overlay();
		img.set_image(cimg);
		img.add_overlay(render_face_detections(shapes));
	}

	void set_data(Landmarks *inputData) {
		data = inputData;
	}

	Landmarks *data;
	boolean recording = false;

private:

	void on_button_clicked()
	{
		// when someone clicks our button it will increment the counter and 
		// display it in our label c.
		++counter;
		ostringstream sout;
		sout << "counter: " << counter;
		c.set_text(sout.str());
	}

	void sendToMaya()
	{
		connector.send(*data);
	}

	void startRecording() {
		recording = !recording;
		if (b2.name() == "Record") {
			b2.set_name("Stop recording");
		}
		else b2.set_name("Record");
	}

	void show_about()
	{
		message_box("About", "This is an attempt to provide a facial animator for Maya, based on dlib and OpenCV. If you find this useful or totally useless, please leave a star or a comment here: https://github.com/A7ocin/FaceAnimator");
	}

	unsigned long counter;
	label c;
	button b1, b2;
	menu_bar mbar;
	image_window img;
};

//Entry point
int main(int argc, char* argv[])
{
	win my_window;
	
	try
	{
		//Start capturing ideo from the webcam
		cv::VideoCapture cap(0);
		if (!cap.isOpened())
		{
			cerr << "Unable to connect to camera" << endl;
			return 1;
		}

		//image_window win;
		//cv::namedWindow("Webcam", CV_WINDOW_AUTOSIZE);

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
		while (!my_window.is_closed())
		{
			//handle input
			if (framecount % 15 == 0)
			{
				char key = cv::waitKey(1);
				if (key == 27) {
					cout << "esc key is pressed by user" << endl;
					break;
				}
				/*else if (key == 'p') {
					record = !record;
					if (record)
						cout << "recording ON" << endl;
					else
						cout << "recording OFF" << endl;
				}*/
			}
			// Grab a frame
			cv::Mat temp, temp_small;
			cap >> temp;
			flip(temp, temp, 1);
			cv::resize(temp, temp_small, cv::Size(), 1.0 / FACE_DOWNSAMPLE_RATIO, 1.0 / FACE_DOWNSAMPLE_RATIO);
			//cv::resize(temp, temp, cv::Size(), 1.0/FACE_DOWNSAMPLE_RATIO, 1.0/FACE_DOWNSAMPLE_RATIO);
			// Turn OpenCV's Mat into something dlib can deal with.  Note that this just
			// wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
			// long as temp is valid.  Also don't do anything to temp that would cause it
			// to reallocate the memory which stores the image as that will make cimg
			// contain dangling pointers.  This basically means you shouldn't modify temp
			// while using cimg.
			cv_image<bgr_pixel> cimg(temp);
			cv_image<bgr_pixel> cimg_small(temp_small);


			// Detect faces 
			std::vector<rectangle> faces = detector(cimg_small);
			// Find the pose of each face.
			std::vector<full_object_detection> shapes;
			for (unsigned long i = 0; i < faces.size(); ++i) {
				rectangle r(
					(long)(faces[i].left() * FACE_DOWNSAMPLE_RATIO),
					(long)(faces[i].top() * FACE_DOWNSAMPLE_RATIO),
					(long)(faces[i].right() * FACE_DOWNSAMPLE_RATIO),
					(long)(faces[i].bottom() * FACE_DOWNSAMPLE_RATIO)
				);
				full_object_detection shape = pose_model(cimg, r);
				shapes.push_back(shape);
			}
			
			Landmarks &data = Landmarks();

			for (int l = 0; l <= N_LANDMARKS; ++l) {
				if (!shapes.empty()) {
					//cout << shapes[0].part(l);
					int x = shapes[0].part(l).x();
					int y = shapes[0].part(l).y();
					data.landmarks[l] = cv::Point2f(x,y);
					float currTime = std::clock() / (float)CLOCKS_PER_SEC;
					data.time = currTime - time;
					time = currTime;
				}
			}
			if (!shapes.empty()) {
				my_window.set_data(&data);
				if (my_window.recording) {
					connector.send(data);
				}
			}
			/*if (record) {
				record = !record;
			}*/

			/*** UNCOMMENT FOR FACE ORIENTATION DETECTION***/

			//// Camera internals
			//double focal_length = temp.cols; // Approximate focal length.
			//cv::Point2d center = cv::Point2d(temp.cols / 2, temp.rows / 2);
			//cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x, 0, focal_length, center.y, 0, 0, 1);
			//cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion

			//																		//cout << "Camera Matrix " << endl << camera_matrix << endl;
			//																		// Output rotation and translation
			//cv::Mat rotation_vector; // Rotation in axis-angle form
			//cv::Mat translation_vector;

			//// 2D image points. If you change the image, you need to change vector
			//std::vector<cv::Point2d> image_points = get_2d_image_points(data);

			//// 3D model points.
			//std::vector<cv::Point3d> model_points = get_3d_model_points();

			//// Solve for pose
			//cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

			//// Project a 3D point (0, 0, 1000.0) onto the image plane.
			//// We use this to draw a line sticking out of the nose

			//std::vector<cv::Point3d> nose_end_point3D;
			//std::vector<cv::Point2d> nose_end_point2D;
			//nose_end_point3D.push_back(cv::Point3d(0, 0, 1000.0));

			//projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);


			//for (int i = 0; i < image_points.size(); i++)
			//{
			//	circle(temp, image_points[i], 3, cv::Scalar(0, 0, 255), -1);
			//}

			//cv::line(temp, image_points[0], nose_end_point2D[0], cv::Scalar(255, 0, 0), 2);

			////cout << "Rotation Vector " << endl << rotation_vector << endl;
			////cout << "Translation Vector" << endl << translation_vector << endl;

			////cout << nose_end_point2D << endl;

			/*** END ***/
				
			// Display it all on the screen
			/*cv::resize(temp, temp, cv::Size(), 0.5, 0.5);
			cv::imshow("Fast Facial Landmark Detector", temp);*/

			my_window.display_frame(cimg, shapes);
			/*win.clear_overlay();
			win.set_image(cimg);
			win.add_overlay(render_face_detections(shapes));*/

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