#pragma once
#include <tchar.h>
#include <fstream>
#include <sstream>
#include <string>

class FaceAnimatorGui : public drawable_window
{

public:
	// b = buttons		c = label		mbar = menu bar		img = face window
	FaceAnimatorGui() :
		c(*this),
		b1(*this),
		b2(*this),
		b3(*this),
		b4(*this),
		b5(*this),
		b6(*this),
		mbar(*this),
		img()
	{
		set_pos(0,515);
		img.set_pos(0, 0);
		img.set_title("Face Animator landmark detector");

		// Interface buttons
		b1.set_pos(10, 60);
		b1.set_name("Send current frame");
		b1.set_style(button_style_toolbar1());

		b2.set_pos(10, 100);
		b2.set_name("Record");
		b2.set_style(button_style_toolbar1());

		b3.set_pos(200, 60);
		b3.set_name("Load image");
		b3.set_style(button_style_toolbar1());

		b4.set_pos(200, 100);
		b4.set_name("Load video");
		b4.set_style(button_style_toolbar1());

		b5.set_pos(410, 60);
		b5.set_name("Open camera");
		b5.set_style(button_style_toolbar1());

		b6.set_pos(410, 100);
		b6.set_name("Reload 3D model");
		b6.set_style(button_style_toolbar1());

		// Label used to give instructions initially
		c.set_pos(b2.left(), b2.bottom() + 30);
		c.set_text("Usage: Help --> Usage");
		
		b1.set_click_handler(*this, &FaceAnimatorGui::sendToMaya);
		b2.set_click_handler(*this, &FaceAnimatorGui::startRecording);
		b3.set_click_handler(*this, &FaceAnimatorGui::loadImage);
		b4.set_click_handler(*this, &FaceAnimatorGui::loadVideo);
		b5.set_click_handler(*this, &FaceAnimatorGui::openCamera);
		b6.set_click_handler(*this, &FaceAnimatorGui::load3DModel);

		// Setting menubar menus
		mbar.set_number_of_menus(5);

		mbar.set_menu_name(0, "Camera", 'C');
		mbar.set_menu_name(1, "Image", 'I');
		mbar.set_menu_name(2, "Video", 'V');
		mbar.set_menu_name(3, "3D Model", 'M');
		mbar.set_menu_name(4, "Help", 'H');

		mbar.menu(0).add_menu_item(menu_item_text("Open Camera", *this, &FaceAnimatorGui::openCamera, 'C'));
		mbar.menu(0).add_menu_item(menu_item_separator());
		mbar.menu(0).add_menu_item(menu_item_text("Send current frame", *this, &FaceAnimatorGui::sendToMaya, 'C'));
		mbar.menu(0).add_menu_item(menu_item_text("Record", *this, &FaceAnimatorGui::startRecording, 'C'));

		mbar.menu(1).add_menu_item(menu_item_text("Load Image", *this, &FaceAnimatorGui::loadImage,'I'));
		mbar.menu(1).add_menu_item(menu_item_separator());
		mbar.menu(1).add_menu_item(menu_item_text("Send current frame", *this, &FaceAnimatorGui::sendToMaya, 'C'));
		mbar.menu(1).add_menu_item(menu_item_text("Record", *this, &FaceAnimatorGui::startRecording, 'C'));

		mbar.menu(2).add_menu_item(menu_item_text("Load Video", *this, &FaceAnimatorGui::loadVideo, 'V'));
		mbar.menu(2).add_menu_item(menu_item_separator());
		mbar.menu(2).add_menu_item(menu_item_text("Send current frame", *this, &FaceAnimatorGui::sendToMaya, 'C'));
		mbar.menu(2).add_menu_item(menu_item_text("Record", *this, &FaceAnimatorGui::startRecording, 'C'));

		mbar.menu(3).add_menu_item(menu_item_text("Reload 3D model", *this, &FaceAnimatorGui::load3DModel, 'R'));
		mbar.menu(3).add_menu_item(menu_item_separator());

		mbar.menu(4).add_menu_item(menu_item_text("Usage", *this, &FaceAnimatorGui::usage, 'U'));
		mbar.menu(4).add_menu_item(menu_item_separator());
		mbar.menu(4).add_menu_item(menu_item_text("About FaceAnimator", *this, &FaceAnimatorGui::show_about, 'A'));
		
		set_size(550, 200);

		set_title("Face Animator controller");
		show();
	}

	~FaceAnimatorGui()
	{ 
		close_window();
	}

	void display_frame(cv_image<bgr_pixel> cimg, std::vector<full_object_detection> shapes)
	{
		img.clear_overlay();
		img.set_image(cimg);
		img.add_overlay(render_face_detections(shapes));
	}

	void clearOverlay() {
		img.clear_overlay();
	}

	void set_data(Landmarks *inputData) {
		data = inputData;
	}

	Landmarks *data;
	boolean recording = false;
	enum sourceEnum
	{
		CAMERA,
		IMAGE,
		VIDEO
	};
	int source = CAMERA;

	string imgPath = "trump.jpg", vidPath = "trump.mp4";
	cv::Mat image;

private:

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

	void load3DModel() {
		std::ifstream ifs("FAConnector.mel");
		std::string script((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
		connector.sendScript(script);
		script = "";
	}

	void loadImage() {
		OPENFILENAME ofn = { 0 };
		TCHAR szFileName[MAX_PATH] = _T("");

		ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
		ofn.hwndOwner = 0;
		ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0\0");
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = _T("txt");

		if (GetOpenFileName(&ofn)) // user selected an input file
		{
			MessageBox(NULL, szFileName, _T("It works!"), MB_OK);
			source = IMAGE;
			imgPath = szFileName;
		}
	}

	void loadVideo() {
		OPENFILENAME ofn = { 0 };
		TCHAR szFileName[MAX_PATH] = _T("");

		ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
		ofn.hwndOwner = 0;
		ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0\0");
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = _T("txt");

		if (GetOpenFileName(&ofn)) // user selected an input file
		{
			MessageBox(NULL, szFileName, _T("It works!"), MB_OK);
			source = VIDEO;
			vidPath = szFileName;
		}
	}

	void openCamera() {
		source = CAMERA;
	}

	void usage()
	{
		message_box("Usage", "Usage:\n 1) Press \"Send current frame\" to map the current face landmarks to the 3D Maya model.\n 2) Press \"Record\" to keep sending frames to Maya (can be slow)\n 3) Press \"Load image\" to animate the model starting from a given face image\n 4) Press \"Load video\" to animate the model starting from a given face video\n 5) Press \"Open camera\" to switch back to camera\n 6) Press \"Reload 3D model\" to re-load the 3D model and start a new capture\n\n Achtung! Use Send current frame and Record even if you are not using the camera!");
	}
	
	void show_about()
	{
		message_box("About", "This is an attempt to provide a facial animator for Maya, based on dlib and OpenCV. If you find this useful or totally useless, please leave a star or a comment here: https://github.com/A7ocin/FaceAnimator");
	}



	unsigned long counter;
	label c;
	button b1, b2, b3, b4, b5, b6;
	menu_bar mbar;
	image_window img;

	// Open dialog window
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	HWND hwnd;              // owner window
	HANDLE hf;              // file handle
	

};