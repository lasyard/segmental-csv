#include <wx/grid.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/textctrl.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <wx/treebook.h>
#include <wx/treectrl.h>

#include "ScvPanel.h"

#include "ScvDefs.h"

IMPLEMENT_DYNAMIC_CLASS(ScvPanel, wxPanel)
IMPLEMENT_TM(ScvPanel)

BEGIN_EVENT_TABLE(ScvPanel, wxPanel)
EVT_UPDATE_UI(ID_INSERT, ScvPanel::OnUpdateInsert)
EVT_MENU(ID_INSERT, ScvPanel::OnInsert)
EVT_UPDATE_UI(wxID_DELETE, ScvPanel::OnUpdateDelete)
EVT_MENU(wxID_DELETE, ScvPanel::OnDelete)
END_EVENT_TABLE()

ScvPanel::ScvPanel(wxWindow *parent) : wxPanel(parent), m_doc(nullptr)
{
    wxLog::AddTraceMask(TM);
    auto *sizer = new wxBoxSizer(wxVERTICAL);
    m_grid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_LEFT);
    sizer->Add(m_grid, wxSizerFlags().Expand().Border(wxALL, 0).Proportion(1));
    SetSizer(sizer);
    m_grid->SetMinSize(wxSize(160, -1));
}

ScvPanel::~ScvPanel()
{
    m_grid->ClearGrid();
}

void ScvPanel::OnUpdate()
{
}

void ScvPanel::SaveContents()
{
}

void ScvPanel::OnUpdateInsert(wxUpdateUIEvent &event)
{
    event.Enable(m_grid->HasFocus());
}

void ScvPanel::OnInsert([[maybe_unused]] wxCommandEvent &event)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
}

void ScvPanel::OnUpdateDelete(wxUpdateUIEvent &event)
{
    event.Enable(m_grid->HasFocus());
}

void ScvPanel::OnDelete([[maybe_unused]] wxCommandEvent &event)
{
}
