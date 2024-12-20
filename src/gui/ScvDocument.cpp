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

struct segment_data {
    char *title;
};

static enum column_type SEGMENT_TYPE[] = {CT_CSTR};

extern "C" {
static void *segment_get_ptr(void *data, int i, [[maybe_unused]] const void *context)
{
    if (i == 0) {
        return &static_cast<struct segment_data *>(data)->title;
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

void scv_write(void *context, const char *buf, [[maybe_unused]] size_t len)
{
    wxTextFile *file = static_cast<wxTextFile *>(context);
    file->AddLine(buf);
}
}

ScvDocument::ScvDocument() : wxDocument(), m_count(0), m_labels(nullptr), m_types(nullptr), m_crm(NULL)
{
    wxLog::AddTraceMask(TM);
    init_parser(&m_ctx.segment_parser_context);
    m_ctx.segment_parser_context.cols = 1;
    m_ctx.segment_parser_context.types = SEGMENT_TYPE;
    m_ctx.segment_parser_context.f_get_ptr = segment_get_ptr;
    m_ctx.segment_data_size = sizeof(struct segment);
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
    wxLogError(_("Create scv files is not allowed!"));
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
    m_ctx.item_data_size = m_crm->bytes;
    lines = segmental_parse(&m_ctx, &m_segments, scv_read, &file);
    if (lines < 0) {
        wxLogError(_("Parse error at line %d"), -lines + 2);
        goto err;
    }
    wxLogStatus(_("%d lines read"), lines + 2);
    file.Close();
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
    wxTextFile file(fileName);
    if (!(wxFile::Exists(fileName) ? file.Open() : file.Create())) {
        wxLogError(_("Cannot open file %s."), fileName);
        return false;
    }
    file.Clear();
    char buf[MAX_LINE_LENGTH];
    output_strings(&m_ctx.item_parser_context.options, buf, m_labels, m_count);
    file.AddLine(buf);
    output_types(&m_ctx.item_parser_context.options, buf, m_types, m_count);
    file.AddLine(buf);
    segmental_output(&m_ctx, &m_segments, scv_write, &file);
    file.Write();
    file.Close();
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

const wxString ScvDocument::GetItemValueString(const struct item *item, int i) const
{
    const parser_context &ctx = m_ctx.item_parser_context;
    if (m_types[i] != CT_CSTR) {
        char buf[MAX_LINE_LENGTH];
        char *p = output_field(&ctx, buf, item->data, i);
        *p = '\0';
        return buf;
    } else {
        return get_cstr_field(&ctx, item->data, i);
    }
}

const wxString ScvDocument::GetSegmentValueString(const struct segment *segment) const
{
    const struct segment_data *str = static_cast<const struct segment_data *>(segment->data);
    if (str->title != NULL) {
        return str->title;
    }
    return "";
}

void ScvDocument::SetItemValueString(struct item *item, int i, const wxString &value)
{
    const parser_context &ctx = m_ctx.item_parser_context;
    parse_field(&ctx, value.c_str(), item->data, i);
}

void ScvDocument::SetSegmentValueString(struct segment *segment, const wxString &value)
{
    const parser_context &ctx = m_ctx.segment_parser_context;
    parse_field(&ctx, value.c_str(), segment->data, 0);
}

struct item *ScvDocument::InsertItem(struct item *pos)
{
    return insert_new_item(&m_ctx, pos);
}

struct item *ScvDocument::InsertItemHead(struct segment *segment)
{
    return insert_new_item_head(&m_ctx, segment);
}

ScvView *ScvDocument::GetView() const
{
    auto *view = this->GetFirstView();
    return view != nullptr ? static_cast<ScvView *>(view) : nullptr;
}
