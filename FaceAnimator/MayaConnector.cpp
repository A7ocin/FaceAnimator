#include "MayaConnector.h"

using namespace cv;

float xOffset = 0.0;
float yOffset = 0.0;
float xModifier = 0.0;
float yModifier = 0.0;

void MayaConnector::connect() {
	try {
		internal_socket.connect("localhost", 5055);
		int flag = 1;
		//setsockopt(internal_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
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

// Create a MEL command to map a point amongst the 68 landmarks point to the corresponding joint in Maya
std::string toMelTranslation(std::string name, Point2f pos) {
	std::string output;
	//return "setAttr " + name + ".translateX " + std::to_string((pos.x - xOffset)) + ";\n setAttr " + name + ".translateY " + std::to_string((-pos.y + yOffset)) + ";\n";
	output = "select -r " + name + ";";
	output += "float $zArr[] = `xform - q - t - ws`;";
	output += "float $z = $zArr[2];";
	output += "move -a -x (" + std::to_string((pos.x)) + "*$xModifier);";
	output += "move -a -y (" + std::to_string(-(pos.y)) + "*$yModifier);";
	return output;
}

// Send data extracted from frames to Maya by constructing a MEL command script (cmd) that is being sent via TCP/IP
int frm = 0;
bool MayaConnector::send(Landmarks &data) {

	// Prevent Maya from collapsing all the joint if no face is detected
	if (data.landmarks[CONTOUR1].x == data.landmarks[CONTOUR17].x
		&& data.landmarks[CONTOUR1].y == data.landmarks[CONTOUR17].y) {
		return false;
	}

	//Manage extreme rotations
	if (abs(data.xRotation * 180 / CV_PI) > 45
		|| abs(data.yRotation * -180 / CV_PI) > 45
		|| abs(data.zRotation * -180 / CV_PI) > 45) {
		return false;
	}

	// Start constructing the cmd
	// Take the sides of the face, the nose and the chin as reference points to build two modifiers (x and y) for the new transformation

	std::string cmd = "";
	xOffset = data.landmarks[NOSE28].x;
	yOffset = -data.landmarks[NOSE28].y;
	cmd += "SelectAll; move -a " + std::to_string(xOffset) + " " + std::to_string(yOffset) + " 0;";
	//cmd += "SelectAll; move -a " + std::to_string(data.xTranslation) + " " + std::to_string(data.yTranslation) + " " + std::to_string(data.zTranslation) + ";";
	
	cmd += "rotate -a " + std::to_string(data.xRotation * 180 / CV_PI) + " " + std::to_string(data.yRotation * -180 / CV_PI) + " " + std::to_string(data.zRotation * -180 / CV_PI) +";";
	cmd += "viewFit;";
	cmd += "float $xReferencePoint1[] = `xform -q -t -ws CONTOUR1`;";
	cmd += "float $xReferencePoint2[] = `xform -q -t -ws CONTOUR17`;";
	cmd += "float $xReferencePoint = ($xReferencePoint1[0]+$xReferencePoint2[0]) / 2;";
	cmd += "float $xModifier = $xReferencePoint/" + std::to_string((data.landmarks[CONTOUR1].x + data.landmarks[CONTOUR17].x)/2) + ";";

	cmd += "float $yReferencePoint1[] = `xform -q -t -ws CONTOUR9`;";
	cmd += "float $yReferencePoint2[] = `xform - q - t - ws NOSE28`;";
	cmd += "float $yReferencePoint = ($yReferencePoint1[1] + $yReferencePoint2[1]) / 2; ";
	cmd += "float $yModifier = abs($yReferencePoint)/" + std::to_string((data.landmarks[CONTOUR9].y + data.landmarks[NOSE28].y)/2) + ";";

	cmd += "currentTime " + std::to_string(frm++) + ";\n";
	//cmd += "currentTime (`currentTime -query` + " + std::to_string(data.timeStep*24) + ");\n";
	//cmd += "currentTime (`currentTime -query` + 1);\n";


	// Pick the correct landmark group and build the correct MEL command for the transformation (modifiers included)

	for (int i = 0; i < N_LANDMARKS; i++) {
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

	// Set a new keyframe for the animation
	cmd += "setKeyframe CONTOUR1 CONTOUR2 CONTOUR3 CONTOUR4 CONTOUR5 CONTOUR6 CONTOUR7 CONTOUR8 CONTOUR9 CONTOUR10 CONTOUR11 CONTOUR12 CONTOUR13 CONTOUR14 CONTOUR15 CONTOUR16 CONTOUR17 R_EYEBROW18 R_EYEBROW19 R_EYEBROW20 R_EYEBROW21 R_EYEBROW22 L_EYEBROW23 L_EYEBROW24 L_EYEBROW25 L_EYEBROW26 L_EYEBROW27 NOSE28 NOSE29 NOSE30 NOSE31 NOSE32 NOSE33 NOSE34 NOSE35 NOSE36 R_EYE37 R_EYE38 R_EYE39 R_EYE40 R_EYE41 R_EYE42 L_EYE43 L_EYE44 L_EYE45 L_EYE46 L_EYE47 L_EYE48 OUTER_LIPS49 OUTER_LIPS50 OUTER_LIPS51 OUTER_LIPS52 OUTER_LIPS53 OUTER_LIPS54 OUTER_LIPS55 OUTER_LIPS56 OUTER_LIPS57 OUTER_LIPS58 OUTER_LIPS59 OUTER_LIPS60 INNER_LIPS61 INNER_LIPS62 INNER_LIPS63 INNER_LIPS64 INNER_LIPS65 INNER_LIPS66 INNER_LIPS67 INNER_LIPS68;\n";
	frm++;
	//std::cout << cmd;


	// Try to send the message via socket

	try {
		char *char_cmd = new char[cmd.length() + 1];
		strcpy(char_cmd, cmd.c_str());

		if (internal_socket.send(char_cmd) < cmd.length()) {
			std::cout << "full message not sent! Expected " << std::to_string(cmd.length()) << " of data." << std::endl;
		}

		delete[] char_cmd;
		return true;
	}
	catch (int e) {
		connect();
		return false;
	}

	return true;
}

bool MayaConnector::sendScript(std::string script) {
	try {
		char *char_cmd = new char[script.length() + 1];
		strcpy(char_cmd, script.c_str());

		if (internal_socket.send(char_cmd) < script.length()) {
			std::cout << "full message not sent! Expected " << std::to_string(script.length()) << " of data." << std::endl;
		}

		delete[] char_cmd;
		return true;
	}
	catch (int e) {
		connect();
		return false;
	}
}

bool MayaConnector::isConnected() {
	return connected;
}