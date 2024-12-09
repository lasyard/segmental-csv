#ifndef _SEGMENTAL_CSV_GUI_SCV_DOCUMENT_H_
#define _SEGMENTAL_CSV_GUI_SCV_DOCUMENT_H_

#include <wx/docview.h>

#include "Common.h"

#include "../segmental_csv_parser.h"
#include "../segments.h"

class ScvView;

class ScvDocument : public wxDocument
{
    DECLARE_DYNAMIC_CLASS(CfvDocument)
    DECLARE_EVENT_TABLE()

public:
    DECLARE_TM()

    ScvDocument();
    virtual ~ScvDocument();

    bool OnNewDocument() override;
    bool OnCloseDocument() override;
    bool DeleteContents() override;
    bool DoOpenDocument(const wxString &fileName) override;
    bool DoSaveDocument(const wxString &fileName) override;

    void Modify(bool modified) override;

    void OnChange(wxCommandEvent &event);

    void GetColLabels(wxArrayString &labels);

    struct segments &GetSegments()
    {
        return m_segments;
    }

    enum column_type GetItemValueType(int i) const
    {
        wxASSERT(i < m_count);
        return m_types[i];
    }

    const wxString GetItemValueString(const struct item *item, int i) const;
    const wxString GetSegmentValueString(const struct segment *segment) const;

    void SetItemValueString(struct item *item, int i, const wxString &value);
    void SetSegmentValueString(struct segment *segment, const wxString &value);

    struct item *InsertItem(struct item *pos);
    struct item *InsertItemHead(struct segment *segment);

private:
    ScvView *GetView() const;

    struct segmental_parser_context m_ctx;
    int m_count;
    char **m_labels;
    enum column_type *m_types;
    struct common_record_meta *m_crm;
    struct segments m_segments;
};

#endif /* _SEGMENTAL_CSV_GUI_SCV_DOCUMENT_H_ */
