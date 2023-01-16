#include "CDocI.h"
#include "CDocHelpListWidgetI.h"
#include "CDocHelpListAllI.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <CQUtil.h>

#define CDOC_LIST_WIDGET_WIDTH  550
#define CDOC_LIST_WIDGET_HEIGHT 500

// Lists the helps (by title) associated with a particular Widget.
extern void
CDocListWidgetHelpsCreatePanel(CDWidgetReferenceList *widget_reference_list)
{
  CDListWidgetData *list_widget_data = new CDListWidgetData(widget_reference_list);

  list_widget_data->show();
}

CDListWidgetData::
CDListWidgetData(CDWidgetReferenceList *widget_reference_list) :
 QWidget(nullptr)
{
  widget_reference_list_ = widget_reference_list;

  setWindowTitle("Help for ...");

  QVBoxLayout *layout = new QVBoxLayout(this);

  help_list_ = new QListWidget;

  help_list_->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(help_list_, SIGNAL(itemSelectionChanged()), this, SLOT(listSelectionSlot()));

  layout->addWidget(help_list_);

  /***********/

  /* Add all the widget's helps to the Scrolled List */

  int no_helps = widget_reference_list->getNumReferences();

  for (int i = 0; i < no_helps; ++i) {
    /* Get help for reference */

    int reference = widget_reference_list->getReference(i);

    CDHelp *help = CDocGetHelpForReference(widget_reference_list_->getProgramHelps(), reference);

    /* If Help exists add to the Scrolled List */

    if (help != nullptr)
      addHelp(help);
    else {
      CDHelpSection *help_section =
        CDocGetHelpSectionForReference(widget_reference_list->getProgramHelps(), reference);

      /* If Help Section exists add to the Scrolled List */

      if (help_section != nullptr)
        addHelpSection(help_section);
    }
  }

  /* Add Entry to List All the Helps for a Program */

  help_list_->addItem("(List All Helps)");

  /* Add Help Information To List */

  CDListWidgetHelpData *list_widget_help_data = new CDListWidgetHelpData;

  list_widget_help_data->type = LIST_WIDGET_LIST_ALL;
  list_widget_help_data->data = nullptr;

  help_data_list_.push_back(list_widget_help_data);
}

CDListWidgetData::
~CDListWidgetData()
{
  for_each(help_data_list_.begin(), help_data_list_.end(), CDeletePointer());
}

// Adds help to a the help list widget.
void
CDListWidgetData::
addHelp(CDHelp *help)
{
  /* Create Help Button using the subject as its label */

  if (help->getSubject() == "" || help->getInternal())
    return;

  help_list_->addItem(help->getSubject().c_str());

  /* Add Help Information To List */

  CDListWidgetHelpData *list_widget_help_data = new CDListWidgetHelpData;

  list_widget_help_data->type = LIST_WIDGET_HELP;
  list_widget_help_data->data = reinterpret_cast<char *>(help);

  help_data_list_.push_back(list_widget_help_data);
}

// Adds a help section to the help list widget.
void
CDListWidgetData::
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

  CDListWidgetHelpData *list_widget_help_data = new CDListWidgetHelpData;

  list_widget_help_data->type = LIST_WIDGET_HELP_SECTION;
  list_widget_help_data->data = reinterpret_cast<char *>(help_section);

  help_data_list_.push_back(list_widget_help_data);
}

// Displays a help's dataset in a panel.
void
CDListWidgetData::
listSelectionSlot()
{
  int pos = help_list_->currentRow();

  /* Get Data associated with Selected Item */

  CDListWidgetHelpData *list_widget_help_data = help_data_list_[pos - 1];

  /* Deselect Item */

  //help_list_->deselectAll();

  /* Process Item */

  if (list_widget_help_data != nullptr) {
    if      (list_widget_help_data->type == LIST_WIDGET_HELP)
      CDocWidgetHelpSelect(reinterpret_cast<CDHelp *>(list_widget_help_data->data), true);
    else if (list_widget_help_data->type == LIST_WIDGET_HELP_SECTION)
      CDocWidgetHelpSectionSelect(reinterpret_cast<CDHelpSection *>(list_widget_help_data->data));
    else
      CDocListAllHelpsCreatePanel(widget_reference_list_->getProgramHelps());
  }
}
