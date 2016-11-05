#include "CDocI.h"
#include "CDocHelpPagePanelI.h"
#include "CDocHelpSectionPanelI.h"

#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>

CDHelpPageNoPanel::
CDHelpPageNoPanel(CDHelpPanel *panel) :
 QDialog(panel), panel_(panel), help_dataset_data_(panel->getHelpDatasetData())
{
  slider_  = NULL;
  section_ = NULL;

  //------

  std::string title = "Select Page for " + help_dataset_data_->getHelp()->getSubject();

  setWindowTitle(title.c_str());

  //------

  QVBoxLayout *layout = new QVBoxLayout(this);

  CDHelpSectionPanel *section_panel = panel_->getSectionPanel();

  if (section_panel != NULL && section_panel->getNumData() > 0) {
    section_ = new QLabel;

    layout->addWidget(section_);
  }
  else
    section_ = NULL;

  //------

  QHBoxLayout *pageLayout = new QHBoxLayout;

  QLabel *sliderLabel = new QLabel("Page No.");

  pageLayout->addWidget(sliderLabel);

  slider_ = new QSlider(Qt::Horizontal);

  slider_->setTickPosition(QSlider::TicksBelow);
  slider_->setTickInterval(1);

  slider_->setMinimum(1);
  slider_->setMaximum(help_dataset_data_->getNumPages());

  slider_->setValue(help_dataset_data_->getPageNum());

  connect(slider_, SIGNAL(valueChanged(int)), this, SLOT(sliderSlot()));

  pageLayout->addWidget(slider_);

  layout->addLayout(pageLayout);

  //------

  if (section_panel != NULL)
    section_panel->setLastSection(NULL);

  if (section_)
    setSection();
}

// Called when the Page No. Slider is dragged.
// This displays the new page of section if it has changed.
void
CDHelpPageNoPanel::
sliderSlot()
{
  int value = slider_->value();

  /* Load New Page if Page Number has changed */

  if (value != help_dataset_data_->getPageNum()) {
    /* Load Required Page Number */
    help_dataset_data_->setPageNum(value);

    panel_->updatePage();

    /* Set Section Header if Page Number has changed */

    if (section_)
      setSection();
  }
}

// Update the Page Number Selection Panel so that the specified page is being displayed.
void
CDHelpPageNoPanel::
setPage(int page_no)
{
  slider_->setValue(page_no);

  sliderSlot();
}

// Update the Section Name displayed on the Page Number Selection Panel to that
// of the specified page.
void
CDHelpPageNoPanel::
setSection()
{
  assert(section_);

  /* Update Section Name */

  CDHelpDatasetSection *help_dataset_section =
    getSectionDataForPage(help_dataset_data_->getPageNum());

  std::string section_string;

  if (help_dataset_section != NULL)
    section_string = "Section : " + help_dataset_section->name;
  else
    section_string = "Section : <Unknown>";

  section_->setText(section_string.c_str());

  CDHelpSectionPanel *section_panel = panel_->getSectionPanel();

  if (section_panel != NULL)
    section_panel->setLastSection(help_dataset_section);
}

// Get the Section Data Structure given a Page Number.
CDHelpDatasetSection *
CDHelpPageNoPanel::
getSectionDataForPage(int page_no)
{
  CDHelpSectionPanel *section_panel = panel_->getSectionPanel();

  if (section_panel == NULL)
    return NULL;

  uint num = section_panel->getNumData();

  CDHelpDatasetSection *help_dataset_section = NULL;

  for (uint i = 0; i < num; ++i) {
    CDHelpDatasetSection *help_dataset_section1 = section_panel->getData(i);

    if (! help_dataset_section1->sub_section && help_dataset_section1->page_no <= page_no)
      help_dataset_section = help_dataset_section1;
  }

  return help_dataset_section;
}
