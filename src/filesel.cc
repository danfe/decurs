/* example-start filesel filesel.cc */

#include "filesel.hh"
// #include <iostream>

//From <iostream> 
// using std::cout;
// using std::endl;

using SigC::slot;



filesel::filesel(const char* name,
            const char* fname,
            SigC::Slot0<void> callb):
    Gtk::FileSelection(name)
{
    // save thos whom shall we call
    callback = callb;

    set_filename(fname);
    /* Connect the ok_button_ to file_ok_sel function */
    get_ok_button()->signal_clicked().connect(slot(*this, &filesel::file_ok_sel));
    get_ok_button()->signal_clicked().connect(callb);
    /* Connect the cancel_button_ to hiding the window */
    get_cancel_button()->signal_clicked().connect(slot(*this, &filesel::hide));
    /* Connect hiding the window to exit the program */
    // hide.connect(Gtk::Main::quit.slot());
}

