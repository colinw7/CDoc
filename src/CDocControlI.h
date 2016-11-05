#ifndef CDOC_CONTROL_I_H
#define CDOC_CONTROL_I_H

/*****************************************************************************/

/* Includes */

#include <CDocControl.h>

/* Structures */

class QWidget;
class CDHelp;
class CDHelpSection;
class CDWidgetReferenceList;

// list of helps for a program
class CDProgramHelps {
 public:
  typedef std::list<CDHelp*>                HelpList;
  typedef std::list<CDHelpSection*>         HelpSectionList;
  typedef std::list<CDWidgetReferenceList*> WidgetReferenceListList;
  typedef std::list<CDTempFile *>           TempFileList;

 private:
  struct CmpHelp {
    std::string subject;
    std::string dataset;
    int         reference;

    CmpHelp(const std::string &subject1, const std::string &dataset1) :
      subject(subject1), dataset(dataset1), reference(-1) {
    }

    CmpHelp(const std::string &dataset1) :
      subject(""), dataset(dataset1), reference(-1) {
    }

    CmpHelp(int reference1) :
      subject(""), dataset(""), reference(reference1) {
    }

    bool operator()(const CDHelp *help);
  };

  struct CmpHelpSection {
    CDHelp*     help;
    std::string section;
    int         reference;

    CmpHelpSection(CDHelp *help1, const std::string &section1) :
      help(help1), section(section1), reference(-1) {
    }

    CmpHelpSection(int reference1) :
      help(NULL), section(""), reference(reference1) {
    }

    bool operator()(const CDHelpSection *help_section);
  };

  struct CmpHelpReferenceList {
    QWidget *widget;

    CmpHelpReferenceList(QWidget *widget1) :
     widget(widget1) {
    }

    bool operator()(const CDWidgetReferenceList *widget_reference_list);
  };

  std::string             program_;
  std::string             directory_;
  HelpList                help_list_;
  HelpSectionList         help_section_list_;
  WidgetReferenceListList widget_reference_list_list_;
  TempFileList            temporary_file_list_;

 public:
  CDProgramHelps(const std::string &program, const std::string &directory);
 ~CDProgramHelps();

  const std::string &getProgram  () const { return program_  ; }
  const std::string &getDirectory() const { return directory_; }

  const HelpList &getHelpList() const { return help_list_; }

  const HelpSectionList &getHelpSectionList() const {
    return help_section_list_;
  }

  void addHelp(CDHelp *help);
  void addHelpSection(CDHelpSection *help_section);
  void addWidgetReferenceList(CDWidgetReferenceList *widget_reference_list);

  void removeHelp(CDHelp *help);

  CDHelp *getHelp(const std::string &subject, const std::string &dataset);
  CDHelp *getHelp(const std::string &dataset);
  CDHelp *getHelp(int reference);

  CDHelpSection *getHelpSection(CDHelp *help, const std::string &section);
  CDHelpSection *getHelpSection(int reference);

  CDWidgetReferenceList *getWidgetReferenceList(QWidget *widget);

  CDTempFile *addTemporaryFile();

  void deleteTemporaryFiles();
};

struct CDTempFile {
 private:
  std::string program_;
  std::string file_;

 public:
  CDTempFile(const std::string &program) {
    program_ = program;
    file_    = CDocInst->getTempFileName();
  }

 ~CDTempFile() {
    remove(file_.c_str());
  }

  const std::string &getProgram() const { return program_; }
  const std::string &getFile   () const { return file_   ; }
};

#endif
