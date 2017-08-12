# FaceAnimator

The aim of this project is to extend Maya with a plug-in for live, real-time facial animation, using images or videos (from webcam as well) of a face. The 68 face landmarks are extracted using Dlib's face landmarks detector and mapped to a rigged 3D face model in Maya.
It currently works just on Windows because it exploits WinSock2 socket library, but all the code can be easily ported on the other OSs.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

-	Visual Studio 2017 or 2015 (I didn’t try with other versions, let me know if you find out other versions work as well)

    -   Make sure to select "Desktop development with C++", together with the latest Windows 10 SDK and The VC++ 2015 development tools (on the right) during the installation
    -   I suggest using VS2015, but the 2017 version works as well. Just make sure you are using the 2015 (v140) build tools when you load FaceAnimator on VS for the first time
	
-	Autodesk Maya 2018 or 2017 (I didn’t try with other versions, let me know if you find out other versions work as well)

    -   I suggest using the 2018 one for increased performance, if you are a student you can get it for free!
	
-	CMake
 
    -   MAKE SURE TO INSTALL IT AFTER Visual Studio 2015!
    -   Make sure to add it to the system path during the installation
	
-	Dlib 19.4 (http://dlib.net/files/dlib-19.4.zip) (I didn’t try with other versions, let me know if you find out other versions work as well)
  
    -	Extract dlib in C:\dlib-19.4 in order to let Visual Studio find it automatically, otherwise edit the project solution file
  
    -   Build dlib (the easy way):
	    Run cmake-gui as administrator
		Select "Browse Source" and insert C:/dlib-19.4
		Select "Browse Build" and insert C:/dlib-19.4/build (if the folder doesn't exist, create it)
		Click "Configure"
		MAKE SURE TO SELECT THE Win64 VERSION OF YOUR Visual Studio Installation!!
		If errors appear, click "Configure" again (no errors should be displayed at this time, just warnings)
		Click generate
		In the output window there should be displayer "Configuring done" and "Generating done"
		Navigate to C:\dlib-19.4\build\dlib and open INSTALL.vcxproj with Visual Studio
		Select "Release" and "x64" on the top
		Build the entire solution using Visual Studio
		Done! :)
		
    -	Build dlib (the not-so-easy way, some errors may occur):
    ```
    cd C:\dlib-19.4\
    mkdir build
    cd build

	//If you installed Visual Studio 2015
    cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX=install .. 
	
	cmake --build . --config Release --target INSTALL
	cd ..
    ```
	
	- If the build fails please let me know

-	OpenCV 3.2.0 (https://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.2.0/opencv-3.2.0-vc14.exe/download) (I didn’t try with other versions, let me know if you find out other versions work as well)

    -   The installer will just extract another extractor. Open that as well and extract it to C:\OpenCV (create the folder if needed
    -	Add an “OPENCV_DIR” environment variable in windows environments variables as well (in my case it is pointing at "C:\OpenCV\opencv\build"), in order to let Visual studio find the path (otherwise just edit the project solution file)
    -   Add "C:\OpenCV\opencv\build\x64\vc14\bin" to the path environment variable
	
-	Windows 10 SDK (https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk) for using WinSock2

    -   It should have been installed during the VS2015 installation, skip this installation if you installed it by using the VS installer
    -   Make sure to install this after everything else

### Installing

Make sure you meet all of the prerequisites. Then navigate to C:\ (or wherever you want) and clone the repository (or just download and unzip it).

```
cd C:\
git clone https://github.com/A7ocin/FaceAnimator.git
```

Then open the “FaceAnimator.sln” solution file with Visual Studio and make sure to be in “Release Mode” and “x64” (on the top of the VS window).

Right click on Source Files --> Add --> Existing Item and add C:\dlib-19.4\dlib\all\source.cpp

Build the solution by right clicking on the solution in the solution explorer (on the right of the VS window) and click “Build Solution”

If no errors are displayed, open the 3D model “Face.mb” under the “Models” folder in the FaceAnimator folder. Then, in Maya, open the script editor (bottom-right button) and load the “FaceAnimatorSimple3.mel” script and run it

Finally, run FaceAnimator by hitting F5 in Visual Studio or by executing the .exe file under “/x64/Release/” folder.

## Contributing

If you are willing to contribute to the project, just open new issues directly here on GitHub or send me an email. 

## Authors

* **Nicola Garau** 
* **Annalisa Congiu** 

## License

## Acknowledgments

