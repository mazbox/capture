#include "wx/wx.h"
#include "VUMeter.h"
#include "wx/tglbtn.h"
#include "Sampler.h"
#include "images.hpp"

enum {
	ID_Quit = 1,
	ID_About,
	ID_Save,
	ID_Load,
	ID_RecordMode,
	ID_AutoTrigger,
	ID_VUMeter,
	ID_Preset,
	ID_Threshold,
	ID_Legato
};

class Capture : public wxFrame {
	public:
		Capture(const wxString& title);
		~Capture();
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		
		
		void onSave(wxCommandEvent& event);
		void onLoad(wxCommandEvent& event);
		void onKeyDown(wxKeyEvent& event);
		void onKeyUp(wxKeyEvent& event);
		void onCharHook();
		void onToggleRecordMode(wxCommandEvent& event);
		void onToggleAutoTrigger(wxCommandEvent& event);
		void OnScroll(wxScrollEvent& event);
		void presetChanged(wxCommandEvent& event);
		void thresholdChanged(wxScrollEvent& event);
		void legatoToggled(wxCommandEvent& event);
		int GetCurWidth();
		void update();
  		bool load(std::string path);
  		void toggleRecordMode();
  		
		DECLARE_EVENT_TABLE()

		Sampler *sampler;
		VUMeter *m_wid;
		wxSlider *thresholdSlider;
		RecordingLight *recordingLight;
		Title *titleImg;
		wxBitmapButton* recordModeButton;
		wxBitmapButton* autotriggerButton;
		wxBitmapButton* saveButton;
		wxBitmapButton* loadButton;
		wxCheckBox* legatoButton;
		wxChoice* presets;
		wxArrayString *presetNames;
		wxArrayString *presetPaths;
		wxString *defaultPath;
		int cur_width;
		
		
	
};
