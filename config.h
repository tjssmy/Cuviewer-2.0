/** config.h
  * Carleton University Department of Electronics
  * Copyright (C) 2001 Bryan Wan and Cliff Dugal
  */
#ifndef CONFIG_H
#define CONFIG_H

#ifdef Q_WS_WIN
static const char DOCUMENTATION[] = "C:\\progra~1\\cuviewer\\cuviewhtml";
#else
static const char DOCUMENTATION[] = "/usr/share/doc/cuviewer/cuviewhtml";
#endif
static const char VERSION[] = "3.6g";
static const char HELP_MESSAGE[] = "Carleton University 3D Viewer 3.6f\n"
"Carleton University Department of Electronics\n"
"Copyright (C) 2000, 2001, 2002 Bryan Wan and Cliff Dugal\n"
"\n"
"CU-Viewer comes with ABSOLUTELY NO WARRANTY. This is free software,\n"
"and you are welcome to redistribute it under certain conditions.\n"
"Read the LICENSE file or click Help->License for details.\n"
"\n"
"CU-Viewer is a visualization tool for 3D objects made of simple primitives.\n"
"\n"
"Usage: cuview [options] [files]\n"
"\n"
"Options:\n"
"\n"
"  -h, --help                Output this help\n"
"  -s SCRIPTFILE,            loads script file to view \n"
"  -noload                   scriptfile cannot load files \n"
"  -e imagefilename          export image in default format \n"
"  -m moviefilename          export a series of images\n"
"  -v, --version             Output version info\n"
"  -bg, -background COLOR    Set the default background color (X11 only)\n"
"  -fg, -foreground COLOR    Set the default foreground color (X11 only)\n"
"  -geometry WIDTHxHEIGHT    Set the size of the main window (X11 only)\n"
"  -lowcolor                 Use fewer colors on display; use with next option\n"
"  -visual TrueColor         Force the use of a TrueColor visual (X11 only)\n"
"  -style STYLE              Change the look and feel of the application:\n"
"                              STYLE = {motif, motifplus, cde, sgi,\n"
"                                       windows(default), platinum}\n"
"\n"
"Please report bugs at: http://cuviewer.sourceforge.net\n";

static const char QUICK_HELP[] ="1. Viewing\n"
"0-9 will display predefined viewpoints\n"
"a anti-aliasing (i.e.. remove jaggedness). b toggle bounding box\n"
"b show bounding box when camera view is moving\n"
"CTRL+B show bounding box always\n"
"c redraw document (opengl refresh) d diffuse light (i.e.. brightness of the total image)\n"
"e show scenes dialog (toggles entities)\n"
"CTRL+E Toggle edit scene mode\n"
"f fog (not implemented yet)\n"
"g gamma lighting filter\n"
"i export image to file\n"
"l lighting\n"
"o outlines\n"
"p bin window\n"
"SHIFT+P toggle bin window palette color/bw\n"
"[ decrease bins"
"] increase bins"
"q opaque wireframes\n"
"r show viewer settings panel\n"
"s shading\n"
"t two sided polygons\n"
"CTRL+T show the transform scene dialog\n"
"v change between perspective and orthogonal views\n"
"w wire frame or line mode\n"
"x transparency\n"
"z zero clipping plane of view\n"
"up,down,right,left arrow keys - orbit camera around object\n"
"/,*,-,+ keypad keys - Slide view left,right,up,down resp.\n"
"PageUp,PageDown - Rotate camera counterclockwise, clockwise\n"
"Home,End - Walk camera in,out\n"
"\n"
"Mouse Button | Keyboard Button\n"
"Left | arrow keys - Spin object about it's center\n"
"Middle | Home,End - Zooms in/out with mouse\n"
"Right - Change the camera's aim\n"
"Left+Middle | PageUp,PageDown - Rotate object about it's center\n"
"Left+Right - Rotate object around camera's center\n"
"Right+Middle | keypad -,+ or /,* - Translate camera vertically or horizontally\n"
"\n"
"2. Command Prompt Usage \n"
"\n"
"example:    cuviewer [OPTIONS] file1 file2 ...\n"
"\n"
"on lowcolor video cards:\n"
"cuviewertc [OPTIONS] file1 file2 ...\n"
"\n"
" X Windows options\n"
" -bg or -background color (X11 only) \n"
" -fg or -foreground color (X11 only) \n"
"    sets the default foreground color \n"
" -geometry WIDTHxHEIGHT (X11 only) \n"
"    sets the client geometry of the main window, example: \n"
"\n"
"     -geometry 800x600\n"
"\n"
" -visual TrueColor \n"
"     make use of all the colors from the underlying window system.\n"
" image export\n"
" -e filename  Export image to file, and uses the default image format that is specified in ~/.cuviewer/cuviewrc\n"
" viewsettings script\n"
" -s filename.script  execute script on first file\n"
"\n"
" Instructions on how to use script file: \n"
"\n"
"     open cuviewer, open a file. \n"
"     make any adjustments to viewing angle or viewer settings.\n"
"     click file->save to script file \n"
"     close and run the program with the '-s filename.script' specified and specify a cuviewer file to open. \n"
" movie script\n"
" -m filename.script  execute movie script on first file\n"
"\n"
"  Instructions on how to use a movie script file:\n"
"\n"
"     open cuviewer, open a file. \n"
"     click file->create movie script  \n"
"     save script to a file \n"
"     set movie properties (frames per second, time limit, etc...), click ok to start recording. \n"
"     exit program anytime to stop recording. \n"
"     run the program with the '-m filename.script' specified at command prompt and specify a cuviewer file to open. \n"
"     frames will be saved to disk as images in the default image format specified under global preferences.\n"

"\n\nInstructions on how to edit scenes: \n\n"
"     open cuviewer, open a file. \n"
"     click on show scenes, a dialog box shows up, and in it\n"
"     choose the scenes to edit from the list.\n"
"     click on edit mode, and the following mouse/keyboard\n"
"     edits the scenes:\n"
"     Mouse: Right button: rotate, Left: Scale, Center: Translate in and out\n"
"     RightMiddle buttons: rotate, LeftMiddle: move vertically and horizontally\n"
"     Keyboard: key arrows: rotate,  Home/End: move in and out \n"
"     keypad / *: move scenes left/right, keypad - +: move scenes up and down\n"
"     \n"
"     \n";

static const char VERSION_MESSAGE[] = "CU-Viewer 3.6f\n"
"Copyright (C) 2000, 2001 Carleton University Department of Electronics\n"
"\n"
"CU-Viewer comes with absolutely no warranty. This is free software,\n"
"and you are welcome to redistribute it under certain conditions.\n"
"Read the LICENSE file or click Help->License for details.\n"
"\n"
"Please report bugs at: "
"http://cuviewer.sourceforge.net\n";

#endif