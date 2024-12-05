#include <wx/aboutdlg.h>
#include <wx/config.h>
#include <wx/notebook.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "ScvMainFrame.h"

#include "ScvApp.h"
#include "ScvDefs.h"
#include "ScvPanel.h"
#include "Utils.h"

IMPLEMENT_DYNAMIC_CLASS(ScvMainFrame, wxDocParentFrame)
IMPLEMENT_TM(ScvMainFrame)

// Event propagating order: doc -> view -> frame.
BEGIN_EVENT_TABLE(ScvMainFrame, wxDocParentFrame)
EVT_CLOSE(ScvMainFrame::OnClose)
EVT_UPDATE_UI(ID_INSERT, ScvMainFrame::OnUpdateMenu)
EVT_UPDATE_UI(wxID_DELETE, ScvMainFrame::OnUpdateMenu)
// Don't know why this ID cannot be wxID_ABOUT.
EVT_MENU(ID_ABOUT, ScvMainFrame::OnAbout)
EVT_MENU(ID_LICENSE, ScvMainFrame::OnLicense)
EVT_MENU(ID_WX_INFO, ScvMainFrame::OnWxInfo)
END_EVENT_TABLE()

const wxString ScvMainFrame::CFG_FILE_HISTORY = "FileHistory";
const wxString ScvMainFrame::LICENSE_FILE_NAME = "LICENSE";

ScvMainFrame::ScvMainFrame() : wxDocParentFrame()
{
    wxLog::AddTraceMask(TM);
}

ScvMainFrame::ScvMainFrame(
    wxDocManager *manager,
    wxFrame *parent,
    wxWindowID id,
    const wxString &title,
    const wxPoint &pos,
    const wxSize &size,
    long type,
    const wxString &name
)
    : wxDocParentFrame(manager, parent, id, title, pos, size, type, name)
{
    wxLog::AddTraceMask(TM);
    auto *res = wxXmlResource::Get();
    auto *menuBar = res->LoadMenuBar("menuBar");
    SetMenuBar(menuBar);
    auto *statusBar = new wxStatusBar(this, wxID_ANY, wxST_SIZEGRIP);
    statusBar->SetFieldsCount();
    SetStatusBar(statusBar);
    auto *sizer = new wxBoxSizer(wxVERTICAL);
    m_panel = new ScvPanel(this);
    sizer->Add(m_panel, wxSizerFlags().Expand().Border(wxALL, 0).Proportion(1));
    SetSizer(sizer);
    m_panel->Show(false);
    SetSize(1080, 720);
    // The config file is in `~/Library/Preferences` on macOS.
    wxConfig config(_(APP_NAME));
    config.SetPath(CFG_FILE_HISTORY);
    m_docManager->FileHistoryLoad(config);
    auto *fileMenu = GetMenuBar()->GetMenu(GetMenuBar()->FindMenu(_("File")));
    m_docManager->FileHistoryUseMenu(fileMenu);
    m_docManager->FileHistoryAddFilesToMenu();
}

ScvMainFrame::~ScvMainFrame()
{
}

void ScvMainFrame::OnClose([[maybe_unused]] wxCloseEvent &event)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    wxConfig config(_(APP_NAME));
    config.SetPath(CFG_FILE_HISTORY);
    m_docManager->FileHistorySave(config);
    Destroy();
    wxExit();
}

void ScvMainFrame::OnAbout([[maybe_unused]] wxCommandEvent &event)
{
    const wxString &appName = wxGetApp().GetAppDisplayName();
    wxAboutDialogInfo info;
    info.SetName(appName);
    info.SetVersion(APP_VERSION);
    wxString date = __DATE__;
    info.SetCopyright("(C) " + date.Right(4) + " " AUTHOR " " EMAIL);
    info.AddDeveloper(AUTHOR " " EMAIL);
    wxAboutBox(info);
}

void ScvMainFrame::OnLicense([[maybe_unused]] wxCommandEvent &event)
{
    wxString text;
    wxString resDir = wxStandardPaths::Get().GetResourcesDir();
    WxCommon::ReadAllText(text, wxFileName(resDir, LICENSE_FILE_NAME).GetFullPath());
#if wxUSE_UNICODE
    const wxString copyrightSign = c("\xc2\xa9");
    text.Replace("(c)", copyrightSign);
    text.Replace("(C)", copyrightSign);
#endif
    WxCommon::ShowTextBox(_("License"), text);
}

void ScvMainFrame::OnWxInfo([[maybe_unused]] wxCommandEvent &event)
{
    wxInfoMessageBox(this);
}

void ScvMainFrame::OnUpdateMenu(wxUpdateUIEvent &event)
{
    // There is no doc/view if the event propagated here.
    event.Enable(false);
}

void ScvMainFrame::OnUpdateCheckMenu(wxUpdateUIEvent &event)
{
    event.Enable(false);
    event.Check(false);
}
