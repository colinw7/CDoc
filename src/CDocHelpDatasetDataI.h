#ifndef CDOC_HELP_DATASET_DATA_H
#define CDOC_HELP_DATASET_DATA_H

#include <CFont.h>

struct CDHelpDatasetPage;
struct CDHelpDatasetFont;
class  CDHelpDatasetCommandData;
struct CDHelpDatasetRefData;
struct CDHelpDatasetHookData;
class  CDHelp;
class  CDHelpPanel;
class  CDProgramHelps;

class QWidget;

class CQWidgetPixelRenderer;

class CDHelpDatasetData {
 public:
  typedef std::vector<CDHelpDatasetPage *>        PageList;
  typedef std::list  <CDHelpDatasetFont *>        FontList;
  typedef std::vector<CDHelpDatasetFont *>        FontArray;
  typedef std::vector<CDHelpDatasetCommandData *> CommandDataList;
  typedef std::vector<CDHelpDatasetRefData *>     RefDataList;
  typedef std::vector<CDHelpDatasetHookData *>    HookDataList;
  typedef std::list  <int>                        PageHistoryList;

 private:
  CDHelp              *help_;
  std::string          dataset_;
  FILE                *fp_;
  CDProgramHelps      *program_helps_;
  bool                 popup_on_display_;
  bool                 show_header_;
  bool                 show_footer_;

  /* Page Data */

  int                  page_num_;
  PageList             page_list_;
  int                  max_page_width_;
  int                  max_page_width1_;
  int                  max_header_height_;
  int                  max_body_height_;
  int                  max_footer_height_;
  int                  max_page_height_;
  int                  left_margin_;
  int                  right_margin_;
  int                  page_length_;

  /* Help Panel Data */

  std::string          title_;
  QWidget *            text_list_;
  QWidget *            text_container_;
  CDHelpDatasetFont   *default_font_;
  CDHelpDatasetFont   *current_font_;
  CFontPtr             current_font_struct_;
  FontList             current_font_list_;
  FontArray            defined_font_list_;
  CommandDataList      page_command_data_list_;
  RefDataList          page_ref_data_list_;
  HookDataList         page_hook_data_list_;
  PageHistoryList      page_history_list_;

  /* Selection Data */

  CDHelpDatasetPage   *select_page_;
  int                  select_x_;
  int                  select_y_;
  int                  select_xmin_;
  int                  select_ymin_;
  int                  select_xmax_;
  int                  select_ymax_;

  /* Print data */

  bool                 printing_;

  /* Panel Data */

  CDHelpPanel          *help_panel_;

 public:
  CDHelpDatasetData(CDHelp *help, const std::string &dataset);
 ~CDHelpDatasetData();

  CDHelp *getHelp() const { return help_; }

  const std::string &getDataset() const { return dataset_; }

  CQWidgetPixelRenderer *getRenderer();

  FILE *getFp() const;

  bool rewindFp();

  CDProgramHelps *getProgramHelps() const { return program_helps_; }

  bool getPopupOnDisplay() const { return popup_on_display_; }

  bool getShowHeader() const { return show_header_; }
  bool getShowFooter() const { return show_footer_; }

  int getPageNum() const { return page_num_; }

  void setPageNum(int page_num) { page_num_ = page_num; }

  int getMaxPageWidth   () const { return max_page_width_   ; }
  int getMaxPageWidth1  () const { return max_page_width1_  ; }
  int getMaxHeaderHeight() const { return max_header_height_; }
  int getMaxBodyHeight  () const { return max_body_height_  ; }
  int getMaxFooterHeight() const { return max_footer_height_; }
  int getMaxPageHeight  () const { return max_page_height_  ; }

  int getLeftMargin () const { return left_margin_ ; }
  int getRightMargin() const { return right_margin_; }

  void setLeftMargin (int left_margin ) { left_margin_  = left_margin ; }
  void setRightMargin(int right_margin) { right_margin_ = right_margin; }

  int getPageLength() const { return page_length_; }

  void setPageLength(int page_length) { page_length_ = page_length; }

  const std::string &getTitle() const { return title_; }

  void setTitle(const std::string &title) { title_ = title; }

  CDHelpPanel *getHelpPanel(bool create_it=false);

  QWidget *getTextList() const { return text_list_; }

  void setTextList(QWidget *w) { text_list_ = w; }

  QWidget *getTextContainer() const { return text_container_; }

  void setTextContainer(QWidget *w) { text_container_ = w; }

  CDHelpDatasetFont *getDefaultFont() const { return default_font_; }

  void setDefaultFont(CDHelpDatasetFont *default_font) {
    default_font_ = default_font;
  }

  CDHelpDatasetFont *getCurrentFont() const { return current_font_; }

  void setCurrentFont(CDHelpDatasetFont *font);

  void setFontType(int font_type);

  int getFontType();

  void setPSTextScale();

  CFontPtr getCurrentFontStruct() const { return current_font_struct_; }

  uint getNumCurrentFonts() { return current_font_list_.size(); }

  CDHelpDatasetFont *getCurrentFont() { return current_font_list_.back(); }

  void addCurrentFont(CDHelpDatasetFont *font) {
    current_font_list_.push_back(font);
  }

  void removeCurrentFont(CDHelpDatasetFont *font);

  uint getNumDefinedFonts() { return defined_font_list_.size(); }

  CDHelpDatasetFont *getDefinedFont(uint i) { return defined_font_list_[i]; }

  void addDefinedFont(CDHelpDatasetFont *font) {
    defined_font_list_.push_back(font);
  }

  CDHelpDatasetPage *getSelectPage() { return select_page_; }

  void setSelectPage(CDHelpDatasetPage *page) { select_page_ = page; }

  int getSelectX   () { return select_x_; }
  int getSelectY   () { return select_y_; }
  int getSelectXMin() { return select_xmin_; }
  int getSelectYMin() { return select_ymin_; }
  int getSelectXMax() { return select_xmax_; }
  int getSelectYMax() { return select_ymax_; }

  void setSelectX   (int x) { select_x_    = x; }
  void setSelectY   (int y) { select_y_    = y; }
  void setSelectXMin(int x) { select_xmin_ = x; }
  void setSelectYMin(int y) { select_ymin_ = y; }
  void setSelectXMax(int x) { select_xmax_ = x; }
  void setSelectYMax(int y) { select_ymax_ = y; }

  void fixSelectRange() {
    if (select_xmin_ > select_xmax_) std::swap(select_xmin_, select_xmax_);
    if (select_ymin_ > select_ymax_) std::swap(select_ymin_, select_ymax_);
  }

  void updatePageSizes();

  void resetPages();

  void clearPages();

  uint getNumPages() const { return page_list_.size(); }

  CDHelpDatasetPage *getPage(uint i) { return page_list_[i]; }

  void addPage(CDHelpDatasetPage *page) { page_list_.push_back(page); }

  uint getNumPageRefDatas() { return page_ref_data_list_.size(); }

  CDHelpDatasetRefData *getPageRefData(uint i) { return page_ref_data_list_[i]; }

  void addRefData(CDHelpDatasetRefData *ref_data);

  void resetRefData();

  uint getNumPageHookDatas() { return page_hook_data_list_.size(); }

  uint getNumPageCommandDatas() { return page_command_data_list_.size(); }

  CDHelpDatasetCommandData *getPageCommandData(uint i) {
    return page_command_data_list_[i];
  }

  void addCommandData(CDHelpDatasetCommandData *command_data);

  void resetCommandData();

  CDHelpDatasetHookData *getPageHookData(uint i) {
    return page_hook_data_list_[i];
  }

  void addHookData(CDHelpDatasetHookData *hook_data);

  uint getNumPageHistories() const { return page_history_list_.size(); }

  void addBackPageHistory(int page_num) {
    page_history_list_.push_back(page_num);
  }

  void addFrontPageHistory(int page_num) {
    page_history_list_.push_front(page_num);
  }

  int getBackPageHistory() const { return page_history_list_.back(); }

  int popBackPageHistory() {
    int page_num = getBackPageHistory();

    page_history_list_.pop_back();

    return page_num;
  }

  void clearPageHistory() {
    page_history_list_.clear();
  }

  void setColor(int color);

  bool initPrint(const std::string &filename);
  void termPrint();

  bool initDrawPages();
  void termDrawPages();

  bool loadFontMetrics();

  void writePSHeader();
  void writePSTrailer();

  void defineFont(const std::string &font_name, const std::string &font_pattern);

  void startFont(const std::string &font_name);
  void endFont();

  void addCommand(const std::string &name, const std::string &command);

  void loadPage(CDHelpDatasetPage *page);

  void drawPage(CDHelpDatasetPage *page);

  void initPSPage();

  void startReference(CDHelpDatasetPage *page, const std::string &page_string, int x, int y);
  void endReference(CDHelpDatasetPage *page, int x, int);

  void startHook(CDHelpDatasetPage *page, const std::string &id,
                 const std::string &data, int x, int y);
  void endHook(CDHelpDatasetPage *page, int x, int y);

  void startHighlight();
  void endHighlight();

  void addButton(const std::string &title, const std::string &command,
                 int x, int y, int width, int height);
  void drawButton(const std::string &str, int *x, int *y, int style);

  void addParagraphLine(CDHelpDatasetPage *page, long pos, const char *line, int *, int *y);

  void beginParagraph(CDHelpDatasetPage *page, int x, int y, int right_margin, int continued);
  void endParagraph(CDHelpDatasetPage *page, int align, int x, int y, int align_last);

  void justifyLine(CDHelpDatasetPage *page, const std::string &line,
                   int x, int y, int right_margin);

  void drawPageLine(CDHelpDatasetPage *page, long pos, const std::string &line,
                    int align, int *x, int *y, int right_margin);

  void drawString(int *x, int *y, const std::string &str);

  void drawSpaces(int *x, int *y, int no_spaces);

  void drawImage(const std::string &id, const std::string &filename, int size,
                 int resize, int rotate, int *, int *y);

  void drawBox(int x1, int y1, int x2, int y2);
  void fillBox(int x1, int y1, int x2, int y2);

  void drawLine(int x1, int y1, int x2, int y2);

  void drawText(int x, int y, const std::string &str);

  void drawBoxChar(int c, int *x, int *y);

  void startSelect();
  void endSelect();
  bool isSelect() const;
  void selectText(int x, int y, const std::string &str);
  void addSelectChar(char c);

  void xorSelection();

  void xorBox(int x1, int y1, int x2, int y2);

  void addImage(const std::string &id, CImagePtr image);
  CImagePtr getImage(const std::string &id);
  void freeImages();

  void setDrawGC();
  void setFillGC();
  void setTextGC();

  void resetDrawGC();
  void resetFillGC();
  void resetTextGC();

  void setDrawColor(const std::string &color);
  void setDrawFontName(const std::string &font_name);
  void setDrawFontStyle(const std::string &font_style);
  void setDrawLineThickness(const std::string &line_thickness);
  void setDrawFillPixmap(const std::string &fill_pixmap);

  void buttonExtent(const std::string &str, int *width, int *height);
  void stringExtent(const std::string &str, int *width, int *height);
  void charExtent(uint c, int *width, int *height);

  int charXToPixel(CDHelpDatasetPage *page, const char *xstr);
  int charYToPixel(CDHelpDatasetPage *page, const char *ystr);

  std::string getFullPathName(const std::string &file_name);
};

//------------

class CDDrawParagraph {
 public:
  typedef std::vector<std::string> LineList;

 private:
  int      x_, y_;
  int      indent1_, indent2_;
  int      right_margin_;
  bool     continued_;
  int      line_no1_, line_no2_;
  LineList line_list_;

 public:
  CDDrawParagraph(int x, int y, int indent, int right_margin, bool continued);
 ~CDDrawParagraph();

  int getX() const { return x_; }
  int getY() const { return y_; }

  int getIndent1() const { return indent1_; }
  int getIndent2() const { return indent2_; }

  void setIndent1(int indent1) { indent1_ = indent1; }
  void setIndent2(int indent2) { indent2_ = indent2; }

  int getRightMargin() const { return right_margin_; }

  bool getContinued() const { return continued_; }

  int getLineNo1() const { return line_no1_; }
  int getLineNo2() const { return line_no2_; }

  void setLineNo1(int line_no1) { line_no1_ = line_no1; }
  void setLineNo2(int line_no2) { line_no2_ = line_no2; }

  uint getNumLines() const { return line_list_.size(); }

  const std::string &getLine(uint i) const { return line_list_[i]; }

  void addLine(const std::string &line) { line_list_.push_back(line); }
};

#endif
