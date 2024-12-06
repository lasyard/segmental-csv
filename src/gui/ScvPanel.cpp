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
#include "ScvDocument.h"
#include "ScvGrid.h"
#include "Utils.h"

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
    m_grid = new ScvGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_LEFT);
    sizer->Add(m_grid, wxSizerFlags().Expand().Border(wxALL, 0).Proportion(1));
    SetSizer(sizer);
    m_grid->SetAttributes();
    m_grid->SetMinSize(wxSize(160, -1));
}

ScvPanel::~ScvPanel()
{
    m_grid->ClearGrid();
}

void ScvPanel::OnUpdate()
{
    m_grid->CreateScvTable(m_doc);
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
    WxCommon::DelegateEvent(m_grid, event);
}

void ScvPanel::OnUpdateDelete(wxUpdateUIEvent &event)
{
    event.Enable(m_grid->HasFocus());
}

void ScvPanel::OnDelete([[maybe_unused]] wxCommandEvent &event)
{
    WxCommon::DelegateEvent(m_grid, event);
}

void ScvPanel::SetDocument(ScvDocument *doc)
{
    if (m_doc != nullptr) {
        // This may be not necessary.
        m_grid->Unbind(wxEVT_GRID_CELL_CHANGED, &ScvDocument::OnChange, m_doc);
    }
    m_doc = doc;
    m_grid->Bind(wxEVT_GRID_CELL_CHANGED, &ScvDocument::OnChange, m_doc);
}
