#include "CDocI.h"
#include "CDocHelpSearchPanelI.h"

#ifdef CDOC_REGEXP
#include <regex.h>
#endif

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

/* Defines */

#define CDOC_FIND_NEXT 1
#define CDOC_FIND_PREV 2

/* Data */

static const char *
cdoc_search_help_text = "\
.* cdocopts -l 2 -r 85 -pagelen 22\n\
.df docfont -*-helvetica-*-*-*-*-13-*-*-*-*-*-*-*\n\
.bf docfont\n\
:p.This panel allows the user to search for a string. The search can\n\
be for a matching word or any occurence of the string's characters\n\
and can be case-sensitive or case-insenstive. Regular Expressions\n\
can be specified in the search string on some platforms.\n\
:p.Press the :hp2.Find First:ehp2. button to find the first occurence,\n\
press the :hp2.Find Last:ehp2. button to find the last occurence, press\n\
the :hp2.Find Prev:ehp2. button to find each each previous occurence\n\
and press the :hp2.Find Next:ehp2. button to find each subsequent\n\
occurence.\n\
:p.If the string is found it will be highlighted otherwise a message\n\
panel will be displayed.\n\
:p.The :hp3.Cancel:ehp3. Button exits the panel and unhighlights any\n\
strings. The :hp3.Help:ehp3. Button displays this help.\n\
";

CDHelpSearchPanel::
CDHelpSearchPanel(CDHelpPanel *panel) :
 QDialog(panel), panel_(panel), help_dataset_data_(panel->getHelpDatasetData())
{
  start_pos = -1;
  end_pos   = -1;
  page_no   = 1;
  line_no   = 1;
  char_no   = 1;

  /******/

  /* Set the Title */

  std::string title = "Search " + help_dataset_data_->getHelp()->getSubject();

  setWindowTitle(title.c_str());

  /***********/

  QVBoxLayout *layout = new QVBoxLayout(this);

  /***********/

  QLabel *label;

  /* Create Search String Label and Text */

  QHBoxLayout *searchLayout = new QHBoxLayout;

  label = new QLabel("Search String");
  str_  = new QLineEdit;

  connect(str_, SIGNAL(returnPressed()), this, SLOT(findNextSlot()));

  searchLayout->addWidget(label);
  searchLayout->addWidget(str_ );

  layout->addLayout(searchLayout);

  /*******/

  /* Create Case Sensitive Check Box */

  case_check_ = new QCheckBox("Case Sensitive");

  layout->addWidget(case_check_);

  /*******/

  /* Create Match Word Check Box */

  word_check_ = new QCheckBox("Match Word");

  layout->addWidget(word_check_);

  /*******/

#ifdef CDOC_REGEXP
  /* Create a form for the Regular Expression Prompt */

  regexp_check_ = new QCheckBox("Regular Expression");

  layout->addWidget(regexp_check_);
#endif

  /*******/

  QHBoxLayout *findLayout = new QHBoxLayout;

  QPushButton *button;

  /* Create the Find First Button */

  button = new QPushButton("Find First");

  connect(button, SIGNAL(clicked()), this, SLOT(findFirstSlot()));

  findLayout->addWidget(button);

  /*******/

  /* Create the Find Last Button */

  button = new QPushButton("Find Last");

  connect(button, SIGNAL(clicked()), this, SLOT(findLastSlot()));

  findLayout->addWidget(button);

  /*******/

  /* Create the Find Prev Button */

  button = new QPushButton("Find Prev");

  connect(button, SIGNAL(clicked()), this, SLOT(findPrevSlot()));

  findLayout->addWidget(button);

  /*******/

  /* Create the Find Next Button */

  button = new QPushButton("Find Next");

  connect(button, SIGNAL(clicked()), this, SLOT(findNextSlot()));

  findLayout->addWidget(button);

  /*******/

  layout->addLayout(findLayout);

  /*******/

  QHBoxLayout *buttonsLayout = new QHBoxLayout;

  /* Create the Cancel Button */

  button = new QPushButton("Cancel");

  connect(button, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonsLayout->addWidget(button);

  /*******/

  /* Create the Help Button */

  button = new QPushButton("Help");

  connect(button, SIGNAL(clicked()), this, SLOT(helpSlot()));

  buttonsLayout->addWidget(button);

  layout->addLayout(buttonsLayout);
}

// Search the Help Dataset for the first occurence of the specified str.
//
// Called the Find First Button is pressed in the Search Panel.
void
CDHelpSearchPanel::
findFirstSlot()
{
  page_no = 1;
  line_no = 1;
  char_no = 1;

  findNext();
}

// Search the Help Dataset for the last occurence of the specified str.
//
// Called the Find Last Button is pressed in the Search Panel.
void
CDHelpSearchPanel::
findLastSlot()
{
  page_no = help_dataset_data_->getNumPages();
  line_no = 9999;
  char_no = 9999;

  findPrev();
}

// Search the Help Dataset for the previous occurence of the specified str.
//
// Called the Find Prev Button is pressed in the Search Panel.
void
CDHelpSearchPanel::
findPrevSlot()
{
  findPrev();
}

// Search the Help Dataset for the first occurence of the specified str.
//
// Called the Find Next Button is pressed in the Search Panel.
void
CDHelpSearchPanel::
findNextSlot()
{
  findNext();
}

// Exit the Search Panel without searching the Help.
//
// Called the Cancel Button is pressed in the Search Panel or the Panel is
// Closed by the user using the Window Manager.
void
CDHelpSearchPanel::
cancelSlot()
{
  /* Popdown the Search Panel */

  hide();

  /* Remove String Highlight */

  start_pos = -1;
  end_pos   = -1;

  panel_->updatePage();
}

// Display Help for the Search Panel.
//
// Called the Help Button is pressed in the Search Panel.
void
CDHelpSearchPanel::
helpSlot()
{
  static int ref = -1;

  if (ref == -1)
    ref = CDocAddFormattedTextHelp(help_dataset_data_->getProgramHelps()->getProgram().c_str(),
                                   "Help for Search Help Panel",
                                   cdoc_search_help_text,
                                   CDocInputScript, CDocOutputCDoc);

  CDHelp *help = CDocGetHelpForReference(help_dataset_data_->getProgramHelps(), ref);

  help->setInternal(true);

  CDocWidgetHelpSelect(help, true);
}

// Find previous occurence of search str in the current file.
//
// Gets and validates all data entered into the panel and then performs the
// required search from the current position. If the str is found then the
// position is saved in a global variable and the page on which the str is
// found is reloaded.
void
CDHelpSearchPanel::
findPrev()
{
  std::string str;
  int         match_word;
  int         case_sensitive;
  int         regular_expression;

  int flag = getValues(str, &case_sensitive, &match_word, &regular_expression);

  if (flag)
    findPrevString(str, case_sensitive, match_word, regular_expression);
}

// Find next occurence of search str in the current file.
//
// Gets and validates all data entered into the panel and then performs the
// required search from the current position. If the str is found then the
// position is saved in a global variable and the page on which the str is
// found is reloaded.
void
CDHelpSearchPanel::
findNext()
{
  std::string str;
  int         match_word;
  int         case_sensitive;
  int         regular_expression;

  int flag = getValues(str, &case_sensitive, &match_word, &regular_expression);

  if (flag)
    findNextString(str, case_sensitive, match_word, regular_expression);
}

// Find next occurence of search str in the current file.
int
CDHelpSearchPanel::
getValues(std::string &str, int *case_sensitive, int *match_word, int *regular_expression)
{
  /* Get Search String */

  str = str_->text().toStdString();

  /* Remove Spaces */

  CStrUtil::stripSpaces(str);

  /* Ignore Empty String */

  if (str[0] == '\0') {
    CDocError::warn("CDoc Help Search", "No Search String Specified");
    return false;
  }

  /*********/

  /* Get Case Sensitive Switch */

  if (case_check_->isChecked())
    *case_sensitive = true;
  else
    *case_sensitive = false;

  /*********/

  /* Get Match Word Switch */

  if (word_check_->isChecked())
    *match_word = true;
  else
    *match_word = false;

  /*********/

  /* Get Regular Expression Switch */

#ifdef CDOC_REGEXP
  if (regexp_check_->isChecked())
    *regular_expression = true;
  else
    *regular_expression = false;

  if (regular_expression) {
    regex_t regex;

    int error = regcomp(&regex, *str, 0);

    if (error != 0) {
      QString msg = QString("String '%1' is not a valid Regular Expression").arg(str);

      CDocError::error("CDoc Help Search", msg.toStdString());

      return false;
    }
  }
#else
  *regular_expression = false;
#endif

  return true;
}

// Find previous occurence of search str in the current file.
void
CDHelpSearchPanel::
findPrevString(const std::string &str, int case_sensitive, int match_word, int regular_expression)
{
  char*      p;
  char*      p1;
  char*      p2;
  int        len;
  char*      line1;
  int        in_escape;
  int        found_page_no;
  int        found_line_no;
  int        found_char_no;
  int        found_end_pos;
  int        found_start_pos;
#ifdef CDOC_REGEXP
  int        error;
  regex_t    regex;
  regmatch_t regmatch;
#endif

  int found = false;

  int page_no = 0;
  int line_no = 1;
  int char_no = 1;

  CDHelpDatasetPage *page = NULL;

  long pos = 0;

  /********/

  /* Compile Regular Expression if specified */

#ifdef CDOC_REGEXP
  if (regular_expression) {
    if (case_sensitive)
      error = regcomp(&regex, str.c_str(), REG_EXTENDED | REG_NEWLINE);
    else
      error = regcomp(&regex, str.c_str(), REG_EXTENDED | REG_NEWLINE | REG_ICASE);

    if (error != 0)
      goto findPrevString_3;
  }
#endif

  /********/

  if (help_dataset_data_->getFp() == NULL)
    goto findPrevString_3;

  /********/

  page = help_dataset_data_->getPage(page_no - 1);

  pos = page->position;

  fseek(help_dataset_data_->getFp(), pos, SEEK_SET);

  /********/

  for (;;) {
    /* End of File or New Page */

    std::string lline;

    char *line;

    if (! CDocReadLineFromFile(help_dataset_data_->getFp(), lline) || lline[0] == NEW_PAGE) {
      page_no--;

      if (page_no < 1)
        goto findPrevString_3;

      line_no = 1;

      page = help_dataset_data_->getPage(page_no - 1);

      fseek(help_dataset_data_->getFp(), page->position, SEEK_SET);

      goto findPrevString_2;
    }

    /**********/

    line = (char *) lline.c_str();

    /**********/

    /* Ignore Lines After the Last Search Line */

    if (page_no == found_page_no && line_no > found_line_no)
      goto findPrevString_1;

    /**********/

    /* CDoc Command */

    if (help_dataset_data_->getHelp()->getFormat() == CDOC_FORMAT_CDOC &&
        strncmp(line, CDOC_CMD_ID, CDOC_CMD_ID_LEN) == 0)
      goto findPrevString_1;

    /**********/

    line1 = line;

    if (page_no == found_page_no && line_no == found_line_no) {
      if (char_no == 1)
        goto findPrevString_1;

      line1[char_no - 2] = '\0';
    }

    /**********/

    CStrUtil::skipSpace(&line1);

    if (line1[0] == '\0')
      goto findPrevString_1;

    /**********/

    /* If Regular Expression then process separately using
       operating systems regular expression routines */

    if (regular_expression) {
#ifdef CDOC_REGEXP
      char  *save_p;
      int   save_len;

      save_p   = NULL;
      save_len = 0;

      for (;;) {
        if (line1 == line)
          error = regexec(&regex, line1, 1, &regmatch, 0);
        else
          error = regexec(&regex, line1, 1, &regmatch, REG_NOTBOL);

        /* No Match so stop search */

        if (error != 0)
          break;

        p   = &line1[regmatch.rm_so];
        len = (int) (regmatch.rm_eo - regmatch.rm_so);

        if (p != NULL)
          in_escape = CDocInEscapeCode(line1, p);

        while (*p != '\0' && (match_word || in_escape)) {
          if (match_word && ! in_escape) {
            p1 = CDocPrevChar(line, p);
            p2 = CDocNextChar(&p[len]);

            if ((p1 == NULL || isspace(*p1) || ispunct(*p1)) &&
                (p2 == '\0' || isspace(*p2) || ispunct(*p2)))
              break;
          }

          line1 = p + 1;
          p     = NULL;

          error = regexec(&regex, line1, 1, &regmatch, REG_NOTBOL);

          /* No Match so goto next line */

          if (error != 0)
            break;

          p   = &line1[regmatch.rm_so];
          len = (int) (regmatch.rm_eo - regmatch.rm_so);

          if (p != NULL)
            in_escape = CDocInEscapeCode(line1, p);
        }

        if (p != NULL) {
          save_p   = p;
          save_len = len;
        }

        if (*line1 == '\0')
          break;

        line1++;
      }

      p   = save_p;
      len = save_len;
#endif
    }
    else {
      /* If Case Sensitive use the standard C string search
         routines to find previous occurrence of the str
         in text ... */

      if (case_sensitive)
        p = CStrUtil::strrstr(line1, str.c_str());

      /* ... otherwise use our own string search routines
         (none case sensitive) to find previous occurrence
         of the str in text. */

      else {
        std::string::size_type pos = CStrUtil::caserfind(line1, str);

        if (pos == std::string::npos)
          p = NULL;
        else
          p = &line1[pos];
      }

      if (p != NULL)
        in_escape = CDocInEscapeCode(line1, p);

      len = str.size();

      while (p != NULL && (match_word || in_escape)) {
        int  c;

        if (match_word && ! in_escape) {
          p1 = CDocPrevChar(line, p);
          p2 = CDocNextChar(&p[len]);

          if ((p1 == NULL || isspace(*p1) || ispunct(*p1)) &&
              (p2 == NULL || isspace(*p2) || ispunct(*p2)))
            break;
        }

        if (p == line) {
          p = NULL;

          break;
        }

        p1 = p;

        c = *p1;

        *p1 = '\0';

        if (case_sensitive)
          p = CStrUtil::strrstr(line1, str.c_str());
        else {
          std::string::size_type pos = CStrUtil::caserfind(line1, str);

          if (pos == std::string::npos)
            p = NULL;
          else
            p = &line1[pos];
        }

        *p1 = c;

        if (p != NULL)
          in_escape = CDocInEscapeCode(line1, p);
      }
    }

    if (p != NULL) {
      if (found && page_no < found_page_no)
        goto findPrevString_3;

      found = true;

      found_page_no = page_no;
      found_line_no = line_no;
      found_char_no = p - line + 1;

      found_start_pos = pos + found_char_no - 1;
      found_end_pos   = found_start_pos + len - 1;

      found_char_no += len;
    }

 findPrevString_1:
    line_no++;

 findPrevString_2:
    pos = ftell(help_dataset_data_->getFp());
  }

 findPrevString_3:
  if (found) {
    page_no = found_page_no;
    line_no = found_line_no;
    char_no = found_char_no;

    start_pos = found_start_pos;
    end_pos   = found_end_pos;

    help_dataset_data_->setPageNum(page_no);

    panel_->updatePage();
  }
  else {
    QString msg = QString("String '%1' Not Found").arg(str.c_str());

    CDocError::info("CDoc Help Search", msg.toStdString());

    start_pos = -1;
    end_pos   = -1;

    panel_->updatePage();
  }
}

// Find previous occurence of search str in the current file.
void
CDHelpSearchPanel::
findNextString(const std::string &str, int case_sensitive, int match_word, int regular_expression)
{
  char*      p;
  char*      p1;
  char*      p2;
  int        len;
  char*      line1;
  int        in_escape;
#ifdef CDOC_REGEXP
  int        error;
  regex_t    regex;
  regmatch_t regmatch;
#endif

  int page_no = 0;
  int line_no = 1;
//int char_no = 1;

  CDHelpDatasetPage *page = NULL;

  long pos = 0;

  /********/

  /* Compile Regular Expression if specified */

#ifdef CDOC_REGEXP
  if (regular_expression) {
    if (case_sensitive)
      error = regcomp(&regex, str, REG_EXTENDED | REG_NEWLINE);
    else
      error = regcomp(&regex, str, REG_EXTENDED | REG_NEWLINE | REG_ICASE);

    if (error != 0)
      goto findNextString_3;
  }
#endif

  /********/

  std::string lline;

  /********/

  if (help_dataset_data_->getFp() == NULL)
    goto findNextString_3;

  /********/

  page = help_dataset_data_->getPage(page_no - 1);

  pos = page->position;

  fseek(help_dataset_data_->getFp(), pos, SEEK_SET);

  /********/

  while (CDocReadLineFromFile(help_dataset_data_->getFp(), lline)) {
    char *line = (char *) lline.c_str();

    /* New Page */

    if (line[0] == NEW_PAGE) {
      page_no++;

      line_no = 1;

      page = help_dataset_data_->getPage(page_no - 1);

      fseek(help_dataset_data_->getFp(), page->position, SEEK_SET);

      goto findNextString_2;
    }

    /**********/

    /* Ignore Lines Before/After the Last Search Line */

//  if (page_no == page_no && line_no < line_no)
//    goto findNextString_1;

    /**********/

    /* CDoc Command */

    if (help_dataset_data_->getHelp()->getFormat() == CDOC_FORMAT_CDOC &&
        strncmp(line, CDOC_CMD_ID, CDOC_CMD_ID_LEN) == 0)
      goto findNextString_1;

    /**********/

    line1 = line;

//  if (page_no == page_no && line_no == line_no)
//    line1 += char_no - 1;

    /**********/

    CStrUtil::skipSpace(&line1);

    if (line1[0] == '\0')
      goto findNextString_1;

    /**********/

    /* If Regular Expression then process separately using
       operating systems regular expression routines */

    if (regular_expression) {
#ifdef CDOC_REGEXP
      if (line1 == line)
        error = regexec(&regex, line1, 1, &regmatch, 0);
      else
        error = regexec(&regex, line1, 1, &regmatch, REG_NOTBOL);

      /* No Match so goto next line */

      if (error != 0)
        goto findNextString_1;

      p   = &line1[regmatch.rm_so];
      len = (int) (regmatch.rm_eo - regmatch.rm_so);

      if (p != NULL)
        in_escape = CDocInEscapeCode(line1, p);

      while (*p != '\0' && match_word) {
        if (match_word && ! in_escape) {
          p1 = CDocPrevChar(line, p);
          p2 = CDocNextChar(&p[len]);

          if ((p1 == NULL || isspace(*p1) || ispunct(*p1)) &&
              (p2 == '\0' || isspace(*p2) || ispunct(*p2)))
            break;
        }

        line1 = p + 1;
        p     = NULL;

        error = regexec(&regex, line1, 1, &regmatch, REG_NOTBOL);

        /* No Match so goto next line */

        if (error != 0)
          goto findNextString_1;

        p   = &line1[regmatch.rm_so];
        len = (int) (regmatch.rm_eo - regmatch.rm_so);

        if (p != NULL)
          in_escape = CDocInEscapeCode(line1, p);
      }
#endif
    }
    else {
      /* If Case Sensitive use the standard C string search
         routines to find next/previous occurrence of the str
         in text ... */

      if (case_sensitive)
        p = strstr(line1, str.c_str());

      /* ... otherwise use our own string search routines
         (none case sensitive) to find next/previous occurrence
         of the str in text. */

      else {
        std::string::size_type pos = CStrUtil::casefind(line1, str);

        if (pos == std::string::npos)
          p = NULL;
        else
          p = &line[pos];
      }

      if (p != NULL)
        in_escape = CDocInEscapeCode(line1, p);

      len = str.size();

      while (p != NULL && match_word) {
        if (match_word && ! in_escape) {
          p1 = CDocPrevChar(line, p);
          p2 = CDocNextChar(&p[len]);

          if ((p1 == NULL || isspace(*p1) || ispunct(*p1)) &&
              (p2 == NULL || isspace(*p2) || ispunct(*p2)))
            break;
        }

        if (p[len] == '\0') {
          p = NULL;

          break;
        }

        line1 = p + 1;

        if (case_sensitive)
          p = strstr(line1, str.c_str());
        else {
          std::string::size_type pos = CStrUtil::casefind(line1, str);

          if (pos == std::string::npos)
            p = NULL;
          else
            p = &line1[pos];
        }

        if (p != NULL)
          in_escape = CDocInEscapeCode(line1, p);
      }
    }

    if (p != NULL) {
      int found_page_no = page_no;
//    int found_line_no = line_no;
      int found_char_no = p - line + 1;

//    int found_start_pos = pos + found_char_no - 1;
//    int found_end_pos   = found_start_pos + len - 1;

      found_char_no += len;

      help_dataset_data_->setPageNum(found_page_no);

      panel_->updatePage();

      goto findNextString_3;
    }

 findNextString_1:
    line_no++;

 findNextString_2:
    pos = ftell(help_dataset_data_->getFp());
  }

  {
  QString msg = QString("String '%1' Not Found").arg(str.c_str());

  CDocError::info("CDoc Help Search", msg.toStdString());
  }

  start_pos = -1;
  end_pos   = -1;

  panel_->updatePage();

 findNextString_3:
  return;
}
