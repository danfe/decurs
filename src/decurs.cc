/* Time-stamp: "2001-12-16 01:43:24 pvsavola" */
#include "decurs.hh"
#include <gtkmm/image.h>
#include <gtkmm/dialog.h>
#include <gtkmm/stock.h>
#include <iostream>

using namespace std;

char *g_back_color = 0;

void print_trace (void);

//=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=
///
// DeCurDrawingArea
//

DeCurDrawingArea::DeCurDrawingArea()
    : Gtk::DrawingArea(), glEdit(0), font(0), glsel(false)
{
    set_events (Gdk::EXPOSURE_MASK
		| Gdk::LEAVE_NOTIFY_MASK
		| Gdk::BUTTON_PRESS_MASK
		| Gdk::POINTER_MOTION_MASK
		| Gdk::KEY_PRESS_MASK
		| Gdk::POINTER_MOTION_HINT_MASK);
    Gtk::Widget::set_flags(Gtk::CAN_FOCUS);
}
DeCurDrawingArea::~DeCurDrawingArea()
{
}
/* Create a new backing pixmap of the appropriate size */
bool DeCurDrawingArea::on_configure_event(GdkEventConfigure * /* event */)
{
    // std::cout << "on_configure_event\n";
    // ::on_configure_event(event);
    if(backstore)
	backstore.clear();
    backstore = Gdk::Pixmap::create(get_window(), get_width(), 
				    get_height(), get_window()->get_depth());

    // Gdk::Color back(g_back_color);
    // get_colormap()->alloc_color(back);
    Glib::RefPtr<Gdk::Colormap> cm = get_default_colormap ();
    Glib::RefPtr<Gdk::Window> pixmap = get_window();
    
    {
	/// back
	back_gc = Gdk::GC::create(pixmap);
	Gdk::Color back(g_back_color);
	cm->alloc_color(back);
	back_gc->set_foreground(back);
	/// black
	black_gc = Gdk::GC::create(pixmap);
	Gdk::Color black("black");
	cm->alloc_color(black);
	black_gc->set_foreground(black);
	// white
	white_gc = Gdk::GC::create(pixmap);
	Gdk::Color white("white");
	cm->alloc_color(white);
	white_gc->set_foreground(white);
	// red
	red_gc = Gdk::GC::create(pixmap);
	Gdk::Color red("red");
	cm->alloc_color(red);
	red_gc->set_foreground(red);        
    }
    backstore->draw_rectangle(back_gc, true, 0, 0, 
			      get_width(), get_height()); 
    FullRepaint();
    return true;
}
/* Redraw the screen from the backing pixmap */
bool DeCurDrawingArea::on_expose_event(GdkEventExpose *event)
{
    DrawBack( event->area.x, event->area.y,
	      event->area.width, event->area.height);
    return false;
}
void DeCurDrawingArea::DrawBack(int x, int y, int w, int h)
{
    gc = get_style()->get_fg_gc(get_state());
    get_window()->draw_drawable(gc,
				backstore,
				x, y,
				x, y,
				w, h);
    return;
}

bool DeCurDrawingArea::on_button_press_event (GdkEventButton *event)
{
    int x = (int) event->x, y = (int) event->y;
    /*
      if(event->is_hint)
    window.get_pointer(x,y,state);
  */
  /*
    if (event->button == 1 && pixmap)
    draw_brush (event->x, event->y);
  */
    x-=LCL_ADDX; x/=LCL_XW;
    y-=LCL_ADDY; y/=LCL_YW;
    
    if((event->button == 3) && glEdit)
    { // right button..
	if(event->state & Gdk::SHIFT_MASK)
	    glEdit->SetBDFPixel(x, y,BDFGlyph::Transparent);
	else
	    glEdit->SetBDFPixel(x, y, BDFGlyph::Black);
    } else if(glEdit && !glsel) {
	if(event->state & Gdk::CONTROL_MASK) {
	    glEdit->SetOrigAt(x, y);
	    FullRepaint();
	    return true;
	} else if(event->state & Gdk::SHIFT_MASK)
	    glEdit->SetBDFPixel(x, y,BDFGlyph::Transparent);
	else
	    glEdit->SetBDFPixel(x, y,BDFGlyph::White);
    } else if(glsel) {
	x = (int) event->x; y = (int) event->y;
	x /= LCL_ADDX; y /= LCL_ADDY;
	y *= get_width() / LCL_ADDX;
	click_at = y + x;
	
	//      sig.connect(callb);
	sig.emit(click_at);
	// cout << "Got click at: " << y << " x: " << x << endl;
	return true;
    }
    PartialRepaint(glEdit, x, y, 0, 0);
    return true;
}
bool DeCurDrawingArea::on_key_press_event(GdkEventKey *event)
{
    int key = event->keyval;
    if(key == 'b' || key == 'B') {
	FontBB bb; int Xoff, Yoff;
	glEdit->CalculateBB(bb, Xoff, Yoff);
	CppString str;
	str.format("width(%d), height(%d),"
		   " \n\tMinX(%d), MinY(%d),"
	       " \n\tOrigoX(%d),  OrigoY(%d)",
		   bb.w, bb.h, bb.ox, bb.oy, Xoff, Yoff);
	std::cout << "Glyph dimensions:\n" << str << std::endl;
	// MessageBox(str, "Glyph dimensions");
    } else if(key=='r') {
	FullRepaint();
    }
    return true;
}



bool DeCurDrawingArea::on_motion_notify_event(GdkEventMotion *event)
{
    int x, y;
    Gdk::ModifierType state;
    if (event->is_hint)
        get_window()->get_pointer (x, y, state);
    else {
        x = (int)event->x;
        y = (int)event->y;
        state = (Gdk::ModifierType) event->state;
    }
    
    if (state & (Gdk::BUTTON1_MASK
		 |Gdk::BUTTON2_MASK
		 |Gdk::BUTTON3_MASK) && glEdit) {
        GdkEventButton buti;
        buti.x = x;
        buti.y = y;
        buti.state = state;
	buti.button = (state & Gdk::BUTTON1_MASK) ? 1 : 3;
        return on_button_press_event(&buti);
    } else if(glsel) {
	// here we can make hot-trails..
	return true;
    } else
	return true;
}
void DeCurDrawingArea::erase()
{     
    // clear pixmap area to white.
    gc = get_style()->get_white_gc();
    backstore->draw_rectangle (gc, true, 0, 0, 
			       get_width(), get_height());
    queue_draw();
}
void DeCurDrawingArea::PartialRepaint(BDFGlyph *glyph, int x, int y, int at_x, int at_y, bool repaint, bool blow)
{
    // std::cout << "Partial Repaint (" << x << ',' << y << "\n";
    if (!glyph) return;
    if (x < 0 || y < 0 ||
	x > 63|| y > 63)
        return;
    /*
    if(glsel)
      {
      // FullRepaint();
      return;
      
      }*/
    GdkRectangle update_rect;
    
    update_rect.x = LCL_ADDX+x*LCL_XW;
    update_rect.y = LCL_ADDY+y*LCL_YW;
    update_rect.width = 8;
    update_rect.height = 8;

    Glib::RefPtr<Gdk::GC> *col=0;
    if(glyph->Pixel(x,y) == white) {
        col = &white_gc;
    } else if(glyph->Pixel (x,y) == black) {
        col = &black_gc;
    } else { // transparent..
        col = &back_gc;
    }
    // draw one dot at x,y
    backstore->draw_point(*col, x + at_x, y + at_y);
    // make a 'enlargement' somewhere.
    if(blow)
	backstore->draw_rectangle(*col, true,
				  update_rect.x, update_rect.y,
				  8, 8);
    if(repaint) // are we requested not to paint it?
    {
        // update the "big pixel"
        queue_draw_area (
			 LCL_ADDX+x*LCL_XW,LCL_ADDY+y*LCL_YW,
			 8, 8);// (&update_rect);
        // and now update "preview"
	queue_draw_area(0, 0, 64, 64);
    }    
}
void DeCurDrawingArea::DrawName(int x, int y, bool clear)
{
    // std::cout << "Drawing Name\n";
    if(clear)
    {
	// draw over glyph's old name.
	backstore->draw_rectangle(back_gc, true,
			       LCL_ADDX, 0,
			       200, LCL_ADDY);
    }
    // draw glyphs name.
    
    Glib::RefPtr<Pango::Context> con=get_pango_context();
    Glib::RefPtr<Pango::Layout> lo =  Pango::Layout::create(con);
    lo->set_text(glEdit->glyph_name.c_str());
    backstore->draw_layout( black_gc,
			    x, y, lo);
}
void DeCurDrawingArea::DrawText(int x, int y, char *txt)
{
    Glib::RefPtr<Pango::Context> con=get_pango_context();
    Glib::RefPtr<Pango::Layout> lo =  Pango::Layout::create(con);
    lo->set_text(txt);
    backstore->draw_layout( black_gc,
			    x, y, lo);
}
void DeCurDrawingArea::DrawGlyph(BDFGlyph *glyph, int at_x, int at_y,
				 bool blow)
{
    // draw the glyph
    int x, y;    
    for(y = 0; y < glyph->Height(); y++)
    {
        // draw one row
        for(x = 0; x < glyph->Width(); x++)
            PartialRepaint(glyph, x, y, at_x, at_y, false, blow);
	if(blow)
	{
	    // clean "line between" rows
	    // the vertical line
	    
	    backstore->draw_line(back_gc,
				 LCL_ADDX + (y * LCL_XW) - 1, LCL_ADDY,
				 LCL_ADDX + (y * LCL_XW) - 1 , LCL_ADDY + (64 * LCL_YW));
	    // the horizontal line.
	    backstore->draw_line(back_gc,
			     LCL_ADDX, LCL_ADDY + (y * LCL_YW) - 1,
			     LCL_ADDX + (64 * LCL_XW), LCL_ADDY + (y * LCL_YW) - 1);
	    
	}
    }
}

void DeCurDrawingArea::DrawSel()
{
    // std::cout << "Drawing selection list\n";
    int i,j=0; int w = get_width(), h = get_height();
    TGlyphList alist = *font->glyphs;
    alist.First();
    while(1)
    {
	if( (j*64) > h) 
	    break;
	for(i = 0; i < w/64; i++)
	{
	    // cout << "drawing next glyph\n";
	    if(alist.Last() && font->GetMasked())
		break; // never draw last glyph when masked.
	    DrawGlyph( &alist.Current(), i*64, j * 64, false);
	    if(alist.Last())
		break;
	    else
		alist.Next();
	    if(font->GetMasked())
		alist.Next();
	}
	j++;
	// break one more time.
	if(alist.Last())
	    break;
    }
    // DrawGlyph(glEdit, 0, 0, false);
    queue_draw();
}
void DeCurDrawingArea::FullRepaint()
{
    if(!backstore) {
	// print_trace();
	return;
    }
    // cout << "fullrepaint\n";
    
    if(!glEdit)
    {
        // clear background.
        backstore->draw_rectangle(back_gc, true, 
				  0, 0, get_width(), get_height());
	
	DrawText(LCL_ADDX + 16, 16, "No font loaded.");
        return;
    }
    backstore->draw_rectangle (back_gc, true, 
			    0, 0, get_width(), get_height());
    if(glsel)
    {	
	DrawSel();
	return;
    }
    DrawName(LCL_ADDX + 16, 16, true);
    //                x  y  blow
    DrawGlyph(glEdit, 0, 0, true);
    // draw origo (hot-spot)
    int ox = glEdit->orig_atx, oy=glEdit->orig_aty;
    // vertical
    backstore->draw_line(red_gc,
		      LCL_ADDX + ox * LCL_XW - 1, LCL_ADDY,
		      LCL_ADDX + ox * LCL_XW - 1, LCL_ADDY + 63 * LCL_YW);
    // horiz.
    backstore->draw_line(red_gc,
		      LCL_ADDX, LCL_ADDY + oy * LCL_YW - 1,
		      LCL_ADDX + 64 * LCL_XW, LCL_ADDY + oy * LCL_YW - 1);
    // draw box around "preview"
    // verical (on right)
    backstore->draw_line(black_gc,
		      LCL_ADDX, 0,
		      LCL_ADDX, LCL_ADDY);
    // horizontal (on bottom)
    backstore->draw_line(black_gc,
                     0, LCL_ADDY,
                     LCL_ADDX, LCL_ADDY);
    queue_draw();
}

void DeCurDrawingArea::SetGlyph(BDFGlyph *ngl)
{
    glEdit=ngl;
    FullRepaint();
}

//=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/=/
//
// DeCurWindow
//

void DeCurWindow::CmSaveFont()
{
    if(sfile)
	sfile->show();
}
void DeCurWindow::CmLoadFont()
{
    if(lfile)
	lfile->show();
}

void DeCurWindow::CmInvert()
{
    font->glyphs->Current().Invert();
    drawing_area.SetGlyph(&font->glyphs->Current());
}
void DeCurWindow::CmMFont()
{
    if(FMasked)
        std::cout << "Font was already masked!\n";
    FMasked = true;
    font->MaskFont();
    if(glSel) 
    {
	//	glSel->SetMasked(true);	
	glSel->SetFont(font);  // trigger repainting.
    }
    drawing_area.SetGlyph(&font->glyphs->Current());
}
void DeCurWindow::CmUMFont()
{
    if(!FMasked)
        std::cout << "Font was already unmasked!\n";
    FMasked = false;
    font->UnMaskFont();
    if(glSel)
    {
	// glSel->SetMasked(false);
	// this triggers repainting.
	glSel->SetFont(font);
    }
    
    drawing_area.FullRepaint();
    // drawing_area.SetGlyph(&font->glyphs->Current());
}
void DeCurWindow::CmNextGlyph()
{
    if(FMasked) // skip the mask..
        font->glyphs->Next();
    font->glyphs->Next();
    drawing_area.SetGlyph(&font->glyphs->Current());
}
void DeCurWindow::CmPrevGlyph()
{
    if(FMasked) // skip the mask..
        font->glyphs->Prev();
    font->glyphs->Prev();
    drawing_area.SetGlyph(&font->glyphs->Current());
}


void DeCurWindow::CmSelGlyph()
{
    if(glSel) {
	glSel->SetFont(font);
	glSel->show();
	return;
    } 
    glSel = new GlyphSelWindow();
    
    if(!glSel)      
      return;
    glSel->SetCallb(slot(*this,&DeCurWindow::sel_glyph_click));
    glSel->SetFont(font);
    glSel->show();
}
Gtk::Dialog *help_dial=0;
void DeCurWindow::CmHideHelp()
{
    if(help_dial)
        help_dial->hide();
}
void DeCurWindow::CmHelp()
{
    if(!help_dial)
        help_dial = new Gtk::Dialog;
    else {
	    help_dial->show();
	    return;
    }    
    if(!help_dial)
        return;
    Gtk::Dialog &dial = *help_dial;
    Gtk::Label *lab=new Gtk::Label("Commands available:\n"
                   "Left Mouse -- white\n"
                   "Right Mouse -- black\n"
                   "Shift + Left Mouse -- 'transparent'\n"
                   "Control + Left Mouse -- Move 'HotSpot'\n"
                   "'B' -- prints some info about glyph\n"
                   "being edited. Hit TAB several times to\n"
                   "make it work (not my fault)");
    Gtk::Button *butt = new Gtk::Button("Enough");
    Gtk::HButtonBox *hbox = dial.get_action_area();
    Gtk::VBox *vbox = dial.get_vbox();
    hbox->pack_start (*butt, true, true, 0);
    butt->signal_clicked().connect(slot(*this, &DeCurWindow::CmHideHelp));
    butt->show();
    vbox->pack_start (*lab, true, true, 0);
    lab->show();
    dial.show();
}
void DeCurWindow::do_load_font()
{
    // destroy it, so it's cleaner.
    if(glSel) glSel->SetFont(0);
    if(font)
        delete font;
    font = new BDFFont();
    FMasked = false;    
    if(!font->LoadFont(lfile->get_filename().c_str(), &std::cout))
        drawing_area.SetGlyph(0);
    else
        drawing_area.SetGlyph(&font->glyphs->Current());

    if(glSel)
      glSel->SetFont(font);
}
void DeCurWindow::do_save_font()
{
    // unmask as needed.
    if(FMasked)
        font->UnMaskFont();
        
    font->SaveFont(sfile->get_filename().c_str());
    if(FMasked)
      font->MaskFont();
    drawing_area.FullRepaint();
}

void DeCurWindow::sel_glyph_click(int at_cl)
{
    if(!font) return;
    if(font->glyphs->Elements() < at_cl) return;
    if(FMasked && (font->glyphs->Elements()/2) < at_cl) return;
    font->glyphs->First();
    while(at_cl--)
    {
	font->glyphs->Next();
	if(FMasked)
	    font->glyphs->Next();
    }
    drawing_area.SetGlyph(&font->glyphs->Current());
}

DeCurWindow::DeCurWindow ()
  :  Gtk::Window(Gtk::WINDOW_TOPLEVEL),
            eraser("erase"),
            button ("quit"),
            vbox (false, 0),
            font(0),
            FMasked(false)
{
    glSel=0;
    add (vbox);
    /* Create the drawing area */
    drawing_area.set_size_request (650, 670);
    vbox.pack_start (toolbar, false, false, 0);
    vbox.pack_start (drawing_area, true, true, 0);
    
    lfile = new filesel("Load Font", "cursor.bdf",
                        slot(*this, &DeCurWindow::do_load_font));
    sfile = new filesel("Save Font", "modified.bdf",
                        slot(*this, &DeCurWindow::do_save_font));
                        
    
    /* Add the buttons */
    
    {
        using namespace Gtk::Toolbar_Helpers;
	toolbar.tools().push_back(StockElem( Gtk::Stock::GO_BACK, 
             slot(*this,&DeCurWindow::CmPrevGlyph),
             "Previous Glyph",""));
	toolbar.tools().push_back(StockElem( Gtk::Stock::INDEX,
             slot(*this,&DeCurWindow::CmSelGlyph),
             "Select Glyph",""));
        toolbar.tools().push_back(StockElem( Gtk::Stock::GO_FORWARD,
             slot(*this,&DeCurWindow::CmNextGlyph),
             "Next Glyph",""));
        
        toolbar.tools().push_back(Space());
        
        toolbar.tools().push_back(ButtonElem( "Mask", 
             slot(*this,&DeCurWindow::CmMFont),
             "Mask Glyphs",""));

        toolbar.tools().push_back(ButtonElem( "UnMask", 
             slot(*this,&DeCurWindow::CmUMFont),
             "Invert Glyphs",""));

        toolbar.tools().push_back(Space());

        toolbar.tools().push_back(ButtonElem( "Invert", 
             slot(*this,&DeCurWindow::CmInvert),
             "Invert Glyph",""));

        toolbar.tools().push_back(StockElem( Gtk::Stock::CLEAR,
             slot(drawing_area,&DeCurDrawingArea::FullRepaint),
             "Redraw window",""));
        
        toolbar.tools().push_back(Space());

        toolbar.tools().push_back(StockElem( Gtk::Stock::OPEN, 
             slot(*this,&DeCurWindow::CmLoadFont),
             "Load Font",""));

        toolbar.tools().push_back(StockElem( Gtk::Stock::SAVE,
             slot(*this,&DeCurWindow::CmSaveFont),
              "Save Font",""));

        toolbar.tools().push_back(Space());

        toolbar.tools().push_back(StockElem( Gtk::Stock::HELP,
             slot(*this,&DeCurWindow::CmHelp),
             "Help",""));
        
        toolbar.tools().push_back(Space());
        
        toolbar.tools().push_back(
            StockElem(Gtk::Stock::QUIT , Gtk::Main::quit));
    }

    toolbar.show();
    drawing_area.show();
    eraser.show();
    button.show();
    vbox.show();
    
    font = new BDFFont();
    if(!font->LoadFont(fname, &std::cout))
        std::cout << "Failed loading font\n";
    else
        drawing_area.SetGlyph(&font->glyphs->Current());
}
DeCurWindow::~DeCurWindow()
{
}
GlyphSelWindow::GlyphSelWindow()
    : vbox(false, 0)
{
    add(vbox);
    drawing_area.SetFont(0);
    drawing_area.SetGlyphSel(true);
    drawing_area.set_size_request(300, 200);
    vbox.pack_start(drawing_area, true, true, 0);
    drawing_area.show();
    vbox.show();
}
GlyphSelWindow::~GlyphSelWindow()
{
}
void GlyphSelWindow::SetFont(BDFFont *font_)
{
    font=font_;  
    drawing_area.SetFont(font);
    if(!font) 
	return; // can't do following..
    drawing_area.SetGlyph(&font->glyphs->Current());
}

int main (int argc, char *argv[])
{
    if (argc > 1) {
	// use first parameter as color.. Lame, I know..
	g_back_color = strdup(argv[1]);
    } else {
	g_back_color = "CadetBlue3";
    }
    Gtk::Main myapp(argc, argv);
    DeCurWindow window;
    window.show();
    myapp.run();
    return 0;
}
#if 0

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

void print_trace (void)
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i;
    
    size = backtrace (array, 10);
    strings = backtrace_symbols (array, size);
    
    printf ("Obtained %zd stack frames.\n", size);
    
    for (i = 0; i < size; i++)
	printf ("%s\n", strings[i]);
    
    free (strings);
}
#endif // trace..
