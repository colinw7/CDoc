#ifndef CDOC_HELP_SECTION_PANEL_I_H
#define CDOC_HELP_SECTION_PANEL_I_H

#include <QDialog>

class QListWidget;

class CDHelpPanel;
class CDHelpDatasetData;
class CDHelpDatasetSection;

class CDHelpSectionPanel : public QDialog {
  Q_OBJECT

 private:
  typedef std::vector<CDHelpDatasetSection *> DataList;

  CDHelpPanel *         panel_;
  CDHelpDatasetData *   help_dataset_data_;
  QListWidget *         list_;
  DataList              data_list_;
  CDHelpDatasetSection *last_section_;

 public:
  CDHelpSectionPanel(CDHelpPanel *panel);
 ~CDHelpSectionPanel();

  void addSection(const std::string &name);

  void addSubSection(const std::string &name);

  CDHelpDatasetSection *getSectionData(const std::string &section) const;

  void addItem(const std::string &item);

  uint getNumData() const { return data_list_.size(); }

  CDHelpDatasetSection *getData(uint i) const { return data_list_[i]; }

  void addData(CDHelpDatasetSection *data) { data_list_.push_back(data); }

  void clearData() { data_list_.clear(); }

  CDHelpDatasetSection *getLastSection() const { return last_section_; }

  void setLastSection(CDHelpDatasetSection *last_section) {
    last_section_ = last_section;
  }

 private slots:
  void selectionProc();
};

#endif
