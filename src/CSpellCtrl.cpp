#include <CSpellCtrl.h>
#include <cstring>

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

/*--------------------------------------------------------------------*/

int                cspell_aflag                 = 0;
int                cspell_lflag                 = 0;
char               cspell_rootword[BUFSIZ]      = "";
CDSpellDEnt       *cspell_last_dent             = NULL;
char              *cspell_hash_strings          = NULL;
CDSpellHashHeader  cspell_hashheader;
char               cspell_hash_file[MAXPATHLEN] = "";
CDSpellDEnt       *cspell_hashtbl               = NULL;
int                cspell_hashsize              = 0;

/*--------------------------------------------------------------------*/

static char *personal_dictionary_filename = NULL;

/*------------------------------------------------------------------*
 *
 * #NAME CSpellInit
 *
 * #FUNCTION
 *       Initialise the Spell Checker by Reading the
 *       Words from the Dictionary File.
 *
 * #CALL_DETAILS
 *       flag = CSpellInit();
 *
 * #INPUT_ARGS
 *       None
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       flag  : Flag to indicate whether the Spell
 *        int  : Checker was successfully initialised.
 *             :  1 - OK, 0 - Failed.
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern bool
CSpellInit()
{
  if (getenv("CSPELL_DIR") != NULL)
    sprintf(cspell_hash_file, "%s/%s", getenv("CSPELL_DIR"), DEFHASH);
  else
    sprintf(cspell_hash_file, "%s", DEFHASH);

  int error = CSpellInitWords();

  if (error < 0)
    return false;

  CSpellInitTree(personal_dictionary_filename);

  return true;
}

extern void
CSpellTerm()
{
  delete [] cspell_hash_strings;
  delete [] cspell_hashtbl;
}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellCheckString
 *
 * #FUNCTION
 *       Checks whether the words in the supplied str
 *       are correctly spelled by looking them up in the
 *       Dictionary.
 *
 * #CALL_DETAILS
 *       CSpellCheckString(str);
 *
 * #INPUT_ARGS
 *       str  : The str to be checked.
 *        char * :
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       None
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern void
CSpellCheckString(const std::string &str)
{
  CSpellCheckString(str.c_str());
}

extern void
CSpellCheckString(const char *str)
{
  std::vector<std::string> words;

  CStrUtil::addWords(str, words);

  uint num_words = words.size();

  for (uint i = 0; i < num_words; i++) {
    int check = CSpellCheckWord(words[i]);

    if (check == 0)
      fprintf(stderr, "C Spell Check : Word not Found : %s\n",
              words[i].c_str());
  }
}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellCheckWord
 *
 * #FUNCTION
 *       Checks whether the supplied word is correctly
 *       spelled by looking it up in the Dictionary.
 *
 * #CALL_DETAILS
 *       int flag = CSpellCheckWord(const std::string &word);
 *
 * #INPUT_ARGS
 *       word    : The word to be checked.
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       flag    : Flag to indicate whether the Word
 *               : was found in the Dictionary.
 *               :  0 - OK, 1 - Failed, -1 - Ignored.
 *
 * #NOTES
 *       Removes leading and Trailing Punctuation.
 *
 *       Ignores word under the following circumstances :-
 *
 *        . Word starts or ends with non-alphabetic
 *        . Word contains characters other than A-Z, a-z,
 *          ' (only one allowed) or '-'.
 *
 *------------------------------------------------------------------*/

extern int
CSpellCheckWord(const std::string &word)
{
  return CSpellCheckWord(word.c_str(), word.size(), NULL);
}

extern int
CSpellCheckWord(const char *word, uint length, char **word1)
{
  char *word2 = CStrUtil::strdup(word);

  char *p = word2;
  int   n = length;

  /* Remove Leading ' " ( [ { < Characters */

  while (n > 0 && strchr("\'\"([{<", p[0]) != NULL) {
    p++;
    n--;
  }

  /* Remove Trailing ' " ) ] } > . , ; : ! ? Characters */

  while (n > 0 && strchr("\'\")]}>.,;:!?", p[n - 1]) != NULL)
    p[--n] = '\0';

  /* Ignore Word starting or ending with non-alphabetic */

  if (! isalpha(p[0]) || ! isalpha(p[n - 1])) {
    delete [] word2;
    return -1;
  }

  /* Ignore Word containing invalid characters */

  int no_quotes = 0;

  for (int i = 0; i < n; i++) {
    if (p[i] == '\'') {
      if (no_quotes != 0) {
        delete [] word2;
        return -1;
      }

      no_quotes = 1;
    }
    else if (p[i] == '-') {
      if (i == 0 || i == n - 1 || ! isalpha(p[i - 1]) || ! isalpha(p[i + 1])) {
        delete [] word2;
        return -1;
      }
    }
    else if (! isalpha(p[i])) {
      delete [] word2;
      return -1;
    }
  }

  /* Check Word's Spelling */

  if (CSpellIsGoodWord(p)) {
    delete [] word2;
    return 1;
  }

  /* Add Word To Dictionary */

  CSpellAddWord(p);

  /* Set word to the actual character string failed with */

  char *p1 = CStrUtil::strdup(p);

  strcpy(p, p1);

  delete [] p1;

  if (word1 != NULL)
    *word1 = p;
  else
    delete [] word2;

  return 0;
}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellAddWord
 *
 * #FUNCTION
 *       Adds the supplied word to the Dictionary.
 *
 * #CALL_DETAILS
 *       CSpellAddWord(const char *word);
 *
 * #INPUT_ARGS
 *       word : The word to be added.
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       None
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern void
CSpellAddWord(const std::string &word)
{
  CSpellAddWord(word.c_str());
}

extern void
CSpellAddWord(const char *word)
{
  CSpellInsertInTree(word, 0);
}
