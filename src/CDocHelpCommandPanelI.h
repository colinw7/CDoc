#ifndef CDOC_HELP_COMMAND_PANEL_I_H
#define CDOC_HELP_COMMAND_PANEL_I_H

#include <QDialog>

class QListWidget;

class CDHelpPanel;
class CDHelpDatasetData;
class CDHelpDatasetCommandData;

class CDHelpCommandPanel : public QDialog {
  Q_OBJECT

 private:
  typedef std::vector<CDHelpDatasetCommandData *> DataList;

  CDHelpPanel       *panel_;
  CDHelpDatasetData *help_dataset_data_;
  QListWidget       *list_;
  DataList           data_list_;

 public:
  CDHelpCommandPanel(CDHelpPanel *panel);
 ~CDHelpCommandPanel();

  void addItem(const std::string &str);

  uint getNumData() const { return data_list_.size(); }

  CDHelpDatasetCommandData *getData(uint i) const { return data_list_[i]; }

  void addData(CDHelpDatasetCommandData *data) { data_list_.push_back(data); }

  void clearData() { data_list_.clear(); }

 private slots:
  void listSelectionSlot();
};

#endif
