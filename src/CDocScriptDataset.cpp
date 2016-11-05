#include "CDocI.h"

/*------------------------------------------------------------------------*/

struct CDDataset {
  std::string id;
  std::string file;
};

/*------------------------------------------------------------------------*/

typedef std::vector<CDDataset *> CDDatasetList;

static CDDatasetList  cdoc_dataset_list;

// Define the association between a dataset id and a UNIX filename.
extern void
CDocScriptDefineDataset(const std::string &id, const std::string &file)
{
  CDDataset *dataset = new CDDataset;

  dataset->id   = id;
  dataset->file = file;

  cdoc_dataset_list.push_back(dataset);
}

// Open the UNIX filename associated with the specified dataset id and
// return the File Pointer.
extern FILE *
CDocScriptImbedDataset(const std::string &id)
{
  std::string file = id;

  int no = cdoc_dataset_list.size();

  for (int i = 1; i <= no; i++) {
    CDDataset *dataset = cdoc_dataset_list[i - 1];

    if (CStrUtil::casecmp(dataset->id, id) == 0) {
      file = dataset->file;
      break;
    }
  }

  FILE *fp = fopen(file.c_str(), "r");

  return fp;
}
