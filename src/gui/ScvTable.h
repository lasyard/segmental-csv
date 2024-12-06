#ifndef _SEGMENTAL_CSV_GUI_SCV_TABLE_H_
#define _SEGMENTAL_CSV_GUI_SCV_TABLE_H_

#include <wx/grid.h>
#include <wx/vector.h>

#include "../column_type.h"

#include "ScvDocument.h"

class ScvTable : public wxGridTableBase
{
public:
    ScvTable(ScvDocument *doc);
    virtual ~ScvTable();

    enum IndexType { SEGMENT, ITEM, OTHER };

    int GetNumberRows() override;
    int GetNumberCols() override;

    wxString GetValue(int row, int col) override;
    wxString GetColLabelValue(int col) override;
    wxString GetRowLabelValue(int row) override;

    void SetValue(int row, int col, const wxString &value) override;

    bool InsertRows(size_t pos, size_t numRows) override;
    bool AppendRows(size_t numRows) override;
    bool DeleteRows(size_t pos, size_t numRows) override;

    bool CanHaveAttributes() override;

    IndexType GetRowType(int row) const
    {
        if ((size_t)row < m_index.size()) {
            return m_index[row].m_type;
        }
        return OTHER;
    }

    enum column_type GetItemFieldType(int col) const
    {
        return m_doc->GetItemValueType(col);
    }

private:
    struct IndexItem {
        explicit IndexItem(struct segment *ptr) : m_ptr(ptr), m_type(SEGMENT), m_seq(0)
        {
        }

        explicit IndexItem(struct item *ptr, int seq) : m_ptr(ptr), m_type(ITEM), m_seq(seq)
        {
        }

        void *m_ptr;
        enum IndexType m_type;
        int m_seq;
    };

    ScvDocument *m_doc;

    std::vector<struct IndexItem> m_index;
    wxVector<wxArrayString> *m_cache;
    wxArrayString m_colLabels;

    void CreateIndexAndCache();

    void CacheCell(int row, int col);
    void CacheRow(int row);
    void CacheCol(int col);

    /**
     * @brief Refresh contents of a grid column and auto resize it to fit the contents.
     *
     * @param col the column
     */
    void RefreshAndAutoSizeGridColumn(int col);

    const wxString GetCellValue(int row, int col);

    void SetCellValue(int row, int col, const wxString &value);

    bool InsertRow(size_t pos);

    bool AppendRow()
    {
        // Never append.
        return false;
    }

    bool DeleteRow([[maybe_unused]] size_t pos)
    {
        // Do nothing, deletion is done in `ScvGrid`.
        return true;
    }
};

#endif /* _SEGMENTAL_CSV_GUI_SCV_TABLE_H_ */
