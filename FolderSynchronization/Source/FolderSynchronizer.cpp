#include "pch.h"

#include "FolderSynchronizer.h"

#include "Config.h"

std::unique_ptr<Folder> Frame::s_Reference = nullptr;
std::unique_ptr<Folder> Frame::s_Folder = nullptr;

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_BUTTON(COMPARE_BUTTON, Frame::OnCompareButton)
	EVT_BUTTON(SYNCHRONIZE_BUTTON, Frame::OnSynchronizeButton)
	
	EVT_DIRPICKER_CHANGED(REFERENCE_DIRECTORY_PICKER, Frame::OnReferenceDirectoryPicker)
	EVT_DIRPICKER_CHANGED(DIRECTORY_PICKER, Frame::OnDirectoryPicker)

	EVT_CHECKBOX(FAST_MODE, Frame::OnFastMode)
wxEND_EVENT_TABLE()

Frame::Frame()
	: wxFrame(nullptr, wxID_ANY, "Folder Synchronizer", wxDefaultPosition, wxSize(1280, 720))
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

	wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mainPanel->SetBackgroundColour(wxColour(245, 245, 245));

	wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* menuPanel = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
	menuPanel->SetForegroundColour(wxColour(78, 126, 146));
	menuPanel->SetBackgroundColour(wxColour(78, 126, 146));

	wxBoxSizer* menuSizer = new wxBoxSizer(wxHORIZONTAL);

	m_CompareButton = new wxButton(menuPanel, COMPARE_BUTTON, wxT("Compare"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_CompareButton->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false, wxT("Ubuntu Light")));
	m_CompareButton->SetForegroundColour(wxColour(255, 255, 255));
	m_CompareButton->SetBackgroundColour(wxColour(78, 126, 146));
	menuSizer->Add(m_CompareButton, 0, wxALL, 10);

	m_SynchronizeButton = new wxButton(menuPanel, SYNCHRONIZE_BUTTON, wxT("Synchronize"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_SynchronizeButton->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false, wxT("Ubuntu Light")));
	m_SynchronizeButton->SetForegroundColour(wxColour(255, 255, 255));
	m_SynchronizeButton->SetBackgroundColour(wxColour(78, 126, 146));
	menuSizer->Add(m_SynchronizeButton, 0, wxALL, 10);

	menuPanel->SetSizer(menuSizer);
	menuPanel->Layout();

	menuSizer->Fit(menuPanel);

	panelSizer->Add(menuPanel, 0, wxALL | wxEXPAND, 0);

	wxBoxSizer* mainAreaSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* referenceDirectorySizer = new wxBoxSizer(wxVERTICAL);
	referenceDirectorySizer->SetMinSize(wxSize(300, 620));
	
	m_ReferenceDirectoryText = new wxStaticText(mainPanel, wxID_ANY, wxT("Reference directory"), wxDefaultPosition, wxDefaultSize, 0);
	m_ReferenceDirectoryText->Wrap(-1);
	m_ReferenceDirectoryText->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Ubuntu")));
	referenceDirectorySizer->Add(m_ReferenceDirectoryText, 0, wxLEFT | wxTOP, 10);

	m_ReferenceDirectoryPicker = new wxDirPickerCtrl(mainPanel, REFERENCE_DIRECTORY_PICKER, wxEmptyString, wxT("Select a reference directory..."), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE);
	referenceDirectorySizer->Add(m_ReferenceDirectoryPicker, 0, wxALL | wxEXPAND, 5);

	m_ReferenceDirectoryTree = new wxGenericDirCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL | wxSUNKEN_BORDER, wxEmptyString, 0);
	m_ReferenceDirectoryTree->ShowHidden(true);
	referenceDirectorySizer->Add(m_ReferenceDirectoryTree, 1, wxEXPAND | wxALL, 5);
	mainAreaSizer->Add(referenceDirectorySizer, 0, wxSHAPED, 0);

	wxBoxSizer* directorySizer = new wxBoxSizer(wxVERTICAL);
	directorySizer->SetMinSize(wxSize(300, 620));
	
	m_DirectoryText = new wxStaticText(mainPanel, wxID_ANY, wxT("Directory"), wxDefaultPosition, wxDefaultSize, 0);
	m_DirectoryText->Wrap(-1);
	m_DirectoryText->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Ubuntu")));
	directorySizer->Add(m_DirectoryText, 0, wxLEFT | wxTOP, 10);

	m_DirectoryPicker = new wxDirPickerCtrl(mainPanel, DIRECTORY_PICKER, wxEmptyString, wxT("Select a directory..."), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE);
	directorySizer->Add(m_DirectoryPicker, 0, wxALL | wxEXPAND, 5);

	m_DirectoryTree = new wxGenericDirCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL | wxSUNKEN_BORDER, wxEmptyString, 0);
	m_DirectoryTree->ShowHidden(true);
	directorySizer->Add(m_DirectoryTree, 1, wxEXPAND | wxALL, 5);

	mainAreaSizer->Add(directorySizer, 0, wxSHAPED, 0);

	m_FastMode = new wxCheckBox(mainPanel, wxID_ANY, wxT("Fast mode"), wxDefaultPosition, wxDefaultSize, 0);
	m_FastMode->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Ubuntu")));

	mainAreaSizer->Add(m_FastMode, 0, wxALL, 5);

	wxBoxSizer* infoSizer = new wxBoxSizer(wxVERTICAL);
	infoSizer->SetMinSize(wxSize(300, 620));

	wxStaticText* items = new wxStaticText(mainPanel, wxID_ANY, wxT("Items"), wxDefaultPosition, wxDefaultSize, 0);
	items->Wrap(-1);
	items->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Ubuntu")));

	infoSizer->Add(items, 0, wxALL, 5);

	wxStaticText* missingFoldersText = new wxStaticText(mainPanel, wxID_ANY, wxT("Missing Folders"), wxDefaultPosition, wxDefaultSize, 0);
	missingFoldersText->Wrap(-1);
	missingFoldersText->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Ubuntu")));
	infoSizer->Add(missingFoldersText, 0, wxLEFT | wxTOP, 10);

	m_MissingFoldersTree = new wxTreeCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	infoSizer->Add(m_MissingFoldersTree, 0, wxALL | wxEXPAND, 5);

	wxStaticText* synchronizedFilesText = new wxStaticText(mainPanel, wxID_ANY, wxT("Synchronized Files"), wxDefaultPosition, wxDefaultSize, 0);
	synchronizedFilesText->Wrap(-1);
	synchronizedFilesText->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Ubuntu")));
	infoSizer->Add(synchronizedFilesText, 0, wxALL, 5);

	m_SynchronizedFilesTree = new wxTreeCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	infoSizer->Add(m_SynchronizedFilesTree, 0, wxALL | wxEXPAND, 5);

	wxStaticText* missingFiles = new wxStaticText(mainPanel, wxID_ANY, wxT("Missing Files"), wxDefaultPosition, wxDefaultSize, 0);
	missingFiles->Wrap(-1);
	missingFiles->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Ubuntu")));
	infoSizer->Add(missingFiles, 0, wxALL, 5);

	m_MissingFilesTree = new wxTreeCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	infoSizer->Add(m_MissingFilesTree, 0, wxALL | wxEXPAND, 5);

	mainAreaSizer->Add(infoSizer, 0, wxSHAPED, 0);

	panelSizer->Add(mainAreaSizer, 1, wxEXPAND, 5);

	mainPanel->SetSizer(panelSizer);
	mainPanel->Layout();

	panelSizer->Fit(mainPanel);

	mainSizer->Add(mainPanel, 1, wxEXPAND, 0);
}

void Frame::OnCompareButton(wxCommandEvent& event)
{
	m_MissingFoldersTree->DeleteAllItems();
	m_SynchronizedFilesTree->DeleteAllItems();
	m_MissingFilesTree->DeleteAllItems();

	std::filesystem::path referencePath = m_ReferenceDirectoryPicker->GetPath().c_str().AsString().ToStdString();
	std::filesystem::path path = m_DirectoryPicker->GetPath().c_str().AsString().ToStdString();

	if (referencePath.empty() || path.empty()) wxMessageBox("Reference or directory path is not selected!", "Folder Synchronizer", wxICON_ERROR);
	else
	{
		if (referencePath == path) wxMessageBox("Paths are the same. Please select different folders.", "Folder Synchronizer", wxICON_INFORMATION | wxOK);
		else
		{
			wxMessageDialog confirmation(nullptr, "Are you sure you want to compare files? This action might take a while to complete.", "Folder Synchronizer", wxICON_AUTH_NEEDED | wxYES_NO);

			if (confirmation.ShowModal() == wxID_YES)
			{
				if (!s_Folder || s_Folder->Path != path) s_Folder = std::make_unique<Folder>(path);
				if (!s_Reference || s_Reference->Path != referencePath) s_Reference = std::make_unique<Folder>(referencePath);

				if (s_Folder->IsSubfolder(*s_Reference))
				{
					std::stringstream message;
					message << "Folder '" << s_Folder->Path.filename() << "' is a subfolder of '" << s_Reference->Path.filename() << "'!";
					
					wxMessageBox(message.str(), "Folder Synchronizer", wxICON_ERROR);
				}
				else
				{
					s_Folder->Compare(*s_Reference);

					AppendItems();

					wxMessageBox("Directories compared successfully!", "Folder Synchronizer", wxICON_INFORMATION | wxOK);
				}
			}
		}
	}
}
void Frame::OnSynchronizeButton(wxCommandEvent& event)
{
	m_MissingFoldersTree->DeleteAllItems();
	m_SynchronizedFilesTree->DeleteAllItems();
	m_MissingFilesTree->DeleteAllItems();

	std::string referencePath = m_ReferenceDirectoryPicker->GetPath().c_str().AsString().ToStdString();
	std::string path = m_DirectoryPicker->GetPath().c_str().AsString().ToStdString();

	if (referencePath.empty() || path.empty()) wxMessageBox("Reference or directory path is not selected!", "Folder Synchronizer", wxICON_ERROR);
	else
	{
		if (referencePath == path) wxMessageBox("Paths are the same. Please select different folders.", "Folder Synchronizer", wxICON_INFORMATION | wxOK);
		else
		{
			wxMessageDialog confirmation(nullptr, "Are you sure you want to synchronize files? This action might take a while to complete and is unreversable.", "Folder Synchronizer", wxICON_AUTH_NEEDED | wxYES_NO);

			if (confirmation.ShowModal() == wxID_YES)
			{
				bool compare = false;

				if (!s_Folder || s_Folder->Path != path)
				{
					s_Folder = std::make_unique<Folder>(path);
				
					compare = true;
				}
				if (!s_Reference || s_Reference->Path != referencePath)
				{
					s_Reference = std::make_unique<Folder>(referencePath);
				
					compare = true;
				}

				if (s_Folder->IsSubfolder(*s_Reference))
				{
					std::stringstream message;
					message << "Folder '" << s_Folder->Path.filename() << "' is a subfolder of '" << s_Reference->Path.filename() << "'!";

					wxMessageBox(message.str(), "Folder Synchronizer", wxICON_ERROR);
				}
				else
				{
					if (compare)
					{
						s_Folder->Compare(*s_Reference);
					
						AppendItems();
					}

					s_Folder->Synchronize(*s_Reference);

					s_Folder = std::make_unique<Folder>(path);

					wxMessageBox("Directories synchronized successfully!", "Folder Synchronizer", wxICON_INFORMATION | wxOK);
				}
			}
		}
	}
}

void Frame::OnReferenceDirectoryPicker(wxFileDirPickerEvent& event)
{
	m_ReferenceDirectoryTree->SetPath(m_ReferenceDirectoryPicker->GetPath());
}
void Frame::OnDirectoryPicker(wxFileDirPickerEvent& event)
{
	m_DirectoryTree->SetPath(m_DirectoryPicker->GetPath());
}

void Frame::OnFastMode(wxCommandEvent& event)
{
	Config::FastMode = m_FastMode->GetValue();
}

void Frame::AppendItems()
{
	if (Result::MissingFolders.size() > 0)
	{
		for (const auto& folder : Result::MissingFolders)
		{
			m_MissingFoldersTree->AppendItem(nullptr, folder.Reference->Path.filename().c_str());
		}
	}
	else m_MissingFoldersTree->AppendItem(nullptr, "No missing folders found");

	if (Result::SynchronizeFiles.size() > 0)
	{
		for (const auto& file : Result::SynchronizeFiles)
		{
			m_SynchronizedFilesTree->AppendItem(nullptr, file.Reference->Path.filename().c_str());
		}
	}
	else m_SynchronizedFilesTree->AppendItem(nullptr, "No synchronized files found");

	if (Result::MissingFiles.size() > 0)
	{
		for (const auto& file : Result::MissingFiles)
		{
			m_MissingFilesTree->AppendItem(nullptr, file.Reference->Path.filename().c_str());
		}
	}
	else m_MissingFilesTree->AppendItem(nullptr, "No missing files found");
}

wxIMPLEMENT_APP(FolderSynchronizer);

bool FolderSynchronizer::OnInit()
{
#ifdef _DEBUG
	InitializeConsole();
#endif

	m_Frame = new Frame;
	m_Frame->Show();

	return true;
}

void FolderSynchronizer::InitializeConsole()
{
	AllocConsole();
	freopen("conout$", "w", stdout);
}