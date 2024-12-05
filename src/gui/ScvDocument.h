#ifndef _SEGMENTAL_CSV_GUI_SCV_DOCUMENT_H_
#define _SEGMENTAL_CSV_GUI_SCV_DOCUMENT_H_

#include <wx/docview.h>

#include "Common.h"

class ScvView;

class ScvDocument : public wxDocument
{
    DECLARE_DYNAMIC_CLASS(CfvDocument)
    DECLARE_EVENT_TABLE()

public:
    DECLARE_TM()

    ScvDocument();
    virtual ~ScvDocument();

    bool OnNewDocument() override;
    bool OnCloseDocument() override;
    bool DeleteContents() override;
    bool DoOpenDocument(const wxString &fileName) override;
    bool DoSaveDocument(const wxString &fileName) override;

    void Modify(bool modified) override;

    void OnChange(wxCommandEvent &event);

private:
    ScvView *GetView() const;
};

#endif /* _SEGMENTAL_CSV_GUI_SCV_DOCUMENT_H_ */
