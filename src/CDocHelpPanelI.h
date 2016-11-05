#ifndef CDOC_HELP_PANEL_I_H
#define CDOC_HELP_PANEL_I_H

#include <QMainWindow>
#include <QWidget>

class QLineEdit;
class QRadioButton;
class QCheckBox;
class QListWidget;
class QSlider;
class QLabel;
class QScrollArea;
class QScrollBar;
class QMouseEvent;

#include <CRGBA.h>
#include <CFont.h>

class CQWidgetPixelRenderer;

/*****************************************************************************/

/* Structures */

struct CDHelpDatasetPage;
class  CDHelpPanel;
class  CDHelpPageNoPanel;
class  CDHelpCommandPanel;
class  CDHelpSectionPanel;
class  CDHelpPrintPanel;
class  CDHelpSearchPanel;
struct CDTableRow;
class  CDHelpDatasetCommandData;
struct CDHelpDatasetRefData;
struct CDHelpDatasetHookData;
struct CDProgramHelps;
struct CDHelp;
class  CDHelpDatasetData;

struct CDHelpDatasetFont {
  std::string name;
  CFontPtr    normal;
  CFontPtr    bold;
  CFontPtr    under;
  CFontPtr    boldu;
};

struct CDHelpDatasetPage {
  long int            position;
  int                 header_width;
  int                 header_height;
  int                 body_width;
  int                 body_height;
  int                 footer_width;
  int                 footer_height;
  int                 page_width1;
  int                 no_lines;
  int                 max_lines;
  int                *line_heights;
  int                 line_no;
  CDHelpDatasetData  *help_dataset_data;

 ~CDHelpDatasetPage() {
    delete [] line_heights;
  }
};

class CDHelpDatasetCommandData {
 private:
  CDHelpDatasetData *help_dataset_data_;
  std::string        title_;
  std::string        command_;
  int                x_, y_;
  int                width_, height_;

 public:
  CDHelpDatasetCommandData(CDHelpDatasetData *help_dataset_data) :
   help_dataset_data_(help_dataset_data), x_(0), y_(0), width_(0), height_(0) {
  }

  const std::string &getTitle() const { return title_; }

  const std::string &getCommand() const { return command_; }

  int getX() const { return x_; }
  int getY() const { return y_; }

  int getWidth() const { return width_; }
  int getHeight() const { return height_; }

  void setTitle(const std::string &title) { title_ = title; }

  void setCommand(const std::string &command) { command_ = command; }

  void setPosition(int x, int y) { x_ = x; y_ = y; }

  void setSize(int width, int height) { width_ = width; height_ = height; }

  bool isInside(int x, int y) const {
    return (x >= x_ && x < x_ + width_ && y >= y_ && y < y_ + height_);
  }

  void exec();
};

struct CDHelpDatasetRefData {
  int                page_no;
  int                x;
  int                y;
  int                width;
  int                height;
  CDHelpDatasetData *help_dataset_data;
};

struct CDHelpDatasetHookData {
  std::string        id;
  std::string        data;
  int                x;
  int                y;
  int                width;
  int                height;
  CDHelpDatasetData *help_dataset_data;
};

struct CDHelpDatasetSection {
  std::string        name;
  int                page_no;
  int                sub_section;
  CDHelpDatasetData *help_dataset_data;

 ~CDHelpDatasetSection() { }
};

class CDocHelpCanvas : public QWidget {
  Q_OBJECT

 private:
  CDHelpPanel           *panel_;
  CQWidgetPixelRenderer *renderer_;

 public:
  CDocHelpCanvas(CDHelpPanel *panel);

  CQWidgetPixelRenderer *getRenderer();

 private:
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);

  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  void mouseEvent(bool press, QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

 private slots:
  void scrolledSlot();
};

class CDHelpPanel : public QMainWindow {
  Q_OBJECT

 private:
  CDHelpDatasetData  *help_dataset_data_;
  QScrollArea        *frame_;
  CDocHelpCanvas     *canvas_;
  QScrollBar         *vbar_;
  QScrollBar         *hbar_;
  CDHelpPageNoPanel  *page_no_panel_;
  CDHelpCommandPanel *command_panel_;
  CDHelpSectionPanel *section_panel_;
  CDHelpPrintPanel   *print_panel_;
  CDHelpSearchPanel  *search_panel_;

 public:
  CDHelpPanel(CDHelpDatasetData *help_dataset_data);
 ~CDHelpPanel();

  void init();

  CDHelpDatasetData *getHelpDatasetData() const { return help_dataset_data_; }

  CQWidgetPixelRenderer *getRenderer();

  CDHelpPageNoPanel  *getPageNoPanel (bool create_it=false);
  CDHelpCommandPanel *getCommandPanel(bool create_it=false);
  CDHelpSectionPanel *getSectionPanel(bool create_it=false);
  CDHelpPrintPanel   *getPrintPanel  (bool create_it=false);
  CDHelpSearchPanel  *getSearchPanel (bool create_it=false);

  void draw();

  void mouseButtonEvent(bool press, QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

  void updatePage();

 private:
  void loadBitmaps();

  void setDefaultFont();

  void setPanelSize();

 private slots:
  void selectSectionSlot();
  void viewRefsSlot();
  void listAllSlot();
  void searchSlot();
  void quitSlot();
  void firstPageSlot();
  void lastPageSlot();
  void nextPageSlot();
  void prevPageSlot();
  void selectPageSlot();
  void helpSlot();

  void clipBoardSlot();

 public slots:
  void printSlot();
};

/* Functions */

extern QWidget *CDocHelpDatasetDisplay
                 (CDHelp *, const std::string &);
extern void     CDocHelpDatasetDisplaySection
                 (CDHelp *, const std::string &);

#endif
