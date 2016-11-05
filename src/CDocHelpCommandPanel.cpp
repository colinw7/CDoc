#include "CDocI.h"
#include "CDocHelpCommandPanelI.h"

#include <QVBoxLayout>
#include <QListWidget>

// Create Panel in which a helps associated commands are listed.
CDHelpCommandPanel::
CDHelpCommandPanel(CDHelpPanel *panel) :
 QDialog(panel), panel_(panel), help_dataset_data_(panel->getHelpDatasetData())
{
  std::string title = "View References for " + help_dataset_data_->getHelp()->getSubject();

  setWindowTitle(title.c_str());

  /***********/

  /* Create List Widget for Commands */

  QVBoxLayout *layout = new QVBoxLayout(this);

  list_ = new QListWidget;

  list_->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(list_, SIGNAL(itemSelectionChanged()), this, SLOT(listSelectionSlot()));

  layout->addWidget(this);
}

CDHelpCommandPanel::
~CDHelpCommandPanel()
{
}

void
CDHelpCommandPanel::
addItem(const std::string &str)
{
  list_->addItem(str.c_str());
}

// Execute the Command associated with an Item in the Command Panel's Command List.
void
CDHelpCommandPanel::
listSelectionSlot()
{
  int pos = list_->currentRow();

  /* Get Data associated with Selected Item */

  CDHelpDatasetCommandData *command_data = getData(pos - 1);

  /* Deselect Item */

  //list_->deselectAll();

  /* Process Item */

  if (command_data != NULL)
    command_data->exec();
}
