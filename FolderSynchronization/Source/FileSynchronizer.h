#pragma once

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/filepicker.h>

class Frame : public wxFrame
{
public:
	Frame();
	~Frame() = default;

private:
	void OnCompareButtonClicked(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();

private:
	enum
	{
		COMPARE_BUTTON = 0
	};

	wxDirPickerCtrl* m_OriginalDirectoryPicker,* m_CompareDirectoryPicker;

	wxButton* m_CompareButton;
	wxCheckBox* m_FastModeCheckBox;
};

class FileSynchronizer : public wxApp
{
public:
	FileSynchronizer();
	~FileSynchronizer() = default;

	virtual bool OnInit();

private:
	Frame* m_Frame;
};