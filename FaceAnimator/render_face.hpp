#pragma once
#ifndef BIGVISION_RENDER_FACE_H_
#define BIGVISION_RENDER_FACE_H_

#include <dlib/image_processing/frontal_face_detector.h>
#include <opencv2/highgui/highgui.hpp>

// draws polylines given 
// - a cv Matrix representing the image
// - 
// - first point in the landmark enumeration corresponding to that face feature
// - last point in the landmark enumeration corresponding to that face feature
// - whether the polyline is a closed polygon or not
void draw_polyline(cv::Mat &img, const dlib::full_object_detection& d, const int start, const int end, bool isClosed = false)
{
	std::vector <cv::Point> points;
	for (int i = start; i <= end; ++i)
	{
		points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
	}
	cv::polylines(img, points, isClosed, cv::Scalar(255, 0, 0), 2, 16);

}

// draw polylines for each face feature
void render_face(cv::Mat &img, const dlib::full_object_detection& d)
{
	DLIB_CASSERT
	(
		d.num_parts() == 68,
		"\t std::vector<image_window::overlay_line> render_face_detections()"
		<< "\n\t Invalid inputs were given to this function. "
		<< "\n\t d.num_parts():  " << d.num_parts()
	);

	draw_polyline(img, d, 0, 16);           // Jaw line
	draw_polyline(img, d, 17, 21);          // Left eyebrow
	draw_polyline(img, d, 22, 26);          // Right eyebrow
	draw_polyline(img, d, 27, 30);          // Nose bridge
	draw_polyline(img, d, 30, 35, true);    // Lower nose
	draw_polyline(img, d, 36, 41, true);    // Left eye
	draw_polyline(img, d, 42, 47, true);    // Right Eye
	draw_polyline(img, d, 48, 59, true);    // Outer lip
	draw_polyline(img, d, 60, 67, true);    // Inner lip

}

#endif // BIGVISION_RENDER_FACE_H_