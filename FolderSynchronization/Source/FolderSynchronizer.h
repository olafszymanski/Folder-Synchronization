#pragma once

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/dirctrl.h>
#include <wx/filepicker.h>
#include <wx/treelist.h>
#include <wx/artprov.h>

#include "Folder.h"

class Frame : public wxFrame
{
public:
	Frame();
	~Frame() = default;

private:
	void OnCompareButton(wxCommandEvent& event);
	void OnSynchronizeButton(wxCommandEvent& event);

	void OnReferenceDirectoryPicker(wxFileDirPickerEvent& event);
	void OnDirectoryPicker(wxFileDirPickerEvent& event);

	wxDECLARE_EVENT_TABLE();

private:
	enum
	{
		COMPARE_BUTTON = 0, SYNCHRONIZE_BUTTON = 1,
		REFERENCE_DIRECTORY_PICKER = 2, DIRECTORY_PICKER = 3,
		FAST_MODE = 4
	};

	wxButton* m_CompareButton,* m_SynchronizeButton;
	
	wxStaticText* m_ReferenceDirectoryText,* m_DirectoryText;
	wxDirPickerCtrl* m_ReferenceDirectoryPicker,* m_DirectoryPicker;
	wxGenericDirCtrl* m_ReferenceDirectoryTree,* m_DirectoryTree;
	
	wxCheckBox* m_FastMode;

	static std::unique_ptr<Folder> s_Reference, s_Folder;
};

class FolderSynchronizer : public wxApp
{
public:
	FolderSynchronizer() = default;
	~FolderSynchronizer() = default;

	virtual bool OnInit();

private:
	void InitializeConsole();

private:
	Frame* m_Frame;
};