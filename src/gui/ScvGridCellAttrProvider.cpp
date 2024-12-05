#include <wx/log.h>

#include "ScvGridCellAttrProvider.h"

IMPLEMENT_TM(ScvGridCellAttrProvider)

ScvGridCellAttrProvider::ScvGridCellAttrProvider()
    : wxGridCellAttrProvider()
    , m_monoFont(16, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)
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
    m_boolAttr->SetEditor(new wxGridCellBoolEditor());
}

ScvGridCellAttrProvider::~ScvGridCellAttrProvider()
{
    m_defaultAttr->DecRef();
    m_numberAttr->DecRef();
    m_boolAttr->DecRef();
}
