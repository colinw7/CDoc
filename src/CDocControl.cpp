#include <CDocI.h>
#include <CTempFile.h>
#include <unistd.h>

CDoc *
CDoc::
getInstance()
{
  static CDoc *instance;

  if (! instance)
    instance = new CDoc;

  return instance;
}

CDoc::
CDoc()
{
  debug_level_ = 0;

  output_format_ = CDOC_OUTPUT_RAW;

  icon_bar_    = false;
  show_header_ = true;
  show_footer_ = true;
  ragged_      = false;
  first_init_  = true;
}

CDoc::
~CDoc()
{
}

/*
 * Initialises CDoc for supplied program to use helps in the supplied directory.
 *
 * program   : The name of the program which will identify the helps.
 *
 * directory : The name of the directory in which the helps are stored.
 *
 * If the named program has already been initialised then it is terminated
 * first so helps are not added twice.
 *
 * The directory must be specified explicitly so a program or subroutine must
 * know where its help files are no matter whether it is being run in
 * production, trials or development.
 *
 * An alternative method is to specify the directory as an empty str ("") and
 * explicitly specify the datasets.
 *
 * If the CDOC_SCRIPT_OPTS environment variable is set then its values will
 * be used the set the default options.
 *
 */
void
CDoc::
initProgramHelps(const std::string &program, const std::string &directory)
{
  /* Ensure this program is not already initialised */
  if (getProgramHelps(program) != NULL)
    termProgramHelps(program);

  /* Create program help control structure and add to list
     all program's help control structures */

  CDProgramHelps *program_helps = new CDProgramHelps(program, directory);

  helps_list_[program] = program_helps;

  /* If first initialise (any program) then set the default print command and
     install an exit procedure to delete any temporary files that may be hanging about */

  if (first_init_) {
    setPrintCommand("lpr %s");

    char *env;

    if ((env = getenv("CDOC_PRINT_COMMAND")) != NULL)
      setPrintCommand(env);

    first_init_ = false;
  }

  /* If the environment variable CDOC_SCRIPT_OPTS set up then
     use this to initialise the CDoc Script Options */

  char *script_opts = getenv("CDOC_SCRIPT_OPTS");

  if (script_opts != NULL) {
    std::vector<std::string> words;

    CStrUtil::toWords(script_opts, words);

    uint num_words = words.size();

    if (num_words > 0) {
      int          no_words1 = num_words;
      const char **words1    = new const char * [no_words1];

      for (int i = 0; i < no_words1; ++i)
        words1[i] = words[i].c_str();

      CDocScriptProcessOptions(words1, &no_words1);

      if (no_words1 != 0)
        CDocScriptWarning("Some Options have not been Processed");

      delete [] words1;
    }
  }
}

// Terminates CDoc for the supplied program name freeing off allocated memory
// and terminating any invocations.
void
CDoc::
termProgramHelps(const std::string &program)
{
  /* If no helps have been set up for the program then we have nothing to terminate */

  CDProgramHelps *program_helps = getProgramHelps(program);

  if (program_helps == NULL)
    return;

  /* Delete the program's help control structure from
     the master list */

  helps_list_.erase(program);

  /* Free off program's help control structure */

  delete program_helps;

  if (helps_list_.empty()) {
    output_filename_ = "";
    print_command_   = "";
  }
}

// Switches CDoc into debugging mode in which diagnostic output is produced
// which helps track down problems with the routines.
void
CDoc::
setDebug(uint debug_level)
{
  debug_level_ = debug_level;
}

uint
CDoc::
getDebug() const
{
  return debug_level_;
}

// Get the help reference structure for a particular program.
//
// Returns NULL if program is not found.
CDProgramHelps *
CDoc::
getProgramHelps(const std::string &program) const
{
  /* Search all program helps for a match */

  CDProgramHelps *program_helps;

  if (find_map_value(helps_list_, program, &program_helps))
    return program_helps;
  else
    return NULL;
}

bool
CDoc::
anyProgramHelps() const
{
  return ! helps_list_.empty();
}

// Set the format to which IBM Script text will be converted to.
//
// This routine should only be used when processing IBM Script text manually
// using CDocScriptProcessFile().
void
CDoc::
setOutputFormat(const std::string &format)
{
  if      (format == CDocOutputCDoc      ) output_format_ = CDOC_OUTPUT_CDOC;
  else if (format == CDocOutputTroff     ) output_format_ = CDOC_OUTPUT_TROFF;
  else if (format == CDocOutputRawCC     ) output_format_ = CDOC_OUTPUT_RAW_CC;
  else if (format == CDocOutputRaw       ) output_format_ = CDOC_OUTPUT_RAW;
  else if (format == CDocOutputShowcase  ) output_format_ = CDOC_OUTPUT_SHOWCASE;
  else if (format == CDocOutputPostscript) output_format_ = CDOC_OUTPUT_POSTSCRIPT;
  else if (format == CDocOutputHTML      ) output_format_ = CDOC_OUTPUT_HTML;
  else if (format == CDocOutputILeaf     ) output_format_ = CDOC_OUTPUT_ILEAF;
  else                                     output_format_ = CDOC_OUTPUT_RAW;
}

CDocOutputFormat
CDoc::
getOutputFormat() const
{
  return output_format_;
}

// Set the dataset to which converted IBM Script text will be written to.
//
// This routine should only be used when processing IBM Script text manually
// using CDocScriptProcessFile().
void
CDoc::
setOutputFilename(const std::string &filename)
{
  output_filename_ = filename;
}

const std::string &
CDoc::
getOutputFilename() const
{
  return output_filename_;
}

// Sets the command used to print a dataset.
//
// The command must contain the characters '%s' at the position where the
// dataset name should be inserted.
void
CDoc::
setPrintCommand(const std::string &command)
{
  bool string_found = false;

  const char *p1 = command.c_str();

  const char *p2;

  while (*p1 != '\0' && (p2 = strchr(p1, '%')) != NULL) {
    switch (*(p2 + 1)) {
      case 's':
        if (string_found) {
          CDocWriteError("Invalid Print Command %s", command.c_str());
          return;
        }
        else
          string_found = true;

        break;
      case '%':
        break;
      default:
        if (! isspace(*(p2 + 1))) {
          CDocWriteError("Invalid Print Command %s", command.c_str());
          return;
        }

        break;
    }

    p1 = p2 + 2;
  }

  if (string_found)
    print_command_ = command;
  else
    CDocWriteError("Invalid Print Command %s", command.c_str());
}

const std::string &
CDoc::
getPrintCommand() const
{
  return print_command_;
}

// Controls whether an Icon Bar is displayed on CDoc's Help Panel which allows
// quick access to the panel's functions.
void
CDoc::
setIconBar(bool flag)
{
  icon_bar_ = flag;
}

bool
CDoc::
getIconBar() const
{
  return icon_bar_;
}

// Controls whether the Header Area is displayed in CDoc's Help Panel.
void
CDoc::
setShowHeader(bool flag)
{
  show_header_ = flag;
}

bool
CDoc::
getShowHeader() const
{
  return show_header_;
}

// Controls whether the Footer Area is displayed in CDoc's Help Panel.
void
CDoc::
setShowFooter(bool flag)
{
  show_footer_ = flag;
}

bool
CDoc::
getShowFooter() const
{
  return show_footer_;
}

// Controls whether the Paragraphs are aligned to the right margin or are ragged.
void
CDoc::
setRagged(bool flag)
{
  ragged_ = flag;
}

bool
CDoc::
getRagged() const
{
  return ragged_;
}

// Set up the name of a new temporary file.
//
// Creates a temporary file in the current directory or the directory specified
// by the "CDOC_TEMP_DIR" environment variable (if set).
std::string
CDoc::
getTempFileName() const
{
  static const char *temp_prefix = NULL;
  static const char *temp_directory = NULL;

  if (temp_directory == NULL) {
    temp_directory = getenv("CDOC_TEMP_DIR");

    if (temp_directory == NULL)
      temp_directory = getenv("TMPDIR");

    if (temp_directory == NULL)
      temp_directory = getenv("MY_TMP_DIR");

    if (temp_directory == NULL)
      temp_directory = getcwd(NULL, 256);
  }

  if (temp_prefix == NULL) {
    temp_prefix = getenv("CDOC_TEMP_PREFIX");

    if (temp_prefix == NULL)
      temp_prefix = ".cdoc";
  }

  std::string filename;

  (void) CTempFile::getTempFileName(temp_directory, filename);

  chmod(filename.c_str(), 0777);

  return filename;
}

// Adds details of a created temporary file so that it can be deleted
// when program is terminated.
CDTempFile *
CDoc::
addTemporaryFile(const std::string &program)
{
  CDProgramHelps *program_helps = getProgramHelps(program);

  assert(program_helps != NULL);

  return program_helps->addTemporaryFile();
}

// Delete a programs temporary files whose details are stored in the
// temporary file list.
void
CDoc::
deleteTemporaryFiles(const std::string &program)
{
  CDProgramHelps *program_helps = getProgramHelps(program);

  assert(program_helps != NULL);

  program_helps->deleteTemporaryFiles();
}

//------------------

CDProgramHelps::
CDProgramHelps(const std::string &program, const std::string &directory) :
 program_  (program),
 directory_(directory)
{
}

CDProgramHelps::
~CDProgramHelps()
{
  deleteTemporaryFiles();

  for_each(help_list_.begin(), help_list_.end(), CDeletePointer());

  for_each(help_section_list_.begin(), help_section_list_.end(), CDeletePointer());

  for_each(widget_reference_list_list_.begin(), widget_reference_list_list_.end(),
           CDeletePointer());
}

void
CDProgramHelps::
addHelp(CDHelp *help)
{
  help_list_.push_back(help);
}

void
CDProgramHelps::
removeHelp(CDHelp *help)
{
  help_list_.remove(help);
}

void
CDProgramHelps::
addHelpSection(CDHelpSection *help_section)
{
  help_section_list_.push_back(help_section);
}

void
CDProgramHelps::
addWidgetReferenceList(CDWidgetReferenceList *widget_reference_list)
{
  widget_reference_list_list_.push_back(widget_reference_list);
}

CDHelp *
CDProgramHelps::
getHelp(const std::string &subject, const std::string &dataset)
{
  HelpList::const_iterator p =
    find_if(help_list_.begin(), help_list_.end(), CmpHelp(subject, dataset));

  if (p != help_list_.end())
    return *p;

  return NULL;
}

CDHelp *
CDProgramHelps::
getHelp(const std::string &dataset)
{
  HelpList::const_iterator p =
    find_if(help_list_.begin(), help_list_.end(), CmpHelp(dataset));

  if (p != help_list_.end())
    return *p;

  return NULL;
}

CDHelp *
CDProgramHelps::
getHelp(int reference)
{
  HelpList::const_iterator p =
    find_if(help_list_.begin(), help_list_.end(), CmpHelp(reference));

  if (p != help_list_.end())
    return *p;

  return NULL;
}

bool
CDProgramHelps::CmpHelp::
operator()(const CDHelp *help)
{
  if      (reference >= 0) {
    return (reference == help->getReference());
  }
  else if (subject != "") {
    return (subject == help->getSubject    () &&
            dataset == help->getOrigDataset());
  }
  else {
    return (dataset == help->getDataset() ||
            dataset == help->getOrigDataset());
  }
}

CDHelpSection *
CDProgramHelps::
getHelpSection(CDHelp *help, const std::string &section)
{
  HelpSectionList::const_iterator p =
    find_if(help_section_list_.begin(), help_section_list_.end(),
            CmpHelpSection(help, section));

  if (p != help_section_list_.end())
    return *p;

  return NULL;
}

CDHelpSection *
CDProgramHelps::
getHelpSection(int reference)
{
  HelpSectionList::const_iterator p =
    find_if(help_section_list_.begin(), help_section_list_.end(),
            CmpHelpSection(reference));

  if (p != help_section_list_.end())
    return *p;

  return NULL;
}

bool
CDProgramHelps::CmpHelpSection::
operator()(const CDHelpSection *help_section)
{
  if (reference >= 0) {
    return (reference == help_section->getReference());
  }
  else
    return (help    == help_section->getHelp() &&
            section == help_section->getSection());
}

CDWidgetReferenceList *
CDProgramHelps::
getWidgetReferenceList(QWidget * widget)
{
  WidgetReferenceListList::const_iterator p =
    find_if(widget_reference_list_list_.begin(),
            widget_reference_list_list_.end(),
            CmpHelpReferenceList(widget));

  if (p != widget_reference_list_list_.end())
    return *p;

  return NULL;
}

// Adds details of a created temporary file so that it can be deleted
// when program is terminated.
CDTempFile *
CDProgramHelps::
addTemporaryFile()
{
  CDTempFile *temp_file = new CDTempFile(program_);

  temporary_file_list_.push_back(temp_file);

  return temp_file;
}

void
CDProgramHelps::
deleteTemporaryFiles()
{
  for_each(temporary_file_list_.begin(), temporary_file_list_.end(),
           CDeletePointer());
}

//----------

bool
CDProgramHelps::CmpHelpReferenceList::
operator()(const CDWidgetReferenceList *widget_reference_list)
{
  return (widget == widget_reference_list->getWidget());
}
