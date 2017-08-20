#pragma once
#ifndef __LANDMARKS_H
#define __LANDMARKS_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

const int N_LANDMARKS = 68;

enum Landmark {
	CONTOUR1 = 0,
	CONTOUR2,
	CONTOUR3,
	CONTOUR4,
	CONTOUR5,
	CONTOUR6,
	CONTOUR7,
	CONTOUR8,
	CONTOUR9,
	CONTOUR10,
	CONTOUR11,
	CONTOUR12,
	CONTOUR13,
	CONTOUR14,
	CONTOUR15,
	CONTOUR16,
	CONTOUR17,
	R_EYEBROW18,
	R_EYEBROW19,
	R_EYEBROW20,
	R_EYEBROW21,
	R_EYEBROW22,
	L_EYEBROW23,
	L_EYEBROW24,
	L_EYEBROW25,
	L_EYEBROW26,
	L_EYEBROW27,
	NOSE28,
	NOSE29,
	NOSE30,
	NOSE31,
	NOSE32,
	NOSE33,
	NOSE34,
	NOSE35,
	NOSE36,
	R_EYE37,
	R_EYE38,
	R_EYE39,
	R_EYE40,
	R_EYE41,
	R_EYE42,
	L_EYE43,
	L_EYE44,
	L_EYE45,
	L_EYE46,
	L_EYE47,
	L_EYE48,
	OUTER_LIPS49,
	OUTER_LIPS50,
	OUTER_LIPS51,
	OUTER_LIPS52,
	OUTER_LIPS53,
	OUTER_LIPS54,
	OUTER_LIPS55,
	OUTER_LIPS56,
	OUTER_LIPS57,
	OUTER_LIPS58,
	OUTER_LIPS59,
	OUTER_LIPS60,
	INNER_LIPS61,
	INNER_LIPS62,
	INNER_LIPS63,
	INNER_LIPS64,
	INNER_LIPS65,
	INNER_LIPS66,
	INNER_LIPS67,
	INNER_LIPS68
};

struct Landmarks {
	Landmarks() {
		// Initialize landmarks to 0,0
		for (int i = 0; i < N_LANDMARKS; i++) {
			landmarks[i] = cv::Point2f(0, 0);
		}
		/*rotationVector = cv::Mat(3, 1, CV_64F);
		translationVector = cv::Mat(3, 1, CV_64F);
		for (int i = 0; i < 3; i++) {
			rotationVector.at<int>(0) = 0.0f;
			translationVector.at<int>(0) = 0.0f;
		}*/

		time = -1;
	}

	double xRotation, yRotation, zRotation;
	double xTranslation, yTranslation, zTranslation;
	cv::Point2f landmarks[N_LANDMARKS]; //?
	float time;
};

#endif 	/* LANDMARKS_H */