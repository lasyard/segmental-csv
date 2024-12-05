#ifndef _SEGMENTAL_CSV_GUI_SCV_VIEW_H_
#define _SEGMENTAL_CSV_GUI_SCV_VIEW_H_

#include <wx/docview.h>

#include "Common.h"

class ScvPanel;

class ScvView : public wxView
{
    DECLARE_DYNAMIC_CLASS(CfvView)
    DECLARE_EVENT_TABLE()

public:
    DECLARE_TM()

    ScvView();
    virtual ~ScvView();

    bool OnCreate(wxDocument *doc, long flags) override;
    bool OnClose(bool deleteWindow) override;

    void OnUpdate(wxView *sender, wxObject *hint) override;
    void OnDraw(wxDC *dc) override;
    void OnClosingDocument() override;

    void OnUpdateMenu(wxUpdateUIEvent &event);
    void OnMenu(wxCommandEvent &event);

    void SaveContents();

private:
    ScvPanel *m_panel;
};

#endif /* _SEGMENTAL_CSV_GUI_SCV_VIEW_H_ */
