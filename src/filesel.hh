#if !defined PSI_FILESEL_H
#define PSI_FILESEL_H

#include <gtkmm/fileselection.h>
//#include <iostream>
//#include <gtk--/main.h>


class filesel: public Gtk::FileSelection
{
public:
    filesel(const char* name,
            const char* fname,
            SigC::Slot0<void> callb);
private:
    SigC::Slot0<void> callback;
    // SigC::Signal1<void> sig;
    /* Get the selected filename and print it to the console */
    void file_ok_sel() {
        hide();
        // cout << "file_ok_sel: " << get_filename() << endl;
    }
    
    bool on_delete_event(GdkEventAny*) {
        hide();
        return true;
        /* Gtk::Main::quit(); return 0; */
    }
    
};

#endif // PSI_FILESEL_H
