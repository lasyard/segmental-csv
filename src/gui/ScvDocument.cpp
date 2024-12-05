#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/textfile.h>

#include "ScvDocument.h"

#include "ScvView.h"

#include "../item.h"
#include "../segment.h"

IMPLEMENT_DYNAMIC_CLASS(ScvDocument, wxDocument)
IMPLEMENT_TM(ScvDocument)

BEGIN_EVENT_TABLE(ScvDocument, wxDocument)
END_EVENT_TABLE()

extern "C" {
static void *segment_get_ptr(void *data, int i, [[maybe_unused]] const void *context)
{
    if (i == 0) {
        return data;
    }
    return NULL;
}

size_t scv_read(void *context, char *buf, size_t len)
{
    wxTextFile *file = static_cast<wxTextFile *>(context);
    wxString line = file->GetNextLine();
    if (file->Eof()) {
        return 0L;
    }
    strncpy(buf, line, len);
    return line.Length();
}
}

ScvDocument::ScvDocument() : wxDocument(), m_count(0), m_labels(nullptr), m_types(nullptr), m_crm(NULL)
{
    wxLog::AddTraceMask(TM);
    init_parser(&m_ctx.segment_parser_context);
    m_ctx.segment_parser_context.cols = 1;
    m_ctx.segment_parser_context.types = SEG_TYPE;
    m_ctx.segment_parser_context.f_get_ptr = segment_get_ptr;
    init_parser(&m_ctx.item_parser_context);
    init_segments(&m_segments);
}

ScvDocument::~ScvDocument()
{
    release_segments(&m_ctx, &m_segments);
    if (m_labels != nullptr) {
        release_strings(m_labels, m_count);
        delete[] m_labels;
    }
    if (m_types != nullptr) {
        delete[] m_types;
    }
    if (m_crm != NULL) {
        free(m_crm);
    }
}

bool ScvDocument::OnNewDocument()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    wxMessageBox("Create scv files is not allowed!");
    return false;
}

bool ScvDocument::OnCloseDocument()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    return wxDocument::OnCloseDocument();
}

// Called twice when closing, one in doc close, one in view close.
bool ScvDocument::DeleteContents()
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    return wxDocument::DeleteContents();
}

bool ScvDocument::DoOpenDocument(const wxString &fileName)
{
    wxLogTrace(TM, "\"%s(%s)\" called.", __WXFUNCTION__, fileName);
    wxTextFile file(fileName);
    file.Open();
    wxString line = file.GetFirstLine();
    const char *p = NULL;
    int lines = 0;
    if (file.Eof()) {
        wxLogError(_("The first line must be labels of columns."));
        goto err;
    }
    m_count = parse_count(&m_ctx.item_parser_context.options, line);
    if (m_count == 0) {
        wxLogError(_("Zero columns are defined."));
        goto err;
    }
    m_labels = new char *[m_count];
    init_strings(m_labels, m_count);
    p = parse_strings(&m_ctx.item_parser_context.options, line, m_labels, m_count);
    if (p == NULL) {
        wxLogError(_("Error occured to parse labels."));
        goto err;
    }
    line = file.GetNextLine();
    if (file.Eof()) {
        wxLogError(_("The second line must be types of columns."));
        goto err;
    }
    m_types = new enum column_type[m_count];
    parse_types(&m_ctx.item_parser_context.options, line, m_types, m_count);
    m_ctx.item_parser_context.cols = m_count;
    m_ctx.item_parser_context.types = m_types;
    m_crm = use_common_record(&m_ctx.item_parser_context);
    lines = segmental_parse(&m_ctx, &m_segments, scv_read, &file);
    wxLogStatus("%d lines read.", lines);
    return true;
err:
    file.Close();
    return false;
}

bool ScvDocument::DoSaveDocument(const wxString &fileName)
{
    wxLogTrace(TM, "\"%s(%s)\" called.", __WXFUNCTION__, fileName);
    auto *view = GetView();
    if (view != nullptr) {
        view->SaveContents();
    }
    return true;
}

void ScvDocument::Modify(bool modified)
{
    wxLogTrace(TM, "\"%s(%i)\" called.", __WXFUNCTION__, modified);
    wxDocument::Modify(modified);
}

void ScvDocument::OnChange(wxCommandEvent &event)
{
    wxLogTrace(TM, "\"%s\" called.", __WXFUNCTION__);
    Modify(true);
    event.Skip();
}

void ScvDocument::GetColLabels(wxArrayString &labels)
{
    for (int i = 0; i < m_count; ++i) {
        labels.push_back(m_labels[i]);
    }
}

const wxString ScvDocument::GetItemValueString(struct item *item, int i) const
{
    char buf[MAX_LINE_LENGTH];
    const parser_context &ctx = m_ctx.item_parser_context;
    output_by_type(&ctx.options, buf, ctx.types[i], item->data);
    return buf;
}

const wxString ScvDocument::GetSegmentValueString(struct segment *segment) const
{
    return *(char *)(*(char **)segment->data);
}

void ScvDocument::SetItemValueString(struct item *item, int i, const wxString &value)
{
    const parser_context &ctx = m_ctx.item_parser_context;
    parse_by_type(&ctx.options, value.c_str(), ctx.types[i], ctx.f_get_ptr(item->data, i, ctx.context));
}

void ScvDocument::SetSegmentValueString(struct segment *segment, const wxString &value)
{
    const parser_context &ctx = m_ctx.segment_parser_context;
    parse_by_type(&ctx.options, value.c_str(), ctx.types[0], ctx.f_get_ptr(segment->data, 0, ctx.context));
}

ScvView *ScvDocument::GetView() const
{
    auto *view = this->GetFirstView();
    return view != nullptr ? static_cast<ScvView *>(view) : nullptr;
}
