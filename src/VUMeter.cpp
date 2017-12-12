#include "VUMeter.h"
#include "Capture.h"

/** VU METER **/
VUMeter::VUMeter(wxPanel *parent, int id, const wxPoint& pos, const wxSize& size)
      : wxPanel(parent, id, pos, size, wxSUNKEN_BORDER) {
 
  m_parent = parent;

  Connect(wxEVT_PAINT, wxPaintEventHandler(VUMeter::OnPaint));
  Connect(wxEVT_SIZE, wxSizeEventHandler(VUMeter::OnSize));

}

void VUMeter::OnPaint(wxPaintEvent& event) {
  wxPaintDC dc(this);
  wxSize size = GetSize();
  int width = size.GetWidth();


  dc.SetPen(wxPen(wxColour(255, 0, 0)));
  dc.SetBrush(wxBrush(wxColour(255, 0, 0)));
  dc.DrawRectangle(0, 0, width*volume, size.GetHeight());
}

void VUMeter::OnSize(wxSizeEvent& event) {
  Refresh();
}


void VUMeter::setVolume(float _volume) {
	volume = _volume;
	Refresh();
}


/** TITLE LOGO **/


Title::Title(wxPanel *parent, int id, const wxPoint& pos, const wxSize& size)
      : wxPanel(parent, id, pos, size, wxBORDER_NONE) {
 
  m_parent = parent;

  Connect(wxEVT_PAINT, wxPaintEventHandler(Title::OnPaint));

}

void Title::OnPaint(wxPaintEvent& event) {
  wxPaintDC dc(this);
  dc.SetPen(wxPen(wxColour(0, 0, 0)));
  dc.SetBrush(wxBrush(wxColour(0, 0, 0)));
  dc.DrawRectangle(0, 0, 280, 53);
  dc.DrawBitmap(*_img_capture, 0, 0);
}



/** RECORDING LIGHT **/
RecordingLight::RecordingLight(wxPanel *parent, int id, const wxPoint& pos, const wxSize& size)
      : wxPanel(parent, id, pos, size, wxSUNKEN_BORDER) {
 
  m_parent = parent;
  recording = false;
  Connect(wxEVT_PAINT, wxPaintEventHandler(RecordingLight::OnPaint));
  Connect(wxEVT_SIZE, wxSizeEventHandler(RecordingLight::OnSize));

}

void RecordingLight::OnPaint(wxPaintEvent& event) {
  wxPaintDC dc(this);
  wxSize size = GetSize();
  int width = size.GetWidth();


  if(recording) {
  	dc.SetPen(wxPen(wxColour(255, 0, 0)));
  	dc.SetBrush(wxBrush(wxColour(255, 0, 0)));
  } else {
  	dc.SetPen(wxPen(wxColour(255, 255, 255)));
  	dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
  }
  
  // draw background
  dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
  
  // then draw the icon
  if(recording) {
  	dc.DrawBitmap(*_img_recon, (size.GetWidth() - _img_recon->GetWidth())/2, (size.GetHeight() - _img_recon->GetHeight())/2);
  } else {
  	dc.DrawBitmap(*_img_recoff, (size.GetWidth() - _img_recoff->GetWidth())/2, (size.GetHeight() - _img_recoff->GetHeight())/2);
  }
}

void RecordingLight::OnSize(wxSizeEvent& event) {
  Refresh();
}


void RecordingLight::setRecording(bool _recording) {
	recording = _recording;
	Refresh();
}
