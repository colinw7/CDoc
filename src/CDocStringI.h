#ifndef CDOC_STRING_I_H
#define CDOC_STRING_I_H

#include "CDocString.h"

#define PARM_ARRAY (1<<8)

#define PARM_IGNORE        0
#define PARM_STR           1
#define PARM_NEW_STR       2
#define PARM_INT           3
#define PARM_REAL          4
#define PARM_CHOICE        5
#define PARM_CLENSTR       6
#define PARM_LLENSTR       7
#define PARM_BOOLEAN       8
#define PARM_FLAG          9
#define PARM_NFLAG         10
#define PARM_VALUE         11
#define PARM_USER_PROC     12
#define PARM_STR_ARRAY     (PARM_STR     | PARM_ARRAY)
#define PARM_INT_ARRAY     (PARM_INT     | PARM_ARRAY)
#define PARM_REAL_ARRAY    (PARM_REAL    | PARM_ARRAY)
#define PARM_CHOICE_ARRAY  (PARM_CHOICE  | PARM_ARRAY)
#define PARM_CLENSTR_ARRAY (PARM_CLENSTR | PARM_ARRAY)
#define PARM_LLENSTR_ARRAY (PARM_LLENSTR | PARM_ARRAY)
#define PARM_BOOL_ARRAY    (PARM_BOOLEAN | PARM_ARRAY)

struct CDParameterData {
  const char *name;
  int         type;
  void       *data;
  uint        offset;
};

struct CDOptionData {
  const char *long_name;
  const char *short_name;
  int         type;
  void       *data;
  uint        offset;
};

struct CDParameterChoiceData {
  const char *name;
  void       *data;
};

typedef void (*CDocExtractParmProc)
                (const std::vector<std::string> &);

/*-------------------------------------------------------------------------*/

extern void    CDocExtractParameterValues
                (const std::vector<std::string> &, const std::vector<std::string> &,
                 CDParameterData *, char *);
extern void    CDocExtractParameters
                (const std::vector<std::string> &, CDParameterData *, char *);
extern void    CDocExtractOptions
                (char **, int *, CDOptionData *, char *);
extern void    CDocExtractParameterValue
                (const std::string &, CDParameterData *, char *);
extern void    CDocFormatStringInWidth
                (const std::string &, int, int, char ***, int *);
extern void    CDocFreeFormattedStrings
                (char **, int);
extern void    CDocAlignStringInWidth
                (char *, char *, int, int);
extern void    CDocStringToFields
                (char *, char ***, int *, int);
extern void    CDocFreeStringFields
                (char **, int);
extern void    CDocStringToWords
                (const std::string &, std::vector<std::string> &);
extern int     CDocStringDisplayLength
                (const std::string &);
extern char   *CDocStringRemoveEscapeCodes
                (char *);
extern int     CDocLengthStringToChars
                (const std::string &);
extern int     CDocLengthStringToLines
                (const std::string &);
extern double  CDocCharsToEms
                (int);
extern int     CDocIsEscapeCode
                (const char *);
extern int     CDocIsPageReference
                (const char *);
extern int     CDocIsSplitter
                (const char *);
extern int     CDocInEscapeCode
                (const char *, const char *);
extern char   *CDocPrevChar
                (char *, char *);
extern char   *CDocNextChar
                (char *);
extern bool    CDocReadLineFromFile
                (FILE *, std::string &);

#endif
