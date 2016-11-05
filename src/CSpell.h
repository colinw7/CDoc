#ifndef CSPELL_H
#define CSPELL_H

#include <string>
#include <sys/types.h>

extern bool  CSpellInit       ();
extern void  CSpellTerm       ();
extern void  CSpellCheckString(const std::string &);
extern void  CSpellCheckString(const char *);
extern int   CSpellCheckWord  (const std::string &);
extern int   CSpellCheckWord  (const char *, uint, char **);
extern void  CSpellAddWord    (const std::string &);
extern void  CSpellAddWord    (const char *);

#endif
