EmbeddedMT
==========

Embedded motion tracker using computer vision. This project envisages generic efficient motion tracking based on computer vision algorithms for resource limited devices.

Dependencies:
-------------

Most dependencies are because of the diverse availability of capturing devices and video en- and decoders available. May be improved later.

For the image processing part:
- gcc or clang
- scons
- cmake
- ffmpeg + swscale (minimal dependencies of ffmpeg: avformat, avcodec, avutil, swscale)
- For Linux:
	- V4L (V1 & V2)
- For MAC OS X:
	- Cocoa framework (normally standard present)
=> Ffmpeg can be replaced by gstreamer package (minimal dependencies: gstreamer, gstapp, gstpbutils, gstriff, gobject, glib2.0)

For GUI:
- python 3 (3.4 recommend, else change in demo/demo\_macosx)
- scipy
- numpy
- matplotlib
- For Linux:
	- GTK2 (minimal dependencies: gtk-x11, gdk-x11, cairo, gdk\_pixbuf-2.0, gobject-2.0, glib-2.0)
- For MAC OS X:
 	- Qtkit (normally standard present)

All necessary packages are available in most Linux package managers and Macports.

Names of the necessary packages in macports:
- gcc 4.8 : gcc48 (it is recommended to install gcc using the 'Command Line Tools' from the App Store, this way you will get the latest clang compiler)
- python2.7 : python27
- python3.4 : python34
- ffmpeg : ffmpeg (gstreamer 1.0 : gestreamer1 and gst-ffmpeg)
- scipy: py34-scipy
- numpy: py34-numpy
- matplotlib: py34-matplotlib

How to build:
-------------
run get3rdpartyGit.sh:	

	$ ./get3rdpartyGit.sh

run demo (first time the project will be built):
- for Linux:	

	$ cd demo; ./demo_linux.sh
		
- for MAC OS X:	

	$ cd demo; ./demo_macosx.sh

How to use:
-----------
For localhost demo's run the demo as described in 'how to build'. Alternatively or for non-localhost usage, use the python wrappers in wrappers/. Use the -h function for more info on each wrapper.

NOTE: for the best results it is recommended to set the exposure time of the capture device to manual.
