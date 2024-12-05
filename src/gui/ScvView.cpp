#include <wx/listbook.h>

#include "ScvView.h"

#include "ScvApp.h"
#include "ScvDefs.h"
#include "ScvDocument.h"
#include "ScvMainFrame.h"
#include "ScvPanel.h"
#include "Utils.h"

IMPLEMENT_DYNAMIC_CLASS(ScvView, wxView)
IMPLEMENT_TM(ScvView)

BEGIN_EVENT_TABLE(ScvView, wxView)
EVT_UPDATE_UI(ID_INSERT, ScvView::OnUpdateMenu)
EVT_MENU(ID_INSERT, ScvView::OnMenu)
EVT_UPDATE_UI(wxID_DELETE, ScvView::OnUpdateMenu)
EVT_MENU(wxID_DELETE, ScvView::OnMenu)
END_EVENT_TABLE()

ScvView::ScvView() : wxView()
{
    wxLog::AddTraceMask(TM);
}

ScvView::~ScvView()
{
}

bool ScvView::OnCreate([[maybe_unused]] wxDocument *doc, [[maybe_unused]] long flags)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    auto *frame = static_cast<ScvMainFrame *>(wxGetApp().GetTopWindow());
    SetFrame(frame);
    m_panel = frame->GetPanel();
    m_panel->SetDocument(static_cast<ScvDocument *>(doc));
    m_panel->Show();
    frame->Layout();
    Activate(true);
    return true;
}

bool ScvView::OnClose(bool deleteWindow)
{
    Activate(false);
    auto *frame = static_cast<ScvMainFrame *>(GetFrame());
    frame->SetTitle("");
    m_panel->ClearContents();
    m_panel->Show(false);
    return wxView::OnClose(deleteWindow);
}

void ScvView::OnUpdate([[maybe_unused]] wxView *sender, [[maybe_unused]] wxObject *hint)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    m_panel->OnUpdate();
}

void ScvView::OnDraw([[maybe_unused]] wxDC *dc)
{
    // Do nothing.
}

void ScvView::OnClosingDocument()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
}

void ScvView::OnUpdateMenu(wxUpdateUIEvent &event)
{
    WxCommon::DelegateEvent(m_panel, event);
}

void ScvView::OnMenu(wxCommandEvent &event)
{
    WxCommon::DelegateEvent(m_panel, event);
}

void ScvView::SaveContents()
{
    m_panel->SaveContents();
}
