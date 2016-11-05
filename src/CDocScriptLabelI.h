#ifndef CDOC_SCRIPT_LABEL_I_H
#define CDOC_SCRIPT_LABEL_I_H

/*---------------------------------------------------------------------*/

struct CDLabel {
  std::string name;
  long        position;
};

/*---------------------------------------------------------------------*/

extern void  CDocScriptAddLabel (const std::string &);
extern void  CDocScriptGotoLabel(const std::string &);

#endif
