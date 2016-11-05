#ifndef CDOC_HELP_PRINT_PANEL_I_H
#define CDOC_HELP_PRINT_PANEL_I_H

#include <QDialog>

class QLineEdit;
class QRadioButton;

class CDHelpPanel;
class CDHelpDatasetData;

class CDHelpPrintPanel : public QDialog {
  Q_OBJECT

 public:
  CDHelpPrintPanel(CDHelpPanel *panel);

  void update();

  int getPerPage() const;

  int getOrient() const;

  int getImageDepth() const;

  void setPageRange(int start, int end);

 private:
  CDHelpPanel       *panel_;
  CDHelpDatasetData *help_dataset_data_;
  QLineEdit         *pageStart_;
  QLineEdit         *pageEnd_;
  QRadioButton      *allPages_;
  QRadioButton      *specificPages_;
  QRadioButton      *normalLayout_;
  QRadioButton      *twoPPLayout_;
  QRadioButton      *fourPPLayout_;
  QRadioButton      *portrait_;
  QRadioButton      *landscape_;
  QRadioButton      *automatic_;
  QRadioButton      *depth2_;
  QRadioButton      *depth4_;
  QRadioButton      *depth16_;
  QRadioButton      *depth256_;
  QLineEdit         *copiesEdit_;
  QLineEdit         *commandEdit_;

 private slots:
  void printSlot();
  void cancelSlot();
  void helpSlot();
};

#endif
