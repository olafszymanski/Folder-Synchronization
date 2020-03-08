#pragma once

#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/filepicker.h>

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

	void OnFastMode(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

	void AppendItems();

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

	wxTreeCtrl* m_MissingFoldersTree,* m_SynchronizedFilesTree,* m_MissingFilesTree;

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