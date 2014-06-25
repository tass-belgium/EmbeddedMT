EmbeddedMT
==========

Embedded motion tracker using computer vision. This project envisages generic efficient motion tracking based on computer vision algorithms for resource limited devices.

Dependencies:
-------------

Most dependencies are because of the diverse availability of capturing devices and video en- and decoders available. May be improved later.

For the image processing part:
- gcc or clang
- python 2.x
- ffmpeg + swscale (minimal dependencies of ffmpeg: avformat, avcodec, avutil, swscale)
- For Linux:
	- V4L (V1 & V2)
- For MAC OS X:
	- Cocoa framework (normally standard present)
=> Ffmpeg can be replaced by gstreamer package (minimal dependencies: gstreamer, gstapp, gstpbutils, gstriff, gobject, glib2.0)

For GUI:
- python 3 (3.4 recommend, else change in demo/demo\_macosx)
- numpy
- matplotlib
- For Linux:
	- GTK2 (minimal dependencies: gtk-x11, gdk-x11, cairo, gdk\_pixbuf-2.0, gobject-2.0, glib-2.0)
- For MAC OS X:
 	- Qtkit (normally standard present)


How to build:
-------------
run get3rdpartyGit.sh:	$ ./get3rdpartyGit.sh
run demo (first time the project will be built):
	for linux:		$ cd demo; ./demo_linux.sh
	for MAC OS X:	$ cd demo; ./demo_macosx.sh

How to use:
-----------
For localhost demo's run the demo as described in 'how to build'. Alternatively or for non-localhost usage, use the python wrappers in wrappers/. Use the -h function for more info on each wrapper.
