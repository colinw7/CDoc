#ifndef CDOC_HELP_LIST_ALL_I_H
#define CDOC_HELP_LIST_ALL_I_H

/*****************************************************************************/

#include <QWidget>

#include <vector>

#define LIST_ALL_HELP         1
#define LIST_ALL_HELP_SECTION 2

struct CDListAllHelpData {
  int   type;
  char *data;
};

typedef std::vector<CDListAllHelpData *> CDListAllHelpDataList;

class CDProgramHelps;
class CDHelp;
class CDHelpSection;
class QListWidget;

class CDListAllData : public QWidget {
  Q_OBJECT

 private:
  CDProgramHelps        *program_helps_;
  CDListAllHelpDataList  help_data_list_;
  QListWidget *          help_list_;

 public:
  CDListAllData(CDProgramHelps *program_helps);
 ~CDListAllData();

  void addHelp(CDHelp *);
  void addHelpSection(CDHelpSection *);

 private slots:
  void listSelectionSlot();
};

/*****************************************************************************/

/* Private Functions */

extern void  CDocListAllHelpsCreatePanel
              (CDProgramHelps *);

#endif
