#include <CDocI.h>
#include <CDocHelpListAllI.h>
#include <CDocHelpListWidgetI.h>
#include <CDocHelpPrintPanelI.h>
#include <CDocHelpPagePanelI.h>
#include <CDocHelpSectionPanelI.h>
#include <CDocHelpCommandPanelI.h>
#include <CDocHelpSearchPanelI.h>

#include <CQUtil.h>
#include <CQImageButton.h>
#include <CQWidgetPixelRenderer.h>

#include <QApplication>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QMouseEvent>

/* Resources */

static bool show_section = false;

/* Internal Data */

typedef std::list<CDHelpDatasetData *> CDHelpDatasetDataList;

static CDHelpDatasetDataList help_dataset_data_list;

static const char *
cdoc_help_text = "\
.* cdocopts -l 2 -r 85 -pagelen 20\n\
.df docfont -*-helvetica-*-*-*-*-13-*-*-*-*-*-*-*\n\
.bf docfont\n\
:p.This panel displays an ASCII text file which may or may not\n\
be divided into pages. If it is divided into pages the Page\n\
menu will be available and the user can move through the\n\
dataset using the menu options.\n\
:p.The File menu has options to view associated helps files (if\n\
any), to list all the helps installed by this helps parent\n\
application (if any), to print the file, and to exit the panel.\n\
:p.For a more detailed description of the functionality of this\n\
panel and more details on the capabilities of the CDoc routines\n\
themselves see CPG for CDoc Parts 1 and 2.\n\
";

#define slct_sect_bitmap_width  20
#define slct_sect_bitmap_height 20

static uchar
slct_sect_bits[] = {
  0x00, 0x00, 0x00, 0xfe, 0xff, 0x01, 0x02, 0x00, 0x01, 0x82, 0x07, 0x07,
  0xc2, 0x0f, 0x05, 0x62, 0x18, 0x07, 0x62, 0x18, 0x05, 0x02, 0x18, 0x07,
  0x02, 0x0c, 0x05, 0x02, 0x06, 0x07, 0x02, 0x03, 0x05, 0x02, 0x03, 0x07,
  0x02, 0x00, 0x05, 0x02, 0x03, 0x07, 0x02, 0x03, 0x05, 0x02, 0x00, 0x07,
  0xfe, 0xff, 0x05, 0xa8, 0xaa, 0x06, 0xf8, 0xff, 0x07, 0x00, 0x00, 0x00
 };

#define prnt_text_bitmap_width  20
#define prnt_text_bitmap_height 20

static uchar
prnt_text_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x03,
  0x20, 0x00, 0x05, 0xa0, 0x6d, 0x07, 0x10, 0x80, 0x00, 0x50, 0xbb, 0x00,
  0x10, 0x80, 0x00, 0x10, 0x80, 0x07, 0xf8, 0xff, 0x06, 0xac, 0xaa, 0x05,
  0xfe, 0xff, 0x04, 0x02, 0x80, 0x04, 0x02, 0xb0, 0x0a, 0x03, 0x80, 0x05,
  0xfe, 0xff, 0x0a, 0x5d, 0x75, 0x05, 0xaa, 0xaa, 0x0a, 0x55, 0x55, 0x05
 };

#define srch_text_bitmap_width  20
#define srch_text_bitmap_height 20

static uchar
srch_text_bits[] = {
  0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0xc0, 0x0f, 0x00, 0xc0, 0x0c, 0x00,
  0x00, 0x0c, 0x00, 0x00, 0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x98, 0x63, 0x00, 0xa4, 0x94, 0x00,
  0xa4, 0x14, 0x00, 0xb4, 0x94, 0x00, 0xa8, 0x63, 0x00, 0x00, 0x00, 0x00
 };

#define frst_page_bitmap_width  20
#define frst_page_bitmap_height 20

static uchar
frst_page_bits[] = {
  0x00, 0x00, 0x00, 0xfc, 0xff, 0x03, 0x04, 0x00, 0x02, 0x04, 0x22, 0x02,
  0x04, 0x33, 0x02, 0x84, 0xfb, 0x02, 0xc4, 0xfd, 0x02, 0xe4, 0xfe, 0x02,
  0x74, 0xff, 0x02, 0xe4, 0xfe, 0x02, 0xc4, 0xfd, 0x02, 0x84, 0xfb, 0x02,
  0x04, 0x33, 0x02, 0x7c, 0x22, 0x02, 0x48, 0x00, 0x02, 0x50, 0x00, 0x02,
  0xa0, 0x00, 0x02, 0xc0, 0x00, 0x02, 0x80, 0xff, 0x03, 0x00, 0x00, 0x00
 };

static uchar
frst_page1_bits[] = {
  0x00, 0x00, 0x00, 0x54, 0x55, 0x01, 0x00, 0x00, 0x02, 0x04, 0x22, 0x00,
  0x00, 0x11, 0x02, 0x84, 0xaa, 0x00, 0x40, 0x55, 0x02, 0xa4, 0xaa, 0x00,
  0x50, 0x55, 0x02, 0xa4, 0xaa, 0x00, 0x40, 0x55, 0x02, 0x84, 0xaa, 0x00,
  0x00, 0x11, 0x02, 0x54, 0x22, 0x00, 0x08, 0x00, 0x02, 0x50, 0x00, 0x00,
  0xa0, 0x00, 0x02, 0x40, 0x00, 0x00, 0x80, 0xaa, 0x02, 0x00, 0x00, 0x00
 };

#define next_page_bitmap_width  20
#define next_page_bitmap_height 20

static uchar
next_page_bits[] = {
  0x00, 0x00, 0x00, 0xfc, 0xff, 0x03, 0x04, 0x00, 0x02, 0x04, 0x04, 0x02,
  0x04, 0x0c, 0x02, 0xe4, 0x1f, 0x02, 0xe4, 0x3f, 0x02, 0xe4, 0x7f, 0x02,
  0xe4, 0xff, 0x02, 0xe4, 0x7f, 0x02, 0xe4, 0x3f, 0x02, 0xe4, 0x1f, 0x02,
  0x04, 0x0c, 0x02, 0x04, 0xe4, 0x03, 0x04, 0x20, 0x01, 0x04, 0xa0, 0x00,
  0x04, 0x50, 0x00, 0x04, 0x30, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0x00, 0x00
 };

static uchar
next_page1_bits[] = {
  0x00, 0x00, 0x00, 0x54, 0x55, 0x01, 0x00, 0x00, 0x02, 0x04, 0x04, 0x00,
  0x00, 0x08, 0x02, 0x44, 0x15, 0x00, 0xa0, 0x2a, 0x02, 0x44, 0x55, 0x00,
  0xa0, 0xaa, 0x02, 0x44, 0x55, 0x00, 0xa0, 0x2a, 0x02, 0x44, 0x15, 0x00,
  0x00, 0x08, 0x02, 0x04, 0x44, 0x01, 0x00, 0x20, 0x01, 0x04, 0x80, 0x00,
  0x00, 0x50, 0x00, 0x04, 0x30, 0x00, 0xa8, 0x0a, 0x00, 0x00, 0x00, 0x00
 };

#define prev_page_bitmap_width  20
#define prev_page_bitmap_height 20

static uchar
prev_page_bits[] = {
  0x00, 0x00, 0x00, 0xfc, 0xff, 0x03, 0x04, 0x00, 0x02, 0x04, 0x02, 0x02,
  0x04, 0x03, 0x02, 0x84, 0x7f, 0x02, 0xc4, 0x7f, 0x02, 0xe4, 0x7f, 0x02,
  0xf4, 0x7f, 0x02, 0xe4, 0x7f, 0x02, 0xc4, 0x7f, 0x02, 0x84, 0x7f, 0x02,
  0x04, 0x03, 0x02, 0x7c, 0x02, 0x02, 0x48, 0x00, 0x02, 0x50, 0x00, 0x02,
  0xa0, 0x00, 0x02, 0xc0, 0x00, 0x02, 0x80, 0xff, 0x03, 0x00, 0x00, 0x00
 };

static uchar
prev_page1_bits[] = {
  0x00, 0x00, 0x00, 0xa8, 0xaa, 0x02, 0x04, 0x00, 0x00, 0x00, 0x02, 0x02,
  0x04, 0x01, 0x00, 0x80, 0x2a, 0x02, 0x44, 0x55, 0x00, 0xa0, 0x2a, 0x02,
  0x54, 0x55, 0x00, 0xa0, 0x2a, 0x02, 0x44, 0x55, 0x00, 0x80, 0x2a, 0x02,
  0x04, 0x01, 0x00, 0x28, 0x02, 0x02, 0x48, 0x00, 0x00, 0x10, 0x00, 0x02,
  0xa0, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 0x55, 0x01, 0x00, 0x00, 0x00
 };

#define last_page_bitmap_width  20
#define last_page_bitmap_height 20

static uchar
last_page_bits[] = {
  0x00, 0x00, 0x00, 0xfc, 0xff, 0x03, 0x04, 0x00, 0x02, 0x44, 0x04, 0x02,
  0xc4, 0x0c, 0x02, 0xf4, 0x1d, 0x02, 0xf4, 0x3b, 0x02, 0xf4, 0x77, 0x02,
  0xf4, 0xef, 0x02, 0xf4, 0x77, 0x02, 0xf4, 0x3b, 0x02, 0xf4, 0x1d, 0x02,
  0xc4, 0x0c, 0x02, 0x44, 0xe4, 0x03, 0x04, 0x20, 0x01, 0x04, 0xa0, 0x00,
  0x04, 0x50, 0x00, 0x04, 0x30, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0x00, 0x00
 };

static uchar
last_page1_bits[] = {
  0x00, 0x00, 0x00, 0x54, 0x55, 0x01, 0x00, 0x00, 0x02, 0x44, 0x04, 0x00,
  0x80, 0x08, 0x02, 0x54, 0x15, 0x00, 0xa0, 0x2a, 0x02, 0x54, 0x55, 0x00,
  0xa0, 0xaa, 0x02, 0x54, 0x55, 0x00, 0xa0, 0x2a, 0x02, 0x54, 0x15, 0x00,
  0x80, 0x08, 0x02, 0x44, 0x44, 0x01, 0x00, 0x20, 0x01, 0x04, 0x80, 0x00,
  0x00, 0x50, 0x00, 0x04, 0x20, 0x00, 0xa8, 0x0a, 0x00, 0x00, 0x00, 0x00
 };

#define slct_page_bitmap_width  20
#define slct_page_bitmap_height 20

static uchar
slct_page_bits[] = {
  0x00, 0x00, 0x00, 0xfc, 0xff, 0x03, 0x04, 0x00, 0x02, 0x04, 0x0f, 0x02,
  0x84, 0x1f, 0x02, 0xc4, 0x30, 0x02, 0xc4, 0x30, 0x02, 0x04, 0x30, 0x02,
  0x04, 0x18, 0x02, 0x04, 0x0c, 0x02, 0x04, 0x06, 0x02, 0x04, 0x06, 0x02,
  0x04, 0x00, 0x02, 0x04, 0xe6, 0x03, 0x04, 0x26, 0x01, 0x04, 0xa0, 0x00,
  0x04, 0x50, 0x00, 0x04, 0x30, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0x00, 0x00
 };

static uchar
slct_page1_bits[] = {
  0x00, 0x00, 0x00, 0x54, 0x55, 0x01, 0x00, 0x00, 0x02, 0x04, 0x0a, 0x00,
  0x00, 0x15, 0x02, 0x84, 0x20, 0x00, 0x40, 0x10, 0x02, 0x04, 0x20, 0x00,
  0x00, 0x10, 0x02, 0x04, 0x08, 0x00, 0x00, 0x04, 0x02, 0x04, 0x02, 0x00,
  0x00, 0x00, 0x02, 0x04, 0x44, 0x01, 0x00, 0x22, 0x01, 0x04, 0x80, 0x00,
  0x00, 0x40, 0x00, 0x04, 0x30, 0x00, 0xa8, 0x0a, 0x00, 0x00, 0x00, 0x00
 };

static CImagePtr slct_sect_image;
static CImagePtr prnt_text_image;
static CImagePtr srch_text_image;
static CImagePtr frst_page_image;
static CImagePtr frst_page1_image;
static CImagePtr prev_page_image;
static CImagePtr prev_page1_image;
static CImagePtr next_page_image;
static CImagePtr next_page1_image;
static CImagePtr last_page_image;
static CImagePtr last_page1_image;
static CImagePtr slct_page_image;
static CImagePtr slct_page1_image;

/* Internal Subroutines */

static CDHelpDatasetData *CDocGetHelpDatasetData
                           (CDHelp *);

/*****************************************************************************/

// Display a Panel containing the specified help dataset. If the required panel
// already exists then just pop it up, otherwise create it.
extern QWidget *
CDocHelpDatasetDisplay(CDHelp *help, const std::string &dataset)
{
  CDHelpDatasetData *help_dataset_data = CDocGetHelpDatasetData(help);

  if (help_dataset_data != NULL) {
    CDHelpPanel *panel = help_dataset_data->getHelpPanel();

    if (cdoc_popup_on_display)
      panel->show();

    return panel;
  }

  //------

  // Create a Panel to display a help dataset in CDoc's format. This Panel allows
  // a page at a time to be viewed and includes options which allow access to other
  // helps cross referenced by this dataset.
  //
  // If help is NULL then we assume we are displaying a stand-alone dataset in
  // cdoc form.

  /* Create and Initialise the Help Dataset Panel Data */

  help_dataset_data = new CDHelpDatasetData(help, dataset);

  /* Save the Help Dataset Structure in a Master List so
     we can check which Helps are currently being displayed
     and reuse them */

  help_dataset_data_list.push_back(help_dataset_data);

  /***********/

  /* Create Help Panel */

  CDHelpPanel *panel = help_dataset_data->getHelpPanel(true);

  /***********/

  /* Display the Panel */

  if (cdoc_popup_on_display) {
    panel->show();

    /***********/

    /* Display the Sections Panel if required */

    CDHelpSectionPanel *section_panel = help_dataset_data->getHelpPanel()->getSectionPanel();

    if (show_section && section_panel != NULL)
      section_panel->show();
  }

  /**********/

  /* Print the Help if Required */

  if (cdoc_print_on_display)
    panel->printSlot();

  /**********/

  return panel;
}

// Display the named Section in an already existing panel displaying the specified help.
//
// This routine allows both Sections and Sub-Sections to be displayed. A Section is
// referenced by its name, a Sub-Section is referenced by its parent Section Name
// and the Sub-Section Name separated by a ':' i.e. the Sub-Section "Introduction"
// of the Section "Background" would be displayed using "Background:Introduction".
extern void
CDocHelpDatasetDisplaySection(CDHelp *help, const std::string &section)
{
  CDHelpDatasetData *help_dataset_data = CDocGetHelpDatasetData(help);

  if (help_dataset_data == NULL)
    return;

  CDHelpPanel *panel = help_dataset_data->getHelpPanel();

  if (section == "") {
    if (help_dataset_data->getPageNum() != 1) {
      help_dataset_data->setPageNum(1);

      panel->updatePage();
    }
  }
  else if (strncmp(section.c_str(), "%%Page", 6) == 0) {
    int  page_no;

    if      (section == "%%PageFirst")
      page_no = 1;
    else if (section == "%%PageLast")
      page_no = help_dataset_data->getNumPages();
    else if (sscanf(section.substr(6).c_str(), "%d", &page_no) != 1)
      page_no = 1;

    if (help_dataset_data->getPageNum() != page_no) {
      help_dataset_data->setPageNum(page_no);

      panel->updatePage();
    }
  }
  else {
    CDHelpSectionPanel *section_panel = panel->getSectionPanel();

    if (section_panel) {
      CDHelpDatasetSection *help_dataset_section = section_panel->getSectionData(section);

      if (help_dataset_section          != NULL &&
          help_dataset_section->page_no != help_dataset_data->getPageNum()) {
        help_dataset_data->setPageNum(help_dataset_section->page_no);

        panel->updatePage();
      }
    }
  }

  if (cdoc_popup_on_display)
    panel->show();
}

// Get the Help Dataset Structure associated with a Help Structure.
static CDHelpDatasetData *
CDocGetHelpDatasetData(CDHelp *help)
{
  CDHelpDatasetDataList::const_iterator p1 = help_dataset_data_list.begin();
  CDHelpDatasetDataList::const_iterator p2 = help_dataset_data_list.end  ();

  for ( ; p1 != p2; ++p1) {
    CDHelpDatasetData *help_dataset_data = *p1;

    if (help_dataset_data->getHelp() == help)
      return help_dataset_data;
  }

  return NULL;
}

//------------

CDHelpPanel::
CDHelpPanel(CDHelpDatasetData *help_dataset_data) :
 QMainWindow       (NULL),
 help_dataset_data_(help_dataset_data),
 page_no_panel_    (NULL),
 command_panel_    (NULL),
 section_panel_    (NULL),
 print_panel_      (NULL),
 search_panel_     (NULL)
{
}

void
CDHelpPanel::
init()
{
  /* Set the title */

  CDHelp *help = help_dataset_data_->getHelp();

  std::string title = (help ? help->getSubject() : "Help");

  setWindowTitle(title.c_str());

  help_dataset_data_->setTitle(title);

  /***********/

  /* Create the Menu Bar containing the required functions
     (definition is in Private Header File) */

  QMenu *fileMenu = menuBar()->addMenu("&File");

  if (help != NULL && ! help->getInternal() && help->getFormat() == CDOC_FORMAT_CDOC) {
    QAction *selectSectionAction  = new QAction("Select &Section ..." , fileMenu);
    QAction *viewReferencesAction = new QAction("View &References ...", fileMenu);
    QAction *listAllAction        = new QAction("List &All Helps ..." , fileMenu);
    QAction *printAction          = new QAction("&Print ..."          , fileMenu);
    QAction *searchAction         = new QAction("Sear&ch ..."         , fileMenu);
    QAction *quitAction           = new QAction("&Quit"               , fileMenu);

    fileMenu->addAction(selectSectionAction);
    fileMenu->addAction(viewReferencesAction);
    fileMenu->addAction(listAllAction);
    fileMenu->addAction(printAction);
    fileMenu->addAction(searchAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    connect(selectSectionAction , SIGNAL(triggered()), this, SLOT(selectSectionSlot()));
    connect(viewReferencesAction, SIGNAL(triggered()), this, SLOT(viewRefsSlot()));
    connect(listAllAction       , SIGNAL(triggered()), this, SLOT(listAllSlot()));
    connect(printAction         , SIGNAL(triggered()), this, SLOT(printSlot()));
    connect(searchAction        , SIGNAL(triggered()), this, SLOT(searchSlot()));
    connect(quitAction          , SIGNAL(triggered()), this, SLOT(quitSlot()));
  }
  else {
    QAction *printAction  = new QAction("&Print ..." , fileMenu);
    QAction *searchAction = new QAction("Sear&ch ...", fileMenu);
    QAction *quitAction   = new QAction("&Quit"      , fileMenu);

    fileMenu->addAction(printAction);
    fileMenu->addAction(searchAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    connect(printAction , SIGNAL(triggered()), this, SLOT(printSlot()));
    connect(searchAction, SIGNAL(triggered()), this, SLOT(searchSlot()));
    connect(quitAction  , SIGNAL(triggered()), this, SLOT(quitSlot()));
  }

  QMenu *pageMenu = menuBar()->addMenu("&Page");

  QAction *firstPageAction  = new QAction("&First"     , pageMenu);
  QAction *lastPageAction   = new QAction("&Last"      , pageMenu);
  QAction *nextPageAction   = new QAction("&Next"      , pageMenu);
  QAction *prevPageAction   = new QAction("&Previous"  , pageMenu);
  QAction *selectPageAction = new QAction("&Select ...", pageMenu);

  pageMenu->addAction(firstPageAction);
  pageMenu->addAction(lastPageAction);
  pageMenu->addAction(nextPageAction);
  pageMenu->addAction(prevPageAction);
  pageMenu->addAction(selectPageAction);

  connect(firstPageAction , SIGNAL(triggered()), this, SLOT(firstPageSlot()));
  connect(lastPageAction  , SIGNAL(triggered()), this, SLOT(lastPageSlot()));
  connect(nextPageAction  , SIGNAL(triggered()), this, SLOT(nextPageSlot()));
  connect(prevPageAction  , SIGNAL(triggered()), this, SLOT(prevPageSlot()));
  connect(selectPageAction, SIGNAL(triggered()), this, SLOT(selectPageSlot()));

  QMenu *helpMenu = menuBar()->addMenu("&Help");

  QAction *helpAction = new QAction("&Help", helpMenu);

  helpMenu->addAction(helpAction);

  connect(helpAction, SIGNAL(triggered()), this, SLOT(helpSlot()));

  /***********/

  QWidget *icons[10];

  if (CDocInst->getIconBar()) {
    QToolBar *toolbar = addToolBar("File");

    /***********/

    loadBitmaps();

    /***********/

    /* Create the Select Section Button */

    icons[0] = new CQImageButton(slct_sect_image);

    connect(icons[0], SIGNAL(clicked()), this, SLOT(selectSectionSlot()));

    /* Create the Print Button */

    icons[1] = new CQImageButton(prnt_text_image);

    connect(icons[1], SIGNAL(clicked()), this, SLOT(printSlot()));

    /* Create the Search Button */

    icons[2] = new CQImageButton(srch_text_image);

    connect(icons[2], SIGNAL(clicked()), this, SLOT(searchSlot()));

    /* Create the First Page Button */

    icons[3] = new CQImageButton(frst_page_image);

    connect(icons[3], SIGNAL(clicked()), this, SLOT(firstPageSlot()));

    /* Create the Prev Page Button */

    icons[4] = new CQImageButton(prev_page_image);

    connect(icons[4], SIGNAL(clicked()), this, SLOT(prevPageSlot()));

    /* Create the Next Page Button */

    icons[5] = new CQImageButton(next_page_image);

    connect(icons[5], SIGNAL(clicked()), this, SLOT(nextPageSlot()));

    /* Create the Last Page Button */

    icons[6] = new CQImageButton(last_page_image);

    connect(icons[6], SIGNAL(clicked()), this, SLOT(lastPageSlot()));

    /* Create the Select Page Button */

    icons[7] = new CQImageButton(slct_page_image);

    connect(icons[7], SIGNAL(clicked()), this, SLOT(selectPageSlot()));

    for (int i = 0; i < 8; ++i)
      toolbar->addWidget(icons[i]);
  }
  else {
    icons[0] = NULL;
    icons[1] = NULL;
    icons[2] = NULL;
    icons[3] = NULL;
    icons[4] = NULL;
    icons[5] = NULL;
    icons[6] = NULL;
    icons[7] = NULL;
  }

  /***********/

  frame_ = new QScrollArea;

  setCentralWidget(frame_);

  //QGridLayout *layout = new QGridLayout(frame_);

  /***********/

  /* Create a Scrolled Window for the Help Text */

  canvas_ = new CDocHelpCanvas(this);

  //layout->addWidget(canvas_, 0, 0);

  //hbar_ = new QScrollBar(Qt::Horizontal);
  //vbar_ = new QScrollBar(Qt::Vertical);

  //QObject::connect(hbar_, SIGNAL(valueChanged(int)), canvas_, SLOT(scrolledSlot()));
  //QObject::connect(vbar_, SIGNAL(valueChanged(int)), canvas_, SLOT(scrolledSlot()));

  //layout->addWidget(hbar_, 1, 0);
  //layout->addWidget(vbar_, 0, 1);

  hbar_ = frame_->horizontalScrollBar();
  vbar_ = frame_->verticalScrollBar();

  frame_->setWidget(canvas_);

  /***********/

  setDefaultFont();

  /***********/

  help_dataset_data_->initDrawPages();

  /***********/

  setPanelSize();

  /***********/

  /* Load first page */

  help_dataset_data_->setPageNum(1);

  updatePage();

  /***********/

  /* Desensitize the Page Menu if only one Page */

  if (help_dataset_data_->getNumPages() <= 1) {
    //pageMenu->setEnabled(false);

    if (icons[3] != NULL) icons[3]->setEnabled(false);
    if (icons[4] != NULL) icons[4]->setEnabled(false);
    if (icons[5] != NULL) icons[5]->setEnabled(false);
    if (icons[6] != NULL) icons[6]->setEnabled(false);
    if (icons[7] != NULL) icons[7]->setEnabled(false);
  }
}

// Free off Help Dataset Panel's data
CDHelpPanel::
~CDHelpPanel()
{
  /* Free the Allocated Data */

  CDHelpDatasetDataList::iterator p =
    std::find(help_dataset_data_list.begin(), help_dataset_data_list.end(), help_dataset_data_);

  if (p != help_dataset_data_list.end())
    help_dataset_data_list.erase(p);

  delete help_dataset_data_;

  delete page_no_panel_;
  delete command_panel_;
  delete section_panel_;
  delete print_panel_;
  delete search_panel_;

  if (help_dataset_data_list.empty())
    exit(0);
}

// Load the Bitmaps used on the Panel's Iconbar.
void
CDHelpPanel::
loadBitmaps()
{
  if (! slct_sect_image.isValid()) {
    CImageXBMSrc src(slct_sect_bits, slct_sect_bitmap_width, slct_sect_bitmap_height);

    slct_sect_image = CImageMgrInst->lookupImage(src);
  }

  if (! prnt_text_image.isValid()) {
    CImageXBMSrc src(prnt_text_bits, prnt_text_bitmap_width, prnt_text_bitmap_height);

    prnt_text_image = CImageMgrInst->lookupImage(src);
  }

  if (! srch_text_image.isValid()) {
    CImageXBMSrc src(srch_text_bits, srch_text_bitmap_width, srch_text_bitmap_height);

    srch_text_image = CImageMgrInst->lookupImage(src);
  }

  if (! frst_page_image.isValid()) {
    CImageXBMSrc src(frst_page_bits, frst_page_bitmap_width, frst_page_bitmap_height);

    frst_page_image = CImageMgrInst->lookupImage(src);
  }

  if (! frst_page1_image.isValid()) {
    CImageXBMSrc src(frst_page1_bits, frst_page_bitmap_width, frst_page_bitmap_height);

    frst_page1_image = CImageMgrInst->lookupImage(src);
  }

  if (! prev_page_image.isValid()) {
    CImageXBMSrc src(prev_page_bits, prev_page_bitmap_width, prev_page_bitmap_height);

    prev_page_image = CImageMgrInst->lookupImage(src);
  }

  if (! prev_page1_image.isValid()) {
    CImageXBMSrc src(prev_page1_bits, prev_page_bitmap_width, prev_page_bitmap_height);

    prev_page1_image = CImageMgrInst->lookupImage(src);
  }

  if (! next_page_image.isValid()) {
    CImageXBMSrc src(next_page_bits, next_page_bitmap_width, next_page_bitmap_height);

    next_page_image = CImageMgrInst->lookupImage(src);
  }

  if (! next_page1_image.isValid()) {
    CImageXBMSrc src(next_page1_bits, next_page_bitmap_width, next_page_bitmap_height);

    next_page1_image = CImageMgrInst->lookupImage(src);
  }

  if (! last_page_image.isValid()) {
    CImageXBMSrc src(last_page_bits, last_page_bitmap_width, last_page_bitmap_height);

    last_page_image = CImageMgrInst->lookupImage(src);
  }

  if (! last_page1_image.isValid()) {
    CImageXBMSrc src(last_page1_bits, last_page_bitmap_width, last_page_bitmap_height);

    last_page1_image = CImageMgrInst->lookupImage(src);
  }

  if (! slct_page_image.isValid()) {
    CImageXBMSrc src(slct_page_bits, slct_page_bitmap_width, slct_page_bitmap_height);

    slct_page_image = CImageMgrInst->lookupImage(src);
  }

  if (! slct_page1_image.isValid()) {
    CImageXBMSrc src(slct_page1_bits, slct_page_bitmap_width, slct_page_bitmap_height);

    slct_page1_image = CImageMgrInst->lookupImage(src);
  }
}

CQWidgetPixelRenderer *
CDHelpPanel::
getRenderer()
{
  return canvas_->getRenderer();
}

// Get the default font list
void
CDHelpPanel::
setDefaultFont()
{
  CDHelpDatasetFont *default_font = new CDHelpDatasetFont;

  default_font->name = "";

  CFontSet fontSet("helvetica", 10);

  default_font->normal = fontSet.getFont(CFONT_STYLE_NORMAL);
  default_font->bold   = fontSet.getFont(CFONT_STYLE_BOLD);
  default_font->under  = fontSet.getFont(CFONT_STYLE_ITALIC);
  default_font->boldu  = fontSet.getFont(CFONT_STYLE_BOLD_ITALIC);

  help_dataset_data_->setDefaultFont(default_font);
}

// Set the size of the Scrolled Window so that it exactly displays the area of the
// largest page without scrollbars.
void
CDHelpPanel::
setPanelSize()
{
  int w = std::max(help_dataset_data_->getMaxPageWidth(), help_dataset_data_->getMaxPageWidth1());
  int h = help_dataset_data_->getMaxPageHeight();

  canvas_->resize(w, h);

#if 0
  /* Add Size of Scrollbars and Spacing */

  int spacing  = 0;
  int margin_w = 0;
  int margin_h = 0;

  int page_width  = std::max(help_dataset_data_->getMaxPageWidth(),
                             help_dataset_data_->getMaxPageWidth1()) +
                             spacing + margin_w;
  int page_height = help_dataset_data_->getMaxPageHeight() +
                    spacing + margin_h;

  int width  = vbar_->width ();
  int height = hbar_->height();

  page_height += height;
  page_width  += width  + 8;

  frame_->resize(page_width, page_height);
#endif

  QWidget *shell = CQUtil::getToplevelWidget(canvas_);

  int w1 = shell->width () - frame_->width () + w;
  int h1 = shell->height() - frame_->height() + h;

  shell->resize(w1, h1);
}

// Display Panel containing list of Sections in the displayed Help from which
// the user can select which section to view.
//
// Called when Select Section is selected from the Help Dataset Panel's File Menu.
void
CDHelpPanel::
selectSectionSlot()
{
  CDHelpSectionPanel *section_panel = getSectionPanel();

  if (section_panel == NULL) {
    CDocError::info("CDoc Help Display", "Dataset has no Sections");
    return;
  }

  section_panel->show();
}

// Display any associated Helps (via embedded commands) for the currently
// displayed dataset.
//
// Called when View References is selected from the Help Dataset Panel's File Menu.
void
CDHelpPanel::
viewRefsSlot()
{
  CDHelpCommandPanel *command_panel = getCommandPanel();

  if (command_panel == NULL) {
    CDocError::info("CDoc Help Display", "Dataset has no Reference Files");
    return;
  }

  command_panel->show();
}

// Display all the Helps associated with the current application.
//
// Called when List All Helps is selected from the Help Dataset Panel's File Menu.
void
CDHelpPanel::
listAllSlot()
{
  uint num_helps = 0;

  CDProgramHelps *program_helps = help_dataset_data_->getProgramHelps();

  const CDProgramHelps::HelpList &help_list = program_helps->getHelpList();

  CDProgramHelps::HelpList::const_iterator ph1 = help_list.begin();
  CDProgramHelps::HelpList::const_iterator ph2 = help_list.end  ();

  for ( ; ph1 != ph2; ++ph1) {
    CDHelp *help = *ph1;

    if (help->getSubject() != "" && ! help->getInternal())
      ++num_helps;
  }

  const CDProgramHelps::HelpSectionList &help_section_list = program_helps->getHelpSectionList();

  CDProgramHelps::HelpSectionList::const_iterator ps1 = help_section_list.begin();
  CDProgramHelps::HelpSectionList::const_iterator ps2 = help_section_list.end  ();

  for ( ; ps1 != ps2; ++ps1) {
    CDHelpSection *help_section = *ps1;

    if (help_section->getHelp()->getSubject() != "" &&
        ! help_section->getHelp()->getInternal())
      ++num_helps;
  }

  if (num_helps <= 1) {
    CDocError::info("CDoc Help Display", "There are no other Helps for this Application");
    return;
  }

  CDocListAllHelpsCreatePanel(help_dataset_data_->getProgramHelps());
}

// Display Panel which allows the User to Print the current Help Dataset.
//
// Called when Print is selected from the Help Dataset Panel's File Menu.
void
CDHelpPanel::
printSlot()
{
  CDHelpPrintPanel *print_panel = getPrintPanel(true);

  print_panel->show();
}

// Display Panel which allows the User to Search for a String in the current Help Dataset.
//
// Called when Search is selected from the Help Dataset Panel's File Menu.
void
CDHelpPanel::
searchSlot()
{
  CDHelpSearchPanel *search_panel = getSearchPanel(true);

  search_panel->show();
}

// Exit Help Dataset Panel
//
// Destroys the Page Number and Command List Panels as well if they exist.
void
CDHelpPanel::
quitSlot()
{
  CDHelpPanel *help_panel = help_dataset_data_->getHelpPanel();

  if (! help_dataset_data_->getPopupOnDisplay())
    help_panel->hide();
  else
    delete help_panel;
}

// Display the first page of the dataset.
//
// Called when First Page is selected from the Help Dataset Panel's Page Menu.
void
CDHelpPanel::
firstPageSlot()
{
  if (help_dataset_data_->getPageNum() != 1) {
    /* Load Required Page Number */
    help_dataset_data_->setPageNum(1);

    updatePage();
  }
}

// Display the last page of the dataset.
//
// Called when Last Page is selected from the Help Dataset Panel's Page Menu.
void
CDHelpPanel::
lastPageSlot()
{
  if (help_dataset_data_->getPageNum() != (int) help_dataset_data_->getNumPages()) {
    /* Load Required Page Number */
    help_dataset_data_->setPageNum(help_dataset_data_->getNumPages());

    updatePage();
  }
}

// Display the next page of the dataset.
//
// Called when Next Page is selected from the Help Dataset Panel's Page Menu.
void
CDHelpPanel::
nextPageSlot()
{
  if (help_dataset_data_->getPageNum() != (int) help_dataset_data_->getNumPages()) {
    /* Load Required Page Number */
    help_dataset_data_->setPageNum(help_dataset_data_->getPageNum() + 1);

    updatePage();
  }
  else
    QApplication::beep();
}

// Display the previous page of the dataset.
//
// Called when Previous Page is selected from the Help Dataset Panel's Page Menu.
void
CDHelpPanel::
prevPageSlot()
{
  if (help_dataset_data_->getPageNum() != 1) {
    /* Load Required Page Number */
    help_dataset_data_->setPageNum(help_dataset_data_->getPageNum() - 1);

    updatePage();
  }
  else
    QApplication::beep();
}

// Display a Panel allowing the Page Number to be selected via a Slider Widget.
//
// Called when Select Page is selected from the Help Dataset Panel's Page Menu.
void
CDHelpPanel::
selectPageSlot()
{
  if (help_dataset_data_->getNumPages() == 1) {
    CDocError::info("CDoc Help Display", "Dataset has only one Page");
    return;
  }

  CDHelpPageNoPanel *page_no_panel = getPageNoPanel(true);

  page_no_panel->show();
}

// Display a Help for the Dataset Panel.
//
// Called when Help is selected from the Help Dataset Panel's Help Menu.
void
CDHelpPanel::
helpSlot()
{
  static int ref = -1;

  if (ref == -1)
    ref = CDocAddFormattedTextHelp(help_dataset_data_->getProgramHelps()->getProgram().c_str(),
                                   "Help for Help Dataset Panel",
                                   cdoc_help_text,
                                   CDocInputScript, CDocOutputCDoc);

  CDHelp *help = CDocGetHelpForReference(help_dataset_data_->getProgramHelps(), ref);

  help->setInternal(true);

  CDocWidgetHelpSelect(help, true);
}

CDHelpPageNoPanel *
CDHelpPanel::
getPageNoPanel(bool create_it)
{
  if (! page_no_panel_ && create_it)
    page_no_panel_ = new CDHelpPageNoPanel(this);

  return page_no_panel_;
}

CDHelpCommandPanel *
CDHelpPanel::
getCommandPanel(bool create_it)
{
  if (! command_panel_ && create_it)
    command_panel_ = new CDHelpCommandPanel(this);

  return command_panel_;
}

CDHelpSectionPanel *
CDHelpPanel::
getSectionPanel(bool create_it)
{
  if (! section_panel_ && create_it)
    section_panel_ = new CDHelpSectionPanel(this);

  return section_panel_;
}

CDHelpPrintPanel *
CDHelpPanel::
getPrintPanel(bool create_it)
{
  if (! print_panel_ && create_it)
    print_panel_ = new CDHelpPrintPanel(this);

  return print_panel_;
}

CDHelpSearchPanel *
CDHelpPanel::
getSearchPanel(bool create_it)
{
  if (! search_panel_ && create_it)
    search_panel_ = new CDHelpSearchPanel(this);

  return search_panel_;
}

void
CDHelpPanel::
draw()
{
  CDHelpDatasetPage *page = help_dataset_data_->getPage(help_dataset_data_->getPageNum() - 1);

  help_dataset_data_->loadPage(page);

  help_dataset_data_->drawPage(page);
}

void
CDHelpPanel::
mouseButtonEvent(bool press, QMouseEvent *e)
{
  static CDHelpDatasetCommandData *last_cmd_data = NULL;
  static int                       x;
  static int                       y;

  // TODO: need to move this into draw loop
  if (last_cmd_data != NULL) {
    int x1 = last_cmd_data->getX();
    int y1 = last_cmd_data->getY();

    help_dataset_data_->drawButton(last_cmd_data->getTitle(), &x1, &y1, CDOC_BUTTON_OUT);

    last_cmd_data = NULL;
  }

  if (e->button() == 1) {
    if (press) {
      int no = help_dataset_data_->getNumPageCommandDatas();

      for (int i = 0; i < no; i++) {
        CDHelpDatasetCommandData *cmd_data = help_dataset_data_->getPageCommandData(i);

        if (cmd_data->isInside(e->x(), e->y())) {
          int x1 = cmd_data->getX();
          int y1 = cmd_data->getY();

          help_dataset_data_->drawButton(cmd_data->getTitle(), &x1, &y1, CDOC_BUTTON_IN);

          last_cmd_data = cmd_data;

          break;
        }
      }

      x = e->x();
      y = e->y();
    }
    else {
      int no = help_dataset_data_->getNumPageCommandDatas();

      for (int i = 0; i < no; i++) {
        CDHelpDatasetCommandData *cmd_data = help_dataset_data_->getPageCommandData(i);

        if (cmd_data->isInside(x, y) && cmd_data->isInside(e->x(), e->y())) {
          cmd_data->exec();

          break;
        }
      }

      no = help_dataset_data_->getNumPageRefDatas();

      for (int i = 0; i < no; ++i) {
        CDHelpDatasetRefData *ref_data = help_dataset_data_->getPageRefData(i);

        if (x >= ref_data->x && x < (ref_data->x + ref_data->width ) &&
            y >= ref_data->y && y < (ref_data->y + ref_data->height) &&
            e->x() >= ref_data->x && e->x() < (ref_data->x + ref_data->width ) &&
            e->y() >= ref_data->y && e->y() < (ref_data->y + ref_data->height)) {
          if (help_dataset_data_->getPageNum() != ref_data->page_no) {
            help_dataset_data_->addBackPageHistory(help_dataset_data_->getPageNum());

            help_dataset_data_->setPageNum(ref_data->page_no);

            updatePage();
          }

          break;
        }
      }

      no = help_dataset_data_->getNumPageHookDatas();

      for (int i = 0; i < no; ++i) {
        CDHelpDatasetHookData *hook_data = help_dataset_data_->getPageHookData(i);

        if (x >= hook_data->x && x <  (hook_data->x + hook_data->width ) &&
            y >= hook_data->y && y <  (hook_data->y + hook_data->height) &&
            e->x() >= hook_data->x && e->x() < (hook_data->x + hook_data->width ) &&
            e->y() >= hook_data->y && e->y() < (hook_data->y + hook_data->height)) {
          CDocHelpProcessHook(hook_data->id, hook_data->data);

          break;
        }
      }
    }
  }
  else if (e->button() == 2) {
    if (press) {
      help_dataset_data_->setSelectXMin(
        std::min(e->x(), help_dataset_data_->getMaxPageWidth()  - 1));
      help_dataset_data_->setSelectYMin(
        std::min(e->y(), help_dataset_data_->getMaxPageHeight() - 1));

      if (help_dataset_data_->getSelectXMin() < 0) help_dataset_data_->setSelectXMin(0);
      if (help_dataset_data_->getSelectYMin() < 0) help_dataset_data_->setSelectYMin(0);

      help_dataset_data_->setSelectXMax(help_dataset_data_->getSelectXMin());
      help_dataset_data_->setSelectYMax(help_dataset_data_->getSelectYMin());

      help_dataset_data_->xorSelection();
    }
    else {
      help_dataset_data_->xorSelection();

      help_dataset_data_->fixSelectRange();

      if (help_dataset_data_->getSelectXMin() < help_dataset_data_->getSelectXMax() &&
          help_dataset_data_->getSelectYMin() < help_dataset_data_->getSelectYMax())
        help_dataset_data_->setSelectPage((CDHelpDatasetPage *)
          help_dataset_data_->getPage(help_dataset_data_->getPageNum() - 1));
      else
        help_dataset_data_->setSelectPage(NULL);

      help_dataset_data_->setSelectX(-1);
      help_dataset_data_->setSelectY(-1);

      if (help_dataset_data_->getSelectPage() != NULL)
        updatePage();
    }
  }
  else if (e->button() == 3) {
    if (press) {
    }
    else {
      if (help_dataset_data_->getNumPageHistories() != 0) {
        int page_no = help_dataset_data_->popBackPageHistory();

        help_dataset_data_->addFrontPageHistory(help_dataset_data_->getPageNum());

        help_dataset_data_->setPageNum(page_no);

        updatePage();
      }
    }
  }
}

void
CDHelpPanel::
mouseMoveEvent(QMouseEvent *e)
{
  help_dataset_data_->xorSelection();

  help_dataset_data_->setSelectXMax(std::min(e->x(), help_dataset_data_->getMaxPageWidth()  - 1));
  help_dataset_data_->setSelectYMax(std::min(e->y(), help_dataset_data_->getMaxPageHeight() - 1));

  if (help_dataset_data_->getSelectXMax() < 0) help_dataset_data_->setSelectXMax(0);
  if (help_dataset_data_->getSelectYMax() < 0) help_dataset_data_->setSelectYMax(0);

  help_dataset_data_->xorSelection();
}

//---------------

CDocHelpCanvas::
CDocHelpCanvas(CDHelpPanel *panel) :
 QWidget(NULL), panel_(panel)
{
  renderer_ = new CQWidgetPixelRenderer(this);
}

CQWidgetPixelRenderer *
CDocHelpCanvas::
getRenderer()
{
  return renderer_;
}

// Redraw the currently displayed page.
void
CDocHelpCanvas::
paintEvent(QPaintEvent *)
{
  panel_->draw();
}

void
CDocHelpCanvas::
resizeEvent(QResizeEvent *)
{
  renderer_->updateSize(width(), height());
}

// Routine called when the mouse button is pressed in the scrolled window. This routine
// checks whether a page reference or command button is selected and if so performs the
// associated operation.
void
CDocHelpCanvas::
mousePressEvent(QMouseEvent *e)
{
  panel_->mouseButtonEvent(true, e);
}

void
CDocHelpCanvas::
mouseReleaseEvent(QMouseEvent *e)
{
  panel_->mouseButtonEvent(false, e);
}

// Routine called when the middle mouse button is held down and moved in the
// scrolled window. This updates the box within which text will be selected.
void
CDocHelpCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  panel_->mouseMoveEvent(e);
}

void
CDocHelpCanvas::
scrolledSlot()
{
}

//---------------

// Routine called when the X selection has been taken by another program.
// This disables the text selection area and redraws the current page if it
// has selected text on it.
void
CDHelpPanel::
clipBoardSlot()
{
  CDHelpDatasetPage *current_page =
    help_dataset_data_->getPage(help_dataset_data_->getPageNum() - 1);

  if (current_page == help_dataset_data_->getSelectPage()) {
    help_dataset_data_->setSelectPage(NULL);

    updatePage();
  }
  else
    help_dataset_data_->setSelectPage(NULL);
}

void
CDHelpPanel::
updatePage()
{
  canvas_->update();
}

//-------------

// Execute a Help Command.
void
CDHelpDatasetCommandData::
exec()
{
  /* Get Data associated with Selected Item */

  /* Extract the name of the command from the command string */

  uint i   = 0;
  uint len = command_.size();

  std::string command_name;

  while (i < len && ! isspace(command_[i]))
    command_name += command_[i++];

  /* Skip Spaces after Command */

  CStrUtil::skipSpace(command_, &i);

  /*-------------------------------------*/

  /* If command is 'showcase' then run 'showcase' (to display
     figures) with the view option set passing it a dataset
     formed from the program's help directory and the command
     arguments (the dataset name) */

  if      (command_name == "showcase") {
    const char *command = getenv("CDOC_SHOWCASE");

    if (command == NULL)
      command = "showcase";

    std::string file_name = help_dataset_data_->getFullPathName(&command[i]);

    if (file_name == "")
      return;

    char system_string[512];

    sprintf(system_string, "%s -v %s", command, file_name.c_str());

    system(system_string);
  }

  /* If command is 'postscript' then run 'xpsview' (to display
     figures) passing it a dataset formed from the program's
     help directory and the command arguments (the dataset name) */

  else if (command_name == "postscript") {
    const char *command = getenv("CDOC_PSVIEW");

    if (command == NULL)
      command = "ghostview";

    std::string file_name = help_dataset_data_->getFullPathName(&command[i]);

    if (file_name == "")
      return;

    char system_string[512];

    sprintf(system_string, "%s %s &", command, file_name.c_str());

    system(system_string);
  }

  /* If command is 'xwd' or 'xwd.Z' then run 'xwud' (to display
     figures) passing it a dataset formed from the program's help
     directory and the command arguments (the dataset name) */

  else if (command_name == "xwd" || command_name == "xwd.Z") {
    const char *command = getenv("CDOC_XWD");

    if (command == NULL)
      command = "xwud";

    std::string file_name = help_dataset_data_->getFullPathName(&command[i]);

    if (file_name == "")
      return;

    /* If we have a 'xwd.Z' file then we must first
       uncompress it to a temporary file */

    char system_string[512];

    if (command_name == "xwd.Z") {
      std::string temp_file = CDocInst->getTempFileName();

      sprintf(system_string, "cp %s %s.Z", file_name.c_str(), temp_file.c_str());

      system(system_string);

      sprintf(system_string, "uncompress %s.Z", temp_file.c_str());

      system(system_string);

      sprintf(system_string, "%s -in %s &", command, temp_file.c_str());

      system(system_string);

      remove(temp_file.c_str());
    }
    else {
      sprintf(system_string, "%s -in %s &", command, file_name.c_str());

      system(system_string);
    }
  }

  /* If command is 'cdoc_help' then search the program's helps
     for a dataset with the same name as the argument and if found
     display it in another Panel */

  else if (command_name == "cdoc_help") {
    CDHelp *help = CDocGetHelpForDataset(help_dataset_data_->getProgramHelps(), &command_[i]);

    /* Add Help if doesn't already exist */

    if (help == NULL) {
      CDocAddFormattedHelp(help_dataset_data_->getProgramHelps()->getProgram(),
                           title_, &command_[i], CDocInputCDoc, CDocOutputCDoc);

      help = CDocGetHelpForDataset(help_dataset_data_->getProgramHelps(), &command_[i]);
    }

    if (help != NULL)
      CDocWidgetHelpSelect(help, true);
    else
      CDocError::warn("CDoc Help Command", "Required CDoc Help not Found");
  }

  /* If command is 'script' then search the program's helps
     for a dataset with the same as the argument and if found
     display it in another Panel */

  else if (command_name == "script") {
    CDHelp *help = CDocGetHelpForDataset(help_dataset_data_->getProgramHelps(), &command_[i]);

    /* Add Help if doesn't already exist */

    if (help == NULL) {
      CDocAddFormattedHelp(help_dataset_data_->getProgramHelps()->getProgram(),
                           title_, &command_[i], CDocInputScript, CDocOutputCDoc);

      help = CDocGetHelpForDataset(help_dataset_data_->getProgramHelps(), &command_[i]);
    }

    if (help != NULL)
      CDocWidgetHelpSelect(help, true);
    else
      CDocError::warn("CDoc Help Command", "Required CDoc Help not Found");
  }

  /* If command is 'exec' then run it with the system call */

  else if (command_name == "exec") {
    char system_string[512];

    sprintf(system_string, "%s &", &command_[i]);

    system(system_string);
  }

  /* If command no recognised output error message */

  else {
    QString msg = QString("Invalid Command Name '%1'").arg(command_name.c_str());

    CDocError::error("CDoc Help Command", msg.toStdString().c_str());
  }
}
