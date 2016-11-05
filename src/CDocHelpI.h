#ifndef CDOC_HELP_I_H
#define CDOC_HELP_I_H

#include <QPushButton>

/*****************************************************************************/

/* Includes */

#include <CDocHelp.h>

/* Defines */

#define CDOC_TITLE_LENGTH   256
#define CDOC_COMMAND_LENGTH 256

enum CFocFormatType {
  CDOC_FORMAT_NONE       = 0,
  CDOC_FORMAT_SCRIPT     = 1,
  CDOC_FORMAT_CDOC       = 2,
  CDOC_FORMAT_RAW        = 3,
  CDOC_FORMAT_SHOWCASE   = 4,
  CDOC_FORMAT_POSTSCRIPT = 5,
  CDOC_FORMAT_XWD        = 6,
  CDOC_FORMAT_HTML       = 7,
  CDOC_FORMAT_ILEAF      = 8
};

#define CDOC_HELP_FONT_STR                  "cdoc_help_font"
#define CDOC_COMMAND_FONT_STR               "cdoc_command_font"
#define CDOC_SECTION_FONT_STR               "cdoc_section_font"

#define CDOC_NORMAL_FONT_STR                "cdoc_normal_font"
#define CDOC_BOLD_FONT_STR                  "cdoc_bold_font"
#define CDOC_ITALIC_FONT_STR                "cdoc_italic_font"
#define CDOC_UNDERLINE_FONT_STR             "cdoc_underline_font"
#define CDOC_BOLD_ITALIC_FONT_STR           "cdoc_bold_italic_font"
#define CDOC_BOLD_UNDERLINE_FONT_STR        "cdoc_bold_underline_font"
#define CDOC_ITALIC_UNDERLINE_FONT_STR      "cdoc_italic_underline_font"
#define CDOC_BOLD_ITALIC_UNDERLINE_FONT_STR "cdoc_bold_italic_underline_font"

/* Structures */

class CDProgramHelps;

class CDHelp {
 private:
  CDProgramHelps *program_helps_;
  int             reference_;
  std::string     subject_;
  std::string     dataset_;
  std::string     orig_dataset_;
  CFocFormatType  format_;
  bool            internal_;

 public:
  CDHelp(CDProgramHelps *program_helps, int reference, const std::string &subject,
         const std::string &dataset, const std::string &orig_dataset="");

  CDProgramHelps *getProgramHelps() const { return program_helps_; }

  int getReference() const { return reference_; }

  const std::string &getSubject() const { return subject_; }

  const std::string &getDataset() const { return dataset_; }

  void setDataset(const std::string &dataset) { dataset_ = dataset; }

  const std::string &getOrigDataset() const { return orig_dataset_; }

  CFocFormatType getFormat() const { return format_; }

  void setFormat(CFocFormatType format) { format_ = format; }

  bool getInternal() const { return internal_; }

  void setInternal(bool internal) { internal_ = internal; }

  bool cmp(const std::string &subject, const std::string &dataset) const {
    return (subject == subject_ && dataset == orig_dataset_);
  }
};

class CDHelpSection {
 private:
  CDHelp      *help_;
  int          reference_;
  std::string  section_;

 public:
  CDHelpSection(CDHelp *help, int reference, const std::string &section);

  CDHelp *getHelp() const { return help_; }

  int getReference() const { return reference_; }

  const std::string &getSection() const { return section_; }
};

class CDWidgetReferenceList {
 public:
  typedef std::vector<int> ReferenceList;

 private:
  CDProgramHelps *program_helps_;
  QWidget *       widget_;
  ReferenceList   reference_list_;

 public:
  CDWidgetReferenceList(CDProgramHelps *program_helps, QWidget *widget);

  CDProgramHelps *getProgramHelps() const { return program_helps_; }

  QWidget *getWidget() const { return widget_; }

  void setWidget(QWidget *widget) { widget_ = widget; }

  void addReference(int reference) { reference_list_.push_back(reference); }

  uint getNumReferences() const { return reference_list_.size(); }

  int getReference(int i) const { return reference_list_[i]; }
};

class CDHelpButton : public QPushButton {
  Q_OBJECT

 private:
  CDWidgetReferenceList *widget_reference_list_;

 public:
  CDHelpButton(CDWidgetReferenceList *widget_reference_list);
 ~CDHelpButton();

  CDWidgetReferenceList *getWidgetReferenceList() const { return widget_reference_list_; }

 private slots:
  void buttonSlot();
};

/* Data */

typedef std::list<QWidget *> CDocShellList;

extern bool cdoc_popup_on_display;
extern bool cdoc_print_on_display;

/* Subroutines */

extern void                   CDocWidgetHelpButtonPress
                               (CDWidgetReferenceList *);
extern void                   CDocWidgetHelpSelect
                               (CDHelp *, int);
extern void                   CDocWidgetHelpSectionSelect
                               (CDHelpSection *);
extern CDHelp                *CDocGetHelpForReference
                               (CDProgramHelps *, int);
extern CDHelpSection         *CDocGetHelpSectionForReference
                               (CDProgramHelps *, int);
extern CDHelp                *CDocGetHelpForDataset
                               (CDProgramHelps *, const std::string &);
extern CDWidgetReferenceList *CDocGetReferenceListForWidget
                               (CDProgramHelps *, QWidget *);
extern void                   CDocDeleteTemporaryFiles
                               (const std::string &);
extern std::string            CDocCreateDatasetName
                               (CDHelp *);

/*****************************************************************************/

#endif
