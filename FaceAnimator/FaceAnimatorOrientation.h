#pragma once
//Construct a set of key points from a 3d face model for face orientation
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

//Construct a set of key points from the camera for face orientation
std::vector<cv::Point2d> get_2d_image_points(Landmarks data)
{
	std::vector<cv::Point2d> imagePoints;

	imagePoints.push_back(data.landmarks[30]);    // Nose tip
	imagePoints.push_back(data.landmarks[8]);    // Chin
	imagePoints.push_back(data.landmarks[45]);     // Left eye left corner
	imagePoints.push_back(data.landmarks[36]);    // Right eye right corner
	imagePoints.push_back(data.landmarks[54]);    // Left Mouth corner
	imagePoints.push_back(data.landmarks[48]);    // Right mouth corner

	return imagePoints;

}