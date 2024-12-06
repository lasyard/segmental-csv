#ifndef _SEGMENTAL_CSV_GUI_SCV_GRID_CELL_ATTR_PROVIDER_H_
#define _SEGMENTAL_CSV_GUI_SCV_GRID_CELL_ATTR_PROVIDER_H_

#include <wx/grid.h>

#include "Common.h"

class ScvTable;

class ScvGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
    DECLARE_TM()

    ScvGridCellAttrProvider(const ScvTable *table);
    virtual ~ScvGridCellAttrProvider();

    wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind) const override;

private:
    wxFont m_monoFont;

    const ScvTable *m_table;

    mutable wxGridCellAttr *m_defaultAttr;
    mutable wxGridCellAttr *m_numberAttr;
    mutable wxGridCellAttr *m_boolAttr;
    mutable wxGridCellAttr *m_segmentAttr;
};

#endif /* _SEGMENTAL_CSV_GUI_SCV_GRID_CELL_ATTR_PROVIDER_H_ */
