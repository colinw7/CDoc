#include "CDocI.h"
#include "CDocHelpListAllI.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <CQUtil.h>

#define CDOC_LIST_ALL_WIDTH  550
#define CDOC_LIST_ALL_HEIGHT 500

// Lists all the helps (by title) associated with a particular program.
extern void
CDocListAllHelpsCreatePanel(CDProgramHelps *program_helps)
{
  CDListAllData *list_all_data = new CDListAllData(program_helps);

  list_all_data->show();
}

CDListAllData::
CDListAllData(CDProgramHelps *program_helps) :
 QWidget(NULL)
{
  program_helps_ = program_helps;

  setWindowTitle("Help for ...");

  QVBoxLayout *layout = new QVBoxLayout(this);

  help_list_ = new QListWidget;

  help_list_->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(help_list_, SIGNAL(itemSelectionChanged()), this, SLOT(listSelectionSlot()));

  layout->addWidget(help_list_);

  /***********/

  /* Add all the program's helps to the Scrolled List */

  const CDProgramHelps::HelpList &help_list = program_helps->getHelpList();

  auto ph1 = help_list.begin();
  auto ph2 = help_list.end  ();

  for ( ; ph1 != ph2; ++ph1)
    addHelp(*ph1);

  const CDProgramHelps::HelpSectionList &help_section_list = program_helps->getHelpSectionList();

  auto ps1 = help_section_list.begin();
  auto ps2 = help_section_list.end  ();

  for ( ; ps1 != ps2; ++ps1)
    addHelpSection(*ps1);
}

CDListAllData::
~CDListAllData()
{
  for_each(help_data_list_.begin(), help_data_list_.end(), CDeletePointer());
}

// Adds a help button to the panel.
void
CDListAllData::
addHelp(CDHelp *help)
{
  /* Create Help Button using the subject as its label */

  if (help->getSubject() == "" || help->getInternal())
    return;

  help_list_->addItem(help->getSubject().c_str());

  /* Add Help Information To List */

  CDListAllHelpData *list_all_help_data = new CDListAllHelpData;

  list_all_help_data->type = LIST_ALL_HELP;
  list_all_help_data->data = reinterpret_cast<char *>(help);

  help_data_list_.push_back(list_all_help_data);
}

// Adds a help button for a sub section of a help to a panel.
void
CDListAllData::
addHelpSection(CDHelpSection *help_section)
{
  /* Create Help Button using the subject as its label */

  if (help_section->getHelp()->getSubject() == "" || help_section->getHelp()->getInternal())
    return;

  std::string str;

  str += help_section->getHelp()->getSubject();
  str += " - ";
  str += help_section->getSection();

  help_list_->addItem(str.c_str());

  /* Add Help Information To List */

  CDListAllHelpData *list_all_help_data = new CDListAllHelpData;

  list_all_help_data->type = LIST_ALL_HELP_SECTION;
  list_all_help_data->data = reinterpret_cast<char *>(help_section);

  help_data_list_.push_back(list_all_help_data);
}

// Displays a help's dataset in a panel.
void
CDListAllData::
listSelectionSlot()
{
  int pos = help_list_->currentRow();

  /* Get Data associated with Selected Item */

  CDListAllHelpData *list_all_help_data = help_data_list_[pos - 1];

  /* Deselect Item */

  //help_list_->deselectAll();

  /* Process Item */

  if (list_all_help_data != NULL) {
    if      (list_all_help_data->type == LIST_ALL_HELP)
      CDocWidgetHelpSelect(reinterpret_cast<CDHelp *>(list_all_help_data->data), true);
    else if (list_all_help_data->type == LIST_ALL_HELP_SECTION)
      CDocWidgetHelpSectionSelect(reinterpret_cast<CDHelpSection *>(list_all_help_data->data));
  }
}
