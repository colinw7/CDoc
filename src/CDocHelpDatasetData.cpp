#include "CDocI.h"
#include "CDocHelpSectionPanelI.h"
#include "CDocHelpCommandPanelI.h"
#include "CDocHelpSearchPanelI.h"
#include "CDocHelpPrintPanelI.h"
#include "CDocHelpPagePanelI.h"

#include <QApplication>
#include <QClipboard>

#include <CQWidgetPixelRenderer.h>
#include <CPrint.h>
#include <CFontMetrics.h>
#include <CRGBName.h>

struct CDFontData {
  std::string           name;
  int                   fixed;
  int                   size;
  CFontMetrics::Family  family;
  CFontMetric          *normal;
  CFontMetric          *bold;
  CFontMetric          *italic;
  CFontMetric          *boldi;
  CFontMetric          *current;
};

typedef std::map<std::string,CImagePtr> CDImageBuffer;

static CDImageBuffer image_buffer;

static bool cdoc_base_font_set = false;

static int   cdoc_ref_page_no = -1;
static int   cdoc_ref_line_no = -1;
static int   cdoc_ref_x       = 0;
static int   cdoc_ref_y       = 0;

static int         cdoc_hook_line_no = -1;
static std::string cdoc_hook_id;
static std::string cdoc_hook_data;
static int         cdoc_hook_x       = 0;
static int         cdoc_hook_y       = 0;

static int cdoc_tab_indent = 0;
static int cdoc_fill_pad   = 0;

static int cdoc_indent_char_width = 8;

static std::string  select_file;
static FILE        *select_fp = NULL;

static CDHelpDatasetPage *last_page;

static CRGBA cdoc_bg_normal(1,1,1);
static CRGBA cdoc_fg_normal(0,0,0);

static const char *
cdoc_colors[] = {
  "Normal",
  "Red",
  "Green",
  "Yellow",
  "Blue",
  "Magenta",
  "Cyan",
  "Inverse",
};

static bool colors_set = false;

static CRGBA highlight_color;
static CRGBA hyper_color;
static CRGBA button_color;
static CRGBA select_color;

static int cdoc_current_font_type = 0;

static CPrint ps_print;

static CFontMetrics *ps_font_metrics = NULL;

static CDFontData *ps_current_font = NULL;
static double      ps_font_x_scale = 1.0;
static double      ps_font_y_scale = 1.0;

typedef std::vector<CDFontData *> CDFontDataList;

static CDFontDataList ps_defined_font_list;
static CDFontDataList ps_current_font_list;

static int ps_max_page_width    = 0;
static int ps_max_page_width1   = 0;
static int ps_max_page_height   = 0;
static int ps_max_header_height = 0;
static int ps_max_body_height   = 0;
static int ps_max_footer_height = 0;

static CRGBA cdoc_button_top;
static CRGBA cdoc_button_bottom;
static CRGBA cdoc_button_select;

static std::string current_draw_color;
static std::string current_draw_font_name;
static std::string current_draw_font_style;
static int         current_draw_line_thickness = 0;
static std::string current_draw_fill_pixmap;

static CRGBA highlight_save_fg;
static CRGBA reference_save_fg;
static CRGBA hook_save_fg;

static CDDrawParagraph *paragraph = NULL;

//----------

CDHelpDatasetData::
CDHelpDatasetData(CDHelp *help, const std::string &dataset)
{
  if (! colors_set) {
    highlight_color = CRGBName::toRGBA("red");
    hyper_color     = CRGBName::toRGBA("blue");
    button_color    = CRGBName::toRGBA("white");
    select_color    = CRGBName::toRGBA("forestgreen");

    colors_set = true;
  }

  /* Create and Initialise the Help Dataset Panel Data */

  help_             = help;
  dataset_          = dataset;
  fp_               = NULL;
  program_helps_    = (help != NULL ?  help->getProgramHelps() : NULL);
  popup_on_display_ = cdoc_popup_on_display;

  show_header_ = CDocInst->getShowHeader();
  show_footer_ = CDocInst->getShowFooter();

  page_num_          = 1;
  max_page_width_    = 0;
  max_page_width1_   = 0;
  max_header_height_ = 0;
  max_body_height_   = 0;
  max_footer_height_ = 0;
  max_page_height_   = 0;
  left_margin_       = cdoc_left_margin;
  right_margin_      = cdoc_right_margin;
  page_length_       = cdoc_lines_per_page;

  text_list_      = NULL;
  text_container_ = NULL;
  default_font_   = NULL;
  current_font_   = NULL;

  select_page_ = NULL;
  select_x_    = -1;
  select_y_    = -1;
  select_xmin_ = -1;
  select_ymin_ = -1;
  select_xmax_ = -1;
  select_ymax_ = -1;

  printing_ = false;

  help_panel_ = NULL;

  /* Disable Footer and Header for Internal Helps */

  if (help_== NULL || help_->getInternal()) {
    show_header_= false;
    show_footer_= false;
  }
}

CDHelpDatasetData::
~CDHelpDatasetData()
{
  /* Free Off the Allocated Data */

  resetPages();

  //------

  if (fp_ != NULL)
    fclose(fp_);

  //------

  FontList::iterator p1 = current_font_list_.begin();
  FontList::iterator p2 = current_font_list_.end  ();

  for ( ; p1 != p2; ++p1) {
    CDHelpDatasetFont *font = *p1;

    delete font;
  }

  current_font_list_.clear();

  //------

  uint num = defined_font_list_.size();

  for (uint i = 0; i < num; ++i) {
    CDHelpDatasetFont *font = defined_font_list_[i];

    delete font;
  }

  defined_font_list_.clear();
}

CDHelpPanel *
CDHelpDatasetData::
getHelpPanel(bool create_it)
{
  if (! help_panel_ && create_it) {
    help_panel_ = new CDHelpPanel(this);

    help_panel_->init();
  }

  return help_panel_;
}

CQWidgetPixelRenderer *
CDHelpDatasetData::
getRenderer()
{
  CDHelpPanel *help_panel = getHelpPanel();

  return help_panel->getRenderer();
}

void
CDHelpDatasetData::
resetPages()
{
  clearPages();

  /*********/

  resetCommandData();

  /*********/

  resetRefData();

  /*********/

  clearPageHistory();
}

void
CDHelpDatasetData::
addRefData(CDHelpDatasetRefData *ref_data)
{
  page_ref_data_list_.push_back(ref_data);
}

void
CDHelpDatasetData::
resetRefData()
{
  uint num = page_ref_data_list_.size();

  for (uint i = 0; i < num; ++i) {
    CDHelpDatasetRefData *ref_data = page_ref_data_list_[i];

    delete ref_data;
  }

  page_ref_data_list_.clear();
}

void
CDHelpDatasetData::
addCommandData(CDHelpDatasetCommandData *command_data)
{
  page_command_data_list_.push_back(command_data);
}

void
CDHelpDatasetData::
resetCommandData()
{
  uint num = page_command_data_list_.size();

  for (uint i = 0; i < num; ++i) {
    CDHelpDatasetCommandData *command_data = page_command_data_list_[i];

    delete command_data;
  }

  page_command_data_list_.clear();
}

void
CDHelpDatasetData::
addHookData(CDHelpDatasetHookData *hook_data)
{
  page_hook_data_list_.push_back(hook_data);
}

void
CDHelpDatasetData::
updatePageSizes()
{
  int width, height;

  charExtent(CDOC_MEAN_CHAR, &width, &height);

  max_page_width_ = right_margin_*width;

  max_page_width1_ = 0;

  max_header_height_ = 0;
  max_body_height_   = height;
  max_footer_height_ = 0;

  uint num_pages = page_list_.size();

  for (uint i = 0; i < num_pages; ++i) {
    CDHelpDatasetPage *page = page_list_[i];

    if (page->header_width > max_page_width_)
      max_page_width_ = page->header_width;
    if (page->body_width   > max_page_width_)
      max_page_width_ = page->body_width;
    if (page->footer_width > max_page_width_)
      max_page_width_ = page->footer_width;

    if (page->page_width1 > max_page_width1_)
      max_page_width1_ = page->page_width1;

    if (page->header_height > max_header_height_)
      max_header_height_ = page->header_height;
    if (page->body_height   > max_body_height_)
      max_body_height_   = page->body_height;
    if (page->footer_height > max_footer_height_)
      max_footer_height_ = page->footer_height;
  }

  if (max_page_width_ > max_page_width1_)
    max_page_width1_ = max_page_width_;

  if (max_header_height_ <= 0)
    max_header_height_ = height/2;

  if (max_footer_height_ <= 0)
    max_footer_height_ = height/2;

  max_page_height_ = max_header_height_ + max_body_height_ + max_footer_height_;
}

void
CDHelpDatasetData::
clearPages()
{
  for_each(page_list_.begin(), page_list_.end(), CDeletePointer());

  page_list_.clear();
}

FILE *
CDHelpDatasetData::
getFp() const
{
  if (fp_ == NULL) {
    CDHelpDatasetData *th = const_cast<CDHelpDatasetData *>(this);

    th->fp_ = fopen(dataset_.c_str(), "r");
  }

  return fp_;
}

bool
CDHelpDatasetData::
rewindFp()
{
  FILE *fp = getFp();

  if (! fp)
    return false;

  rewind(fp);

  return true;
}

void
CDHelpDatasetData::
setCurrentFont(CDHelpDatasetFont *font)
{
  current_font_ = font;

  if (current_font_)
    current_font_struct_ = current_font_->normal;
}

// Set the current font name to the current font's normal, bold, underline or
// bold underline font name.
//
// A value for 'font' which is not 0, 1, 3, or 4 is assumed to be 1 i.e. bold.
void
CDHelpDatasetData::
setFontType(int font_type)
{
  static int         last_font_type  = -1;
  static int         last_font_fixed = -1;
  static CDFontData *last_font       = NULL;

  if (font_type != 0 && font_type != 1 && font_type != 3 && font_type != 4)
    font_type = 1;

  cdoc_current_font_type = font_type;

  if (printing_) {
    //setFontType(font_type);

    if      (font_type == 0) ps_current_font->current = ps_current_font->normal;
    else if (font_type == 1) ps_current_font->current = ps_current_font->bold;
    else if (font_type == 3) ps_current_font->current = ps_current_font->italic;
    else if (font_type == 4) ps_current_font->current = ps_current_font->boldi;
    else                     ps_current_font->current = ps_current_font->bold;

    setPSTextScale();

    if (! printing_)
      return;

    if (ps_current_font->fixed != last_font_fixed ||
        cdoc_current_font_type != last_font_type ||
        ps_current_font        != last_font) {
      std::string fontName;

      if      (ps_current_font->family == CFontMetrics::COURIER) {
        if      (font_type == 0) fontName = "FontCN" ;
        else if (font_type == 1) fontName = "FontCB" ;
        else if (font_type == 3) fontName = "FontCI" ;
        else if (font_type == 4) fontName = "FontCBI";
        else                     fontName = "FontCB" ;
      }
      else if (ps_current_font->family == CFontMetrics::HELVETICA) {
        if      (font_type == 0) fontName = "FontHN" ;
        else if (font_type == 1) fontName = "FontHB" ;
        else if (font_type == 3) fontName = "FontHI" ;
        else if (font_type == 4) fontName = "FontHBI";
        else                     fontName = "FontHB" ;
      }
      else if (ps_current_font->family == CFontMetrics::TIMES) {
        if      (font_type == 0) fontName = "FontTN" ;
        else if (font_type == 1) fontName = "FontTB" ;
        else if (font_type == 3) fontName = "FontTI" ;
        else if (font_type == 4) fontName = "FontTBI";
        else                     fontName = "FontTB" ;
      }
      else if (ps_current_font->normal == NULL) {
        if      (font_type == 0) fontName = "FontSN";
        else if (font_type == 1) fontName = "FontSN";
        else if (font_type == 3) fontName = "FontSN";
        else if (font_type == 4) fontName = "FontSN";
        else                     fontName = "FontSN";
      }

      double x_size = ps_current_font->size*ps_font_x_scale;
      double y_size = ps_current_font->size*ps_font_y_scale;

      ps_print.setFont(fontName, x_size, y_size);

      last_font_fixed = ps_current_font->fixed;
      last_font_type  = cdoc_current_font_type;
      last_font       = ps_current_font;
    }
  }
  else {
    if      (font_type == 0) current_font_struct_ = current_font_->normal;
    else if (font_type == 1) current_font_struct_ = current_font_->bold;
    else if (font_type == 3) current_font_struct_ = current_font_->boldu;
    else if (font_type == 4) current_font_struct_ = current_font_->under;
    else                     current_font_struct_ = current_font_->normal;
  }
}

// Get the current font's type i.e. normal, bold, underline or bold underline.
int
CDHelpDatasetData::
getFontType()
{
  if (printing_) {
    if      (ps_current_font->current == ps_current_font->normal) return 0;
    else if (ps_current_font->current == ps_current_font->bold  ) return 1;
    else if (ps_current_font->current == ps_current_font->italic) return 3;
    else if (ps_current_font->current == ps_current_font->boldi ) return 4;
    else                                                          return 0;
  }
  else {
    if      (current_font_struct_ == current_font_->normal) return 0;
    else if (current_font_struct_ == current_font_->bold  ) return 1;
    else if (current_font_struct_ == current_font_->boldu ) return 3;
    else if (current_font_struct_ == current_font_->under ) return 4;
    else                                                    return 0;
  }
}

// Set the Font Scaling factor between the Postscript Font and the equivalent X Font.
//
// Scale Factors are stored in 'ps_font_x_scale' and 'ps_font_y_scale' global variables.
void
CDHelpDatasetData::
setPSTextScale()
{
  static const char *text1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  ps_font_x_scale = 1.0;
  ps_font_y_scale = 1.0;

  int width1, height1;

  stringExtent(text1, &width1, &height1);

  int width2, height2;

  if (ps_current_font != NULL) {
    width2 = 0;

    if (ps_current_font->current != NULL) {
      for (int i = 0; text1[i] != '\0'; i++) {
        uint c = text1[i];

        width2 += ps_current_font->current->getCharWidth(c);
      }
    }
    else
      width2 = strlen(text1);

    if (ps_current_font->current != NULL)
      height2 = ps_current_font->current->getYMax() - ps_current_font->current->getYMin();
    else
      height2 = 1;

    width2  *= int(ps_current_font->size*ps_font_x_scale);
    height2 *= int(ps_current_font->size*ps_font_y_scale);
  }
  else {
    width2  =  8*strlen(text1);
    height2 = 10;
  }

  ps_font_x_scale = fabs((double) width1 )/fabs( width2/1000.0);
  ps_font_y_scale = fabs((double) height1)/fabs(height2/1000.0);

  if (ps_font_x_scale > ps_font_y_scale)
    ps_font_x_scale = ps_font_y_scale;
  else
    ps_font_y_scale = ps_font_x_scale;
}

void
CDHelpDatasetData::
removeCurrentFont(CDHelpDatasetFont *font)
{
  FontList::iterator p1 = current_font_list_.begin();
  FontList::iterator p2 = current_font_list_.end  ();

  while (p1 != p2) {
    if (*p1 == font) {
      current_font_list_.erase(p1);

      p1 = current_font_list_.begin();
      p2 = current_font_list_.end  ();
    }
    else
      ++p1;
  }
}

// Set the current foreground color.
//
// to be 0 i.e. Normal.
void
CDHelpDatasetData::
setColor(int color)
{
  if (printing_) {
  }
  else {
    if (color == 8) {
      startHighlight();
      return;
    }

    if (color == 9) {
      endHighlight();
      return;
    }

    CPixelRenderer *renderer = getRenderer();

    if      (color <= 0 || color > 7) {
      renderer->setForeground(cdoc_fg_normal);
      renderer->setBackground(cdoc_bg_normal);
    }
    else if (color == 7) {
      renderer->setForeground(cdoc_bg_normal);
      renderer->setBackground(cdoc_fg_normal);
    }
    else {
      CRGBA allocated_color = CRGBName::toRGBA(cdoc_colors[color]);

      renderer->setForeground(allocated_color);
      renderer->setBackground(cdoc_bg_normal);
    }
  }
}

bool
CDHelpDatasetData::
initPrint(const std::string &filename)
{
  if (! ps_print.setFilename(filename)) {
    CDocError::error("CDoc Help Print", "Failed to Write Print File");
    return false;
  }

  printing_ = true;

  return initDrawPages();
}

void
CDHelpDatasetData::
termPrint()
{
  termDrawPages();

 /* Close Output File */

  ps_print.setFilename("");

  printing_ = false;
}

// Build the Information on each Page of the Help Dataset being displayed by CDoc.
// This information includes the Page Dimensions and the Height of each Line on the
// Page.
bool
CDHelpDatasetData::
initDrawPages()
{
  last_page = NULL;

  if (printing_) {
    if (! loadFontMetrics())
      return false;
  }

  /***********/

  if (printing_) {
    CDHelpPrintPanel *print_panel = getHelpPanel()->getPrintPanel();

    ps_print.setCreator("CDoc");

    ps_print.setTitle(getHelp()->getSubject());

    ps_print.setPerPage(print_panel->getPerPage());
  }

  /***********/

  cdoc_base_font_set = false;

  /* Set Initial Font */

  startFont("");

  /* Set Indent Width */

  int width, height;

  charExtent(INDENT_CHAR, &width, &height);

  cdoc_indent_char_width = width;

  /***********/

  /* Init colors */

  cdoc_button_top    = CRGBA(0.8,0.8,0.8);
  cdoc_button_bottom = CRGBA(0.6,0.6,0.6);
  cdoc_button_select = CRGBA(0.9,0.9,0.9);

  /***********/

  /* Find the positions of each Page and the Size of Each Page */

  CDHelpDatasetPage *page = new CDHelpDatasetPage;

  page->position          = 0;
  page->header_width      = 0;
  page->header_height     = 0;
  page->body_width        = 0;
  page->body_height       = 0;
  page->footer_width      = 0;
  page->footer_height     = 0;
  page->page_width1       = 0;
  page->no_lines          = 0;
  page->max_lines         = 100;
  page->line_heights      = new int [page->max_lines];
  page->line_no           = 1;
  page->help_dataset_data = this;

  addPage(page);

  int *page_width1 = &page->page_width1;

  int *page_width  = &page->body_width;
  int *page_height = &page->body_height;

  int *save_page_width = page_width;

  cdoc_tab_indent = 0;

  /*******/

  setPageNum(1);

  if (! rewindFp())
    return false;

  bool processing = true;

  std::string line;

  while (CDocReadLineFromFile(getFp(), line)) {
    char *line1 = (char *) line.c_str();

    /* Initialise the Line Height */

    page->line_heights[page->line_no - 1] = 0;

    /* Ignore line if not processing */

    if (! processing) {
      if (getHelp()->getFormat() == CDOC_FORMAT_CDOC &&
          strncmp(line1, CDOC_CMD_ID, CDOC_CMD_ID_LEN) == 0) {
        if ((! getShowHeader() && strncmp(&line1[8], "end_header", 10) == 0) ||
            (! getShowFooter() && strncmp(&line1[8], "end_footer", 10) == 0)) {
          processing = true;

          page_width  = &page->body_width;
          page_height = &page->body_height;
        }
      }

      goto next_line;
    }

    /* Start New Page */

    if (line1[0] == NEW_PAGE)
      goto next_line;

    /* Process CDoc Command */

    if (getHelp()->getFormat() == CDOC_FORMAT_CDOC &&
        strncmp(line1, CDOC_CMD_ID, CDOC_CMD_ID_LEN) == 0) {
      /* Replace space after command name with NULL
         so we can use 'strcmp' on the command name */

      char *p1 = strchr(&line1[8], ' ');

      if (p1 != NULL) {
        *p1 = '\0';

        p1++;
      }

      /* Page Parts */

      if      (strcmp(&line1[8], "start_header") == 0) {
        if (! getShowHeader()) {
          processing = false;

          goto next_line;
        }

        page_width  = &page->header_width;
        page_height = &page->header_height;
      }
      else if (strcmp(&line1[8], "end_header") == 0) {
        page_width  = &page->body_width;
        page_height = &page->body_height;
      }
      else if (strcmp(&line1[8], "start_footer") == 0) {
        if (! getShowFooter()) {
          processing = false;

          goto next_line;
        }

        page_width  = &page->footer_width;
        page_height = &page->footer_height;
      }
      else if (strcmp(&line1[8], "end_footer") == 0) {
        page_width  = &page->body_width;
        page_height = &page->body_height;
      }

      /* Page Dimensions */

      else if (strcmp(&line1[8], "left_margin") == 0) {
        if (p1 != NULL)
          setLeftMargin(CStrUtil::toInteger(p1));
        else
          CDocWriteError("Invalid Left Margin Command %s", line1);
      }
      else if (strcmp(&line1[8], "right_margin") == 0) {
        if (p1 != NULL) {
          setRightMargin(CStrUtil::toInteger(p1));

          if (getRightMargin() == 0)
            setRightMargin(cdoc_right_margin);
        }
        else
          CDocWriteError("Invalid Right Margin Command %s", line1);
      }
      else if (strcmp(&line1[8], "page_length") == 0) {
        if (p1 != NULL) {
          setPageLength(CStrUtil::toInteger(p1));

          if (getPageLength() == 0)
            setPageLength(cdoc_lines_per_page);
        }
        else
          CDocWriteError("Invalid Page Length Command %s", line1);
      }

      /* Define Font Mapping */

      else if (strcmp(&line1[8], "def_font") == 0) {
        char *p2   = NULL;
        char *name = NULL;

        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          name = p1;

          p2 = strchr(p1, '=');
        }

        if (p2 != NULL) {
          *p2 = '\0';

          p2++;

          defineFont(name, p2);
        }
        else
          CDocWriteError("Invalid Define Font Command %s", line1);
      }

      /* Start New Font */

      else if (strcmp(&line1[8], "start_font") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          startFont(p1);
        }
        else
          CDocWriteError("Invalid Start Font Command %s", line1);
      }

      /* End Current Font */

      else if (strcmp(&line1[8], "end_font") == 0)
        endFont();

      /* Record Section Details */

      else if (strcmp(&line1[8], "section") == 0) {
        if (! printing_) {
          if (p1 != NULL) {
            CStrUtil::skipSpace(&p1);

            CDHelpSectionPanel *section_panel = getHelpPanel(true)->getSectionPanel(true);

            section_panel->addSection(p1);
          }
          else
            CDocWriteError("Invalid Section Command %s", line1);
        }
      }

      /* Record Sub-Section Details */

      else if (strcmp(&line1[8], "sub_section") == 0) {
        if (! printing_) {
          if (p1 != NULL) {
            CStrUtil::skipSpace(&p1);

            CDHelpSectionPanel *section_panel = getHelpPanel(true)->getSectionPanel(true);

            section_panel->addSubSection(p1);
          }
          else
            CDocWriteError("Invalid Sub-Section Command %s", line1);
        }
      }

      /* Current Indentation */

      else if (strcmp(&line1[8], "indent") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          cdoc_tab_indent = CStrUtil::toInteger(p1);
        }
        else
          CDocWriteError("Invalid Indent Command %s", line1);
      }

      /* Load Image Into Pixmap Database */

      else if (strcmp(&line1[8], "xwd"   ) == 0 ||
               strcmp(&line1[8], "xwd.Z" ) == 0 ||
               strcmp(&line1[8], "bitmap") == 0 ||
               strcmp(&line1[8], "image" ) == 0) {
        if (p1 == NULL) {
          CDocWriteError("Invalid Image Command");
          goto next_line;
        }

        CStrUtil::skipSpace(&p1);

        char *p2 = p1;

        CStrUtil::skipNonSpace(&p2);

        int size   = 0;
        int resize = false;
        int rotate = 0;

        if (*p2 != '\0') {
          *p2 = '\0';

          char *p3 = p2 + 1;

          CStrUtil::skipSpace(&p3);

          std::vector<std::string> words;

          CStrUtil::addWords(p3, words);

          if (words.size() > 0)
            size = CStrUtil::toInteger(words[0]);

          for (uint i = 1; i < words.size(); i++) {
            if      (CStrUtil::casecmp(words[i], "resize"   ) == 0) resize = true;
            else if (CStrUtil::casecmp(words[i], "rotate90" ) == 0) rotate = 90;
            else if (CStrUtil::casecmp(words[i], "rotate180") == 0) rotate = 180;
            else if (CStrUtil::casecmp(words[i], "rotate270") == 0) rotate = 270;
          }
        }

        /***********/

        char *filename = new char [getProgramHelps()->getDirectory().size() + strlen(p1) + 16];

        if (*p1 == '/' || getProgramHelps()->getDirectory() == "")
          strcpy(filename, p1);
        else
          sprintf(filename, "%s/%s", getProgramHelps()->getDirectory().c_str(), p1);

        CImagePtr image;

        if (strcmp(&line1[8], "xwd.Z" ) == 0)
          image = CImagePtr();
        else
          image = CImageMgrInst->lookupImage(filename);

        /***********/

        int width1, height1;

        charExtent(CDOC_MEAN_CHAR, &width1, &height1);

        int size1;

        if (size > 0)
          size1 = size*height1;
        else
          size1 = 4*height1;

        if (image.isValid()) {
          if (! resize) {
            int size2;

            char *env = getenv("CDOC_IMAGE_DEBUG");

            if (rotate == 90 || rotate == 270) {
              if ((int) image->getHeight() > *page_width1)
                *page_width1 = image->getHeight();

              *page_height += image->getWidth();

              page->line_heights[page->line_no - 1] = image->getWidth();

              size2 = image->getWidth()/height1;

              if (image->getWidth() % height1 != 0)
                size2 += 1;
            }
            else {
              if ((int) image->getWidth() > *page_width1)
                *page_width1 = image->getWidth();

              *page_height += image->getHeight();

              page->line_heights[page->line_no - 1] = image->getHeight();

              size2 = image->getHeight()/height1;

              if (image->getHeight() % height1 != 0)
                size2 += 1;
            }

            if (size2 != size && env != NULL)
              CDocWriteError("%s '%s': Actual %d, Supplied %d",
                             "Image Lines MisMatch for", p1, size2, size);
          }
          else {
            double scale;

            if (rotate == 90 || rotate == 270) {
              scale = ((double) size1)/image->getWidth();

              if (image->getHeight()*scale > *page_width1)
                *page_width1 = int(image->getHeight()*scale);

              *page_height += size1;

              page->line_heights[page->line_no - 1] = size1;
            }
            else {
              scale = ((double) size1)/image->getHeight();

              if (image->getWidth()*scale > *page_width1)
                *page_width1 = int(image->getWidth()*scale);

              *page_height += size1;

              page->line_heights[page->line_no - 1] = size1;
            }
          }
        }
        else {
          if (strcmp(&line1[8], "xwd.Z" ) != 0)
            CDocWriteError("Image File %s - Not Found", filename);

          *page_height += size1;

          page->line_heights[page->line_no - 1] = size1;
        }

        delete [] filename;
      }

      /* Add Command Button and add to Command panel */

      else if (strcmp(&line1[8], "command") == 0) {
        /* Replace tab after command title with NULL so it can be extracted */

        char *p2 = NULL;

        if (p1 != NULL)
          p2 = strchr(p1, '\t');

        if (p2 != NULL) {
          *p2 = '\0';

          p2++;

          char *blanks = CStrUtil::strdup(p1);

          char *p3 = blanks;

          CStrUtil::skipSpace(&p3);

          if (! printing_)
            addCommand(p3, p2);

          int width2, height2;

          buttonExtent(p3, &width2, &height2);

          *p3 = '\0';

          int width1, height1;

          stringExtent(blanks, &width1, &height1);

          if (width1 + width2 > *page_width1)
            *page_width1 = width1 + width2;

          *page_height += height2;

          page->line_heights[page->line_no - 1] = height2;

          delete [] blanks;
        }
      }

      /* Draw a Line */

      else if (strcmp(&line1[8], "draw_line") == 0) {
        int no = 0;

        char temp_string1[32];
        char temp_string2[32];
        char temp_string3[32];
        char temp_string4[32];

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %s %s", temp_string1, temp_string2, temp_string3, temp_string4);

        if (no != 4) {
          CDocWriteError("Invalid Draw Line Command %s", line1);
          goto next_line;
        }

        int px1 = charXToPixel(page, temp_string1);
        int px2 = charXToPixel(page, temp_string3);

        if (px1 > *page_width1) *page_width1 = px1;
        if (px2 > *page_width1) *page_width1 = px2;
      }

      /* Draw a Box */

      else if (strcmp(&line1[8], "draw_box") == 0) {
        int no = 0;

        char temp_string1[32];
        char temp_string2[32];
        char temp_string3[32];
        char temp_string4[32];

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %s %s", temp_string1, temp_string2, temp_string3, temp_string4);

        if (no != 4) {
          CDocWriteError("Invalid Draw Box Command %s", line1);
          goto next_line;
        }

        int px1 = charXToPixel(page, temp_string1);
        int px2 = charXToPixel(page, temp_string3);

        if (px1 > *page_width1) *page_width1 = px1;
        if (px2 > *page_width1) *page_width1 = px2;
      }

      /* Fill a Box */

      else if (strcmp(&line1[8], "fill_box") == 0) {
        int no = 0;

        char temp_string1[32];
        char temp_string2[32];
        char temp_string3[32];
        char temp_string4[32];

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %s %s", temp_string1, temp_string2, temp_string3, temp_string4);

        if (no != 4) {
          CDocWriteError("Invalid Fill Box Command %s", line1);
          goto next_line;
        }

        int px1 = charXToPixel(page, temp_string1);
        int px2 = charXToPixel(page, temp_string3);

        if (px1 > *page_width1) *page_width1 = px1;
        if (px2 > *page_width1) *page_width1 = px2;
      }

      /* Draw a String */

      else if (strcmp(&line1[8], "draw_string") == 0) {
        int no  = 0;
        int pos = 0;

        char temp_string1[32];
        char temp_string2[32];

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %n", temp_string1, temp_string2, &pos);

        if (no != 2 || p1[pos] == '\0') {
          CDocWriteError("Invalid Draw String Command %s", line1);
          goto next_line;
        }

        int px = charXToPixel(page, temp_string1);

        int width1, height1;

        stringExtent(&p1[pos], &width1, &height1);

        if (px + width1 > *page_width1) *page_width1 = px + width1;
      }

      /* Set Draw Color */

      else if (strcmp(&line1[8], "set_color") == 0 ||
               strcmp(&line1[8], "set_colour") == 0) {
      }

      /* Set Draw String Font Name */

      else if (strcmp(&line1[8], "set_font_name") == 0)
        ;

      /* Set Draw String Font Style */

      else if (strcmp(&line1[8], "set_font_style") == 0)
        ;

      /* Set Draw Line/Box Thickness */

      else if (strcmp(&line1[8], "set_thickness") == 0)
        ;

      /* Set Fill Pixmap Name */

      else if (strcmp(&line1[8], "set_fill_pixmap") == 0)
        ;

      /* Begin Paragraph */

      else if (strcmp(&line1[8], "paragraph_begin") == 0) {
        int right_margin = getRightMargin();

        if (p1 != NULL) {
          int no = sscanf(p1, "%d", &right_margin);

          if (no != 1)
            right_margin = getRightMargin();

          charExtent(CDOC_MEAN_CHAR, &width, &height);

          if (width*right_margin > *page_width)
            *page_width = width*right_margin;
        }
      }

      /* End Paragraph */

      else if (strcmp(&line1[8], "paragraph_end") == 0)
        ;

      /* Start Paragraph */

      else if (strcmp(&line1[8], "paragraph_start") == 0) {
        int right_margin = getRightMargin();

        if (p1 != NULL) {
          int no = sscanf(p1, "%d", &right_margin);

          if (no != 1)
            right_margin = getRightMargin();

          charExtent(CDOC_MEAN_CHAR, &width, &height);

          if (width*right_margin > *page_width)
            *page_width = width*right_margin;
        }
      }

      /* Stop Paragraph */

      else if (strcmp(&line1[8], "paragraph_stop") == 0)
        ;

      /* Start Goto */

      else if (strcmp(&line1[8], "goto_start") == 0)
        ;

      /* End Goto */

      else if (strcmp(&line1[8], "goto_end") == 0)
        ;

      /* Start Color */

      else if (strcmp(&line1[8], "color_start") == 0 ||
               strcmp(&line1[8], "colour_start") == 0) {
      }

      /* Start Font Type */

      else if (strcmp(&line1[8], "font_start") == 0)
        ;

      /* Revision Control */

      else if (strcmp(&line1[8], "rev_cntrl") == 0)
        ;

      /* Set Line */

      else if (strcmp(&line1[8], "set_line") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          int no_lines = CStrUtil::toInteger(p1);

          int i1 = 1;

          while (no_lines < 0) {
            *page_height -=
              page->line_heights[page->line_no - i1 - 1];

            i1++;

            no_lines++;
          }
        }
        else
          CDocWriteError("Invalid Set Line Command %s", line1);
      }

      /* Centre Next Line */

      else if (strcmp(&line1[8], "left") == 0)
        ;

      /* Centre Next Line */

      else if (strcmp(&line1[8], "centre") == 0)
        ;

      /* Centre Next Line */

      else if (strcmp(&line1[8], "right") == 0)
        ;

      /* Centre Next Line */

      else if (strcmp(&line1[8], "justify") == 0)
        ;

      /* Formatting On/Off */

      else if (strcmp(&line1[8], "format") == 0) {
        if      (p1 != NULL && strcmp(p1, "on") == 0)
          page_width = save_page_width;
        else if (p1 != NULL && strcmp(p1, "off") == 0) {
          save_page_width = page_width;
          page_width      = page_width1;
        }
      }
    }

    /* Process Normal Text Line */

    else {
      int j = 0;

      int x = 0;
      int y = 0;

      if (line1[j] == '\t') {
        x = cdoc_tab_indent*cdoc_indent_char_width;

        if (height > y)
          y = height;

        j++;
      }

      int k = j;

      CStrUtil::skipSpace(line1, &j);

      charExtent(SPACE_CHAR, &width, &height);

      x += (j - k)*width;

      if (height > y)
        y = height;

      k = 0;

      if (line1[j] != '\0') {
        char *str = CStrUtil::strdup(&line1[j]);

        int no;

        while (line1[j] != '\0') {
          if      (line1[j] == '\t') {
            /* Calculate size of string */

            str[k] = '\0';

            stringExtent(str, &width, &height);

            x += width;

            if (height > y)
              y = height;

            k = 0;

            stringExtent("        ", &width, &height);

            x += width;

            if (height > y)
              y = height;

            j++;
          }
          else if (isspace(line1[j])) {
            str[k] = '\0';

            stringExtent(str, &width, &height);

            x += width;

            if (height > y)
              y = height;

            k = 0;

            charExtent(SPACE_CHAR, &width, &height);

            for ( ; line1[j] != '\0' && isspace(line1[j]); j++)
              x += width;
          }
          else if ((no = CDocIsEscapeCode(&line1[j])) != 0) {
            /* Calculate size of string */

            str[k] = '\0';

            stringExtent(str, &width, &height);

            x += width;

            if (height > y)
              y = height;

            k = 0;

            /* If Font Escape Code then switch to normal, bold, italic or bold italic font */

            if      (CDOC_IS_FONT_ESC(&line1[j]))
              setFontType(line1[j + 2] - '0');

            /* If Inline Font Escape Code then start/end font */

            else if (CDOC_IS_INLINE_FONT_ESC(&line1[j])) {
              if (line1[j + 1] == 'E')
                endFont();
              else {
                int j1 = j + 2;

                int c = line1[j1++];

                std::string temp_string;

                while (line1[j1] != '\0' && line1[j1] != c)
                  temp_string += line1[j1++];

                startFont(temp_string);
              }
            }

            /* If fill escape then calculate padding size */

            else if (CDOC_IS_FILL_ESC(&line1[j])) {
              int j1 = j + 2;

              std::string temp_string;

              while (line1[j1] != '\0' && isdigit(line1[j1]))
                temp_string += line1[j1++];

              int start = CStrUtil::toInteger(temp_string);

              j1++;

              temp_string = "";

              while (line1[j1] != '\0' && isdigit(line1[j1]))
                temp_string += line1[j1++];

              int end = CStrUtil::toInteger(temp_string);

              j1++;

              charExtent(CDOC_MEAN_CHAR, &width, &height);

              if (width*start + cdoc_fill_pad < x)
                cdoc_fill_pad = x - width*start;

              x = width*start + cdoc_fill_pad;

              if (height > y)
                y = height;

              charExtent(line1[j1], &width, &height);

              x = width*(end + 1) + cdoc_fill_pad;

              if (height > y)
                y = height;
            }

            j += no;
          }

          /* Add Word Characters */

          else
            str[k++] = line1[j++];
        }

        str[k] = '\0';

        stringExtent(str, &width, &height);

        x += width;

        if (height > y)
          y = height;

        delete [] str;
      }

      if (x > *page_width)
        *page_width = x;

      *page_height += y;

      page->line_heights[page->line_no - 1] = y;
    }

 next_line:
    /* New Page */

    if (line1[0] == NEW_PAGE) {
      page->no_lines = page->line_no - 1;

      /*-----------*/

      page = new CDHelpDatasetPage;

      page->position          = ftell(getFp());
      page->header_width      = 0;
      page->header_height     = 0;
      page->body_width        = 0;
      page->body_height       = 0;
      page->footer_width      = 0;
      page->footer_height     = 0;
      page->page_width1       = 0;
      page->no_lines          = 0;
      page->max_lines         = 100;
      page->line_heights      = new int [page->max_lines];
      page->line_no           = 1;
      page->help_dataset_data = this;

      addPage(page);

      page_width1 = &page->page_width1;

      page_width  = &page->body_width;
      page_height = &page->body_height;

      /*-----------*/

      setPageNum(getPageNum() + 1);
    }
    else {
      /* Allocate Space for More Line Heights */

      if (page->line_no >= page->max_lines) {
        page->max_lines += 100;

        int *line_heights = new int [page->max_lines];

        memcpy(line_heights, page->line_heights, page->line_no*sizeof(int));

        delete [] page->line_heights;

        page->line_heights = line_heights;
      }

      /* Update Line Number */

      page->line_no++;
    }
  }

  /*********/

  page->no_lines = page->line_no - 1;

  /*********/

  startFont("");

  if (printing_) {
    charExtent(CDOC_MEAN_CHAR, &width, &height);

    ps_max_page_width = cdoc_right_margin*width;

    ps_max_page_width1 = 0;

    ps_max_header_height = 0;
    ps_max_body_height   = height;
    ps_max_footer_height = 0;

    uint num_pages = getNumPages();

    for (uint i = 0; i < num_pages; ++i) {
      CDHelpDatasetPage *page = page_list_[i];

      if (page->header_width > ps_max_page_width) ps_max_page_width = page->header_width;
      if (page->body_width   > ps_max_page_width) ps_max_page_width = page->body_width;
      if (page->footer_width > ps_max_page_width) ps_max_page_width = page->footer_width;

      if (page->page_width1 > ps_max_page_width1) ps_max_page_width1 = page->page_width1;

      if (page->header_height > ps_max_header_height) ps_max_header_height = page->header_height;
      if (page->body_height   > ps_max_body_height  ) ps_max_body_height   = page->body_height;
      if (page->footer_height > ps_max_footer_height) ps_max_footer_height = page->footer_height;
    }

    if (ps_max_page_width > ps_max_page_width1) ps_max_page_width1 = ps_max_page_width;

    if (ps_max_header_height <= 0) ps_max_header_height = height/2;
    if (ps_max_footer_height <= 0) ps_max_footer_height = height/2;

    ps_max_page_height = ps_max_header_height + ps_max_body_height + ps_max_footer_height;

    /***********/

    /* Write the Postscript File Header Block */

    writePSHeader();
  }
  else {
    updatePageSizes();
  }

  cdoc_tab_indent = 0;

  return true;
}

// Free all resources used by the page display structures.
void
CDHelpDatasetData::
termDrawPages()
{
  if (printing_)
    writePSTrailer();

  /**********/

  resetPages();

  /**********/

  if (! ps_defined_font_list.empty()) {
    for_each(ps_defined_font_list.begin(), ps_defined_font_list.end(), CDeletePointer());

    ps_defined_font_list.clear();
  }

  /**********/

  current_draw_color          = "";
  current_draw_font_name      = "";
  current_draw_font_style     = "";
  current_draw_line_thickness = 0;
  current_draw_fill_pixmap    = "";
}

// Read the Adode Font Metrics (afm) files for Courier, Helvetica and Times
// for for the four font styles (Normal, Bold, Italic and Bold Italic).
//
// The 'CDOC_PS_DATA_DIR' environment variable should point to the directory
// containing the required files.
bool
CDHelpDatasetData::
loadFontMetrics()
{
  if (ps_font_metrics) return true;

  char *ps_dir = getenv("AFM_DATA_DIR");

  char dir[256];

  if (ps_dir != NULL) {
    strcpy(dir, ps_dir);
    strcat(dir, "/");
  }
  else
    strcpy(dir, "");

  ps_font_metrics = new CFontMetrics(dir);

  return true;
}

// Write the Postscript Header block to the print file.
void
CDHelpDatasetData::
writePSHeader()
{
  int max_page_width  = CDocMax(ps_max_page_width, ps_max_page_width1);
  int max_page_width1 = max_page_width + max_page_width/20;

  /* Determine whether two data pages should be printed on one printer page */

  CDHelpPrintPanel *print_panel = getHelpPanel()->getPrintPanel();

  int per_page = print_panel->getPerPage();

  ps_print.setPerPage(per_page);

  /*------------------------------------*/

  /* Calculate Aspect Ratio of Page */

  double aspect = ((double) max_page_width)/((double) ps_max_page_height);

  /*------------------------------------*/

  /* Determine whether page should be printed in Portrait or Landscape Orientation */

  int orient = print_panel->getOrient();

  /* If orientation is automatic then use portrait is aspect is less than 1 and
     lanscape if it is greater than 1 */

  if (orient == CDOC_PRINT_AUTOMATIC) {
    if (aspect <= 1.0)
      orient = CDOC_PRINT_PORTRAIT;
    else
      orient = CDOC_PRINT_LANDSCAPE;
  }

  ps_print.setOrientation(orient == CDOC_PRINT_PORTRAIT ? CPrint::PORTRAIT : CPrint::LANDSCAPE);

  /*------------------------------------*/

  ps_print.setPageSize(max_page_width1, ps_max_page_height);

  /*------------------------------------*/

  ps_print.writeHeader();

  /*------------------------*/

  startFont("");
}

// Write the Postscript Trailer block to the print file.
void
CDHelpDatasetData::
writePSTrailer()
{
  /* Determine whether two data pages should be printed on one printer page */

  CDHelpPrintPanel *print_panel = getHelpPanel()->getPrintPanel();

  int per_page = print_panel->getPerPage();

  bool showpage = ((last_page && last_page->line_no != 1) || getPageNum() % per_page != 1);

  ps_print.writeFooter(showpage);
}

// Get the Normal, Bold, Underline and Bold Underline X font names for the specified
// font str and store them using the supplied font name as a key.
//
// The font str is a non-fully specified X font name with an asterisk character in
// positions where the X font part definition is unspecified e.g. the str
// "-*-courier-*-*-*-*-12-*-*-*-*-*-*-*" specified any courier 12 point font. The
// routine searches all X font names which match this non-fully specified X font name
// an assigns the Normal, Bold, Underline and Bold Underline font names to those which
// match the style required. If no match is found a default font is used.
void
CDHelpDatasetData::
defineFont(const std::string &font_name, const std::string &font_pattern)
{
  if (printing_) {
    CDFontData *font = new CDFontData;

    font->name = font_name;

    CFontSet fontSet(font_pattern);

    CFontPtr normal = fontSet.getFont(CFONT_STYLE_NORMAL);

    if (normal->isProportional()) {
      font->fixed  = false;
      font->family = CFontMetrics::HELVETICA;
      font->normal = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_NORMAL);
      font->bold   = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_BOLD);
      font->italic = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_ITALIC);
      font->boldi  = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_BOLD_ITALIC);
    }
    else {
      font->fixed  = true;
      font->family = CFontMetrics::COURIER;
      font->normal = ps_font_metrics->getCourierMetric(CFONT_STYLE_NORMAL);
      font->bold   = ps_font_metrics->getCourierMetric(CFONT_STYLE_BOLD);
      font->italic = ps_font_metrics->getCourierMetric(CFONT_STYLE_ITALIC);
      font->boldi  = ps_font_metrics->getCourierMetric(CFONT_STYLE_BOLD_ITALIC);
    }

    font->size    = normal->getCharHeight();
    font->current = font->normal;

    ps_defined_font_list.push_back(font);
  }
  else {
    CDHelpDatasetFont *font = new CDHelpDatasetFont;

    font->name = font_name;

    CFontSet fontSet(font_pattern);

    font->normal = fontSet.getFont(CFONT_STYLE_NORMAL);
    font->bold   = fontSet.getFont(CFONT_STYLE_BOLD);
    font->under  = fontSet.getFont(CFONT_STYLE_ITALIC);
    font->boldu  = fontSet.getFont(CFONT_STYLE_BOLD_ITALIC);

    addDefinedFont(font);
  }
}

// Start a New Font of a Supplied Name.
//
// If the name has been defined using a 'def_font' command then the normal, bold,
// underline and bold underline font names are set to the names generated for that
// font, otherwise the default font names are used.
void
CDHelpDatasetData::
startFont(const std::string &font_name)
{
  if (printing_) {
    //startFont(font_name);

    CDFontData *font = new CDFontData;

    if (font_name != "") {
      int no = ps_defined_font_list.size();

      int i = 1;

      CDFontData *font1;

      for ( ; i <= no; i++) {
        font1 = ps_defined_font_list[i - 1];

        if (CStrUtil::casecmp(font1->name, font_name) == 0)
          break;
      }

      if (i < no + 1) {
        font->name   = font_name;
        font->fixed  = font1->fixed;
        font->size   = font1->size;
        font->family = font1->family;
        font->normal = font1->normal;
        font->bold   = font1->bold;
        font->italic = font1->italic;
        font->boldi  = font1->boldi;
      }
      else {
        if (font_name == "xmpfont") {
          font->name   = CStrUtil::strdup("xmpfont");
          font->fixed  = true;
          font->size   = 12;
          font->family = CFontMetrics::COURIER;
          font->normal = ps_font_metrics->getCourierMetric(CFONT_STYLE_NORMAL);
          font->bold   = ps_font_metrics->getCourierMetric(CFONT_STYLE_BOLD);
          font->italic = ps_font_metrics->getCourierMetric(CFONT_STYLE_ITALIC);
          font->boldi  = ps_font_metrics->getCourierMetric(CFONT_STYLE_BOLD_ITALIC);
        }
        else {
          font->name   = CStrUtil::strdup("default");
          font->fixed  = false;
          font->size   = 12;
          font->family = CFontMetrics::HELVETICA;
          font->normal = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_NORMAL);
          font->bold   = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_BOLD);
          font->italic = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_ITALIC);
          font->boldi  = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_BOLD_ITALIC);
        }
      }
    }
    else {
      font->name   = CStrUtil::strdup("default");
      font->fixed  = false;
      font->size   = 12;
      font->family = CFontMetrics::HELVETICA;
      font->normal = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_NORMAL);
      font->bold   = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_BOLD);
      font->italic = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_ITALIC);
      font->boldi  = ps_font_metrics->getHelveticaMetric(CFONT_STYLE_BOLD_ITALIC);
    }

    font->current = font->normal;

    ps_current_font_list.push_back(font);

    ps_current_font = font;

    setFontType(0);
  }
  else {
    CDHelpDatasetFont *font = new CDHelpDatasetFont;

    addCurrentFont(font);

    if (font_name != "") {
      int no = getNumDefinedFonts();

      int i = 0;

      for ( ; i < no; ++i) {
        CDHelpDatasetFont *font1 = getDefinedFont(i);

        if (CStrUtil::casecmp(font1->name, font_name) == 0) {
          font->name   = font1->name;
          font->normal = font1->normal;
          font->bold   = font1->bold;
          font->under  = font1->under;
          font->boldu  = font1->boldu;

          break;
        }
      }

      if (no == 0 || i >= no) {
        font->name   = "default";
        font->normal = getDefaultFont()->normal;
        font->bold   = getDefaultFont()->bold;
        font->under  = getDefaultFont()->under;
        font->boldu  = getDefaultFont()->boldu;
      }
    }
    else {
      font->name   = "default";
      font->normal = getDefaultFont()->normal;
      font->bold   = getDefaultFont()->bold;
      font->under  = getDefaultFont()->under;
      font->boldu  = getDefaultFont()->boldu;
    }

    setCurrentFont(font);
  }

  if (font_name != "" && ! cdoc_base_font_set) {
    /* Set Indent Width */

    int width, height;

    charExtent(INDENT_CHAR, &width, &height);

    cdoc_indent_char_width = width;

    cdoc_base_font_set = true;
  }
}

// Delete the Current Font and revert to the previous font.
void
CDHelpDatasetData::
endFont()
{
  if (printing_) {
    //endFont();

    CDFontDataList::iterator p =
      find(ps_current_font_list.begin(), ps_current_font_list.end(), ps_current_font);

    if (p != ps_current_font_list.end())
      ps_current_font_list.erase(p);

    delete ps_current_font;

    ps_current_font = ps_current_font_list.back();

    if (ps_current_font != NULL)
      setFontType(0);
    else
      startFont("");
  }
  else {
    CDHelpDatasetFont *font = getCurrentFont();

    removeCurrentFont(font);

    delete font;

    font = getCurrentFont();

    setCurrentFont(font);

    if (font == NULL)
      startFont("");
  }
}

// Add a Help Command to the Command Panel.
void
CDHelpDatasetData::
addCommand(const std::string &name, const std::string &command)
{
  // Get Panel
  CDHelpCommandPanel *command_panel = help_panel_->getCommandPanel();

  /* Add Command String */
  command_panel->addItem(name);

  /* Add Command Information To List */
  CDHelpDatasetCommandData *command_data = new CDHelpDatasetCommandData(this);

  command_data->setTitle(name);

  command_data->setCommand(command);

  command_panel->addData(command_data);
}

// Load the current page from the Help Dataset into the Panel.
void
CDHelpDatasetData::
loadPage(CDHelpDatasetPage *page)
{
  CDHelpPrintPanel *print_panel = NULL;

  if (getHelpPanel())
    print_panel = getHelpPanel()->getPrintPanel();

  /* If Print Panel exists update Start and End Page to Current */
  if (print_panel)
    print_panel->setPageRange(getPageNum(), getPageNum());

  /**************/

  /* Remove any Existing Command Button Areas */

  resetCommandData();

  /* Remove any Existing Cross Reference Areas */

  resetRefData();

  /**********/

  /* Move to the required position in the file for the page
     number specified. If the page number is invalid then fail */

  if (getFp() == NULL)
    return;

  fseek(getFp(), page->position, SEEK_SET);

  page->line_no = 1;

  /**********/

  /* Update Page Selection Panel */

  CDHelpPageNoPanel *page_no_panel = NULL;

  if (getHelpPanel())
    page_no_panel = getHelpPanel()->getPageNoPanel();

  if (page_no_panel != NULL)
    page_no_panel->setPage(getPageNum());

  /*********/

  /* Set Dataset Panel Title */

  CDHelpPanel *help_panel = getHelpPanel();

  if (help_panel) {
    std::string title = getTitle();

    title += " (Page ";
    title += CStrUtil::toString(getPageNum());
    title += " of ";
    title += CStrUtil::toString(getNumPages());
    title += ")";

    help_panel->setWindowTitle(title.c_str());
  }
}

void
CDHelpDatasetData::
drawPage(CDHelpDatasetPage *page)
{
  if (page != last_page) {
    freeImages();

    last_page = page;
  }

  //------

  CPixelRenderer *renderer;

  if (printing_) {
    page->line_no = 1;

    fseek(getFp(), page->position, SEEK_SET);

    initPSPage();
  }
  else {
    renderer = getRenderer();

    renderer->beginDraw();

    renderer->setForeground(CRGBA(0,0,0));
    renderer->setBackground(CRGBA(1,1,1));

    renderer->clear();

    //------

    if (page == getSelectPage())
      startSelect();
  }

  //------

  int align = CHALIGN_TYPE_LEFT;

  int header_y = 0;
  int body_y   = getMaxHeaderHeight();
  int footer_y = getMaxHeaderHeight() + getMaxBodyHeight();

  int *y = &body_y;

  long pos = ftell(getFp());

  bool processing = true;

  std::string line;

  int x = 0;

  while (CDocReadLineFromFile(getFp(), line)) {
    char *line1 = (char *) line.c_str();

    x = 0;

    /* Ignore line if not processing */

    if (! processing) {
      if (getHelp()->getFormat() == CDOC_FORMAT_CDOC &&
          strncmp(line1, CDOC_CMD_ID, CDOC_CMD_ID_LEN) == 0) {
        if ((! getShowHeader() && strncmp(&line1[8], "end_header", 10) == 0) ||
            (! getShowFooter() && strncmp(&line1[8], "end_footer", 10) == 0)) {
          processing = true;

          y = &body_y;
        }
      }

      goto drawPage_1;
    }

    /* End of Page */

    if (line1[0] == NEW_PAGE)
      break;

    /* Process CDoc Command */

    if (getHelp()->getFormat() == CDOC_FORMAT_CDOC &&
        strncmp(line1, CDOC_CMD_ID, CDOC_CMD_ID_LEN) == 0) {
      /* Replace space after command name with NULL
         so we can use 'strcmp' on the command name */

      char *p1 = strchr(&line1[8], ' ');

      if (p1 != NULL) {
        *p1 = '\0';

        p1++;
      }

      /* Page Parts */

      if      (strcmp(&line1[8], "start_header") == 0) {
        if (! getShowHeader()) {
          processing = false;

          goto drawPage_1;
        }

        y = &header_y;
      }
      else if (strcmp(&line1[8], "end_header") == 0)
        y = &body_y;
      else if (strcmp(&line1[8], "start_footer") == 0) {
        if (! getShowHeader()) {
          processing = false;

          goto drawPage_1;
        }

        y = &footer_y;
      }
      else if (strcmp(&line1[8], "end_footer") == 0)
        y = &body_y;

      /* Page Dimensions (already processed) */

      else if (strcmp(&line1[8], "left_margin") == 0)
        ;
      else if (strcmp(&line1[8], "right_margin") == 0)
        ;
      else if (strcmp(&line1[8], "page_length") == 0)
        ;

      /* Define Font Mapping (already processed) */

      else if (strcmp(&line1[8], "def_font") == 0)
        ;

      /* Start New Font */

      else if (strcmp(&line1[8], "start_font") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          startFont(p1);
        }
      }

      /* End Current Font */

      else if (strcmp(&line1[8], "end_font") == 0)
        endFont();

      /* Section (already processed) */

      else if (strcmp(&line1[8], "section") == 0)
        ;

      /* Sub-Section (already processed) */

      else if (strcmp(&line1[8], "sub_section") == 0)
        ;

      /* Current Indentation */

      else if (strcmp(&line1[8], "indent") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          cdoc_tab_indent = CStrUtil::toInteger(p1);
        }

        if (paragraph != NULL) {
          if (paragraph->getNumLines() == 0)
            paragraph->setIndent1(cdoc_tab_indent);
          else {
            int right_margin = paragraph->getRightMargin();

            endParagraph(page, align, x, *y, true);

            beginParagraph(page, x, *y, right_margin, true);
          }
        }
      }

      /* Display Bitmap or Image */

      else if (strcmp(&line1[8], "xwd"   ) == 0 ||
               strcmp(&line1[8], "xwd.Z" ) == 0 ||
               strcmp(&line1[8], "bitmap") == 0 ||
               strcmp(&line1[8], "image" ) == 0) {
        if (p1 == NULL)
          goto drawPage_1;

        CStrUtil::skipSpace(&p1);

        char *p2 = p1;

        CStrUtil::skipNonSpace(&p2);

        int size   = 0;
        int resize = false;
        int rotate = 0;

        if (*p2 != '\0') {
          *p2 = '\0';

          char *p3 = p2 + 1;

          CStrUtil::skipSpace(&p3);

          std::vector<std::string> words;

          CStrUtil::addWords(p3, words);

          if (words.size() > 0)
            size = CStrUtil::toInteger(words[0]);

          for (uint i = 1; i < words.size(); i++) {
            if      (CStrUtil::casecmp(words[i], "resize"   ) == 0)
              resize = true;
            else if (CStrUtil::casecmp(words[i], "rotate90" ) == 0)
              rotate = 90;
            else if (CStrUtil::casecmp(words[i], "rotate180") == 0)
              rotate = 180;
            else if (CStrUtil::casecmp(words[i], "rotate270") == 0)
              rotate = 270;
          }
        }

        /***********/

        char *filename = new char [getProgramHelps()->getDirectory().size() + strlen(p1) + 16];

        if (*p1 == '/' || getProgramHelps()->getDirectory() == "")
          strcpy(filename, p1);
        else
          sprintf(filename, "%s/%s", getProgramHelps()->getDirectory().c_str(), p1);

        if (strcmp(&line1[8], "xwd.Z" ) == 0) {
          std::string temp_file = CDocInst->getTempFileName();

          char temp_string[256];

          sprintf(temp_string, "cp %s %s.Z", filename, temp_file.c_str());

          system(temp_string);

          sprintf(temp_string, "uncompress %s.Z", temp_file.c_str());

          system(temp_string);

          drawImage(filename, temp_file, size, resize, rotate, &x, y);

          remove(temp_file.c_str());
        }
        else
          drawImage(filename, filename, size, resize, rotate, &x, y);

        delete [] filename;
      }

      /* Add Command Button */

      else if (strcmp(&line1[8], "command") == 0) {
        /* Replace tab after command title with NULL so it can be extracted */

        char *p2 = NULL;

        if (p1 != NULL)
          p2 = strchr(p1, '\t');

        if (p2 == NULL)
          goto drawPage_1;

        *p2 = '\0';

        p2++;

        char *blanks = CStrUtil::strdup(p1);

        char *p3 = blanks;

        CStrUtil::skipSpace(&p3);

        std::string title = std::string(p3);

        int width2, height2;

        buttonExtent(p3, &width2, &height2);

        *p3 = '\0';

        int width1, height1;

        stringExtent(blanks, &width1, &height1);

        delete [] blanks;

        x += width1;

        addButton(title, p2, x, *y, width2, height2);

        drawButton(title, &x, y, CDOC_BUTTON_OUT);

        *y += height2;
      }

      /* Draw a Line */

      else if (strcmp(&line1[8], "draw_line") == 0) {
        char x1[32];
        char y1[32];
        char x2[32];
        char y2[32];

        int no = 0;

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %s %s", x1, y1, x2, y2);

        if (no != 4)
          goto drawPage_1;

        int px1 = charXToPixel(page, x1);
        int py1 = charYToPixel(page, y1);
        int px2 = charXToPixel(page, x2);
        int py2 = charYToPixel(page, y2);

        drawLine(px1, py1 + *y, px2, py2 + *y);
      }

      /* Draw a Box */

      else if (strcmp(&line1[8], "draw_box") == 0) {
        char x1[32];
        char y1[32];
        char x2[32];
        char y2[32];

        int no = 0;

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %s %s", x1, y1, x2, y2);

        if (no != 4)
          goto drawPage_1;

        int px1 = charXToPixel(page, x1);
        int py1 = charYToPixel(page, y1);
        int px2 = charXToPixel(page, x2);
        int py2 = charYToPixel(page, y2);

        drawBox(px1, py1 + *y, px2, py2 + *y);
      }

      /* Fill a Box */

      else if (strcmp(&line1[8], "fill_box") == 0) {
        char x1[32];
        char y1[32];
        char x2[32];
        char y2[32];

        int no = 0;

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %s %s", x1, y1, x2, y2);

        if (no != 4)
          goto drawPage_1;

        int px1 = charXToPixel(page, x1);
        int py1 = charYToPixel(page, y1);
        int px2 = charXToPixel(page, x2);
        int py2 = charYToPixel(page, y2);

        fillBox(px1, py1 + *y, px2, py2 + *y);
      }

      /* Draw a String */

      else if (strcmp(&line1[8], "draw_string") == 0) {
        int  pos1;
        char x1[32];
        char y1[32];

        int no = 0;

        if (p1 != NULL)
          no = sscanf(p1, "%s %s %n", x1, y1, &pos1);

        if (no != 2 || p1[pos1] == '\0')
          goto drawPage_1;

        int px = charXToPixel(page, x1);
        int py = charYToPixel(page, y1);

        drawText(px, py + *y, &p1[pos1]);
      }

      /* Set Draw Color */

      else if (strcmp(&line1[8], "set_color") == 0 ||
               strcmp(&line1[8], "set_colour") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          setDrawColor(p1);
        }
      }

      /* Set Draw String Font Name */

      else if (strcmp(&line1[8], "set_font_name") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          setDrawFontName(p1);
        }
      }

      /* Set Draw String Font Style */

      else if (strcmp(&line1[8], "set_font_style") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          setDrawFontStyle(p1);
        }
      }

      /* Set Draw Line/Box Thickness */

      else if (strcmp(&line1[8], "set_thickness") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          setDrawLineThickness(p1);
        }
      }

      /* Set Fill Pixmap Name */

      else if (strcmp(&line1[8], "set_fill_pixmap") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          setDrawFillPixmap(p1);
        }
      }

      /* Begin Paragraph */

      else if (strcmp(&line1[8], "paragraph_begin") == 0) {
        int right_margin = getRightMargin();

        if (p1 != NULL) {
          int no = sscanf(p1, "%d", &right_margin);

          if (no != 1)
            right_margin = getRightMargin();
        }

        beginParagraph(page, x, *y, right_margin, false);
      }

      /* End Paragraph */

      else if (strcmp(&line1[8], "paragraph_end") == 0)
        endParagraph(page, align, x, *y, false);

      /* Start Paragraph */

      else if (strcmp(&line1[8], "paragraph_start") == 0) {
        int right_margin = getRightMargin();

        if (p1 != NULL) {
          int no = sscanf(p1, "%d", &right_margin);

          if (no != 1)
            right_margin = getRightMargin();
        }

        beginParagraph(page, x, *y, right_margin, true);
      }

      /* Stop Paragraph */

      else if (strcmp(&line1[8], "paragraph_stop") == 0)
        endParagraph(page, align, x, *y, true);

      /* Start Goto */

      else if (strcmp(&line1[8], "goto_start") == 0) {
        if (p1 != NULL)
          startReference(page, p1, x, *y);
      }

      /* End Goto */

      else if (strcmp(&line1[8], "goto_end") == 0)
        endReference(page, x, *y);

      /* Start Color */

      else if (strcmp(&line1[8], "color_start") == 0 ||
               strcmp(&line1[8], "colour_start") == 0) {
        if (p1 != NULL)
          setColor(CStrUtil::toInteger(p1));
      }

      /* Start Font Type */

      else if (strcmp(&line1[8], "font_start") == 0) {
        if (p1 != NULL)
          setFontType(CStrUtil::toInteger(p1));
      }

      /* Revision Control */

      else if (strcmp(&line1[8], "rev_cntrl") == 0) {
        char rc[2];
        int  in_paragraph;
        int  right_margin;

        rc[0] = *p1;
        rc[1] = '\0';

        if (paragraph != NULL) {
          in_paragraph = true;
          right_margin = paragraph->getRightMargin();

          endParagraph(page, align, x, *y, true);
        }
        else
          in_paragraph = false;

        int x1 = cdoc_indent_char_width/2;
        int y1 = *y;

        drawString(&x1, &y1, rc);

        if (in_paragraph)
          beginParagraph(page, x, *y, right_margin, true);
      }

      /* Set Line */

      else if (strcmp(&line1[8], "set_line") == 0) {
        if (p1 != NULL) {
          CStrUtil::skipSpace(&p1);

          int no_lines = CStrUtil::toInteger(p1);

          int i = 1;

          while (no_lines < 0) {
            *y -= page->line_heights[page->line_no - i - 1];

            i++;

            no_lines++;
          }
        }
      }

      /* Left Next Line */

      else if (strcmp(&line1[8], "left") == 0)
        align = CHALIGN_TYPE_LEFT;

      /* Centre Next Line */

      else if (strcmp(&line1[8], "centre") == 0)
        align = CHALIGN_TYPE_CENTRE;

      /* Right Next Line */

      else if (strcmp(&line1[8], "right") == 0)
        align = CHALIGN_TYPE_RIGHT;

      /* Justify Next Line */

      else if (strcmp(&line1[8], "justify") == 0)
        align = CHALIGN_TYPE_JUSTIFY;

      /* Formatting On/Off (Already processed) */

      else if (strcmp(&line1[8], "format") == 0) {
      }
    }

    /* Add Line to Paragraph */

    else if (paragraph != NULL)
      addParagraphLine(page, pos, line1, &x, y);

    /* Process Normal Text Line */

    else {
      if (align == CHALIGN_TYPE_JUSTIFY) {
        beginParagraph(page, x, *y, getRightMargin(), false);

        addParagraphLine(page, pos, line1, &x, y);

        endParagraph(page, align, x, *y, false);
      }
      else
        drawPageLine(page, pos, line1, align, &x, y, getRightMargin());

      align = CHALIGN_TYPE_LEFT;
    }

 drawPage_1:
    pos = ftell(getFp());

    page->line_no++;
  }

  if (paragraph != NULL) {
    fprintf(stderr, "Line %d : Paragraph not Terminated at End of Page\n", page->line_no);

    endParagraph(page, align, x, *y, false);
  }

  //-------

  if (printing_) {
    if (page->line_no == 1)
      page->line_no++;
  }
  else {
    if (page == getSelectPage())
      endSelect();

    //------

    renderer->endDraw();
  }
}

// Initialise Postscript environment ready for a page to be drawn.
void
CDHelpDatasetData::
initPSPage()
{
  int max_page_width = CDocMax(ps_max_page_width, ps_max_page_width1);

  /* Determine whether two data pages should be printed on one printer page */

  CDHelpPrintPanel *print_panel = getHelpPanel()->getPrintPanel();

  //int per_page = print_panel->getPerPage();

  /* Determine whether page should be printed in Portrait or Landscape Orientation */

  int orient = print_panel->getOrient();

  /*------------------------------------*/

  /* Calculate Aspect Ratio of Page */

  double aspect = ((double) max_page_width)/((double) ps_max_page_height);

  /*------------------------------------*/

  /* If orientation is automatic then use portrait is aspect
     is less than 1 and lanscape if it is greater than 1 */

  if (orient == CDOC_PRINT_AUTOMATIC) {
    if (aspect <= 1.0)
      orient = CDOC_PRINT_PORTRAIT;
    else
      orient = CDOC_PRINT_LANDSCAPE;
  }

  /*------------------------------------*/

  ps_print.startPage(getPageNum(), getPageNum());
}

// Start Highlighting of str containing page reference which when selected will
// move to the specified page.
void
CDHelpDatasetData::
startReference(CDHelpDatasetPage *page, const std::string &page_string, int x, int y)
{
  if (printing_) return;

  if (cdoc_ref_page_no != -1) {
    fprintf(stderr, "Line %d : Ref not Terminated at Start of Ref\n", page->line_no);
    return;
  }

  cdoc_ref_page_no = CStrUtil::toInteger(page_string);
  cdoc_ref_line_no = page->line_no;
  cdoc_ref_x       = x;
  cdoc_ref_y       = y;

  CPixelRenderer *renderer = getRenderer();

  renderer->getForeground(reference_save_fg);

  renderer->setForeground(hyper_color);
}

// End Highlighting of str containing page reference which when selected will move
// to the specified page.
void
CDHelpDatasetData::
endReference(CDHelpDatasetPage *page, int x, int)
{
  if (printing_) return;

  CDHelpDatasetRefData  *ref_data;

  if (cdoc_ref_page_no == -1) {
    fprintf(stderr, "Line %d : No Current Ref for End of Ref\n", page->line_no);
    return;
  }

  CPixelRenderer *renderer = getRenderer();

  renderer->setForeground(reference_save_fg);

  while (cdoc_ref_line_no < page->line_no) {
    ref_data = new CDHelpDatasetRefData;

    ref_data->page_no           = cdoc_ref_page_no;
    ref_data->help_dataset_data = this;
    ref_data->x                 = cdoc_ref_x;
    ref_data->y                 = cdoc_ref_y;
    ref_data->width             = getMaxPageWidth();
    ref_data->height            = page->line_heights[cdoc_ref_line_no - 1];

    addRefData(ref_data);

    cdoc_ref_x  = 0;
    cdoc_ref_y += ref_data->height;

    cdoc_ref_line_no++;
  }

  ref_data = new CDHelpDatasetRefData;

  ref_data->page_no           = cdoc_ref_page_no;
  ref_data->help_dataset_data = this;
  ref_data->x                 = cdoc_ref_x;
  ref_data->y                 = cdoc_ref_y;
  ref_data->width             = x - cdoc_ref_x;
  ref_data->height            = page->line_heights[cdoc_ref_line_no - 1];

  addRefData(ref_data);

  cdoc_ref_page_no = -1;
  cdoc_ref_line_no = -1;
  cdoc_ref_x       = 0;
  cdoc_ref_y       = 0;
}

// Start Highlighting of str containing Hook when selected will call the
// associated routine.
void
CDHelpDatasetData::
startHook(CDHelpDatasetPage *page, const std::string &id, const std::string &data, int x, int y)
{
  if (printing_) return;

  if (cdoc_ref_line_no != -1) {
    fprintf(stderr, "Line %d : Hook not Terminated at Start of Hook\n", page->line_no);
    return;
  }

  cdoc_hook_line_no = page->line_no;
  cdoc_hook_id      = id;
  cdoc_hook_data    = data;
  cdoc_hook_x       = x;
  cdoc_hook_y       = y;

  CPixelRenderer *renderer = getRenderer();

  renderer->getForeground(hook_save_fg);

  renderer->setForeground(hyper_color);
}

// End Highlighting of str containing Hook which when selected will call the
// assocated routine.
void
CDHelpDatasetData::
endHook(CDHelpDatasetPage *page, int x, int)
{
  if (printing_) return;

  if (cdoc_hook_line_no == -1) {
    fprintf(stderr, "Line %d : No Current Hook for End of Hook\n", page->line_no);
    return;
  }

  CPixelRenderer *renderer = getRenderer();

  renderer->setForeground(hook_save_fg);

  while (cdoc_hook_line_no < page->line_no) {
    CDHelpDatasetHookData *hook_data = new CDHelpDatasetHookData;

    hook_data->help_dataset_data = this;
    hook_data->id                = cdoc_hook_id;
    hook_data->data              = cdoc_hook_data;
    hook_data->x                 = cdoc_hook_x;
    hook_data->y                 = cdoc_hook_y;
    hook_data->width             = getMaxPageWidth();
    hook_data->height            = page->line_heights[cdoc_hook_line_no - 1];

    addHookData(hook_data);

    cdoc_hook_x  = 0;
    cdoc_hook_y += hook_data->height;

    cdoc_hook_line_no++;
  }

  CDHelpDatasetHookData *hook_data = new CDHelpDatasetHookData;

  hook_data->help_dataset_data = this;
  hook_data->id                = cdoc_hook_id;
  hook_data->data              = cdoc_hook_data;
  hook_data->x                 = cdoc_hook_x;
  hook_data->y                 = cdoc_hook_y;
  hook_data->width             = x - cdoc_hook_x;
  hook_data->height            = page->line_heights[page->line_no - 1];

  addHookData(hook_data);

  cdoc_hook_line_no = -1;
  cdoc_hook_id      = "";
  cdoc_hook_data    = "";
  cdoc_hook_x       = 0;
  cdoc_hook_y       = 0;
}

// Start Highlighting of str found using the search panel.
void
CDHelpDatasetData::
startHighlight()
{
  CPixelRenderer *renderer = getRenderer();

  renderer->getForeground(highlight_save_fg);

  renderer->setForeground(highlight_color);
}

// End Highlighting of str found using the search panel.
void
CDHelpDatasetData::
endHighlight()
{
  CPixelRenderer *renderer = getRenderer();

  renderer->setForeground(highlight_save_fg);
}

// Add a Button to the current page which when pressed will execute the specified command.
void
CDHelpDatasetData::
addButton(const std::string &title, const std::string &command, int x, int y, int width, int height)
{
  CDHelpDatasetCommandData *command_data = new CDHelpDatasetCommandData(this);

  command_data->setTitle(title);

  command_data->setCommand(command);

  command_data->setPosition(x, y);

  command_data->setSize(width, height);

  addCommandData(command_data);
}

// Draw a Button with the specified label and style.
void
CDHelpDatasetData::
drawButton(const std::string &str, int *x, int *y, int style)
{
  startFont("");

  int width, height;

  stringExtent(str, &width, &height);

  int width1, height1;

  charExtent(CDOC_MEAN_CHAR, &width1, &height1);

  int x1 = *x + width1 /2;
  int y1 = *y + height1/2;

  if (x1 < 4) x1 = 4;
  if (y1 < 4) y1 = 4;

  if (printing_) {
    if (x1 < 4) x1 = 4;
    if (y1 < 4) y1 = 4;

    int xo = width1 /4;
    int yo = height1/4;

    ps_print.setGray(0);

    ps_print.setLineWidth(width1/6.0);

    drawLine(x1 - xo        , y1 - yo        , x1 + width + xo, y1 - yo         );
    drawLine(x1 - xo        , y1 - yo        , x1 - xo        , y1 + height + yo);

    ps_print.setGray(0.75);

    drawLine(x1 - xo        , y1 + height + yo, x1 + width + xo, y1 + height + yo);
    drawLine(x1 + width + xo, y1 - yo         , x1 + width + xo, y1 + height + yo);

    ps_print.setGray(0);

    ps_print.setLineWidth(1);
  }
  else {
    CPixelRenderer *renderer = getRenderer();

    if (style == CDOC_BUTTON_OUT)
      renderer->setForeground(button_color);
    else
      renderer->setForeground(cdoc_button_select);

    renderer->fillRectangle(CIBBox2D(x1, y1, width + 1, height + 1));

    if (style == CDOC_BUTTON_OUT)
      renderer->setForeground(cdoc_button_top);
    else
      renderer->setForeground(cdoc_button_bottom);

    renderer->drawLine(CIPoint2D(x1 - 1, y1 - 1), CIPoint2D(x1 + width + 1, y1 - 1));
    renderer->drawLine(CIPoint2D(x1 - 2, y1 - 2), CIPoint2D(x1 + width + 2, y1 - 2));

    renderer->drawLine(CIPoint2D(x1 - 1, y1 - 1), CIPoint2D(x1 - 1, y1 + height + 1));
    renderer->drawLine(CIPoint2D(x1 - 2, y1 - 2), CIPoint2D(x1 - 2, y1 + height + 2));

    if (style == CDOC_BUTTON_OUT)
      renderer->setForeground(cdoc_button_bottom);
    else
      renderer->setForeground(cdoc_button_top);

    renderer->drawLine(CIPoint2D(x1 - 2, y1 + height + 2),
                       CIPoint2D(x1 + width + 2, y1 + height + 2));
    renderer->drawLine(CIPoint2D(x1 - 1, y1 + height + 1),
                       CIPoint2D(x1 + width + 1, y1 + height + 1));

    renderer->drawLine(CIPoint2D(x1 + width + 1, y1 - 1),
                       CIPoint2D(x1 + width + 1, y1 + height + 1));
    renderer->drawLine(CIPoint2D(x1 + width + 2, y1 - 2),
                       CIPoint2D(x1 + width + 2, y1 + height + 2));

    renderer->setForeground(cdoc_fg_normal);
  }

  drawString(&x1, &y1, str);

  endFont();
}

// Add a line to the current paragraph.
void
CDHelpDatasetData::
addParagraphLine(CDHelpDatasetPage *page, long pos, const char *line, int *, int *y)
{
  CDHelpSearchPanel *search_panel;

  int i = 0;

  if (line[i] == '\t') {
    i++;

    int j = i;

    CStrUtil::skipSpace(line, &i);

    if (line[i] == '\0') {
      *y += page->line_heights[page->line_no - 1];
      return;
    }

    if (paragraph->getNumLines() == 0 && ! paragraph->getContinued())
      paragraph->setIndent2(i - j);
  }
  else {
    int j = i;

    CStrUtil::skipSpace(line, &i);

    if (line[i] == '\0') {
      *y += page->line_heights[page->line_no - 1];
      return;
    }

    if (paragraph->getNumLines() == 0 && ! paragraph->getContinued()) {
      paragraph->setIndent2(i - j - paragraph->getIndent1());

      if (paragraph->getIndent2() < 0)
        paragraph->setIndent2(0);
    }
  }

  if (! printing_)
    search_panel = getHelpPanel()->getSearchPanel();

  int j = 0;

  int   string_len = strlen(line) + 256;
  char *str        = new char [string_len];

  int no;

  while (line[i] != '\0') {
    if (j + 16 > string_len) {
      string_len += 256;

      char *str1 = new char [string_len];

      strcpy(str1, str);

      delete [] str;

      str = str1;
    }

    if (! printing_ && search_panel != NULL) {
      long start_pos = search_panel->getStartPos();
      long end_pos   = search_panel->getEndPos  ();

      /* Start Highlight of Found String */

      if      (start_pos != -1 && start_pos == pos + i) {
        str[j++] = ESC;
        str[j++] = '[';
        str[j++] = '3';
        str[j++] = '8';
        str[j++] = 'm';
      }

      /* End Highlight of Found String */

      else if (end_pos != -1 && end_pos == pos + i - 1) {
        str[j++] = ESC;
        str[j++] = '[';
        str[j++] = '3';
        str[j++] = '9';
        str[j++] = 'm';
      }
    }

    /***********/

    /* Add Tab Character */

    if      (line[i] == '\t') {
      for (int no = 0; no < 8; no++)
        str[j++] = ' ';

      i++;
    }

    /* Add Box Character */

    else if (line[i] >= BOX_HOR_C && line[i] <= BOX_TL__C)
      str[j++] = line[i++];

    /* Add Escape Codes */

    else if ((no = CDocIsEscapeCode(&line[i])) != 0) {
      for (; no > 0; no--)
        str[j++] = line[i++];
    }

    /* Add normal character to buffer */

    else
      str[j++] = line[i++];
  }

  /* Terminate Highlighting if Still on */

  if (! printing_ && search_panel != NULL) {
    long end_pos = search_panel->getEndPos();

    if (end_pos != -1 && end_pos == pos + i - 1) {
      str[j++] = ESC;
      str[j++] = '[';
      str[j++] = '3';
      str[j++] = '0';
      str[j++] = 'm';
    }
  }

  str[j] = '\0';

  /* Add Line to Paragraph Line List */

  paragraph->addLine(str);

  if (paragraph->getLineNo1() == -1)
    paragraph->setLineNo1(page->line_no);

  if (paragraph->getLineNo2() == -1)
    paragraph->setLineNo2(page->line_no);

  delete [] str;

  *y += page->line_heights[page->line_no - 1];
}

// All text lines from here to the next 'paragraph_end' or 'paragraph_stop' are
// considered to belong to a paragraph.
//
// Grouping these lines together aids the formatting process.
void
CDHelpDatasetData::
beginParagraph(CDHelpDatasetPage *page, int x, int y, int right_margin, int continued)
{
  /* No paragraph should be currently active (if it is issue an error message and generate
     a 'paragraph_end' to try to recover) */

  if (paragraph != NULL) {
    fprintf(stderr, "Line %d : Start Paragraph when already in Paragraph\n", page->line_no);

    endParagraph(page, CHALIGN_TYPE_LEFT, x, y, false);
  }

  /* Allocate and Initailise the Paragraph Structure */

  paragraph = new CDDrawParagraph(x, y, cdoc_tab_indent, right_margin, continued);
}

// Signal that the current paragraph has been terminated (end of text or page throw).
// The text is now formatted so that it justified to the page width.
void
CDHelpDatasetData::
endParagraph(CDHelpDatasetPage *page, int /*align*/, int x, int y, int align_last)
{
  if (paragraph == NULL) {
    fprintf(stderr, "Line No %d, End Paragraph when not in Paragraph\n", page->line_no);

    beginParagraph(page, x, y, getRightMargin(), true);
  }

  if (paragraph == NULL || paragraph->getNumLines() == 0) {
    delete paragraph;

    paragraph = NULL;

    return;
  }

  int indent1 = paragraph->getIndent1()*cdoc_indent_char_width;
  int indent2 = paragraph->getIndent2()*cdoc_indent_char_width;

  int x1 = paragraph->getX() + indent1 + indent2;
  int y1 = paragraph->getY();

  int no = paragraph->getNumLines();

  int save_line_no = page->line_no;

  page->line_no = paragraph->getLineNo1();

  for (int i = 0; i < no - 1; ++i) {
    const std::string &line = paragraph->getLine(i);

    int j = 0;

    CStrUtil::skipSpace(line, &j);

    if (! CDocInst->getRagged())
      justifyLine(page, line.substr(j), x1, y1, paragraph->getRightMargin());
    else {
      int x2 = x1;
      int y2 = y1;

      drawPageLine(page, -1, line.substr(j), CHALIGN_TYPE_LEFT, &x2, &y2,
                   paragraph->getRightMargin());
    }

    x1 = paragraph->getX() + indent1;

    while (page->line_heights[page->line_no - 1] == 0 &&
           page->line_no < page->no_lines)
      page->line_no++;

    y1 += page->line_heights[page->line_no - 1];

    page->line_no++;
  }

  const std::string &line = paragraph->getLine(no - 1);

  int j = 0;

  CStrUtil::skipSpace(line, &j);

  if (! CDocInst->getRagged() && align_last)
    justifyLine(page, line.substr(j), x1, y1, paragraph->getRightMargin());
  else
    drawPageLine(page, -1, line.substr(j), CHALIGN_TYPE_LEFT, &x1, &y1,
                 paragraph->getRightMargin());

  page->line_no = save_line_no;

  delete paragraph;

  paragraph = NULL;
}

// Justify the supplied line so that the text is aligned with the left and right margins.
//
// Grouping these lines together aids the formatting process.
void
CDHelpDatasetData::
justifyLine(CDHelpDatasetPage *page, const std::string &line, int x, int y, int /*right_margin*/)
{
  std::string string1 = line;

  /*----------*/

  /* Count the Number if Words */

  int no_words = 0;

  uint i = 0;

  CStrUtil::skipSpace(string1, &i);

  while (string1[i] != '\0') {
    if (string1[i] != '\0' && ! isspace(string1[i])) {
      no_words++;

      CStrUtil::skipNonSpace(string1, &i);
    }

    CStrUtil::skipSpace(string1, &i);
  }

  if (no_words == 0)
    return;

  /*----------*/

  int init_font = getFontType();

  /*----------*/

  int x1 = x;
  int y1 = y;

  i = 0;

  int j = i;

  CStrUtil::skipSpace(string1, &i);

  int width, height;

  charExtent(SPACE_CHAR, &width, &height);

  x1 += (i - j)*width;

  int no = 0;

  if (no_words == 1) {
    std::string word;

    while (string1[i] != '\0') {
      if ((no = CDocIsEscapeCode(&string1[i])) != 0) {
        drawString(&x1, &y1, word);

        word = "";

        /* If Font Escape Code then switch to normal, bold, italic or bold italic font */

        if      (CDOC_IS_FONT_ESC(&string1[i]))
          setFontType(string1[i + 2] - '0');

        /* If Inline Font Escape Code then start/end font */

        else if (CDOC_IS_INLINE_FONT_ESC(&string1[i])) {
          if (string1[i + 1] == 'E')
            endFont();
          else {
            int i1 = i + 2;

            int c = string1[i1++];

            std::string temp_string;

            while (string1[i1] != '\0' && string1[i1] != c)
              temp_string += string1[i1++];

            startFont(temp_string);
          }
        }

        /* If Foreground Escape Code then switch to specified
           Foreground Color */

        else if (CDOC_IS_FOREGROUND_ESC(&string1[i]))
          setColor(string1[i + 3] - '0');

        /* If Goto Escape Code then Start/End Reference */

        else if (CDOC_IS_GOTO_ESC(&string1[i])) {
          if (string1[i + no - 1] == 'B') {
            int i1 = i + 2;

            std::string temp_string;

            while (string1[i1] != '\0' && isdigit(string1[i1]))
              temp_string += string1[i1++];

            startReference(page, temp_string, x1, y1);
          }
          else
            endReference(page, x1, y1);
        }

        /* If Hook Escape Code then Start/End Hook */

        else if (CDOC_IS_HOOK_ESC(&string1[i])) {
          if (string1[i + 2] == 'B') {
            int j1 = i + 2;

            std::string id;

            while (string1[j1] != '\0' && string1[j1] != '\t')
              id += string1[j1++];

            if (string1[j1] == '\t')
              j1++;

            std::string data;

            while (string1[j1] != '\0' && string1[j1] != '\033')
              data += string1[j1++];

            if (string1[j1] == '\033')
              j1++;

            startHook(page, id, data, x1, y1);
          }
          else
            endHook(page, x1, y1);
        }

        i += no;
      }
      else
        word += string1[i++];
    }

    drawString(&x1, &y1, word);

    return;
  }

  /*----------*/

  /* Allocate Arrays for Line Breaks and Inter-Word Spaces */

  int *spaces = new int [no_words - 1];

  for (int k = 0; k < no_words - 1; k++)
    spaces[k] = 0;

  /*----------*/

  while (string1[i] != '\0') {
    std::string word;

    while (string1[i] != '\0' && ! isspace(string1[i])) {
      if ((no = CDocIsEscapeCode(&string1[i])) != 0) {
        stringExtent(word, &width, &height);

        x1 += width;

        word = "";

        /* If Font Escape Code then switch to normal, bold, italic or bold italic font */

        if      (CDOC_IS_FONT_ESC(&string1[i]))
          setFontType(string1[i + 2] - '0');

        /* If Inline Font Escape Code then start/end font */

        else if (CDOC_IS_INLINE_FONT_ESC(&string1[i])) {
          if (string1[i + 1] == 'E')
            endFont();
          else {
            int i1 = i + 2;

            int c = string1[i1++];

            std::string temp_string;

            while (string1[i1] != '\0' && string1[i1] != c)
              temp_string += string1[i1++];

            startFont(temp_string);
          }
        }

        i += no;
      }
      else
        word += string1[i++];
    }

    stringExtent(word, &width, &height);

    x1 += width;

    word = word.substr(0, i);

    CStrUtil::skipSpace(string1, &i);

    if (string1[i] != '\0') {
      charExtent(SPACE_CHAR, &width, &height);

      x1 += (i - word.size())*width;
    }
  }

  /*----------*/

  int no_spaces = getMaxPageWidth() - x1;

  j = 0;

  while (no_spaces > 0) {
    spaces[j++]++;

    no_spaces--;

    if (j >= no_words - 1)
      j = 0;
  }

  /*----------*/

  setFontType(init_font);

  /*----------*/

  x1 = x;
  y1 = y;

  i = 0;
  j = i;

  CStrUtil::skipSpace(string1, &i);

  charExtent(SPACE_CHAR, &width, &height);

  x1 += (i - j)*width;

  no_words = 0;

  while (string1[i] != '\0') {
    if (no_words > 0)
      x1 += spaces[no_words - 1];

    j = 0;

    std::string word;

    while (string1[i] != '\0' && ! isspace(string1[i])) {
      if ((no = CDocIsEscapeCode(&string1[i])) != 0) {
        drawString(&x1, &y1, word);

        j = 0;

        /* If Font Escape Code then switch to normal, bold, italic or bold italic font */

        if      (CDOC_IS_FONT_ESC(&string1[i]))
          setFontType(string1[i + 2] - '0');

        /* If Inline Font Escape Code then start/end font */

        else if (CDOC_IS_INLINE_FONT_ESC(&string1[i])) {
          if (string1[i + 1] == 'E')
            endFont();
          else {
            int i1 = i + 2;

            int c = string1[i1++];

            std::string temp_string;

            while (string1[i1] != '\0' && string1[i1] != c)
              temp_string += string1[i1++];

            startFont(temp_string);
          }
        }

        /* If Foreground Escape Code then switch to specified
           Foreground Color */

        else if (CDOC_IS_FOREGROUND_ESC(&string1[i]))
          setColor(string1[i + 3] - '0');

        /* If Goto Escape Code then Start/End Reference */

        else if (CDOC_IS_GOTO_ESC(&string1[i])) {
          if (string1[i + no - 1] == 'B') {
            int i1 = i + 2;

            std::string temp_string;

            while (string1[i1] != '\0' && isdigit(string1[i1]))
              temp_string += string1[i1++];

            if (temp_string != "")
              startReference(page, temp_string, x1, y1);
          }
          else
            endReference(page, x1, y1);
        }

        /* If Hook Escape Code then Start/End Hook */

        else if (CDOC_IS_HOOK_ESC(&string1[i])) {
          if (string1[i + 2] == 'B') {
            int j1 = i + 3;

            std::string id;

            while (string1[j1] != '\0' && string1[j1] != '\t')
              id += string1[j1++];

            if (string1[j1] == '\t')
              j1++;

            std::string data;

            while (string1[j1] != '\0' && string1[j1] != '\033')
              data += string1[j1++];

            if (string1[j1] == '\033')
              j1++;

            startHook(page, id, data, x1, y1);
          }
          else
            endHook(page, x1, y1);
        }

        i += no;
      }
      else
        word += string1[i++];
    }

    drawString(&x1, &y1, word);

    no_words++;

    j = i;

    CStrUtil::skipSpace(string1, &i);

    if (string1[i] != '\0')
      drawSpaces(&x1, &y1, i - j);
  }

  /*----------*/

  delete [] spaces;
}

// Draw the supplied text line using the specified alignment.
void
CDHelpDatasetData::
drawPageLine(CDHelpDatasetPage *page, long pos, const std::string &line,
             int align, int *x, int *y, int right_margin)
{
  int i = 0;

  CStrUtil::skipSpace(line, &i);

  if (i >= (int) line.size()) {
    *y += page->line_heights[page->line_no - 1];
    return;
  }

  char *str = CStrUtil::strdup(line.c_str());

  i = 0;

  if (line[i] == '\t') {
    *x += cdoc_tab_indent*cdoc_indent_char_width;

    i++;
  }

  int j = i;

  CStrUtil::skipSpace(line, &i);

  int width, height;

  charExtent(SPACE_CHAR, &width, &height);

  *x += (i - j)*width;

  j = 0;

  int no = 0;

  if (align == CHALIGN_TYPE_CENTRE || align == CHALIGN_TYPE_RIGHT) {
    uint j1 = i;

    while (j1 < line.size()) {
      if ((no = CDocIsEscapeCode(&line[j1])) != 0)
        j1 += no;
      else
        str[j++] = line[j1++];
    }

    str[j] = '\0';

    int width1, height1;

    stringExtent(str, &width1, &height1);

    int spaces = right_margin*width - *x - width1;

    if (spaces < 0)
      spaces = 0;

    if (align == CHALIGN_TYPE_CENTRE)
      *x += spaces/2;
    else
      *x += spaces;

    j = 0;
  }

  CDHelpSearchPanel *search_panel = NULL;

  if (! printing_)
    search_panel = getHelpPanel()->getSearchPanel();

  while (i < (int) line.size()) {
    if (! printing_ && search_panel != NULL) {
      long start_pos = search_panel->getStartPos();
      long end_pos   = search_panel->getEndPos  ();

      if      (start_pos != -1 && start_pos == pos + i) {
        str[j] = '\0';

        drawString(x, y, str);

        j = 0;

        startHighlight();
      }
      else if (end_pos != -1 && end_pos == pos + i - 1) {
        str[j] = '\0';

        drawString(x, y, str);

        j = 0;

        endHighlight();
      }
    }

    /*************/

    /* If Tab Character Draw Required Spaces */

    if      (line[i] == '\t') {
      str[j] = '\0';

      drawString(x, y, str);

      j = 0;

      drawString(x, y, "        ");

      i++;
    }

    /* If Box Character Draw Required Lines */

    else if (line[i] >= BOX_HOR_C && line[i] <= BOX_TL__C) {
      str[j] = '\0';

      drawString(x, y, str);

      j = 0;

      drawBoxChar(line[i], x, y);

      i++;
    }

    else if ((no = CDocIsEscapeCode(&line[i])) != 0) {
      str[j] = '\0';

      drawString(x, y, str);

      j = 0;

      /* If Font Escape Code then switch to normal, bold, italic or bold italic font */

      if      (CDOC_IS_FONT_ESC(&line[i]))
        setFontType(line[i + 2] - '0');

      /* If Inline Font Escape Code then start/end font */

      else if (CDOC_IS_INLINE_FONT_ESC(&line[i])) {
        if (line[i + 1] == 'E')
          endFont();
        else {
          uint i1 = i + 2;

          char c = line[i1++];

          std::string temp_string;

          while (i1 < line.size() && line[i1] != c)
            temp_string += line[i1++];

          startFont(temp_string);
        }
      }

      /* If Foreground Escape Code then switch to specified Foreground Color */

      else if (CDOC_IS_FOREGROUND_ESC(&line[i]))
        setColor(line[i + 3] - '0');

      /* If Goto Escape Code then Start/End Reference */

      else if (CDOC_IS_GOTO_ESC(&line[i])) {
        if (line[i + no - 1] == 'B') {
          uint j1 = i + 2;

          std::string temp_string;

          while (j1 < line.size() && isdigit(line[j1]))
            temp_string += line[j1++];

          startReference(page, temp_string, *x, *y);
        }
        else
          endReference(page, *x, *y);
      }

      /* If Hook Escape Code then Start/End Hook */

      else if (CDOC_IS_HOOK_ESC(&line[i])) {
        if (line[i + 2] == 'B') {
          uint j1 = i + 3;

          std::string id;

          while (j1 < line.size() && line[j1] != '\t')
            id += line[j1++];

          if (line[j1] == '\t')
            j1++;

          std::string data;

          while (j1 < line.size() && line[j1] != '\033')
            data += line[j1++];

          if (line[j1] == '\033')
            j1++;

          startHook(page, id, data, *x, *y);
        }
        else
          endHook(page, *x, *y);
      }

      /* If Fill Escape then do Fill */

      else if (CDOC_IS_FILL_ESC(&line[i])) {
        uint j1 = i + 2;

        std::string temp_string;

        while (j1 < line.size() && isdigit(line[j1]))
          temp_string += line[j1++];

        int start = CStrUtil::toInteger(temp_string);

        j1++;

        temp_string = "";

        while (j1 < line.size() && isdigit(line[j1]))
          temp_string += line[j1++];

        int end = CStrUtil::toInteger(temp_string);

        j1++;

        int c = line[j1];

        int width1, height1;

        charExtent(CDOC_MEAN_CHAR, &width1, &height1);

        int x1 = width1*start + cdoc_fill_pad;

        char fill_string[2];

        fill_string[0] = c;
        fill_string[1] = '\0';

        for (int no = end - start + 1; no > 0; no--) {
          *x = x1;

          drawString(x, y, fill_string);

          x1 += width1;
        }

        *x = x1;
      }

      i += no;
    }

    /* Add normal character to buffer */

    else
      str[j++] = line[i++];
  }

  /* Draw Text */

  str[j] = '\0';

  drawString(x, y, str);

  /* Terminate Highlighting if Still on */

  if (! printing_ && search_panel != NULL) {
    long end_pos = search_panel->getEndPos();

    if (end_pos != -1 && end_pos == pos + i - 1)
      endHighlight();
  }

  delete [] str;

  *y += page->line_heights[page->line_no - 1];
}

// Draw a String.
//
// Normally this just draws the str and updates the coordinate but if a selection
// box has been specified characters lying within it are highlighted.
void
CDHelpDatasetData::
drawString(int *x, int *y, const std::string &str)
{
  if (str == "")
    return;

  if (printing_) {
    /* Allocate extra space for escaped characters (potentially each character) */

    char *string1 = new char [str.size()*2 + 1];

    /* Get size of single character */

    int width1, height1;

    charExtent(SPACE_CHAR, &width1, &height1);

    /* Output each block of non-space characters escaping Postscript's
       special characters */

    int i = 0;

    while (str[i] != '\0') {
      /* Don't Output Spaces */

      int j = i;

      CStrUtil::skipSpace(str, &i);

      *x += (i - j)*width1;

      j = 0;

      int x1 = *x;

      while (str[i] != '\0' && ! isspace(str[i])) {
        /* Escape Postscript Special Characters */

        if      (str[i] == '\\' || str[i] == '(' || str[i] == ')') {
          string1[j++] = '\\';
          string1[j++] = str[i];
        }

        /* Skip dangerous characters */

        else if (str[i] == '')
          ;

        else
          string1[j++] = str[i];

        int width2, height2;

        charExtent(string1[j - 1], &width2, &height2);

        *x += width2;

        i++;
      }

      if (j > 0) {
        string1[j] = '\0';

        //double y1 = ps_max_page_height - *y - ps_current_font->size*ps_font_y_scale;
        double y1 = ps_max_page_height - *y - ps_current_font->size;

        if (ps_current_font->current != NULL)
          //y1 -= ps_current_font->current->getDescender()*ps_current_font->size*ps_font_y_scale;
          y1 -= (ps_current_font->current->getDescender()/1000.0)*ps_current_font->size;

        ps_print.showText(x1, y1, string1);
      }
    }
  }
  else {
    CPixelRenderer *renderer = getRenderer();

    if (getCurrentFontStruct().isValid())
      renderer->setFont(getCurrentFontStruct());

    int char_ascent = 10;

    if (getCurrentFontStruct().isValid())
      char_ascent = getCurrentFontStruct()->getCharAscent();

    renderer->drawString(CIPoint2D(*x, *y + char_ascent), str);

    if (isSelect())
      selectText(*x, *y, str);

    int width, height;

    stringExtent(str, &width, &height);

    *x += width;
  }
}

// Draw a specified number of spaces.
//
// Normally this just updates the coordinate but if a selection box has be
// specified the spaces lying within it are highlighted.
void
CDHelpDatasetData::
drawSpaces(int *x, int *y, int no_spaces)
{
  int width, height;

  charExtent(SPACE_CHAR, &width, &height);

  if (isSelect()) {
    for (int i = 0; i < no_spaces; i++)
      selectText(*x + i*width, *y, " ");
  }

  *x += no_spaces*width;
}

// Draw the Image Data contained in the specified file.
void
CDHelpDatasetData::
drawImage(const std::string &id, const std::string &filename, int size, int resize,
          int rotate, int *x, int *y)
{
  if (printing_) {
    CImagePtr image = CImageMgrInst->lookupImage(filename);

    int width1, height1;

    charExtent(SPACE_CHAR, &width1, &height1);

    int indent = cdoc_tab_indent*cdoc_indent_char_width;

    int height;

    if (size > 0)
      height = size*height1;
    else
      height = 4*height1;

    int width = ps_max_page_width1 - indent - 2*width1;

    if (width < 0)
      width = width1;

    int x1 = indent + width1;
    int y1 = *y;
    int x2 = x1 + width;
    int y2 = y1 + height;

    if (image.isValid()) {
      /* Determine the Depth of any Printed Images */

      CDHelpPrintPanel *print_panel = getHelpPanel()->getPrintPanel();

      int image_depth = print_panel->getImageDepth();

      if      (image_depth == 1) ps_print.setOutputDepth(1);
      else if (image_depth == 2) ps_print.setOutputDepth(2);
      else if (image_depth == 3) ps_print.setOutputDepth(4);
      else if (image_depth == 4) ps_print.setOutputDepth(8);

      /* Output Image */

      ps_print.drawImage(image, x1, y1, width, height, resize, rotate);
    }
    else {
      ps_print.setGray(0.75);

      ps_print.fillRectangle(x1, ps_max_page_height - y1, x2, ps_max_page_height - y2);

      ps_print.setGray(0);
    }

    *x = 0;
    *y = y2;
  }
  else {
    int indent = cdoc_tab_indent*cdoc_indent_char_width;

    int width, height;

    charExtent(SPACE_CHAR, &width, &height);

    int width1 = getMaxPageWidth1() - indent - 2*width;

    if (width1 < 0)
      width1 = width;

    int height1;

    if (size > 0)
      height1 = size*height;
    else
      height1 = 4*height;

    CImagePtr image = getImage(id);

    if (! image.isValid()) {
      image = CImageMgrInst->lookupImage(filename);

      if (image.isValid() && rotate != 0) {
        CImagePtr image1 = image->rotate(rotate);

        image = image1;
      }

      if (image.isValid() && resize) {
        double scale = ((double) height1)/image->getHeight();

        CImagePtr image1 = image->resize(int(image->getWidth()*scale), height1);

        char *env = getenv("CDOC_IMAGE_DEBUG");

        if (scale != 1.0 && env != NULL)
          CDocWriteError("Image '%s' Rescaled - Factor %lf", filename.c_str(), scale);

        image = image1;
      }

      if (image.isValid())
        addImage(id, image);
    }

    CPixelRenderer *renderer = getRenderer();

    if (image.isValid()) {
      int x1 = indent + (getMaxPageWidth1() - indent - image->getWidth())/2;

      if (x1 + (int) image->getWidth() > getMaxPageWidth1())
        x1 = getMaxPageWidth1() - image->getWidth();

      if (x1 < 0)
        x1 = 0;

      renderer->drawImage(CIPoint2D(x1, *y), image);

      *y += image->getHeight();
    }
    else {
      renderer->fillPatternRectangle(CIBBox2D(indent + width, *y, width1, height1),
                                     CBrushPattern::DOTTED6);

      *y += height1;
    }
  }
}

// Draw a Box.
void
CDHelpDatasetData::
drawBox(int x1, int y1, int x2, int y2)
{
  setDrawGC();

  if (printing_) {
    y1 = ps_max_page_height - y1;
    y2 = ps_max_page_height - y2;

    ps_print.drawRectangle(x1, y1, x2, y2);
  }
  else {
    if (x2 < x1) std::swap(x1, x2);
    if (y2 < y1) std::swap(y1, y2);

    CPixelRenderer *renderer = getRenderer();

    renderer->drawRectangle(CIBBox2D(x1, y1, x2 - x1, y2 - y1));
  }

  resetDrawGC();
}

// Fill a Box.
void
CDHelpDatasetData::
fillBox(int x1, int y1, int x2, int y2)
{
  setFillGC();

  if (printing_) {
    y1 = ps_max_page_height - y1;
    y2 = ps_max_page_height - y2;

    ps_print.fillRectangle(x1, y1, x2, y2);
  }
  else {
    if (x2 < x1) std::swap(x1, x2);
    if (y2 < y1) std::swap(y1, y2);

    CPixelRenderer *renderer = getRenderer();

    renderer->fillRectangle(CIBBox2D(x1, y1, x2 - x1, y2 - y1));
  }

  resetFillGC();
}

// Draw a Line.
void
CDHelpDatasetData::
drawLine(int x1, int y1, int x2, int y2)
{
  setDrawGC();

  if (printing_) {
    y1 = ps_max_page_height - y1;
    y2 = ps_max_page_height - y2;

    ps_print.drawLine(x1, y1, x2, y2);
  }
  else {
    CPixelRenderer *renderer = getRenderer();

    renderer->drawLine(CIPoint2D(x1, y1), CIPoint2D(x2, y2));
  }

  resetDrawGC();
}

// Draw a String.
void
CDHelpDatasetData::
drawText(int x, int y, const std::string &str)
{
  if (str == "")
    return;

  setTextGC();

  if (printing_) {
    /* Allocate extra space for escaped characters (potentially each character) */

    char *string1 = new char [str.size()*2 + 1];

    /* Get size of single character */

    int width1, height1;

    charExtent(SPACE_CHAR, &width1, &height1);

    /* Output each block of non-space characters escaping Postscript's special characters */

    int i = 0;

    int len = str.size();

    while (i < len) {
      /* Don't Output Spaces */

      int j = i;

      CStrUtil::skipSpace(str, &i);

      x += (i - j)*width1;

      j = 0;

      int x1 = x;

      while (i < len && ! isspace(str[i])) {
        /* Escape Postscript Special Characters */

        if      (str[i] == '\\' || str[i] == '(' || str[i] == ')') {
          string1[j++] = '\\';
          string1[j++] = str[i];
        }

        /* Skip dangerous characters */

        else if (str[i] == '')
          ;

        else
          string1[j++] = str[i];

        int width2, height2;

        charExtent(string1[j - 1], &width2, &height2);

        x += width2;

        i++;
      }

      if (j > 0) {
        string1[j] = '\0';

        //double y1 = ps_max_page_height - y - ps_current_font->size*ps_font_y_scale;
        double y1 = ps_max_page_height - y - ps_current_font->size;

        if (ps_current_font->current != NULL)
          //y1 -= ps_current_font->current->getDescender()*ps_current_font->size*ps_font_y_scale;
          y1 -= (ps_current_font->current->getDescender()/1000.0)*ps_current_font->size;

        ps_print.showText(x1, y1, string1);
      }
    }
  }
  else {
    int char_ascent = 10;

    if (getCurrentFontStruct().isValid())
      char_ascent = getCurrentFontStruct()->getCharAscent();

    CPixelRenderer *renderer = getRenderer();

    renderer->drawString(CIPoint2D(x, y + char_ascent), str);
  }

  resetTextGC();
}

// Draw a special box character.
void
CDHelpDatasetData::
drawBoxChar(int c, int *x, int *y)
{
  int width, height;

  charExtent(CDOC_MEAN_CHAR, &width, &height);

  if (printing_) {
    ps_print.setLineWidth(width/4.0);

    ps_print.setLineDash(CILineDash());

    if (c == BOX_TL__C || c == BOX_BL__C || c == BOX_LPR_C)
      drawLine(*x + width/2, *y + height/2, *x + width  , *y + height/2);

    if (c == BOX_TR__C || c == BOX_BR__C || c == BOX_RPR_C)
      drawLine(*x          , *y + height/2, *x + width/2, *y + height/2);

    if (c == BOX_TL__C || c == BOX_TR__C || c == BOX_TPR_C)
      drawLine(*x + width/2, *y + height/2, *x + width/2, *y + height  );

    if (c == BOX_BL__C || c == BOX_BR__C || c == BOX_BPR_C)
      drawLine(*x + width/2, *y           , *x + width/2, *y + height/2);

    if (c == BOX_LPR_C || c == BOX_RPR_C ||
        c == BOX_VRT_C || c == BOX_MID_C)
      drawLine(*x + width/2, *y           , *x + width/2, *y + height  );

    if (c == BOX_TPR_C || c == BOX_BPR_C ||
        c == BOX_HOR_C || c == BOX_MID_C)
      drawLine(*x          , *y + height/2, *x + width  , *y + height/2);
  }
  else {
    CPixelRenderer *renderer = getRenderer();

    renderer->setLineWidth(2);

    if (c == BOX_TL__C || c == BOX_BL__C || c == BOX_LPR_C)
      renderer->drawLine(CIPoint2D(*x + width/2, *y + height/2),
                         CIPoint2D(*x + width  , *y + height/2));

    if (c == BOX_TR__C || c == BOX_BR__C || c == BOX_RPR_C)
      renderer->drawLine(CIPoint2D(*x          , *y + height/2),
                         CIPoint2D(*x + width/2, *y + height/2));

    if (c == BOX_TL__C || c == BOX_TR__C || c == BOX_TPR_C)
      renderer->drawLine(CIPoint2D(*x + width/2, *y + height/2),
                         CIPoint2D(*x + width/2, *y + height  ));

    if (c == BOX_BL__C || c == BOX_BR__C || c == BOX_BPR_C)
      renderer->drawLine(CIPoint2D(*x + width/2, *y           ),
                         CIPoint2D(*x + width/2, *y + height/2));

    if (c == BOX_LPR_C || c == BOX_RPR_C || c == BOX_VRT_C || c == BOX_MID_C)
      renderer->drawLine(CIPoint2D(*x + width/2, *y           ),
                         CIPoint2D(*x + width/2, *y + height  ));

    if (c == BOX_TPR_C || c == BOX_BPR_C || c == BOX_HOR_C || c == BOX_MID_C)
      renderer->drawLine(CIPoint2D(*x          , *y + height/2),
                         CIPoint2D(*x + width  , *y + height/2));
  }

  *x += width;
}

// Start the selection of text within a specified area.
//
// Opens a temporary file which will contain the selected text.
void
CDHelpDatasetData::
startSelect()
{
  select_file = CDocInst->getTempFileName();
  select_fp   = fopen(select_file.c_str(), "w");
}
// End the selection of text within a specified area.
//
// Closes the temporary file containing the selected text, reads the text from the
// file and sets the X selection text.
void
CDHelpDatasetData::
endSelect()
{
  if (select_fp == NULL)
    return;

  /********/

  fclose(select_fp);

  select_fp = NULL;

  /********/

  struct stat file_stat;

  int error = stat(select_file.c_str(), &file_stat);

  if (error != 0)
    return;

  char *buffer = new char [file_stat.st_size + 1];

  select_fp = fopen(select_file.c_str(), "r");

  if (select_fp == NULL)
    return;

  int no = fread(buffer, sizeof(char), file_stat.st_size, select_fp);

  buffer[no] = '\0';

  fclose(select_fp);

  select_fp = NULL;

  remove(select_file.c_str());

  /********/

  QClipboard *clipboard = QApplication::clipboard();

  clipboard->setText(buffer);

  //connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipBoardSlot()));
}

bool
CDHelpDatasetData::
isSelect() const
{
  return (select_fp != NULL);
}

// Add any characters in the supplied str to the selection text if they lie within the
// selection area. The selected characters are highlighted using the selection color.
//
// The selected characters are written to the selection file.
void
CDHelpDatasetData::
selectText(int x, int y, const std::string &str)
{
  int char_ascent = 10;

  if (getCurrentFontStruct().isValid())
    char_ascent = getCurrentFontStruct()->getCharAscent();

  CRGBA bg, fg;

  CPixelRenderer *renderer = getRenderer();

  renderer->getForeground(fg);
  renderer->getBackground(bg);

  if (select_color != bg)
    renderer->setForeground(select_color);

  int i = 0;

  while (str[i] != '\0') {
    int width, height;

    charExtent(str[i], &width, &height);

    if (x              >= getSelectXMin() && x + width  - 1 <= getSelectXMax() &&
        y              >= getSelectYMin() && y + height - 1 <= getSelectYMax()) {
      if (getSelectY() != -1 && y > getSelectY()) {
        int line_no = getSelectPage()->line_no;

        int y1 = y;

        while (line_no > 1 && y1 > getSelectY()) {
          if (getSelectPage()->line_heights[line_no - 1] > 0) {
            y1 -= getSelectPage()->line_heights[line_no - 1];

            addSelectChar('\n');

            setSelectX(-1);
          }

          line_no--;
        }
      }

      if (getSelectX() != -1 && x > getSelectX())
        addSelectChar(' ');

      addSelectChar(str[i]);

      if (select_color != bg)
        renderer->drawString(CIPoint2D(x, y + char_ascent), std::string(&str[i], 1));
      else
        renderer->fillRectangle(CIBBox2D(x, y + 1, width, height));

      setSelectX(x + width);
      setSelectY(y);
    }

    x += width;

    i++;
  }

  if (select_color != bg)
    renderer->setForeground(fg);
}

void
CDHelpDatasetData::
addSelectChar(char c)
{
  fputc(c, select_fp);
}

void
CDHelpDatasetData::
xorSelection()
{
  xorBox(getSelectXMin(), getSelectYMin(), getSelectXMax(), getSelectYMax());
}

// Draw a box with the specified upper left hand and bottom right hand coordinates
// in xor mode.
void
CDHelpDatasetData::
xorBox(int x1, int y1, int x2, int y2)
{
  assert(x1 + y1 + x2 + y2);
  /* Draw each side of the Box in Xor Mode */

  //renderer->drawLine(CIPoint2D(x1, y1), CIPoint2D(x2, y1));
  //renderer->drawLine(CIPoint2D(x2, y1), CIPoint2D(x2, y2));
  //renderer->drawLine(CIPoint2D(x2, y2), CIPoint2D(x1, y2));
  //renderer->drawLine(CIPoint2D(x1, y2), CIPoint2D(x1, y1));
}

// Add the named image to the cache.
void
CDHelpDatasetData::
addImage(const std::string &id, CImagePtr image)
{
  image_buffer[id] = image;
}

// Add the named image from the cache.
CImagePtr
CDHelpDatasetData::
getImage(const std::string &id)
{
  CDImageBuffer::iterator p = image_buffer.find(id);

  if (p == image_buffer.end())
    return CImagePtr();

  return (*p).second;
}

// Free all the buffered Images.
void
CDHelpDatasetData::
freeImages()
{
  image_buffer.clear();
}

// Set the graphics options for drawing lines or boxes.
// This sets the line thickness and color.
void
CDHelpDatasetData::
setDrawGC()
{
  if (printing_)
    ps_print.setLineWidth(current_draw_line_thickness);
  else {
    CPixelRenderer *renderer = getRenderer();

    if (current_draw_line_thickness > 0)
      renderer->setLineWidth(current_draw_line_thickness);
    else
      renderer->setLineWidth(0);

    if (current_draw_color != "") {
      CRGBA screen_color = CRGBName::toRGBA(current_draw_color);

      renderer->setForeground(screen_color);
    }
  }
}

// Set the graphics options for filling boxes.
// This sets the line thickness, color and pixmap.
void
CDHelpDatasetData::
setFillGC()
{
  if (printing_) {
    if (current_draw_line_thickness > 0)
      ps_print.setLineWidth(current_draw_line_thickness);

    if (current_draw_fill_pixmap != "")
      ps_print.setGray(0.75);
  }
  else {
    CPixelRenderer *renderer = getRenderer();

    if (current_draw_line_thickness > 0)
      renderer->setLineWidth(current_draw_line_thickness);
    else
      renderer->setLineWidth(0);

    if (current_draw_color != "") {
      CRGBA screen_color = CRGBName::toRGBA(current_draw_color);

      renderer->setForeground(screen_color);
    }

    if (current_draw_fill_pixmap != "") {
      //XSetFillStyle(FillTiled);

      //XSetTile(current_draw_fill_pixmap);
    }
  }
}

// Set the graphics options for drawing text.
// This sets the color and font.
void
CDHelpDatasetData::
setTextGC()
{
  if (printing_) {
    CDFontData *font = NULL;

    if (current_draw_font_name != "") {
      int no = ps_defined_font_list.size();

      int i;

      for (i = 1; i <= no; i++) {
        font = ps_defined_font_list[i - 1];

        if (CStrUtil::casecmp(font->name, current_draw_font_name) == 0)
          break;
      }

      if (i > no)
        font = NULL;
    }

    CFontMetric *font_metric = NULL;

    if (font != NULL) {
      if (current_draw_font_style != "") {
        if      (CStrUtil::casecmp(current_draw_font_style, "normal"        ) == 0)
          font_metric = font->normal;
        else if (CStrUtil::casecmp(current_draw_font_style, "bold"          ) == 0)
          font_metric = font->bold;
        else if (CStrUtil::casecmp(current_draw_font_style, "underline"     ) == 0)
          font_metric = font->italic;
        else if (CStrUtil::casecmp(current_draw_font_style, "bold_underline") == 0)
          font_metric = font->boldi;
        else
          font_metric = font->normal;
      }
      else
        font_metric = font->normal;
    }

    if      (font_metric == font->normal)
      setFontType(0);
    else if (font_metric == font->bold)
      setFontType(1);
    else if (font_metric == font->italic)
      setFontType(3);
    else if (font_metric == font->boldi)
      setFontType(4);
  }
  else {
    CDHelpDatasetFont *font = NULL;

    if (current_draw_font_name != "") {
      int no = getNumDefinedFonts();

      int i = 0;

      for ( ; i < no; i++) {
        font = getDefinedFont(i);

        if (CStrUtil::casecmp(font->name, current_draw_font_name) == 0)
          break;
      }

      if (no == 0 || i >= no)
        font = NULL;
    }

    CFontPtr fontptr;

    if (font != NULL) {
      if (current_draw_font_style != "") {
        if      (CStrUtil::casecmp(current_draw_font_style, "normal"        ) == 0)
          fontptr = font->normal;
        else if (CStrUtil::casecmp(current_draw_font_style, "bold"          ) == 0)
          fontptr = font->bold;
        else if (CStrUtil::casecmp(current_draw_font_style, "underline"     ) == 0)
          fontptr = font->under;
        else if (CStrUtil::casecmp(current_draw_font_style, "bold_underline") == 0)
          fontptr = font->boldu;
        else
          fontptr = font->normal;
      }
      else
        fontptr = font->normal;
    }

    CPixelRenderer *renderer = getRenderer();

    if (fontptr.isValid())
      renderer->setFont(fontptr);

    if (current_draw_color != "") {
      CRGBA screen_color = CRGBName::toRGBA(current_draw_color);

      renderer->setForeground(screen_color);
    }
  }
}

// Reset the graphics options to their state before the setDrawGC call.
void
CDHelpDatasetData::
resetDrawGC()
{
  CPixelRenderer *renderer = getRenderer();

  if (printing_) {
    if (current_draw_line_thickness > 0)
      ps_print.setLineWidth(1);
  }
  else
    renderer->setLineWidth(0);

  renderer->setForeground(cdoc_fg_normal);
}

// Reset the graphics options to their state before the setFillGC call.
void
CDHelpDatasetData::
resetFillGC()
{
  if (printing_) {
    if (current_draw_line_thickness > 0)
      ps_print.setLineWidth(1);

    if (current_draw_fill_pixmap != "")
      ps_print.setGray(0);
  }
  else {
    CPixelRenderer *renderer = getRenderer();

    renderer->setLineWidth(0);

    renderer->setForeground(cdoc_fg_normal);
  }
}

// Reset the graphics options to their state before the setTextGC call.
void
CDHelpDatasetData::
resetTextGC()
{
  if (printing_) {
    if (ps_current_font != NULL) {
      if      (ps_current_font->current == ps_current_font->normal)
        setFontType(0);
      else if (ps_current_font->current == ps_current_font->bold)
        setFontType(1);
      else if (ps_current_font->current == ps_current_font->italic)
        setFontType(3);
      else if (ps_current_font->current == ps_current_font->boldi)
        setFontType(4);
    }
  }
  else {
    CPixelRenderer *renderer = getRenderer();

    if (getCurrentFontStruct().isValid())
      renderer->setFont(getCurrentFontStruct());

    renderer->setForeground(cdoc_fg_normal);
  }
}

// Set the color in which lines, boxes and characters are drawn.
void
CDHelpDatasetData::
setDrawColor(const std::string &color)
{
  current_draw_color = color;
}

// Set the name of font to use when drawing characters.
void
CDHelpDatasetData::
setDrawFontName(const std::string &font_name)
{
  current_draw_font_name = font_name;
}

// Set the name of font style to use when drawing characters.
void
CDHelpDatasetData::
setDrawFontStyle(const std::string &font_style)
{
  current_draw_font_style = font_style;
}

// Set the thickness used to draw lines and boxes.
void
CDHelpDatasetData::
setDrawLineThickness(const std::string &line_thickness)
{
  int thickness = 0;

  if (line_thickness != "") {
    if (sscanf(line_thickness.c_str(), "%d", &thickness) != 1)
      thickness = 0;
  }

  current_draw_line_thickness = thickness;
}

// Set the name of the pixmap used to fill boxes.
void
CDHelpDatasetData::
setDrawFillPixmap(const std::string &fill_pixmap)
{
  current_draw_fill_pixmap = fill_pixmap;
}

// Calculate the Width and Height of a Button with the specified label.
void
CDHelpDatasetData::
buttonExtent(const std::string &str, int *width, int *height)
{
  startFont("");

  stringExtent(str, width, height);

  int width1, height1;

  charExtent(CDOC_MEAN_CHAR, &width1, &height1);

  *width  += width1;
  *height += height1;

  endFont();
}

// Calculate the Width and Height of a String.
void
CDHelpDatasetData::
stringExtent(const std::string &str, int *width, int *height)
{
  CFontPtr fontptr = getCurrentFontStruct();

  if (fontptr.isValid()) {
    *width  = fontptr->getStringWidth(str);
    *height = fontptr->getCharHeight();
  }
  else {
    *width  = 8*str.size();
    *height = 10;
  }
}

// Calculate the Width and Height of a Character.
void
CDHelpDatasetData::
charExtent(uint c, int *width, int *height)
{
  CFontPtr fontptr = getCurrentFontStruct();

  if (fontptr.isValid()) {
    char str[2];

    str[0] = c;
    str[1] = '\0';

    *width  = fontptr->getStringWidth(str);
    *height = fontptr->getCharHeight();
  }
  else {
    *width  = 8;
    *height = 10;
  }
}

// Convert a X Character Coordinate String to a Pixel Value.
int
CDHelpDatasetData::
charXToPixel(CDHelpDatasetPage *, const char *xstr)
{
  int i = 0;

  int px = 0;

  if (xstr[0] == 'i') {
    px = cdoc_tab_indent*cdoc_indent_char_width;

    i++;
  }

  std::string temp_string;

  if (xstr[i] == '-' || xstr[i] == '+')
    temp_string += xstr[i++];

  while (xstr[i] != '\0' && isdigit(xstr[i]))
    temp_string += xstr[i++];

  int x = 0;

  if (! temp_string.empty())
    x = CStrUtil::toInteger(temp_string);

  int width, height;

  charExtent(CDOC_MEAN_CHAR, &width, &height);

  if      (xstr[i] == 'l' || xstr[i] == 'L')
    px += x*width/10;
  else if (xstr[i] == 'c' || xstr[i] == 'C') {
    if (getMaxPageWidth() > 0)
      px += getMaxPageWidth()/2 + x*width/10;
  }
  else if (xstr[i] == 'r' || xstr[i] == 'R') {
    if (getMaxPageWidth() > 0)
      px += getMaxPageWidth() - 1 - x*width/10;
  }
  else if (xstr[i] == '%') {
    if (getMaxPageWidth() > 0)
      px += x*(getMaxPageWidth() - 1)/100;
  }
  else
    px += x*width/10;

  if (px < 0)
    px += getMaxPageWidth();

  if (getMaxPageWidth() > 0 && px > getMaxPageWidth() - 1)
    px = getMaxPageWidth() - 1;

  return px;
}

// Convert a Y Line Coordinate String to a Pixel Value.
int
CDHelpDatasetData::
charYToPixel(CDHelpDatasetPage *page, const char *ystr)
{
  int i = 0;

  std::string temp_string;

  if (ystr[i] == '-' || ystr[i] == '+')
    temp_string += ystr[i++];

  while (ystr[i] != '\0' && isdigit(ystr[i]))
    temp_string += ystr[i++];

  int y = 0;

  if (! temp_string.empty())
    y = CStrUtil::toInteger(temp_string);

  int line_no = page->line_no;

  int py = 0;

  if (y > 0) {
    int y1 = (y/10)*10;
    int y2 = y - y1;

    while (page->line_heights[line_no - 1] == 0 && line_no < page->no_lines)
      line_no++;

    py = y2*page->line_heights[line_no - 1]/10;

    while (y1 > 0) {
      if (line_no < page->no_lines)
        line_no++;

      while (page->line_heights[line_no - 1] == 0 && line_no < page->no_lines)
        line_no++;

      py += page->line_heights[line_no - 1];

      y1 -= 10;
    }
  }
  else {
    int y1 = (-y/10)*10;
    int y2 = -y - y1;

    if (line_no > 1)
      line_no--;

    while (page->line_heights[line_no - 1] == 0 && line_no > 1)
      line_no--;

    py = -y2*page->line_heights[line_no - 1]/10;

    while (y1 > 0) {
      if (line_no > 1)
        line_no--;

      while (page->line_heights[line_no - 1] == 0 && line_no > 1)
        line_no--;

      py -= page->line_heights[line_no - 1];

      y1 -= 10;
    }
  }

  return py;
}

// Get the fully qualified file name associated with the supplied file name.
std::string
CDHelpDatasetData::
getFullPathName(const std::string &file_name)
{
  /* Set Help Directory depending on whether we have a fully qualified filename or not */

  std::string directory;

  if (file_name.empty() || file_name[0] != '/')
    directory = getProgramHelps()->getDirectory();

  /* Create File Name */

  std::string full_file_name;

  if (directory != "")
    full_file_name = directory + "/" + file_name;
  else
    full_file_name = file_name;

  /*-------------------------------------*/

  /* Ensure File Name Exists */

  struct stat file_stat;

  int error = stat(full_file_name.c_str(), &file_stat);

  if (error == -1) {
    QString msg = QString("Required File '%1' not Found").arg(full_file_name.c_str());

    CDocError::error("CDoc Help Command", msg.toStdString().c_str());

    return "";
  }

  return full_file_name;
}

//---------------

CDDrawParagraph::
CDDrawParagraph(int x, int y, int indent, int right_margin, bool continued)
{
  x_            = x;
  y_            = y;
  indent1_      = indent;
  indent2_      = 0;
  right_margin_ = right_margin;
  continued_    = continued;
  line_no1_     = -1;
  line_no2_     = -1;
}

CDDrawParagraph::
~CDDrawParagraph()
{
}
