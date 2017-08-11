# FaceAnimator

The aim of this project is to extend Maya with a plug-in for live, real-time facial animation, using images or videos (from webcam as well) of a face. The 68 face landmarks are extracted using Dlib's face landmarks detector and mapped to a rigged 3D face model in Maya.
It currently works just on Windows because it exploits WinSock2 socket library, but all the code can be easily ported on the other OSs.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

-	Visual Studio 2015 (I didn’t try with other versions, let me know if you find out other versions work as well)
-	Autodesk Maya 2017 (I didn’t try with other versions, let me know if you find out other versions work as well)
-	Dlib 19.4 (I didn’t try with other versions, let me know if you find out other versions work as well)
  
    -	Extract dlib in C:\dlib-19.4 in order to let Visual Studio find it automatically, otherwise edit the project solution file
  
    -	Build dlib:
    ```
    cd C:\dlib-19.4\
    mkdir build
    cd build

    cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX=install ..
    ```

-	OpenCV 3.2.0 (I didn’t try with other versions, let me know if you find out other versions work as well)

    -	Add an “OPENCV_DIR” environment variable in windows environments variables as well (in my case it is pointing at "C:\OpenCV\opencv\build"), in order to let Visual studio find the path (otherwise just edit the project solution file)
-	Windows 10 SDK (https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk) for using WinSock2

### Installing

Make sure you meet all of the prerequisites. Then navigate to C:\ (or wherever you want) and clone the repository (or just download and unzip it).

```
cd C:\
git clone https://github.com/A7ocin/FaceAnimator.git
```

Then open the “FaceAnimator.sln” solution file with Visual Studio and make sure to be in “Release Mode” and “x64” (on the top of the VS window).

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

