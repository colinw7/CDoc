#ifndef CDOC_HELP_LIST_WIDGET_I_H
#define CDOC_HELP_LIST_WIDGET_I_H

/*****************************************************************************/

#include <QWidget>

#include <vector>

#define LIST_WIDGET_HELP         1
#define LIST_WIDGET_HELP_SECTION 2
#define LIST_WIDGET_LIST_ALL     3

struct CDListWidgetHelpData {
  int   type;
  char *data;
};

typedef std::vector<CDListWidgetHelpData *> CDListWidgetHelpDataList;

class CDWidgetReferenceList;
class CDHelp;
class CDHelpSection;
class QListWidget;

class CDListWidgetData : public QWidget {
  Q_OBJECT

 private:
  CDWidgetReferenceList *  widget_reference_list_;
  CDListWidgetHelpDataList help_data_list_;
  QListWidget *            help_list_;

 public:
  CDListWidgetData(CDWidgetReferenceList *widget_reference_list);
 ~CDListWidgetData();

  void addHelp(CDHelp *);
  void addHelpSection(CDHelpSection *);

 private slots:
  void listSelectionSlot();
};

/*****************************************************************************/

/* Private Functions */

extern void  CDocListWidgetHelpsCreatePanel
              (CDWidgetReferenceList *);

#endif
