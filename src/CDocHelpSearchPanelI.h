#ifndef CDOC_HELP_SEARCH_PANEL_I_H
#define CDOC_HELP_SEARCH_PANEL_I_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

class CDHelpPanel;
class CDHelpDatasetData;

class CDHelpSearchPanel : public QDialog {
  Q_OBJECT

 private:
  CDHelpPanel       *panel_;
  CDHelpDatasetData *help_dataset_data_;
  QLineEdit         *str_;
  QCheckBox         *case_check_;
  QCheckBox         *word_check_;
  QCheckBox         *regexp_check_;
  long               start_pos;
  long               end_pos;
  int                page_no;
  int                line_no;
  int                char_no;

 public:
  CDHelpSearchPanel(CDHelpPanel *panel);

  long getStartPos() const { return start_pos; }
  long getEndPos  () const { return end_pos  ; }

 private:
  void findPrev();
  void findNext();

  int getValues(std::string &str, int *case_sensitive, int *match_word, int *regular_expression);

  void findPrevString(const std::string &str, int case_sensitive,
                      int match_word, int regular_expression);
  void findNextString(const std::string &str, int case_sensitive,
                      int match_word, int regular_expression);

 private slots:
  void  findFirstSlot();
  void  findLastSlot();
  void  findPrevSlot();
  void  findNextSlot();
  void  cancelSlot();
  void  helpSlot();
};

#endif
