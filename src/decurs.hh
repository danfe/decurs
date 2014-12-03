/* Time-stamp: <2001-12-15 23:31:05 pvsavola> */
#if !defined DECUR_HH
#define DECUR_HH

#include <gtkmm/main.h>
#include <gtkmm/style.h>
#include <gtkmm/window.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>

#include "bdf.hh"
#include "filesel.hh"
#include "config.h"

#define LCL_ADDY    64
#define LCL_ADDX    64
#define LCL_YW      9
#define LCL_XW      9

using namespace SigC;
using SigC::slot;

const char fname[] = "cursor.bdf";
const char sfname[] = "modif.bdf";

class DeCurDrawingArea  : public Gtk::DrawingArea
{
    /* Backing pixmap for drawing area */
    Glib::RefPtr<Gdk::Pixmap> backstore;
    // Glib::RefPtr<Gdk::Window> pixmap;
    
    Glib::RefPtr<Gdk::GC> gc, back_gc, black_gc, white_gc, red_gc;
    Glib::RefPtr<Gdk::Window> win;
    Glib::RefPtr<Gdk::Visual> visual;
    BDFGlyph *glEdit;
    BDFFont *font;  
    bool glsel; // is this "selection area"
    Signal1<void,int> sig;
    
    int click_at;

    virtual bool on_configure_event (GdkEventConfigure *event);
    virtual bool on_expose_event (GdkEventExpose *event);
    virtual bool on_button_press_event (GdkEventButton *event);
    virtual bool on_motion_notify_event (GdkEventMotion *event);
    virtual bool on_key_press_event (GdkEventKey *even);
    void draw_brush (gdouble x, gdouble y);
    void DrawName(int x, int y, bool clear=false);
    void DrawText(int x, int y, char *txt);
    void DrawGlyph(BDFGlyph *glyph, int at_x, int at_y, bool blow);
    void DrawSel();
    void DrawBack(int x, int y, int w, int h);
public:    
    void FullRepaint();
    void PartialRepaint(BDFGlyph *glyph, int x, int y, int at_x, int at_y, bool rep = true, bool blow=true);
    void SetGlyph(BDFGlyph *ngl);
    void SetFont(BDFFont *nfont) { font=nfont; };
    void erase();
    void SetGlyphSel(bool nglsel) { glsel = nglsel; };
    // void SetMasked(bool nmask) { masked=nmask; };

    void SetCallb(SigC::Slot1<void,int> callb_) { sig.connect(callb_); };
    int GetClickAt() { return click_at; };
    DeCurDrawingArea ();
    ~DeCurDrawingArea ();
};


class GlyphSelWindow : public Gtk::Window
{
  DeCurDrawingArea drawing_area;
  Gtk::VBox vbox;
  BDFFont *font;

public:
  void SetFont(BDFFont *font_);
  void SetCallb(SigC::Slot1<void,int> callb_) { drawing_area.SetCallb(callb_); };
  bool on_delete_event(GdkEventAny*) { hide(); return true;};
  GlyphSelWindow();
  ~GlyphSelWindow();
};


class DeCurWindow : public Gtk::Window
{
    
    DeCurDrawingArea drawing_area;
    Gtk::Button eraser;
    Gtk::Button button;
    Gtk::Toolbar toolbar;
    Gtk::VBox vbox;
    filesel *lfile, *sfile;
    BDFFont *font;
    GlyphSelWindow *glSel;
    bool FMasked;
    // void toolbar_button_cb(char*);
    // void toolbar_void_cb(void);
    void CmNextGlyph();
    void CmSelGlyph();
    void CmPrevGlyph();
    void CmMFont();
    void CmUMFont();
    void CmInvert();
    void CmLoadFont();
    void CmSaveFont();
    void CmQuit();
    void CmHelp();
    void CmHideHelp();
    void do_load_font();
    void do_save_font();
  
  void sel_glyph_click(int);
public: 
    DeCurWindow ();
    ~DeCurWindow ();
};



#endif // DECUR_HH
