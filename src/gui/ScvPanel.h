#ifndef _SEGMENTAL_CSV_GUI_SCV_PANEL_H_
#define _SEGMENTAL_CSV_GUI_SCV_PANEL_H_

#include <wx/panel.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

#include "Common.h"

class ScvDocument;
class ScvGrid;

class ScvPanel : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(CfvPanel)
    DECLARE_EVENT_TABLE()

public:
    DECLARE_TM()

    ScvPanel(wxWindow *parent = nullptr);
    virtual ~ScvPanel();

    void OnUpdate();
    void SaveContents();

    void OnUpdateInsert(wxUpdateUIEvent &event);
    void OnInsert(wxCommandEvent &event);
    void OnUpdateDelete(wxUpdateUIEvent &event);
    void OnDelete(wxCommandEvent &event);

    void SetDocument(ScvDocument *doc);

    void ClearContents()
    {
    }

private:
    ScvDocument *m_doc;
    ScvGrid *m_grid;
};

#endif /* _SEGMENTAL_CSV_GUI_SCV_PANEL_H_ */
