#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

#include "ScvDocument.h"

#include "ScvView.h"

IMPLEMENT_DYNAMIC_CLASS(ScvDocument, wxDocument)
IMPLEMENT_TM(ScvDocument)

BEGIN_EVENT_TABLE(ScvDocument, wxDocument)
END_EVENT_TABLE()

ScvDocument::ScvDocument() : wxDocument()
{
    wxLog::AddTraceMask(TM);
}

ScvDocument::~ScvDocument()
{
}

bool ScvDocument::OnNewDocument()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    // Updating must be emitted explicitly here.
    UpdateAllViews();
    return wxDocument::OnNewDocument();
}

bool ScvDocument::OnCloseDocument()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    return wxDocument::OnCloseDocument();
}

// Called twice when closing, one in doc close, one in view close.
bool ScvDocument::DeleteContents()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    return wxDocument::DeleteContents();
}

bool ScvDocument::DoOpenDocument(const wxString &fileName)
{
    wxLogTrace(TM, "\"%s(%s)\" called.", __WXFUNCTION__, fileName);
    return true;
}

bool ScvDocument::DoSaveDocument(const wxString &fileName)
{
    wxLogTrace(TM, "\"%s(%s)\" called.", __WXFUNCTION__, fileName);
    auto *view = GetView();
    if (view != nullptr) {
        view->SaveContents();
    }
    return true;
}

void ScvDocument::Modify(bool modified)
{
    wxLogTrace(TM, "\"%s(%i)\" called.", __WXFUNCTION__, modified);
    wxDocument::Modify(modified);
}

void ScvDocument::OnChange(wxCommandEvent &event)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    Modify(true);
    event.Skip();
}

ScvView *ScvDocument::GetView() const
{
    auto *view = this->GetFirstView();
    return view != nullptr ? static_cast<ScvView *>(view) : nullptr;
}
