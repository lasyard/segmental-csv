#ifndef _SEGMENTAL_CSV_GUI_SCV_APP_H_
#define _SEGMENTAL_CSV_GUI_SCV_APP_H_

#include <wx/app.h>
#include <wx/docview.h>

#include "Common.h"

class ScvApp : public wxApp
{
public:
    DECLARE_TM()

    bool OnInit() override;
    int OnExit() override;

private:
    wxDocManager *m_docManager;
    // This must be long lived.
    wxLocale m_locale;
};

DECLARE_APP(ScvApp)

#endif /* _SEGMENTAL_CSV_GUI_SCV_APP_H_ */
