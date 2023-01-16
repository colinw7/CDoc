#include <CDocI.h>
#include <CDocHelpPrintPanelI.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>

#include <CQUtil.h>

/* Data */

static const char *
cdoc_print_help_text = "\
.* cdocopts -l 2 -r 85 -pagelen 50\n\
.df docfont -*-helvetica-*-*-*-*-13-*-*-*-*-*-*-*\n\
.bf docfont\n\
:p.This panel allows the user to print any subset of pages and any\n\
number of copies of the help dataset currently being displayed by\n\
CDoc to a PostScript compatible printer.\n\
.sp 1\n\
:p.The :hp3.Pages:ehp3. section allows the user to specify all, or\n\
a selection of pages to be printed. In the latter case the\n\
:hp3.Start:ehp3. and :hp3.End:ehp3. prompts specify the range of pages\n\
to be printed. The start and end pages must be specified as integers\n\
from 1 to the number of pages in the document (see title bar) and the\n\
end must be greater than or equal to the start.\n\
.sp 1\n\
:p.The :hp3.Appearance:ehp3. section allows the user to specify how\n\
many pages appear on each piece printed sheet, in which direction the\n\
page is oriented and how many shades of gray are used for any images\n\
in the document.\n\
:p.The :hp3.Layout:ehp3. prompt specifies whether the output will be\n\
printed with one text page on one sheet of paper (Normal), printed\n\
with two text pages on one sheet of paper (Two Per Page) or printed\n\
with four text pages on one sheet of paper (Four Per Page). Using\n\
:hp3.Two Per Page:ehp3. or :hp3.Four Per Page:ehp3. mode produces a\n\
considerable saving in paper at the cost of a smaller font size.\n\
:note.The environment variable :hp2.CDOC_PRINT_PER_PAGE:ehp2. can\n\
be set to 1, 2 or 4 to set the default number of pages per sheet of\n\
paper.\n\
:p.The :hp3.Orientation:ehp3. prompt specifies whether the output will\n\
be printed in portrait mode (long side of paper positioned vertically),\n\
in landscape mode (long side of paper positioned horizontally) or\n\
whether the code will automatically select portrait or landscape mode\n\
so that the best fit with the page shape is obtained i.e. if the page\n\
is larger vertically portrait mode will be used and if the page is\n\
larger horizontally landscape mode will be used.\n\
:note.The environment variables :hp2.CDOC_PRINT_PORTRAIT:ehp2. and\n\
:hp2.CDOC_PRINT_LANDSCAPE:ehp2. can be set to make Portrait or\n\
Landscape the default.\n\
:p.The :hp3.Image Grey Levels:ehp3. prompt specifies how many levels of\n\
grey to use when displaying any embedded images. A level of 2 gives\n\
black and white, a level of 4 gives black, white and two levels of grey\n\
etc..\n\
:note.Higher levels of Grey significantly increase the size of print\n\
file and thus increase the time taken to produce the print file and\n\
slow down the printing of the file.\n\
:note.The environment variable :hp2.CDOC_PRINT_IMAGE_DEPTH:ehp2. can\n\
be set to 1, 2, 4 or 8 to specify 2, 4, 16 or 256 gray levels as\n\
the default.\n\
.sp 1\n\
:p.The :hp3.Output:ehp3. section allows the user to specify how\n\
copies will be produced and what actual UNIX command will be used\n\
to produce the print.\n\
:p.The :hp3.Copies:ehp3. prompt specifies how many copies of the\n\
requested page range are required. This must be an integer between 1\n\
and 9999.\n\
:p.The :hp3.Command:ehp3. prompt specifies the command used when printing\n\
the dataset. This should be set up correctly by the application but if\n\
it is not, or the user wishes to use a different command, then this may\n\
be entered in the prompt. The command must contain the characters '%s' at\n\
the position where the dataset name should be inserted, the rest of the\n\
string should specify a command which when entered at a shell prompt would\n\
result in a dataset being printed.\n\
:note.The environment variables :hp2.CDOC_PRINT_COMMAND:ehp2. can be\n\
set to specify the default print command.\n\
.sp 1\n\
:p.The :hp3.Print:ehp3. Button prints the dataset and exits, the\n\
:hp3.Cancel:ehp3. Button exits without printing and the :hp3.Help:ehp3.\n\
Button displays this help text.\n\
";

// Panel which allows the User to Print the current Help Dataset.
CDHelpPrintPanel::
CDHelpPrintPanel(CDHelpPanel *panel) :
 QDialog(panel), panel_(panel), help_dataset_data_(panel->getHelpDatasetData())
{
  int orient = CDOC_PRINT_AUTOMATIC;
  int copies = 1;

  if (! cdoc_print_on_display) {
    /* Override with Environment Variable if set */

    if (getenv("CDOC_PRINT_COMMAND") != NULL)
      CDocInst->setPrintCommand(getenv("CDOC_PRINT_COMMAND"));
  }

  //------

  /* Set the Title */

  std::string title = "Print " + help_dataset_data_->getHelp()->getSubject();

  setWindowTitle(title.c_str());

  //------

  QVBoxLayout *layout = new QVBoxLayout(this);

  //------

  QGroupBox *pageGroup = new QGroupBox("Pages");

  QHBoxLayout *pageLayout = new QHBoxLayout(pageGroup);

  allPages_      = new QRadioButton("All");
  specificPages_ = new QRadioButton("Specific");

  pageLayout->addWidget(allPages_);
  pageLayout->addWidget(specificPages_);

  if (help_dataset_data_->getPageNum() != 1)
    specificPages_->setChecked(true);
  else
    allPages_->setChecked(true);

  connect(allPages_     , SIGNAL(toggled(bool)), this, SLOT(update()));
  connect(specificPages_, SIGNAL(toggled(bool)), this, SLOT(update()));

  layout->addWidget(pageGroup);

  //------

  std::string str = CStrUtil::toString(help_dataset_data_->getPageNum());

  QGridLayout *startEndGrid = new QGridLayout;

  QLabel *startLabel = new QLabel("Start");
  QLabel *endLabel   = new QLabel("End");

  pageStart_ = new QLineEdit;
  pageEnd_   = new QLineEdit;

  pageStart_->setText(str.c_str());
  pageEnd_  ->setText(str.c_str());

  startEndGrid->addWidget(startLabel, 0, 0);
  startEndGrid->addWidget(pageStart_, 0, 1);
  startEndGrid->addWidget(endLabel  , 1, 0);
  startEndGrid->addWidget(pageEnd_  , 1, 1);

  layout->addLayout(startEndGrid);

  //------

  QGroupBox *appearanceGroup = new QGroupBox("Appearence");

  layout->addWidget(appearanceGroup);

  QVBoxLayout *appearanceLayout = new QVBoxLayout(appearanceGroup);

  //------

  QGroupBox *layoutGroup = new QGroupBox("Layout");

  appearanceLayout->addWidget(layoutGroup);

  QHBoxLayout *layoutLayout = new QHBoxLayout(layoutGroup);

  int per_page = 1;

  if (getenv("CDOC_PRINT_PER_PAGE") != NULL)
    per_page = CStrUtil::toInteger(getenv("CDOC_PRINT_PER_PAGE"));

  if (per_page != 1 && per_page != 2 && per_page != 4)
    per_page = 1;

  normalLayout_ = new QRadioButton("Normal");
  twoPPLayout_  = new QRadioButton("Two Per Page");
  fourPPLayout_ = new QRadioButton("Four Per Page");

  if      (per_page == 1) normalLayout_->setChecked(true);
  else if (per_page == 2) twoPPLayout_ ->setChecked(true);
  else                    fourPPLayout_->setChecked(true);

  layoutLayout->addWidget(normalLayout_);
  layoutLayout->addWidget(twoPPLayout_);
  layoutLayout->addWidget(fourPPLayout_);

  //------

  QGroupBox *orientationGroup = new QGroupBox("Orientation");

  appearanceLayout->addWidget(orientationGroup);

  QHBoxLayout *orientationLayout = new QHBoxLayout(orientationGroup);

  if      (getenv("CDOC_PRINT_PORTRAIT") != NULL)
    orient = CDOC_PRINT_PORTRAIT;
  else if (getenv("CDOC_PRINT_LANDSCAPE") != NULL)
    orient = CDOC_PRINT_LANDSCAPE;

  portrait_  = new QRadioButton("Portrait");
  landscape_ = new QRadioButton("Landscape");
  automatic_ = new QRadioButton("Automatic");

  if      (orient == CDOC_PRINT_PORTRAIT ) portrait_ ->setChecked(true);
  else if (orient == CDOC_PRINT_LANDSCAPE) landscape_->setChecked(true);
  else                                     automatic_->setChecked(true);

  orientationLayout->addWidget(portrait_);
  orientationLayout->addWidget(landscape_);
  orientationLayout->addWidget(automatic_);

  //------

  QGroupBox *levelsGroup = new QGroupBox("Image Gray Levels");

  appearanceLayout->addWidget(levelsGroup);

  QHBoxLayout *levelsLayout = new QHBoxLayout(levelsGroup);

  int image_depth = 4;

  if (getenv("CDOC_PRINT_IMAGE_DEPTH") != NULL) {
    int no = sscanf(getenv("CDOC_PRINT_IMAGE_DEPTH"), "%d", &image_depth);

    if (no != 1)
      image_depth = 4;

    if (image_depth != 1 && image_depth != 2 &&
        image_depth != 4 && image_depth != 8)
      image_depth = 4;
  }
  else
    image_depth = 4;

  depth2_   = new QRadioButton("2");
  depth4_   = new QRadioButton("4");
  depth16_  = new QRadioButton("16");
  depth256_ = new QRadioButton("256");

  if      (image_depth == 1) depth2_  ->setChecked(true);
  else if (image_depth == 2) depth4_  ->setChecked(true);
  else if (image_depth == 4) depth16_ ->setChecked(true);
  else if (image_depth == 8) depth256_->setChecked(true);
  else                       depth2_  ->setChecked(true);

  levelsLayout->addWidget(depth2_);
  levelsLayout->addWidget(depth4_);
  levelsLayout->addWidget(depth16_);
  levelsLayout->addWidget(depth256_);

  //------

  QGroupBox *outputGroup = new QGroupBox("Output");

  appearanceLayout->addWidget(outputGroup);

  QHBoxLayout *outputLayout = new QHBoxLayout(outputGroup);

  QLabel *copiesLabel = new QLabel("Copies");

  str = CStrUtil::toString(copies);

  copiesEdit_ = new QLineEdit;

  copiesEdit_->setText(str.c_str());

  outputLayout->addWidget(copiesLabel);
  outputLayout->addWidget(copiesEdit_);

  //------

  QLabel *commandLabel = new QLabel("Command");

  commandEdit_ = new QLineEdit;

  commandEdit_->setText(CDocInst->getPrintCommand().c_str());

  outputLayout->addWidget(commandLabel);
  outputLayout->addWidget(commandEdit_);

  //------

  QHBoxLayout *buttonsLayout = new QHBoxLayout;

  layout->addLayout(buttonsLayout);

  QPushButton *printButton  = new QPushButton("Print");
  QPushButton *cancelButton = new QPushButton("Cancel");
  QPushButton *helpButton   = new QPushButton("Help");

  connect(printButton , SIGNAL(clicked()), this, SLOT(printSlot()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));
  connect(helpButton  , SIGNAL(clicked()), this, SLOT(helpSlot()));

  buttonsLayout->addWidget(printButton);
  buttonsLayout->addWidget(cancelButton);
  buttonsLayout->addWidget(helpButton);

  //------

  update();

  if (cdoc_print_on_display)
    printSlot();
}

void
CDHelpPrintPanel::
update()
{
  /* Set the Start and End Page Numbers to Current Page */

  char str[32];

  sprintf(str, "%d", help_dataset_data_->getPageNum());

  pageStart_->setText(str);
  pageEnd_  ->setText(str);

  specificPages_->setChecked(help_dataset_data_->getPageNum() > 1);

  pageStart_->setEnabled(specificPages_->isChecked());
  pageEnd_  ->setEnabled(specificPages_->isChecked());
}

// Print the selected part of the current Help Dataset using the current print command.
void
CDHelpPrintPanel::
printSlot()
{
  /* Get Page Start and Page End */

  int page_start = 0;
  int page_end   = 0;

  if (! cdoc_print_on_display && specificPages_->isChecked()) {
    if (! CStrUtil::toInteger(pageStart_->text().toStdString(), &page_start)) {
      CDocError::warn("CDoc Help Print", "Invalid Integer for Page Start");
      return;
    }

    if (page_start < 1 || page_start > int(help_dataset_data_->getNumPages())) {
      CDocError::warn("CDoc Help Print", "Invalid Value for Page Start");
      return;
    }

    if (! CStrUtil::toInteger(pageEnd_->text().toStdString(), &page_end)) {
      CDocError::warn("CDoc Help Print", "Invalid Integer for Page End");
      return;
    }

    if (page_end < page_start || page_end > int(help_dataset_data_->getNumPages())) {
      CDocError::warn("CDoc Help Print", "Invalid Value for Page End");
      return;
    }
  }
  else {
    page_start = 1;
    page_end   = help_dataset_data_->getNumPages();
  }

  /* Get Number of Copies */

  int copies = 0;

  if (! CStrUtil::toInteger(copiesEdit_->text().toStdString(), &copies)) {
    CDocError::warn("CDoc Help Print", "Invalid Integer for Copies");
    return;
  }

  if (copies < 1 || copies > 9999) {
    CDocError::warn("CDoc Help Print", "Invalid Value for Copies");
    return;
  }

  /* Get the Print Command */

  std::string command = commandEdit_->text().toStdString();

  if (CDocInst->getPrintCommand() != command) {
    CDocInst->setPrintCommand(command);

    if (CDocInst->getPrintCommand() != command)
      return;
  }

  /*------------------------------------*/

  /* Get Temporary Filename */

  std::string temp_file_name = CDocInst->getTempFileName();

  /*------------------------------------*/

  for (int i = 0; i < copies; i++) {
    bool flag = help_dataset_data_->initPrint(temp_file_name);

    if (! flag) {
      CDocError::error("CDoc Help Print", "Failed to Initialize environment for Postscript");
      return;
    }

    for (int page_no = page_start; page_no <= page_end; page_no++) {
      help_dataset_data_->setPageNum(page_no);

      CDHelpDatasetPage *page = help_dataset_data_->getPage(page_no - 1);

      help_dataset_data_->drawPage(page);
    }

    help_dataset_data_->termPrint();

    /* Insert Filename into print command */

    char command_string[CDOC_COMMAND_LENGTH];

    sprintf(command_string, CDocInst->getPrintCommand().c_str(), temp_file_name.c_str());

    /* Execute print command */

    system(command_string);

    /* Remove Temporary File */

    remove(temp_file_name.c_str());
  }

  /* Popdown the Print Panel */

  hide();
}

// Exit the Print Panel without printing the Help.
void
CDHelpPrintPanel::
cancelSlot()
{
  hide();
}

// Display Help for the Print Panel.

void
CDHelpPrintPanel::
helpSlot()
{
  static int ref = -1;

  if (ref == -1)
    ref = CDocAddFormattedTextHelp(help_dataset_data_->getProgramHelps()->getProgram().c_str(),
                                   "Help for Print Help Panel",
                                   cdoc_print_help_text,
                                   CDocInputScript, CDocOutputCDoc);

  CDHelp *help = CDocGetHelpForReference(help_dataset_data_->getProgramHelps(), ref);

  help->setInternal(true);

  CDocWidgetHelpSelect(help, true);
}

int
CDHelpPrintPanel::
getPerPage() const
{
  if (normalLayout_->isChecked()) return 1;
  if (twoPPLayout_ ->isChecked()) return 2;
  if (fourPPLayout_->isChecked()) return 4;

  return 1;
}

int
CDHelpPrintPanel::
getOrient() const
{
  if      (portrait_ ->isChecked()) return CDOC_PRINT_PORTRAIT;
  else if (landscape_->isChecked()) return CDOC_PRINT_LANDSCAPE;
  else                              return CDOC_PRINT_AUTOMATIC;
}

int
CDHelpPrintPanel::
getImageDepth() const
{
  if      (depth2_  ->isChecked()) return 1;
  else if (depth4_  ->isChecked()) return 2;
  else if (depth16_ ->isChecked()) return 4;
  else if (depth256_->isChecked()) return 8;
  else                             return 1;
}

void
CDHelpPrintPanel::
setPageRange(int start, int end)
{
  pageStart_->setText(CStrUtil::toString(start).c_str());
  pageEnd_  ->setText(CStrUtil::toString(end  ).c_str());
}
