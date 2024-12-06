#include <wx/log.h>

#include "ScvGridCellAttrProvider.h"

#include "ScvTable.h"

IMPLEMENT_TM(ScvGridCellAttrProvider)

ScvGridCellAttrProvider::ScvGridCellAttrProvider(const ScvTable *table)
    : wxGridCellAttrProvider()
    , m_monoFont(16, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)
    , m_table(table)
{
    wxLog::AddTraceMask(TM);

    m_defaultAttr = new wxGridCellAttr();
    // Do not use `wxALIGN_CENTER_HORIZONTAL` or `wxALIGN_CENTER_VERTICAL` for `hAlign` and `vAlign`.
    m_defaultAttr->SetAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

    m_numberAttr = m_defaultAttr->Clone();
    m_numberAttr->SetAlignment(wxALIGN_RIGHT, wxALIGN_CENTER);
    m_numberAttr->SetFont(m_monoFont);

    m_boolAttr = m_defaultAttr->Clone();
    m_boolAttr->SetRenderer(new wxGridCellBoolRenderer());
    auto *boolEditor = new wxGridCellBoolEditor();
    boolEditor->UseStringValues("1", "0");
    m_boolAttr->SetEditor(boolEditor);

    m_segmentAttr = m_defaultAttr->Clone();
    m_segmentAttr->SetSize(1, m_table->GetColsCount());
    m_segmentAttr->SetBackgroundColour(*wxLIGHT_GREY);
    m_segmentAttr->SetAlignment(wxALIGN_CENTER_VERTICAL, wxALIGN_LEFT);
}

ScvGridCellAttrProvider::~ScvGridCellAttrProvider()
{
    m_defaultAttr->DecRef();
    m_numberAttr->DecRef();
    m_boolAttr->DecRef();
}

wxGridCellAttr *ScvGridCellAttrProvider::GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind) const
{
    // Seems `kind` is always `Any`.
    if (kind == wxGridCellAttr::wxAttrKind::Any || kind == wxGridCellAttr::wxAttrKind::Cell) {
        switch (m_table->GetRowType(row)) {
        case ScvTable::ITEM:
            switch (m_table->GetItemFieldType(col)) {
            case CT_INT32:
            case CT_INT64:
            case CT_MONEY:
            case CT_DATE:
            case CT_TIME:
                m_numberAttr->IncRef();
                return m_numberAttr;
            case CT_BOOL:
                m_boolAttr->IncRef();
                return m_boolAttr;
            default:
                break;
            }
            break;
        case ScvTable::SEGMENT:
            // Do not return colSpan > 1 for col > 0, or there will be index out of bound problem.
            if (col == 0) {
                m_segmentAttr->IncRef();
                return m_segmentAttr;
            }
            break;
        default:
            break;
        }
    }
    m_defaultAttr->IncRef();
    return m_defaultAttr;
}
