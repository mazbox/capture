#ifndef WIDGET_H
#define WIDGET_H

#include <wx/wx.h>

class Capture;

class VUMeter : public wxPanel {
public:
  VUMeter(wxPanel *parent, int id, const wxPoint& pos, const wxSize& size);

  wxPanel *m_parent;

  void setVolume(float volume);
  void OnSize(wxSizeEvent& event);
  void OnPaint(wxPaintEvent& event);  
  private:
  	float volume;
};


class Title : public wxPanel {
public:
  Title(wxPanel *parent, int id, const wxPoint& pos, const wxSize& size);

  wxPanel *m_parent;

  void OnPaint(wxPaintEvent& event);  

};



class RecordingLight : public wxPanel {
public:
  RecordingLight(wxPanel *parent, int id, const wxPoint& pos, const wxSize& size);

  wxPanel *m_parent;

  void setRecording(bool _recording);
  void OnSize(wxSizeEvent& event);
  void OnPaint(wxPaintEvent& event);  
  private:
  	bool recording;
};


#endif

