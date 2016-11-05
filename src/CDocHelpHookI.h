#ifndef CDOC_HELP_HOOK_I_H
#define CDOC_HELP_HOOK_I_H

/*****************************************************************************/

/* Includes */

#include "CDocHelpHook.h"

class CDHookData {
 private:
  std::string id_;
  std::string data_;
  std::string text_;

 public:
  CDHookData(const std::string &id, const std::string &data, const std::string &text) :
   id_(id), data_(data), text_(text) {
  }

  const std::string &getId  () const { return id_  ; }
  const std::string &getData() const { return data_; }
  const std::string &getText() const { return text_; }
};

/* External Routines */

extern std::string CDocScriptGetHookText
                    (CDHookData *);
extern void        CDocHelpProcessHook
                    (const std::string &, const std::string &);

/*****************************************************************************/

#endif
