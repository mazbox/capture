





#include "wx/wx.h"
#include "Capture.h"
//#include <Carbon/Carbon.h>


// wx Stuff                         
class MyApp: public wxApp, wxTimer {
	public:
		virtual bool OnInit();
		virtual int OnExit();
		void Notify();
		Capture *tron;
		bool ProcessEvent(wxEvent& event) {
			if(event.GetEventType()==wxEVT_KEY_DOWN) {
				tron->onKeyDown((wxKeyEvent&)event);
				//printf("App key downed\n");
			} else if(event.GetEventType()==wxEVT_KEY_UP) {
				tron->onKeyUp((wxKeyEvent&)event);
			} else {
				event.Skip();
				return false;
			}
			return true;
		};
		virtual void MacOpenFile(const wxString &fileName);
};

void MyApp::MacOpenFile(const wxString &fileName) {
	tron->load(std::string(fileName.mb_str()));
}
		
bool MyApp::OnInit() {
	wxImage::AddHandler(new wxPNGHandler);
	tron = new Capture(wxT("Capture"));
    tron->Show(true);
	SetTopWindow(tron);
	//tron->SetFocus();
	
	Start(10);
	
	
	
	
	
	
	
	
	
	return TRUE;
	
	
	
	
	
	
} 

void MyApp::Notify() {
	tron->update();
}

int MyApp::OnExit() {
	printf("MyApp::OnExit()\n");
	Stop();
	
}













IMPLEMENT_APP(MyApp)


