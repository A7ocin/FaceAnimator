#include "MayaConnector.h"

#include <string>

#include "opencv2/core/core.hpp"

using namespace cv;

float xOffset = 0.0;
float yOffset = 0.0;
float xModifier = 0.0;
float yModifier = 0.0;

void MayaConnector::connect() {
	try {
		internal_socket.connect("localhost", 5055);
		connected = true;
	}
	catch (int e) {
		connected = false;
	}
}

MayaConnector::MayaConnector() {
	connect();
}

MayaConnector::~MayaConnector() {

}

std::string toMelTranslation(std::string name, Point2f pos) {
	//FULKOD! borde kanske basera skalning på något annat, typ ansiktsbredd istället för att bara dela med godtyckligt tal? kanske göra det möjligt att välja för olika delar av ansiktet?
	// Ja tack!
	//Provar att sätta vikter i mel-scriptet!
	float modifier = 0.001;
	//return "setAttr " + name + ".translateX " + std::to_string((pos.x - xOffset)) + ";\n setAttr " + name + ".translateY " + std::to_string((-pos.y + yOffset)) + ";\n";
	return "select -r " + name + ";float $zArr[] = `xform -q -t -ws`;float $z = $zArr[2];move -a (" + std::to_string((pos.x)) + "*$xModifier) (" + std::to_string((-pos.y)) + "*$yModifier)" + " $z;";
}

int frm = 0;
bool MayaConnector::send(Landmarks &data) {
	std::string cmd = "";
	xOffset = data.landmarks[NOSE28].x;
	yOffset = -data.landmarks[NOSE28].y;
	cmd += "SelectAll; move -a " + std::to_string(xOffset) + " " + std::to_string(yOffset) + " 0;";
	cmd += "float $xReferencePoint[] = `xform -q -t -ws CONTOUR1`;float $xModifier = $xReferencePoint[0]/" + std::to_string(data.landmarks[CONTOUR1].x) + ";";
	cmd += "float $yReferencePoint[] = `xform -q -t -ws CONTOUR9`;float $yModifier = -$yReferencePoint[1]/" + std::to_string(data.landmarks[CONTOUR9].y) + ";";
	//std::cout << "--> "+ std::to_string(data.landmarks[CONTOUR17].x - xOffset) + " - " + std::to_string(data.landmarks[CONTOUR1].x - xOffset) + "\n";
	//std::cout << "--> " + std::to_string(data.landmarks[INNER_LIPS68].x) + " ; " + std::to_string(data.landmarks[INNER_LIPS68].y) + "\n";
	//xModifier = 165.643982 / (data.landmarks[CONTOUR17].x - data.landmarks[CONTOUR1].x);
	//yModifier = -18.124014 / (-data.landmarks[CONTOUR1].y + yOffset);

	//cmd += "currentTime " + std::to_string(frm++) + ";\n";
	//cmd += "currentTime (`currentTime -query` + " + std::to_string(data.timeStep*24) + ");\n";
	//cmd += "currentTime (`currentTime -query` + 1);\n";

	for (int i = 0; i < 68; i++) {
		if (i < 17) {
			cmd += toMelTranslation("CONTOUR" + std::to_string(i+1), data.landmarks[i]);
		}
		else if (i < 22) {
			cmd += toMelTranslation("R_EYEBROW" + std::to_string(i+1), data.landmarks[i]);
		}
		else if (i<27) {
			cmd += toMelTranslation("L_EYEBROW" + std::to_string(i+1), data.landmarks[i]);
		}
		else if (i < 36) {
			cmd += toMelTranslation("NOSE" + std::to_string(i+1), data.landmarks[i]);
		}
		else if (i<42) {
			cmd += toMelTranslation("R_EYE" + std::to_string(i+1), data.landmarks[i]);
		}
		else if (i < 48) {
			cmd += toMelTranslation("L_EYE" + std::to_string(i+1), data.landmarks[i]);
		}
		else if (i<60) {
			cmd += toMelTranslation("OUTER_LIPS" + std::to_string(i+1), data.landmarks[i]);
		}
		else {
			cmd += toMelTranslation("INNER_LIPS" + std::to_string(i+1), data.landmarks[i]);
		}
	}

	//cmd += "setKeyframe C1 C2 C3 C4 C5 C6 C7 C8 C9 C10 C11 C12 C13 C14 C15 C16 C17 REB18 REB19 REB20 REB21 REB22 LEB23 LEB24 LEB25 LEB26 LEB27 N28 N29 N30 N31 N32 N33 N34 N35 N36 RE37 RE38 RE39 RE40 RE41 RE42 LE43 LE44 LE45 LE46 LE47 LE48 OL49 OL50 OL51 OL52 OL53 OL54 OL55 OL56 OL57 OL58 OL59 OL60 IL61 IL62 IL63 IL64 IL65 IL66 IL67 IL68;\n";
	frm++;
	std::cout << cmd;

	try {
		if (internal_socket.send(cmd) < cmd.length()) {
			std::cout << "full message not sent!" << std::endl;
		}
		return true;
	}
	catch (int e) {
		connect();
		return false;
	}

	return true;
}

bool MayaConnector::isConnected() {
	return connected;
}