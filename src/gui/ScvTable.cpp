#include <wx/log.h>

#include "ScvTable.h"

#include "../item.h"
#include "../segment.h"

#include "ScvGridCellAttrProvider.h"

ScvTable::ScvTable(ScvDocument *doc) : wxGridTableBase(), m_doc(doc), m_cache(nullptr)
{
    m_doc->GetColLabels(m_colLabels);
    CreateIndexAndCache();
    // must after index and cache are created
    SetAttrProvider(new ScvGridCellAttrProvider(this));
}

ScvTable::~ScvTable()
{
    if (m_cache != nullptr) {
        delete m_cache;
    }
}

int ScvTable::GetNumberRows()
{
    return m_cache->size();
}

int ScvTable::GetNumberCols()
{
    return m_colLabels.size();
}

wxString ScvTable::GetValue(int row, int col)
{
    return (*m_cache)[row][col];
}

wxString ScvTable::GetColLabelValue(int col)
{
    return m_colLabels[col];
}

wxString ScvTable::GetRowLabelValue(int row)
{
    if ((size_t)row < m_index.size()) {
        const struct IndexItem &item = m_index[row];
        if (item.m_type == ITEM) {
            return wxString::Format("%d", item.m_seq);
        }
    }
    return "";
}

void ScvTable::SetValue(int row, int col, const wxString &value)
{
    try {
        SetCellValue(row, col, value);
    } catch (std::runtime_error &e) {
        wxLogError(e.what());
    }
    CacheCell(row, col);
    RefreshAndAutoSizeGridColumn(col);
}

bool ScvTable::InsertRows(size_t pos, size_t numRows)
{
    size_t i;
    for (i = 0; i < numRows; ++i) {
        if (!InsertRow(pos)) {
            break;
        }
        m_cache->insert(std::next(m_cache->begin(), pos), wxArrayString());
        CacheRow(pos);
    }
    if (i > 0) {
        auto grid = GetView();
        if (grid != nullptr) {
            wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_INSERTED, pos, i);
            grid->ProcessTableMessage(msg);
        }
        return true;
    }
    return false;
}

bool ScvTable::AppendRows(size_t numRows)
{
    size_t i;
    for (i = 0; i < numRows; ++i) {
        if (!AppendRow()) {
            break;
        }
        m_cache->push_back(wxArrayString());
        CacheRow(m_cache->size() - 1);
    }
    if (i > 0) {
        auto grid = GetView();
        if (grid != nullptr && i > 0) {
            wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, i, 0);
            grid->ProcessTableMessage(msg);
        }
        return true;
    }
    return false;
}

bool ScvTable::DeleteRows(size_t pos, size_t numRows)
{
    size_t i;
    for (i = 0; i < numRows; ++i) {
        if (!DeleteRow(pos)) {
            break;
        }
    }
    m_cache->erase(std::next(m_cache->begin(), pos), std::next(m_cache->begin(), pos + i));
    if (i > 0) {
        auto grid = GetView();
        if (grid != nullptr && i > 0) {
            wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, i);
            grid->ProcessTableMessage(msg);
        }
        return true;
    }
    return false;
}

bool ScvTable::CanHaveAttributes()
{
    return true;
}

void ScvTable::CreateIndexAndCache()
{
    struct segments &segments = m_doc->GetSegments();
    m_index.clear();
    for (struct list_item *p = segments.segments.first; p != NULL; p = p->next) {
        struct segment *segment = get_segment(p);
        m_index.push_back(IndexItem(segment));
        int seq = 1;
        for (struct list_item *q = segment->items.first; q != NULL; q = q->next) {
            struct item *item = get_item(q);
            m_index.push_back(IndexItem(item, seq++));
        }
    }
    int rows = m_index.size();
    m_cache = new wxVector<wxArrayString>(rows);
    for (auto i = 0; i < rows; ++i) {
        CacheRow(i);
    }
}

void ScvTable::CacheCell(int row, int col)
{
    (*m_cache)[row][col] = GetCellValue(row, col);
}

void ScvTable::CacheRow(int row)
{
    (*m_cache)[row].Empty();
    int cols = GetNumberCols();
    for (auto col = 0; col < cols; ++col) {
        (*m_cache)[row].Add(GetCellValue(row, col));
    }
}

void ScvTable::CacheCol(int col)
{
    for (auto i = 0; i < GetNumberRows(); ++i) {
        CacheCell(i, col);
    }
}

void ScvTable::RefreshAndAutoSizeGridColumn(int col)
{
    auto grid = GetView();
    if (grid != nullptr) {
        grid->BeginBatch();
        grid->RefreshBlock(0, col, m_cache->size() - 1, col);
        grid->AutoSizeColumn(col);
        grid->EndBatch();
    }
}

const wxString ScvTable::GetCellValue(int row, int col)
{
    switch (GetRowType(row)) {
    case ITEM:
        if ((size_t)col < m_colLabels.size()) {
            return m_doc->GetItemValueString(static_cast<struct item *>(m_index[row].m_ptr), col);
        }
        break;
    case SEGMENT:
        if (col == 0) {
            return m_doc->GetSegmentValueString(static_cast<struct segment *>(m_index[row].m_ptr));
        }
        break;
    case OTHER:
        break;
    }
    return "";
}

void ScvTable::SetCellValue(int row, int col, const wxString &value)
{
    switch (GetRowType(row)) {
    case ITEM:
        if ((size_t)col < m_colLabels.size()) {
            m_doc->SetItemValueString(static_cast<struct item *>(m_index[row].m_ptr), col, value);
        }
        break;
    case SEGMENT:
        if (col == 0) {
            m_doc->SetSegmentValueString(static_cast<struct segment *>(m_index[row].m_ptr), value);
        }
        break;
    case OTHER:
        break;
    }
}

bool ScvTable::InsertRow(size_t pos)
{
    auto &index = m_index[pos];
    struct item *item = nullptr;
    if (index.m_type == ITEM) {
        // item = insert_item((struct item *)index.m_ptr);
    } else if (index.m_type == SEGMENT) {
        // item = add_item_head((struct segment *)index.m_ptr);
    }
    if (item != nullptr) {
        int seq = index.m_seq + 1;
        m_index.insert(std::next(m_index.begin(), pos + 1), IndexItem(item, seq));
        for (auto p = pos + 2; p < m_index.size() && m_index[p].m_type != SEGMENT; ++p) {
            ++m_index[p].m_seq;
        }
        return true;
    }
    return false;
}
