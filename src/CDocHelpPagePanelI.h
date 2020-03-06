#ifndef CDOC_HELP_PAGE_PANEL_I_H
#define CDOC_HELP_PAGE_PANEL_I_H

#include <QDialog>

class QSlider;
class QLabel;

class  CDHelpPanel;
class  CDHelpDatasetData;
struct CDHelpDatasetSection;

class CDHelpPageNoPanel : public QDialog {
  Q_OBJECT

 private:
  CDHelpPanel       *panel_;
  CDHelpDatasetData *help_dataset_data_;
  QSlider           *slider_;
  QLabel            *section_;

 public:
  CDHelpPageNoPanel(CDHelpPanel *panel);

  void setPage(int page_no);
  void setSection();

 private:
  CDHelpDatasetSection *getSectionDataForPage(int page_no);

 private slots:
  void sliderSlot();
};

#endif
