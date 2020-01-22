#include "pch.h"

#include "FileSynchronizer.h"

#include "FileLoader.h"

#include "Config.h"

#include "Util/Logger.h"

#include "Util/Timer.h"

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_BUTTON(COMPARE_BUTTON, Frame::OnCompareButtonClicked)
wxEND_EVENT_TABLE()

Frame::Frame()
	: wxFrame(nullptr, wxID_ANY, "File Synchronizer", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX)
{
	SetBackgroundColour(wxColor("white"));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* originalDirectory = new wxStaticText(this, wxID_ANY, "Original directory");
	wxStaticText* compareDirectory = new wxStaticText(this, wxID_ANY, "Compare directory");

	m_OriginalDirectoryPicker = new wxDirPickerCtrl(this, wxID_ANY, "", "Please select the original folder...");
	m_CompareDirectoryPicker = new wxDirPickerCtrl(this, wxID_ANY, "", "Please select a folder you want to synchronize...");

	wxBoxSizer* inputSizer = new wxBoxSizer(wxHORIZONTAL);
	
	m_CompareButton = new wxButton(this, COMPARE_BUTTON, "Synchronize");
	m_FastModeCheckBox = new wxCheckBox(this, wxID_ANY, "Fast mode");

	inputSizer->Add(m_CompareButton, 1, wxEXPAND | wxALL, 20);
	inputSizer->Add(m_FastModeCheckBox, 1, wxEXPAND | wxALL, 20);

	mainSizer->Add(originalDirectory, 1, wxEXPAND | wxLEFT, 1);
	mainSizer->Add(m_OriginalDirectoryPicker, 1, wxEXPAND | wxALL, 1);
	
	mainSizer->Add(compareDirectory, 1, wxEXPAND | wxLEFT, 1);
	mainSizer->Add(m_CompareDirectoryPicker, 1, wxEXPAND | wxALL, 1);

	mainSizer->Add(inputSizer);

	SetSizerAndFit(mainSizer);
	mainSizer->Layout();
}

void Frame::OnCompareButtonClicked(wxCommandEvent& event)
{
	std::string originalDirectoryPath(m_OriginalDirectoryPicker->GetDirName().GetPath().mb_str());
	std::string compareDirectoryPath(m_CompareDirectoryPicker->GetDirName().GetPath().mb_str());

	if (originalDirectoryPath.empty() || compareDirectoryPath.empty())
	{
		wxDialog* dialog = new wxDialog(this, wxID_ANY, "Error!", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

		dialog->Show();
	}
	else
	{
		Config::FastMode = m_FastModeCheckBox->GetValue();

		Folder original = FolderLoader::Load(originalDirectoryPath);
		Folder compare = FolderLoader::Load(compareDirectoryPath);

		Timer timer;

		compare.Synchronize(original);
	
		LOG_DEBUG("Synchronizing files took {0}ms.", timer.Stop());
	}

	event.Skip();
}

wxIMPLEMENT_APP(FileSynchronizer);

FileSynchronizer::FileSynchronizer()
	: m_Frame(new Frame)
{
}

bool FileSynchronizer::OnInit()
{
#ifdef _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
#endif

	m_Frame->Show();

	return true;
}