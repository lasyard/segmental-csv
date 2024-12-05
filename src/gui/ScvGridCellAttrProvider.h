#ifndef _SEGMENTAL_CSV_GUI_SCV_GRID_CELL_ATTR_PROVIDER_H_
#define _SEGMENTAL_CSV_GUI_SCV_GRID_CELL_ATTR_PROVIDER_H_

#include <wx/grid.h>

#include "Common.h"

class ScvGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
    DECLARE_TM()

    ScvGridCellAttrProvider();
    virtual ~ScvGridCellAttrProvider();

protected:
    wxFont m_monoFont;

    mutable wxGridCellAttr *m_defaultAttr;
    mutable wxGridCellAttr *m_numberAttr;
    mutable wxGridCellAttr *m_boolAttr;
};

#endif /* _SEGMENTAL_CSV_GUI_SCV_GRID_CELL_ATTR_PROVIDER_H_ */
