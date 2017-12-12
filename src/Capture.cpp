#include "Capture.h"
#include "wx/dir.h"

bool shutdowning = false;



#define WIDTH 250

// windows needs a little extra space 
// for the menu bar in the window
#ifdef __MACOSX_CORE__
	#define HEIGHT 270
#else
	#define HEIGHT 295
#endif

#define RECORDLIGHTWIDTH 50
#define VUHEIGHT 20
int ID_SLIDER = 1;
#define THRESHOLDMULTIPLIER (1000.f)


Capture::Capture(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WIDTH, HEIGHT), wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN) {
	
	sampler = new Sampler();
	initialize_images();
	
	
	int titleHeight = 53;
	int padding = 10;
	int buttonSize = 50;
	int presetPos = titleHeight+padding;
	int presetHeight = 30;
	int buttonY = presetPos + presetHeight + padding;
	
	
	wxPanel *panel = new wxPanel(this, wxID_ANY);
	titleImg = new Title(panel, wxID_ANY, wxPoint(-1, -1), wxSize(WIDTH , titleHeight));
	presetPaths = new wxArrayString();
	presetNames = new wxArrayString();
	
	presetNames->Add(_("Load Preset..."));
	presetNames->Add(_("   "));

	
	presets = new wxChoice(panel, ID_Preset,wxPoint(padding, presetPos), wxSize(WIDTH-2*padding, presetHeight), *presetNames);
	presets->SetSelection(1);	
	loadButton = new wxBitmapButton(panel, ID_Load, *_img_open, wxPoint(padding, buttonY), wxSize(buttonSize,buttonSize));	
	saveButton = new wxBitmapButton(panel, ID_Save, *_img_save, wxPoint(padding*2+buttonSize, buttonY),wxSize(buttonSize,buttonSize));
	recordModeButton = new wxBitmapButton(panel, ID_RecordMode, *_img_playmode, wxPoint(padding*3+buttonSize*2, buttonY), wxSize(buttonSize,buttonSize));
	autotriggerButton = new wxBitmapButton(panel, ID_AutoTrigger, *_img_auto, wxPoint(padding*4+buttonSize*3, buttonY),  wxSize(buttonSize,buttonSize));
	
	
	thresholdSlider = new wxSlider(panel, ID_Threshold, 0 , 0, 100, wxPoint(padding, buttonY+buttonSize+padding+VUHEIGHT+padding), wxSize(WIDTH - padding*2, 25), wxSL_HORIZONTAL, wxDefaultValidator, _("Threshold"));
	thresholdSlider->SetValue(round(sampler->getTriggerLevel()*THRESHOLDMULTIPLIER));
	thresholdSlider->SetLabel(_("Autotrigger Level"));
	autotriggerButton->Disable();
	thresholdSlider->Disable();
	
	legatoButton = new wxCheckBox(panel, ID_Legato, _("Legato Mode"), wxPoint(padding, buttonY+buttonSize+padding+VUHEIGHT+padding+10+padding+padding));
	legatoButton->SetValue(sampler->getLegatoMode());
	/*
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);
	*/
	m_wid = new VUMeter(panel, wxID_ANY, wxPoint(padding, buttonY+buttonSize+padding), wxSize(WIDTH - (padding*3 + RECORDLIGHTWIDTH), VUHEIGHT));
	recordingLight = new RecordingLight(panel, wxID_ANY, wxPoint(WIDTH - (padding+RECORDLIGHTWIDTH), buttonY+buttonSize+padding), wxSize(RECORDLIGHTWIDTH, VUHEIGHT));
	
	//hbox->Add(m_wid, 1, wxEXPAND);
	/*
	hbox2->Add(loadButton, 1, wxEXPAND);
	hbox2->Add(saveButton, 1, wxEXPAND);
	hbox2->Add(quitButton, 1, wxEXPAND);
	hbox2->Add(recordModeButton, 1, wxEXPAND);
	hbox2->Add(autotriggerButton, 1, wxEXPAND);
	*/

	
	
	
	
	
	//centerPanel->SetSizer(hbox3);
	
	//vbox->Add(hbox2, 1, wxEXPAND);
	//vbox->Add(hbox, 0, wxEXPAND);

	//panel->SetSizer(vbox);
	
	Centre();
	
	
	wxMenu *menuFile = new wxMenu;
	
	menuFile->Append( ID_About, _T("&About...") );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Load, _T("&Open...") );
	menuFile->Append( ID_Save, _T("&Save...") );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, _T("E&xit") );
	
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _T("&File") );
	
	SetMenuBar( menuBar );
	
	printf("Finished initing gui\n");
	
	
}


void Capture::update() {
	if(!shutdowning) {
		if(sampler && m_wid) m_wid->setVolume(sampler->getVolume());
		if(sampler && recordingLight) recordingLight->setRecording(sampler->getRecording());
	}
	
}

Capture::~Capture() {
	shutdowning = true;
};




BEGIN_EVENT_TABLE(Capture, wxFrame)

//
// These should be triggered at app level rather than frame level.
// I don't know if they are triggering from here or not?
EVT_KEY_DOWN(Capture::onKeyDown)
EVT_KEY_UP(Capture::onKeyUp)


EVT_MENU(ID_Quit, Capture::OnQuit)
EVT_MENU(ID_Load, Capture::onLoad)
EVT_MENU(ID_Save, Capture::onSave)
EVT_BUTTON(ID_Quit, Capture::OnQuit)

EVT_MENU(ID_About, Capture::OnAbout)



EVT_BUTTON(ID_Load, Capture::onLoad)
EVT_BUTTON(ID_Save, Capture::onSave)
EVT_BUTTON(ID_RecordMode, Capture::onToggleRecordMode)
EVT_BUTTON(ID_AutoTrigger, Capture::onToggleAutoTrigger)
EVT_CHOICE(ID_Preset, Capture::presetChanged)
EVT_COMMAND_SCROLL(ID_Threshold, Capture::thresholdChanged)
EVT_CHECKBOX(ID_Legato, Capture::legatoToggled)
END_EVENT_TABLE()













bool recmode = false;
bool autotrigger = false;
void Capture::onToggleRecordMode(wxCommandEvent& event) {
	toggleRecordMode();
}

void Capture::onToggleAutoTrigger(wxCommandEvent& event) {
	SetFocus();
	autotrigger ^= 1;
	sampler->setAutoTrigger(autotrigger);
	thresholdSlider->Enable(autotrigger);
	if(autotrigger) autotriggerButton->SetBitmapLabel(*_img_auto_over);
	else autotriggerButton->SetBitmapLabel(*_img_auto);
}

void Capture::onSave(wxCommandEvent& event) {

	// save button
	wxFileDialog *dlg = new wxFileDialog(this, (const wxString)_("Save file"), _(""), _(""), _("DLS files (*.dls)|*.dls|GIG files (*.gig)|*.gig"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
	if(dlg->ShowModal()==wxID_OK) {
		if(!sampler->save(std::string(dlg->GetPath().mb_str()))) {
			wxMessageBox(_("There was an error saving the file"),
	_T("Error"), wxOK | wxICON_ERROR, this);
		}
		
		
		
	}
	SetFocus();
}

bool Capture::load(std::string path) {
	
	if(!sampler->load(path)) {
			wxMessageBox(_T("Invalid file. Perhaps this wasn't created with Capture."),
			_T("Error"), wxOK | wxICON_ERROR, this);
			return false;
	}
	return true;
}

void Capture::presetChanged(wxCommandEvent& event) {
	if(presets->GetCurrentSelection()==0) onLoad(event);
	else if(presets->GetCurrentSelection()>1) {
		load(std::string(presetPaths->Item(presets->GetCurrentSelection()-2).mb_str()));
	}
}

void Capture::thresholdChanged(wxScrollEvent& event) {
	float val = thresholdSlider->GetValue();
	sampler->setTriggerLevel(val/THRESHOLDMULTIPLIER);
	thresholdSlider->SetValue(round(sampler->getTriggerLevel()*THRESHOLDMULTIPLIER));
	SetFocus();
}

void Capture::legatoToggled(wxCommandEvent& event) {
	sampler->setLegatoMode(legatoButton->GetValue());
	SetFocus();
}

void Capture::onLoad(wxCommandEvent& event) {
	if(recmode) toggleRecordMode();
	wxFileDialog *dlg = new wxFileDialog(this, (const wxString)_("Choose a file"), _(""), _(""), _("DLS and GIG files (*.dls;*.gif)|*.dls;*.gif|DLS files (*.dls)|*.dls|GIG files (*.gig)|*.gig"));
	if(dlg->ShowModal()==wxID_OK) {
		load(std::string(dlg->GetPath().mb_str()));
		/*if(!sampler->load(std::string(dlg->GetPath().mb_str()))) {
			wxMessageBox(_T("Invalid file. Perhaps this wasn't created with Capture."),
	_T("Error"), wxOK | wxICON_ERROR, this);
		}*/
		wxString dirPath = dlg->GetPath().BeforeLast('/');
		wxDir *dir = new wxDir(dirPath);

		presetPaths->Clear();
		presetNames->Clear();

		presetNames->Add(_("Load Preset..."));
		presetNames->Add(_("   "));
		wxArrayString *_presetPaths = new wxArrayString();
		dir->GetAllFiles(dirPath, _presetPaths, _(""));
		int presetIndex = 0;
		int badFileCount = 0;
		for(size_t i = 0; i < _presetPaths->GetCount(); i++) {
			wxString pre = _presetPaths->Item(i);
			wxString ext = pre.AfterLast('.');
			if(ext==_("dls") || ext==_("gig")) {
				presetNames->Add(pre.AfterLast('/').BeforeLast('.'));
				presetPaths->Add(pre);
				if(dlg->GetPath()==pre) {
					presetIndex = i+2-badFileCount;
				}
			} else {
				badFileCount++;
			}
		}
		
		presets->Clear();
		presets->Append(*presetNames);
		if(presetIndex>0) {
			presets->SetSelection(presetIndex);
		}
		SetFocus();
	}
	
	// load the preset list with all the presets in this folder
	
}

int keys[] = {
	96,
	90,
	88,
	67,
	86,
	66,
	78,
	77,
	44,
	46,
	47,
	50,
	
	65,
	83,
	68,
	70,
	71,
	72,
	74,
	75,
	76,
	59,
	39,
	92,

	81,
	87,
	69,
	82,
	84,
	89,
	85,
	73,
	79,
	80,
	91,
	93,

	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	48,
	45,
	61
};

// converts keys to midi numbers. 
// each row is an octave
// the lowest (`zxcvb...) is the lowest in pitch
// the highest (1234567...) is highest in pitch
int mapQwertyToMidi(char qwertyIn) {
	for(int i = 0; i < 48; i++) {
		if(((int)qwertyIn)==keys[i]) {
			return i+24;
		}
	}
	return qwertyIn - 12;
}

void Capture::toggleRecordMode() {
	recmode ^= 1;
	sampler->setRecordMode(recmode);
	autotriggerButton->Enable(recmode);
	if(recmode) recordModeButton->SetBitmapLabel(*_img_recmode);
	else recordModeButton->SetBitmapLabel(*_img_playmode);
}

void Capture::onKeyDown(wxKeyEvent& event) {
	// 32 = space
	// 33 - 126 is keys
	if(event.m_keyCode==32) {// space
		toggleRecordMode();
	} else {// send to sampler
		sampler->notePressed(mapQwertyToMidi(event.m_keyCode), 100);
	}
}

void Capture::onKeyUp(wxKeyEvent& event) {

	if(event.m_keyCode!=32) {
		sampler->noteReleased(mapQwertyToMidi(event.m_keyCode));
	}
}




void Capture::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	shutdowning = true;
	Close(TRUE);

// oooh evil, but it will have to do now because the
// above line isn't working in windows yet.
#ifdef __WXMSW__
	exit(0);
#endif
}


void Capture::OnAbout(wxCommandEvent& WXUNUSED(event)) {
	wxMessageBox(_T("The democratic sampler"),
	_T("About Capture"), wxOK | wxICON_INFORMATION, this);
}








