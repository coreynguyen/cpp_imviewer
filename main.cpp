/*
Image Viewer for quicker tagging datasets for stable diffusion using FLTK

April 8 2023
decided to abondon the use the SQLite after trying to adapt it for several weeks and re-write.
its just to damn slow and I don't have the education with the software to make proper use.
I will be switching back to writting text files to store any app data.

-Corey


*/


#include <iostream>
#include <string>
#include <vector>

#include "version.h"
#include "window.h"
#include "bytestream.h"
#include "filesystem.h"
#include "stringenc.h"
#include "stringext.h"
#include "resource.h"
#include "database.h"


#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

using namespace std;

int main (int argc, char** argv) {


    // Include Build Date in the Application Title
    SetConsoleTitleA(appver);

    // Get Commands in Wide Characters
    int argcW; LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), &argcW); if (argcW == 0) {return 0;}

    make_window();
    Fl::add_handler(keyhandler);


    if (argc > 1) {
        read_image(unicode_to_utf8(argvW[1]));

        }
    LocalFree(argvW); // Free memory allocated for CommandLineToArgvW arguments.

    return Fl::run();


    //return 0;
    }
