#include <wx/artprov.h>
#include <wx/dc.h>
#include <wx/filedlg.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

#include "ScvGrid.h"

#include "ScvDefs.h"
#include "ScvDocument.h"
#include "ScvTable.h"

IMPLEMENT_DYNAMIC_CLASS(ScvGrid, wxGrid)
IMPLEMENT_TM(ScvGrid)

BEGIN_EVENT_TABLE(ScvGrid, wxGrid)
EVT_UPDATE_UI(ID_INSERT, ScvGrid::OnUpdateInsert)
EVT_MENU(ID_INSERT, ScvGrid::OnInsert)
EVT_UPDATE_UI(wxID_DELETE, ScvGrid::OnUpdateDelete)
EVT_MENU(wxID_DELETE, ScvGrid::OnDelete)
END_EVENT_TABLE()

ScvGrid::ScvGrid() : wxGrid()
{
    wxLog::AddTraceMask(TM);
}

ScvGrid::ScvGrid(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    long style,
    const wxString &name
)
    : wxGrid(parent, id, pos, size, style, name)
{
    wxLog::AddTraceMask(TM);
}

ScvGrid::~ScvGrid()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    CheckEventHandler();
}

wxPen ScvGrid::GetRowGridLinePen([[maybe_unused]] int row)
{
    return *wxTRANSPARENT_PEN;
}

wxPen ScvGrid::GetColGridLinePen([[maybe_unused]] int col)
{
    return *wxTRANSPARENT_PEN;
}

void ScvGrid::DrawCornerLabel(wxDC &dc)
{
    dc.DrawBitmap(wxArtProvider::GetBitmap("logo"), 1, 1);
}

void ScvGrid::SetAttributes()
{
    BeginBatch();
    SetColMinimalAcceptableWidth(80);
    SetRowMinimalAcceptableHeight(ROW_HEIGHT);
    auto logo = wxArtProvider::GetBitmap("logo");
    SetRowLabelSize(logo.GetWidth() + 2);
    SetColLabelSize(logo.GetHeight() + 2);
    DisableDragColMove();
    EndBatch();
}

void ScvGrid::OnGridSelectCell(wxGridEvent &event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    int numRows, numCols;
    // GetCellSize never returns CellSpan_Inside, so test the first column.
    if (col != 0 && GetCellSize(row, 0, &numRows, &numCols) == CellSpan_Main) {
        // This will re-generate this event.
        SetGridCursor(row, 0);
        event.Veto();
    }
}

void ScvGrid::OnUpdateInsert(wxUpdateUIEvent &event)
{
    event.Enable(HasFocus() && GetGridCursorRow() >= 0);
}

void ScvGrid::OnInsert([[maybe_unused]] wxCommandEvent &event)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    BeginBatch();
    auto row = GetGridCursorRow();
    if (InsertRows(row + 1)) {
        AutoSizeRow(row + 1);
        SetGridCursor(row + 1, 1);
    }
    EndBatch();
}

void ScvGrid::OnUpdateDelete(wxUpdateUIEvent &event)
{
    if (HasFocus()) {
        if (IsSelection()) {
            event.Enable(true);
        } else {
            auto coords = GetGridCursorCoords();
            event.Enable(coords != wxGridNoCellCoords);
        }
    } else {
        event.Enable(false);
    }
}

void ScvGrid::OnDelete([[maybe_unused]] wxCommandEvent &event)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    BeginBatch();
    const auto blocks = GetSelectedBlocks();
    if (blocks.begin() != blocks.end()) { // Not empty
        for (const auto block : GetSelectedBlocks()) {
            auto topLeft = block.GetTopLeft();
            auto bottomRight = block.GetBottomRight();
            for (int i = topLeft.GetRow(); i <= bottomRight.GetRow(); ++i) {
                for (int j = topLeft.GetCol(); j <= bottomRight.GetCol(); ++j) {
                    SetCellValue(i, j, "");
                }
            }
        }
        ClearSelection();
    } else {
        SetCellValue(GetGridCursorCoords(), "");
    }
    EndBatch();
}

void ScvGrid::CreateScvTable(ScvDocument *doc)
{
    auto table = new ScvTable(doc);
    // Vital, for the original grid cursor may be out of range.
    int cursorRow = GetGridCursorRow();
    int maxRow = table->GetNumberRows() - 1;
    if (cursorRow > maxRow) {
        SetGridCursor(maxRow, GetGridCursorCol());
    }
    // Change table after cursor set.
    SetTable(table, true);
    RefreshContent();
}

void ScvGrid::CheckEventHandler()
{
    auto win = GetGridWindow();
    auto &children = win->GetChildren();
    for (auto &child : children) {
        auto *handler = child->GetEventHandler();
        if (handler != child) {
            wxLogTrace(
                TM,
                "Pushed event handler foud. win = \"%s\", class of handler = \"%s\"",
                child->GetName(),
                handler->GetClassInfo()->GetClassName()
            );
        }
    }
}
