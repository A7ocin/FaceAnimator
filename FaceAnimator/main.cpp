#include "FaceAnimatorHeaders.h"

// Entry point
int main(int argc, char* argv[])
{
	//Hide cmd window
	//FreeConsole();

	// Generate FaceAnimator windows
	FaceAnimatorGui my_window;
	my_window.set_background_color(255, 255, 255);
	
	try
	{
		// Start capturing video from the webcam
		cv::VideoCapture cap(0);
		cv::Mat capImage = cv::imread(my_window.imgPath, CV_LOAD_IMAGE_COLOR);
		cv::VideoCapture capVideo = cv::VideoCapture(my_window.vidPath);
		
		if (!cap.isOpened())
		{
			cerr << "Unable to connect to camera" << endl;
			return 1;
		}

		// Setup FPS calculation
		int framecount = 0;
		std::clock_t start;
		double duration;
		start = std::clock();

		bool record = false;
		bool loadedI = false;
		// bool loadedV = false;

		float time = 0;

		// Load face detection and pose estimation models.
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

		// Grab and process frames until the main window is closed by the user.
		while (!my_window.is_closed())
		{
			// Handle input
			if (framecount % 15 == 0)
			{
				char key = cv::waitKey(1);
				if (key == 27) {
					cout << "esc key is pressed by user" << endl;
					break;
				}
			}

			// Grab a frame
			cv::Mat temp, temp_small;
			switch (my_window.source)
			{
				case my_window.CAMERA:
					cap >> temp;
					break;
				case my_window.IMAGE:
					capImage = cv::imread(my_window.imgPath, CV_LOAD_IMAGE_COLOR);
					temp = capImage;
					break;
				case my_window.VIDEO:
					if (capVideo.get(cv::CAP_PROP_POS_FRAMES) == capVideo.get(CV_CAP_PROP_FRAME_COUNT) - 1) {
						capVideo = cv::VideoCapture(my_window.vidPath);
						capVideo.set(cv::CAP_PROP_POS_FRAMES, 1);
					}
					capVideo >> temp;
					break;
			}
			
			if (my_window.source == my_window.CAMERA) {
				flip(temp, temp, 1);
			}

			//// Select ROI
			//cv::Rect2d croppedFace = cv::selectROI(temp);

			//// Crop image
			//temp = temp(croppedFace);

			double scaleFactor = (temp.size().height * 100)/480;

			//640x480
			//cv::resize(temp, temp, cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT)), 0, 0, CV_INTER_LINEAR);
			cv::resize(temp, temp, cv::Size(temp.size().width * 100 / scaleFactor, temp.size().height * 100 / scaleFactor), 0, 0, CV_INTER_LINEAR);
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
			std::vector<rectangle> faces = detector(cimg);

			// Find the pose of each face.
			std::vector<full_object_detection> shapes;
			for (unsigned long i = 0; i < faces.size(); ++i) {
				rectangle r(
					(long)(faces[i].left() * FACE_DOWNSAMPLE_RATIO),
					(long)(faces[i].top() * FACE_DOWNSAMPLE_RATIO),
					(long)(faces[i].right() * FACE_DOWNSAMPLE_RATIO),
					(long)(faces[i].bottom() * FACE_DOWNSAMPLE_RATIO)
				);
				full_object_detection shape = pose_model(cimg, faces[i]);
				shapes.push_back(shape);
			}
			
			Landmarks &data = Landmarks();
			// Update of landmarks based on shapes (current face pose)
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

			/*** COMMENT/UNCOMMENT FOR FACE ORIENTATION DETECTION***/

			if (!shapes.empty()) {
				// Camera internals
				double focal_length = temp.cols; // Approximate focal length.
				cv::Point2d center = cv::Point2d(temp.cols / 2, temp.rows / 2);
				cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x, 0, focal_length, center.y, 0, 0, 1);
				cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion

				//cout << "Camera Matrix " << endl << camera_matrix << endl;
				// Output rotation and translation
				cv::Mat rotation_vector; // Rotation in axis-angle form
				cv::Mat translation_vector;

				// 2D image points. If you change the image, you need to change vector
				std::vector<cv::Point2d> image_points = get_2d_image_points(data);

				// 3D model points.
				std::vector<cv::Point3d> model_points = get_3d_model_points();

				// Solve for pose
				cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

				// Project a 3D point (0, 0, 1000.0) onto the image plane.
				// We use this to draw a line sticking out of the nose
				std::vector<cv::Point3d> nose_end_point3D;
				std::vector<cv::Point2d> nose_end_point2D;
				nose_end_point3D.push_back(cv::Point3d(0, 0, 1000.0));

				projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);

				// Draw a tiny red dot for each image point (nose tip, corners of eyes and mouth, chin)
				for (int i = 0; i < image_points.size(); i++)
				{
					circle(temp, image_points[i], 3, cv::Scalar(0, 0, 255), -1);
				}

				// Draw a line connecting the tip of the nose with the tip of Pinocchio's nose
				cv::line(temp, image_points[0], nose_end_point2D[0], cv::Scalar(255, 0, 0), 2);
				
				// Grab face rotation and translation values to send them to Maya
				data.xRotation = rotation_vector.at<double>(0);
				data.yRotation = rotation_vector.at<double>(1);
				data.zRotation = rotation_vector.at<double>(2);

				data.xTranslation = translation_vector.at<double>(0);
				data.yTranslation = translation_vector.at<double>(1);
				data.zTranslation = translation_vector.at<double>(2);

				//cout << "Rotation Vector " << endl << rotation_vector << endl;
				//cout << "Translation Vector" << endl << translation_vector << endl;

				//cout << nose_end_point2D << endl;
			}

			/*** END ORIENTATION DETECTION***/


			// Done, send everything to Maya

			if (!shapes.empty()) {
				my_window.set_data(&data);
				if (my_window.recording) {
					connector.send(data);
				}
				/*if (my_window.loadIm){

				}*/
			}

			
				
			// Now display it all on the screen
			/*cv::resize(temp, temp, cv::Size(), 0.5, 0.5);
			cv::imshow("Fast Facial Landmark Detector", temp);*/

			// Display frame and face detected countours on overlay
			my_window.display_frame(cimg, shapes);
			/*win.clear_overlay();
			win.set_image(cimg);
			win.add_overlay(render_face_detections(shapes));*/
			shapes.clear();

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


