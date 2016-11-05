#include "CDocI.h"
#include "CDocHelpSectionPanelI.h"

#include <QVBoxLayout>
#include <QListWidget>

CDHelpSectionPanel::
CDHelpSectionPanel(CDHelpPanel *panel) :
 QDialog(panel), panel_(panel), help_dataset_data_(panel->getHelpDatasetData())
{
  setLastSection(NULL);

  /* Create Panel panel shifted 20 pixels right and down from
     the parent widget */

  std::string title = "Sections for " + help_dataset_data_->getHelp()->getSubject();

  setWindowTitle(title.c_str());

  /***********/

  QVBoxLayout *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(list_, SIGNAL(itemSelectionChanged()), this, SLOT(selectionProc()));

  layout->addWidget(list_);
}

CDHelpSectionPanel::
~CDHelpSectionPanel()
{
  uint num = getNumData();

  for (uint i = 0; i < num; ++i) {
    CDHelpDatasetSection *help_dataset_section = getData(i);

    delete help_dataset_section;
  }

  clearData();
}

// Add a Help Section to the Section Panel.
void
CDHelpSectionPanel::
addSection(const std::string &name)
{
  /* Add Section String */

  addItem(name);

  /* Add Section Information To List */

  CDHelpDatasetSection *help_dataset_section = new CDHelpDatasetSection;

  help_dataset_section->name              = name;
  help_dataset_section->page_no           = help_dataset_data_->getPageNum();
  help_dataset_section->sub_section       = false;
  help_dataset_section->help_dataset_data = help_dataset_data_;

  addData(help_dataset_section);

  setLastSection(help_dataset_section);
}

// Add a Help Sub-Section to the Section Panel.
void
CDHelpSectionPanel::
addSubSection(const std::string &name)
{
  /* Add Section String */

  std::string name1 = "  " + name;

  addItem(name1);

  /* Add Section Information To List */

  std::string temp_string;

  if (getLastSection() != NULL)
    temp_string = getLastSection()->name + ":" + name;
  else
    temp_string = ":" + name;

  CDHelpDatasetSection *help_dataset_section = new CDHelpDatasetSection;

  help_dataset_section->name              = temp_string;
  help_dataset_section->page_no           = help_dataset_data_->getPageNum();
  help_dataset_section->sub_section       = true;
  help_dataset_section->help_dataset_data = help_dataset_data_;

  addData(help_dataset_section);
}

void
CDHelpSectionPanel::
addItem(const std::string &str)
{
  list_->addItem(str.c_str());
}

// Move to the Section associated with an Item in the Section Panel's Section List.
void
CDHelpSectionPanel::
selectionProc()
{
  int pos = list_->currentRow();

  /* Get Data associated with Selected Item */

  CDHelpDatasetSection *help_dataset_section = getData(pos - 1);

  /* Deselect Item */

  //list_->deselectAll();

  /* Process Item */

  if (help_dataset_section == NULL)
    return;

  if (help_dataset_section->page_no != help_dataset_data_->getPageNum()) {
    /* Load Required Page Number */

    help_dataset_data_->setPageNum(help_dataset_section->page_no);

    panel_->updatePage();
  }
}

// Get the Section Data Structure given a Section Name.
CDHelpDatasetSection *
CDHelpSectionPanel::
getSectionData(const std::string &section) const
{
  uint num = getNumData();

  for (uint i = 0; i < num; ++i) {
    CDHelpDatasetSection *help_dataset_section = getData(i);

    if (CStrUtil::casecmp(help_dataset_section->name, section) == 0)
      return help_dataset_section;
  }

  return NULL;
}
