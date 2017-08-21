#pragma once

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

		// Label used to give instructions initially
		c.set_pos(b2.left(), b2.bottom() + 20);
		c.set_text("Usage:\n 1) Press \"Send current frame\" to map the current face landmarks to the 3D Maya model.\n 2) Press \"Record\" to keep sending frames to Maya (can be slow)\n 3) Press \"Load image\" to animate the model starting from a given face image\n 4) Press \"Load videp\" to animate the model starting from a given face video");
		
		b1.set_click_handler(*this, &FaceAnimatorGui::sendToMaya);
		b2.set_click_handler(*this, &FaceAnimatorGui::startRecording);
		b3.set_click_handler(*this, &FaceAnimatorGui::loadImage);
		//b4.set_click_handler(*this, &FaceAnimatorGui::loadVideo);

		// Setting menubar menus
		mbar.set_number_of_menus(4);

		mbar.set_menu_name(0, "Camera", 'C');
		mbar.set_menu_name(1, "Image", 'I');
		mbar.set_menu_name(2, "Video", 'V');
		mbar.set_menu_name(3, "About", 'A');

		mbar.menu(0).add_menu_item(menu_item_text("Send current frame", *this, &FaceAnimatorGui::sendToMaya, 'C'));
		mbar.menu(0).add_menu_item(menu_item_text("Record", *this, &FaceAnimatorGui::startRecording, 'C'));
		mbar.menu(0).add_menu_item(menu_item_separator());

		mbar.menu(1).add_menu_item(menu_item_text("Load Image", *this, &FaceAnimatorGui::loadImage,'I'));
		mbar.menu(1).add_menu_item(menu_item_separator());

		//mbar.menu(2).add_menu_item(menu_item_text("Load Video", *this, &FaceAnimatorGui::loadVideo, 'V'));
		//mbar.menu(2).add_menu_item(menu_item_separator());

		mbar.menu(3).add_menu_item(menu_item_text("About FaceAnimator", *this, &FaceAnimatorGui::show_about, 'A'));
		
		set_size(550, 250);

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

	void set_data(Landmarks *inputData) {
		data = inputData;
	}

	Landmarks *data;
	boolean recording = false;
	boolean loadIm = false;

	string path;
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

	void loadImage() {
		// Initialize OPENFILENAME (Open dialogue box)
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 

		if (GetOpenFileName(&ofn) == TRUE)
			hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

		path = ofn.lpstrFile;

		image = cv::imread(path, CV_LOAD_IMAGE_COLOR);   // Read the file
		loadIm = !loadIm;
		if (!image.data) {
			message_box("Error", "It was not possible to open the image.");
		}
		// constructor for the creation of an Open dialog box.
		BOOL WINAPI GetOpenFileName(
			_Inout_ LPOPENFILENAME lpofn
		);
	}
	
	void show_about()
	{
		message_box("About", "This is an attempt to provide a facial animator for Maya, based on dlib and OpenCV. If you find this useful or totally useless, please leave a star or a comment here: https://github.com/A7ocin/FaceAnimator");
	}



	unsigned long counter;
	label c;
	button b1, b2, b3, b4;
	menu_bar mbar;
	image_window img;

	// Open dialog window
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	HWND hwnd;              // owner window
	HANDLE hf;              // file handle
	

};