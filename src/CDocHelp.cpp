#include <CDocI.h>
#include <CDocHelpListWidgetI.h>
#include <sys/stat.h>

#include <QApplication>

/* Defines */

#define CDOC_LIST_ALL_WIDTH  550
#define CDOC_LIST_ALL_HEIGHT 500

#define CDOC_HELP_WIDTH  650
#define CDOC_HELP_HEIGHT 500

#define CDOC_HELP_SECTION_FLAG (1<<10)

/* Structures */

struct CDGetWidgetReferenceList {
  QWidget *              widget;
  CDWidgetReferenceList *widget_reference_list;
};

struct CDGetHelp {
  int         reference;
  std::string dataset;
  CDHelp*     help;
};

struct CDGetHelpSection {
  int            reference;
  std::string    dataset;
  CDHelpSection *help_section;
};

/* Private Data */

bool cdoc_popup_on_display = true;
bool cdoc_print_on_display = false;

// Displays the contents of the specified file on the screen using CDoc's Help Dataset
// Display Panel. The dataset is assumed to be in CDoc format.

extern void
CDocDatasetDisplay(const std::string &title, const std::string &dataset)
{
  CDProgramHelps *program_helps = CDocInst->getProgramHelps("cdoc");

  if (program_helps == NULL) {
    CDocInst->initProgramHelps("cdoc", "");

    program_helps = CDocInst->getProgramHelps("cdoc");

    if (program_helps == NULL)
      return;
  }

  int ref = CDocAddHelp("cdoc", title, dataset);

  CDHelp *help = CDocGetHelpForReference(program_helps, ref);

  if (help != NULL) {
    help->setInternal(true);

    CDocWidgetHelpSelect(help, true);
  }
}

// Adds a help dataset for a specified program to that program's list of help
// dataset's for later use.
extern int
CDocAddHelp(const std::string &program, const std::string &subject, const std::string &dataset)
{
  /* Use New Routine */

  int ind = CDocAddFormattedHelp(program, subject, dataset, CDocInputCDoc, CDocOutputCDoc);

  return ind;
}

// Adds a Text String Help for a specified program to that program's list of
// help's for later use.
extern int
CDocAddTextHelp(const std::string &program, const std::string &subject, const std::string &str)
{
  CDocAddFormattedTextHelp(program, subject, str, CDocInputCDoc, CDocOutputCDoc);

  return true;
}

// Adds a formatted help dataset for a specified program to that program's list of
// help dataset's for later use.
extern int
CDocAddFormattedHelp(const std::string &program, const std::string &subject,
                     const std::string &dataset, const std::string &input_format,
                     const std::string &output_format)
{
  /* If list of all program's help control structures is
     empty then fail as this program has not been initialised */

  if (! CDocInst->anyProgramHelps()) {
    CDocWriteError("No Helps Initialised");
    return(-1);
  }

  /* Get help control structure for program */

  CDProgramHelps *program_helps = CDocInst->getProgramHelps(program);

  /* If help control structure not found then fail as this
     program has not been initialised */

  if (program_helps == NULL) {
    CDocWriteError("Helps not Initialised for %s", program.c_str());
    return(-1);
  }

  /* Check Input/Output Format */

  if      (output_format == CDocOutputCDoc) {
    if (input_format !=  CDocInputScript &&
        input_format !=  CDocInputCDoc   &&
        input_format !=  CDocInputRaw   ) {
      CDocWriteError("Invalid Input Format for CDoc");
      return(-1);
    }
  }
  else if (output_format == CDocOutputShowcase) {
    if (input_format != CDocInputShowcase) {
      CDocWriteError("Invalid Input Format for Showcase");
      return(-1);
    }
  }
  else if (output_format == CDocOutputPostscript) {
    if (input_format != CDocInputPostscript) {
      CDocWriteError("Invalid Input Format for Postscript");
      return(-1);
    }
  }
  else if (output_format == CDocOutputXWD) {
    if (input_format != CDocInputXWD) {
      CDocWriteError("Invalid Input Format for X Image");
      return(-1);
    }
  }
  else if (output_format == CDocOutputHTML) {
    if (input_format != CDocInputScript &&
        input_format != CDocInputHTML  ) {
      CDocWriteError("Invalid Input Format for HTML");
      return(-1);
    }
  }
  else if (output_format == CDocOutputILeaf) {
    if (input_format != CDocInputScript &&
        input_format != CDocInputILeaf ) {
      CDocWriteError("Invalid Input Format for Interleaf");
      return(-1);
    }
  }
  else {
    CDocWriteError("Invalid Output Format %s", output_format.c_str());
    return(-1);
  }

  /* Reuse any matching existing help structure */

  CDHelp *help = program_helps->getHelp(subject, dataset);

  if (help != NULL)
    return help->getReference();

  /* Get index of this new help */

  int no = 0;

  int reference = no + 1;

  /* Create help structure for this dataset and add to the programs
     list of help structures */

  help = new CDHelp(program_helps, reference, subject, dataset);

  if      (input_format == CDocInputScript)
    help->setFormat(CDOC_FORMAT_SCRIPT);
  else if (input_format == CDocInputCDoc)
    help->setFormat(CDOC_FORMAT_CDOC);
  else if (input_format == CDocInputRaw)
    help->setFormat(CDOC_FORMAT_RAW);
  else if (input_format == CDocInputShowcase)
    help->setFormat(CDOC_FORMAT_SHOWCASE);
  else if (input_format == CDocInputPostscript)
    help->setFormat(CDOC_FORMAT_POSTSCRIPT);
  else if (input_format == CDocInputXWD)
    help->setFormat(CDOC_FORMAT_XWD);
  else if (input_format == CDocInputHTML)
    help->setFormat(CDOC_FORMAT_HTML);
  else if (input_format == CDocInputILeaf)
    help->setFormat(CDOC_FORMAT_ILEAF);

  program_helps->addHelp(help);

  CDocInst->setOutputFormat(output_format.c_str());

  std::string full_dataset = CDocCreateDatasetName(help);

  if (help->getFormat() == CDOC_FORMAT_SCRIPT) {
    CDTempFile *temp_file = CDocInst->addTemporaryFile(program);

    const std::string &temp_file_name = temp_file->getFile();

    CDocInst->setOutputFilename(temp_file_name);

    CDocScriptProcessFile(full_dataset);

    help->setDataset(temp_file_name);

    full_dataset = CDocCreateDatasetName(help);

    int output_format = CDocInst->getOutputFormat();

    if      (output_format == CDOC_OUTPUT_CDOC      ) help->setFormat(CDOC_FORMAT_CDOC);
    else if (output_format == CDOC_OUTPUT_TROFF     ) help->setFormat(CDOC_FORMAT_RAW);
    else if (output_format == CDOC_OUTPUT_RAW_CC    ) help->setFormat(CDOC_FORMAT_RAW);
    else if (output_format == CDOC_OUTPUT_RAW       ) help->setFormat(CDOC_FORMAT_RAW);
    else if (output_format == CDOC_OUTPUT_SHOWCASE  ) help->setFormat(CDOC_FORMAT_SHOWCASE);
    else if (output_format == CDOC_OUTPUT_POSTSCRIPT) help->setFormat(CDOC_FORMAT_CDOC);
    else if (output_format == CDOC_OUTPUT_XWD       ) help->setFormat(CDOC_FORMAT_XWD);
    else if (output_format == CDOC_OUTPUT_HTML      ) help->setFormat(CDOC_FORMAT_HTML);
    else if (output_format == CDOC_OUTPUT_ILEAF     ) help->setFormat(CDOC_FORMAT_ILEAF);
  }

  return reference;
}

// Adds a Text String Help for a specified program to that program's list
// of help's for later use.
extern int
CDocAddFormattedTextHelp(const std::string &program, const std::string &subject,
                         const std::string &str, const std::string &input_format,
                         const std::string &output_format)
{
  /* Check Invocation Arguments */

  if (input_format != CDocInputScript     &&
      input_format != CDocInputCDoc       &&
      input_format != CDocInputRaw        &&
      input_format != CDocInputPostscript &&
      input_format != CDocInputHTML       &&
      input_format != CDocInputILeaf     )
    return -1;

  /********/

  /* Write String to Temporary File */

  CDTempFile *temp_file = CDocInst->addTemporaryFile(program);

  const std::string &temp_file_name = temp_file->getFile();

  FILE *fp = fopen(temp_file_name.c_str(), "w");

  if (fp == NULL) {
    delete temp_file;
    return -1;
  }

  char *string1 = CStrUtil::strdup(str);

  char *p = string1;

  char *p1;

  while ((p1 = strchr(p, '\n')) != NULL) {
    *p1 = '\0';

    fprintf(fp, "%s\n", p);

    p = p1 + 1;
  }

  if (*p != '\0')
    fprintf(fp, "%s\n", p);

  fclose(fp);

  delete [] string1;

  /**********/

  /* Add Temporary File as Formatted Help File */

  int index1 =
    CDocAddFormattedHelp(program, subject, temp_file_name, input_format, output_format);

  /**********/

  return index1;
}

// Add a section as a sub-section of an existing help.
// This is used so that when the CDoc Help Dataset Panel is displayed and this
// help section is selected the displayed help will move to the page containing
// the named section.
//
// An empty section name specifies that the first page will be shown.
//
// For Helps Displayed using CDoc the section name can be either a section or a
// sub-section. A section is referenced by its name, a Sub-Section is referenced
// by its parent Section Name and the Sub-Section Name separated by a ':' i.e. the
// Sub-Section "Introduction" of the Section "Background" would be displayed using
// "Background:Introduction".
//
// Helps Displayed using CDoc also recognise specially formatted section names :-
//
//   %%Page<no>   Move to Page <no>
//   %%PageStart  Move to First Page
//   %%PageEnd    Move to Last Page
//
// Error messages are sent to stderr as these should only occur due to errors
// during program development.
extern int
CDocAddHelpSection(const std::string &program, int reference, const std::string &section)
{
  /* If list of all program's help control structures is
     empty then fail as this program has not been initialised */

  if (! CDocInst->anyProgramHelps()) {
    CDocWriteError("No Helps Initialised");
    return(-1);
  }

  /* Get help control structure for program */

  CDProgramHelps *program_helps = CDocInst->getProgramHelps(program);

  /* If help control structure not found then fail as this
     program has not been initialised */

  if (program_helps == NULL) {
    CDocWriteError("Helps not Initialised for %s", program.c_str());
    return(-1);
  }

  /* Get help for reference */

  CDHelp *help = CDocGetHelpForReference(program_helps, reference);

  if (help == NULL) {
    CDocWriteError("Help Reference %d not Initialised", reference);
    return(-1);
  }

  /* Reuse any matching existing help structure */

  CDHelpSection *help_section = program_helps->getHelpSection(help, section);

  if (help_section)
    return help_section->getReference();

  /* Get index of this new help */

  int no = 0;

  int section_reference = (no + 1) | CDOC_HELP_SECTION_FLAG;

  /* Create help structure for this dataset and add to the
     programs list of help structures */

  help_section = new CDHelpSection(help, section_reference, section);

  program_helps->addHelpSection(help_section);

  return section_reference;
}

// Adds a list of helps to be displayed when the help key is pressed.
//
// This is handled by using the help callback which is available on all widgets.
//
// To specfify a single reference set 'references' to the address of the reference
// and set 'no_references' to 1.
extern void
CDocAddWidgetKeyboardHelps(const std::string &program, QWidget * widget,
                           int *references, int no_references)
{
  /* If list of all program's help control structures is
     empty then fail as this program has not been initialised */

  if (! CDocInst->anyProgramHelps()) {
    CDocWriteError("No Helps Initialised");
    return;
  }

  /* Get help control structure for program */

  CDProgramHelps *program_helps = CDocInst->getProgramHelps(program);

  /* If help control structure not found then fail as this
     program has not been initialised */

  if (program_helps == NULL) {
    CDocWriteError("Helps not Initialised for %s", program.c_str());
    return;
  }

  /* Get any existing helps for this widget */

  CDWidgetReferenceList *widget_reference_list =
    CDocGetReferenceListForWidget(program_helps, widget);

  /* If there are no existing helps then create the required
     structure and callbacks. */

  if (widget_reference_list == NULL) {
    /* Create a widget help structure */

    widget_reference_list = new CDWidgetReferenceList(program_helps, widget);

    /* Add to the programs list of widget help structures */

    program_helps->addWidgetReferenceList(widget_reference_list);

    /* Add a help callback for the widget */

    //connect(widget, SIGNAL(help()), this, SLOT(helpSlot()));
  }

  /* Add the supplied help references to the list of
     references for this widget */

  for (int i = 0; i < no_references; ++i)
    widget_reference_list->addReference(references[i]);
}

// Creates a Help Button which when selected will display the supplied list of helps.
//
// To specfify a single reference set 'references' to the address of the reference
// and set 'no_references' to 1.
//
// The default label for the button is 'Help' and the widget name of the button
// is 'cdoc_help_button'.

extern QPushButton *
CDocCreateHelpButton(const std::string &program, int *references, int no_references)
{
  /* If list of all program's help control structures is
     empty then fail as this program has not been initialised */

  if (! CDocInst->anyProgramHelps()) {
    CDocWriteError("No Helps Initialised");
    return NULL;
  }

  /* Get help control structure for program */

  CDProgramHelps *program_helps = CDocInst->getProgramHelps(program);

  /* If help control structure not found then fail as this
     program has not been initialised */

  if (program_helps == NULL) {
    CDocWriteError("Helps not Initialised for %s", program.c_str());
    return NULL;
  }

  /*****/

  /* Create a widget help structure for this new widget */

  CDWidgetReferenceList *widget_reference_list = new CDWidgetReferenceList(program_helps, NULL);

  /* Add to the programs list of widget help structures */

  program_helps->addWidgetReferenceList(widget_reference_list);

  /*****/

  /* create the button widget */

  CDHelpButton *button = new CDHelpButton(widget_reference_list);

  /*****/

  /* Add the supplied help references to the list of
     references for this widget */

  for (int i = 0; i < no_references; ++i)
    widget_reference_list->addReference(references[i]);

  return button;
}

// Adds a list of helps to an existing button so that they are displayed when
// the button is selected.
//
// To specfify a single reference set 'references' to the address of the reference
// and set 'no_references' to 1.
extern void
CDocAddHelpsToButton(const std::string &program, CDHelpButton *button,
                     int *references, int no_references)
{
  /* If list of all program's help control structures is
     empty then fail as this program has not been initialised */

  if (! CDocInst->anyProgramHelps()) {
    CDocWriteError("No Helps Initialised");
    return;
  }

  /* Get help control structure for program */

  CDProgramHelps *program_helps = CDocInst->getProgramHelps(program);

  /* If help control structure not found then fail as this
     program has not been initialised */

  if (program_helps == NULL) {
    CDocWriteError("Helps not Initialised for %s", program.c_str());
    return;
  }

  /*****/

  /* Get any existing helps for this widget */

  CDWidgetReferenceList *widget_reference_list = button->getWidgetReferenceList();

  /* Add the supplied help references to the list of
     references for this widget */

  for (int i = 0; i < no_references; ++i)
    widget_reference_list->addReference(references[i]);
}

// Display a list of helps
//
// To specfify a single reference set 'references' to the address of the reference
// and set 'no_references' to 1.
//
// If the supplied Widget is NULL then CDoc will start a new X Application to generate
// a Widget from which the helps can be displayed.
extern void
CDocDisplayHelpsForWidget(const std::string &program, QWidget *widget,
                          int *references, int no_references)
{
  /* If list of all program's help control structures is
     empty then fail as this program has not been initialised */

  if (! CDocInst->anyProgramHelps()) {
    CDocWriteError("No Helps Initialised");
    return;
  }

  /* Get help control structure for program */

  CDProgramHelps *program_helps = CDocInst->getProgramHelps(program);

  /* If help control structure not found then fail as this
     program has not been initialised */

  if (program_helps == NULL) {
    CDocWriteError("Helps not Initialised for %s", program.c_str());
    return;
  }

  /*****/

#if 0
  bool one_shot = false;

  if (widget == NULL)
    one_shot = true;
#endif

  /*****/

  /* Create a widget help structure */

  CDWidgetReferenceList *widget_reference_list =
    new CDWidgetReferenceList(program_helps, widget);

  /* Add the supplied help references to the list of
     references for this widget */

  for (int i = 0; i < no_references; ++i)
    widget_reference_list->addReference(references[i]);

  /*****/

  /* Display the helps by pretending we have pressed a
     button which has these helps defined on it. */

  CDocWidgetHelpButtonPress(widget_reference_list);

  /*****/

  /* Free off the widget reference list structure */

  delete widget_reference_list;
}

// Display Helps associated with a Button Widget.
//
// Does not use a Scrolled Window to display the help buttons so could be larger
// than screen size for a lot of helps on one button.
//* This situation should be rare (I hope).

extern void
CDocWidgetHelpButtonPress(CDWidgetReferenceList *widget_reference_list)
{
  /* If there is only one help then display its associated dataset ... */

  int no_helps = widget_reference_list->getNumReferences();

  if (no_helps == 1) {
    /* Get help for reference */

    int reference = widget_reference_list->getReference(0);

    CDHelp *help =
      CDocGetHelpForReference(widget_reference_list->getProgramHelps(), reference);

    /* Display help dataset if information on this help exists */

    if (help != NULL)
      CDocWidgetHelpSelect(help, true);
    else {
      CDHelpSection *help_section =
        CDocGetHelpSectionForReference(widget_reference_list->getProgramHelps(), reference);

      /* Display help dataset section if information on this help exists */

      if (help_section != NULL)
        CDocWidgetHelpSectionSelect(help_section);
    }
  }

  /* ... otherwise create a panel with a list of all the widgets
     help so the user can select which help he/she wishes to view */

  else
    CDocListWidgetHelpsCreatePanel(widget_reference_list);
}

// Displays a help's dataset in a panel.
extern void
CDocWidgetHelpSelect(CDHelp *help, int)
{
  static char system_string[1024];

  int format = help->getFormat();

  if      (format == CDOC_FORMAT_SHOWCASE) {
    const char *program = getenv("CDOC_SHOWCASE");

    if (program == NULL)
      program = "showcase";

    sprintf(system_string, "%s -v %s", program, help->getDataset().c_str());

    system(system_string);
  }
  else if (format == CDOC_FORMAT_POSTSCRIPT) {
    const char *program = getenv("CDOC_PSVIEW");

    if (program == NULL)
      program = "ghostview";

    sprintf(system_string, "%s %s &", program, help->getDataset().c_str());

    system(system_string);
  }
  else if (format == CDOC_FORMAT_XWD) {
    const char *program = getenv("CDOC_XWD");

    if (program == NULL)
      program = "xwud";

    sprintf(system_string, "%s -in %s &", program, help->getDataset().c_str());

    system(system_string);
  }
  else if (format == CDOC_FORMAT_HTML) {
    const char *program = getenv("CDOC_HTML");

    if (program == NULL)
      program = "netscape";

    sprintf(system_string, "%s %s &", program, help->getDataset().c_str());

    system(system_string);
  }
  else if (format == CDOC_FORMAT_ILEAF) {
    const char *program = getenv("CDOC_ILEAF");

    if (program == NULL)
      program = "ileaf";

    sprintf(system_string, "%s %s &", program, help->getDataset().c_str());

    system(system_string);
  }
  else {
    /* Create the name of the dataset using the directory and
       dataset name. If directory is NULL (empty str) then
       just use the dataset name. */

    std::string full_dataset = CDocCreateDatasetName(help);

    /* Open the dataset and display error message if it cannot be read */

    struct stat file_stat;

    int error = stat(full_dataset.c_str(), &file_stat);

    if (error == 0)
      CDocHelpDatasetDisplay(help, full_dataset);
    else {
      QString msg = QString("Cannot Open Help Dataset %1").arg(full_dataset.c_str());

      CDocError::error("CDoc Help Select", msg.toStdString());
    }
  }
}

// Select a Sub-Section of a currently displayed Help which is using the
// CDoc Panel for its output.
extern void
CDocWidgetHelpSectionSelect(CDHelpSection *help_section)
{
  /* Process CDoc Help Section */

  if      (help_section->getHelp()->getFormat() == CDOC_FORMAT_CDOC) {
    /* Select the help that this sub-section belongs to */

    CDocSetPopupOnDisplay(false);

    CDocWidgetHelpSelect(help_section->getHelp(), false);

    CDocSetPopupOnDisplay(true);

    /* Select the section */

    CDocHelpDatasetDisplaySection(help_section->getHelp(), help_section->getSection());
  }
}

// Get the help definition for a given reference in a program's set of helps.
//
// If no help definition is found then NULL is returned.
extern CDHelp *
CDocGetHelpForReference(CDProgramHelps *program_helps, int reference)
{
  /* Search all program's helps for a match */

  return program_helps->getHelp(reference);
}

// Get the help section definition for a given reference in a program's
// set of help sections.
//
// If no help section definition is found then NULL is returned.
extern CDHelpSection *
CDocGetHelpSectionForReference(CDProgramHelps *program_helps, int reference)
{
  /* Search all program's help sections for a match */

  return program_helps->getHelpSection(reference);
}

// Get the help definition for a given dataset name in a program's
// set of helps.
//
// If no help definition is found then NULL is returned.
extern CDHelp *
CDocGetHelpForDataset(CDProgramHelps *program_helps, const std::string &dataset)
{
  /* Search all program's helps for a match */

  return program_helps->getHelp(dataset);
}

// Get the reference list for a particular widget.
//
// Returns NULL if reference list is not found.
extern CDWidgetReferenceList *
CDocGetReferenceListForWidget(CDProgramHelps *program_helps, QWidget * widget)
{
  /* Search all program's widget reference lists for a match */

  return program_helps->getWidgetReferenceList(widget);
}

// Sets whether the Help Panel will be displayed on screen when a help
// is selected for display.
//
// Useful if you wish to start cdoc in the background so it will repond as
// quickly as possible when a help is required by user.
extern void
CDocSetPopupOnDisplay(int flag)
{
  if (flag)
    cdoc_popup_on_display = true;
  else
    cdoc_popup_on_display = false;
}

// Sets whether the Help will be printed when it is displayed.
//
// This is a hack used by the stand-alone cdoc program to get a help
// to be automatically printed.
extern void
CDocSetPrintOnDisplay(int flag)
{
  if (flag)
    cdoc_print_on_display = true;
  else
    cdoc_print_on_display = false;
}

// Gets the full pathname of the dataset associated with a help.
extern std::string
CDocCreateDatasetName(CDHelp *help)
{
  std::string dataset;

  /* If help directory is defined and help dataset is not
     an absolute filename then build the filename from
     the help directory and help dataset name ... */

  if (help->getProgramHelps()->getDirectory().size() > 0 && help->getDataset()[0] != '/') {
    dataset += help->getProgramHelps()->getDirectory();
    dataset += "/";
    dataset +=  help->getDataset();
  }

  /* ... otherwise the full dataset name is just the help dataset name */

  else
    dataset = help->getDataset();

  return dataset;
}

//---------

CDHelp::
CDHelp(CDProgramHelps *program_helps, int reference, const std::string &subject,
       const std::string &dataset, const std::string &orig_dataset) :
 program_helps_(program_helps),
 reference_    (reference),
 subject_      (subject),
 dataset_      (dataset),
 orig_dataset_ (orig_dataset)
{
  if (orig_dataset_ == "")
    orig_dataset_ = dataset_;

  internal_ = false;

  format_ = CDOC_FORMAT_RAW;
}

//---------

CDHelpSection::
CDHelpSection(CDHelp *help, int reference, const std::string &section) :
 help_     (help),
 reference_(reference),
 section_  (section)
{
}

//---------

CDWidgetReferenceList::
CDWidgetReferenceList(CDProgramHelps *program_helps, QWidget * widget) :
 program_helps_(program_helps),
 widget_       (widget)
{
}

//---------

CDHelpButton::
CDHelpButton(CDWidgetReferenceList *widget_reference_list) :
 QPushButton(NULL), widget_reference_list_(widget_reference_list)
{
  setText("Help");

  widget_reference_list_->setWidget(this);

  connect(this, SIGNAL(clicked()), this, SLOT(buttonSlot()));
}

CDHelpButton::
~CDHelpButton()
{
}

void
CDHelpButton::
buttonSlot()
{
  CDocWidgetHelpButtonPress(widget_reference_list_);
}
