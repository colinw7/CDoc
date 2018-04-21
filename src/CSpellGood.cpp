#include <CSpellGood.h>
#include <cstring>

/* Data */

static int wordok = false;

/* Internal Subroutines */

#ifdef CSPELL_CAPS
static int  CSpellCapOK    (char *, CDSpellDEnt *);
#endif
#if 0
static void CSpellFlagPrint(char *, int, char *);
#endif
static void CSpellGEnding  (char *, int);
static void CSpellDEnding  (char *, int);
static void CSpellTEnding  (char *, int);
static void CSpellREnding  (char *, int);
static void CSpellHEnding  (char *, int);
static void CSpellSEnding  (char *, int);
static void CSpellNEnding  (char *, int);
static void CSpellEEnding  (char *, int);
static void CSpellYEnding  (char *, int);
static int  CSpellIsVowel  (int);

/*------------------------------------------------------------------*
 *
 * CSpellIsGoodWord
 *   Checks whether the supplied word is correctly
 *   spelled by looking it up in the Dictionary.
 *
 *   int flag = CSpellIsGoodWord(char *word);
 *
 *     word : The word to be checked.
 *
 *     flag : Flag to indicate whether the Word
 *          : was found in the Dictionary.
 *          :  1 - OK, 0 - Failed.
 *
 * NOTES
 *   The routine CSpellCheckWord() should be used
 *   in preference to this routine (which is called
 *   by CSpellCheckWord()) so that all access to
 *   the Spell Checked from the rest of C (non
 *   cspell routines) is confined to routines in
 *   the cspell.c file.
 *
 *------------------------------------------------------------------*/

extern int
CSpellIsGoodWord(char *w)
{
  int   n;
  char *p;
  char *q;
  char  nword[100];

  /* Make an uppercase copy of the word we are checking.  */

  for (p = w, q = nword; *p; p++, q++) {
    if (islower(*p))
      *q = toupper(*p);
    else
      *q = *p;
  }

  *q = '\0';

  /* Initialise storage in which the root word (i.e. not
     including the optional prefixes which may have caused
     a match) is stored in. */

  cspell_rootword[0] = '\0';

  /* Check Capitalized Word */

  if (CSpellLookupWord(nword, q - nword, 1) != NULL) {
#ifdef CSPELL_CAPS
    return CSpellCapOK(w, cspell_last_dent);
#else
    return true;
#endif
  }

  /* Try Stripping off Suffixes */

  n = strlen(w);

  if (n == 1)
    return true;

  if (n < 4)
    return false;

  wordok = false;

  /* Check ending dependant on last character */

  switch (q[-1]) {
    case 'D': /* FOR "CREATED", "IMPLIED", "CROSSED" */
      CSpellDEnding(nword, n);
      break;
    case 'T': /* FOR "LATEST", "DIRTIEST", "BOLDEST" */
      CSpellTEnding(nword, n);
      break;
    case 'R': /* FOR "LATER", "DIRTIER", "BOLDER" */
      CSpellREnding(nword, n);
      break;
    case 'G': /* FOR "CREATING", "FIXING" */
      CSpellGEnding(nword, n);
      break;
    case 'H': /* FOR "HUNDREDTH", "TWENTIETH" */
      CSpellHEnding(nword, n);
      break;
    case 'S': /* FOR ALL SORTS OF THINGS ENDING IN "S" */
      CSpellSEnding(nword, n);
      break;
    case 'N': /* "TIGHTEN", "CREATION", "MULTIPLICATION" */
      CSpellNEnding(nword, n);
      break;
    case 'E': /* FOR "CREATIVE", "PREVENTIVE" */
      CSpellEEnding(nword, n);
      break;
    case 'Y': /* FOR "QUICKLY" */
      CSpellYEnding(nword, n);
      break;
    default:
      break;
  }

  if (wordok) {
    strcpy(cspell_rootword, cspell_last_dent->word);

#ifdef CSPELL_CAPS
    return CSpellCapOK(w, cspell_last_dent);
#else
    return true;
#endif
  }

  return false;
}

/*------------------------------------------------------------------*
 *
 * CSpellCapOK
 *  Checks whether a capitaized version of a word is
 *  correct by checking the works allowed capitalizations.
 *
 *   int flag = CSpellCapOK(char *word, CDSpellDEnt *dent);
 *
 *     word : The word to be checked.
 *
 *     dent : The Dictionary Entry found
 *          : for the word (non-case sensitive)
 *
 *     flag : Flag to indicate whether the
 *          : capitalization is valid.
 *          :  1 - OK, 0 - Failed.
 *
 * NOTES
 *   All capitals is always legal.
 *
 *------------------------------------------------------------------*/

#ifdef CSPELL_CAPS
static int
CSpellCapOK(char *word, CDSpellDEnt *dent)
{
  char *w;
  char *dword;
  int   wcount;

  /* All caps is always legal. */

  for (dword = word; *dword; dword++) {
    if (islower(*dword))
      break;
  }

  /* It was all caps */

  if (*dword == '\0')
    return true;

  /* Not all caps and required to be */

  if (dent->allcaps)
    return false;

  if (dent->followcase) {
    /*
    ** It's a followcase word.  The correct capitalizations are
    ** found following the main dent word.  When we find a
    ** mismatch between letters, we assume we are in the suffix,
    ** and begin insisting on the same case as the last letter
    ** that matched.
    */

    dword = dent->word + strlen (dent->word) + 1;

    wcount = *dword++ & 0xFF;

    while (--wcount >= 0) {
      /* Skip over keep flag */

      dword++;

      for (w = word; *w; w++, dword++) {
        if (*dword != *w) {
          /* Begin suffix processing. */

          if (isupper (dword[-1])) {
            while (*w && !islower(*w))
              w++;

            if (*w == '\0')
              return true;
          }
          else {
            while (*w && !isupper (*w))
              w++;

            if (*w == '\0')
              return true;
          }

          break;
        }
      }

      if (*w == '\0')
        return true;

      /* Skip to next prototype */

      while (*dword++)
        ;
    }
  }

  /*
  ** If it's a capitalize word, and the first letter is lowercase,
  ** it's illegal.  Note that all-lowercase followcase words will
  ** be found by the str scan above.
  */

  if (dent->capitalize && islower(*word))
    return false;

  /*
  ** If it's not a followcase word, or if the capitalize flag is set,
  ** capitalization (e.g. at the beginning of a sentence) is always
  ** legal.  All-lowercase is also legal for non-followcase words.
  */

  if (! dent->followcase || dent->capitalize) {
    for (dword = word + 1; *dword; dword++) {
      if (isupper(*dword))
        break;
    }

    /* It was all-lower or capitalized */

    if (*dword == '\0')
      return true;
  }

  /* Word has a bad mix of cases */

  return false;
}
#endif

/*------------------------------------------------------------------*
 *
 * CSpellGEnding
 *   Checks whether the supplied word has a "ING" ending.
 *
 *   CSpellGEnding(char *word, int n);
 *
 *     word : The word to be checked.
 *     n    : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellGEnding(char *w, int n)
{
  CDSpellDEnt *dent;

  /* if the word ends in 'ing', then *p == 'i' */

  char *p = w + n - 3;

  if (strcmp (p, "ING") != 0)
    return;

  /* change I to E, like in CREATING */

  p[0] = 'E';
  p[1] = '\0';

  n -= 2;

  if (n < 2)
    return;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->g_flag) {
    wordok = true;

    return;
  }

  p[0] = '\0';

  n--;

  if (n < 2)
    return;

  /* this stops CREATEING */

  if (p[-1] == 'E')
    return;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL) {
    if (dent->g_flag)
      wordok = true;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * CSpellDEnding
 *   Checks whether the supplied word has a "ED" ending.
 *
 *   CSpellDEnding(chat *word, int n);
 *
 *     word    : The word to be checked.
 *     n       : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellDEnding(char *w, int n)
{
  char        *p;
  CDSpellDEnt *dent;

  p = w + n - 2;

  if (strcmp(p, "ED") != 0)
    return;

  /* kill 'D' */

  p[1] = '\0';

  n--;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL) {
    /* e.g. CREATED */

    if (dent->d_flag) {
      wordok = true;
      return;
    }
  }

  if (n < 3)
    return;

  p[0] = '\0';

  n--;
  p--;

  /* ED is now completely gone */

  if (p[0] == 'I' && ! CSpellIsVowel(p[-1])) {
    p[0] = 'Y';

    if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->d_flag) {
      wordok = true;
      return;
    }

    p[0] = 'I';
  }

  if ((p[0] != 'E' && p[0] != 'Y') ||
      (p[0] == 'Y' && CSpellIsVowel(p[-1]))) {
    if ((dent = CSpellLookupWord(w, n, 1)) != NULL) {
      if (dent->d_flag)
        wordok = true;

      return;
    }
  }
}

/*------------------------------------------------------------------*
 *
 * CSpellTEnding
 *   Checks whether the supplied word has a "EST" ending.
 *
 *   CSpellTEnding(char *word, int n);
 *
 *    word : The word to be checked.
 *    n    : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellTEnding(char *w, int n)
{
  char        *p;
  CDSpellDEnt *dent;

  p = w + n - 3;

  if (strcmp(p, "EST") != 0)
    return;

  /* kill "ST" */

  p[1] = '\0';
  n -= 2;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->t_flag) {
    wordok = true;
    return;
  }

  if (n < 3)
    return;

  /* kill 'E' */

  p[0] = '\0';
  n--;
  p--;

  /* EST is now completely gone */

  if (p[0] == 'I' && ! CSpellIsVowel(p[-1])) {
    p[0] = 'Y';

    if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->t_flag) {
      wordok = true;
      return;
    }

    p[0] = 'I';
  }

  if ((p[0] != 'E' && p[0] != 'Y') ||
      (p[0] == 'Y' && CSpellIsVowel(p[-1]))) {
    if ((dent = CSpellLookupWord(w, n, 1)) != NULL) {
      if (dent->t_flag)
        wordok = true;

      return;
    }
  }
}

/*------------------------------------------------------------------*
 *
 * CSpellREnding
 *   Checks whether the supplied word has a "ER" ending.
 *
 *   CSpellREnding(char *word, int n);
 *
 *     word    : The word to be checked.
 *     n       : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellREnding(char *w, int n)
{
  CDSpellDEnt *dent;

  char *p = w + n - 2;

  if (strcmp(p, "ER") != 0)
    return;

  /* kill 'R' */

  p[1] = '\0';
  n--;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->r_flag) {
    wordok = true;
    return;
  }

  if (n < 3)
    return;

  /* kill 'E' */

  p[0] = '\0';
  n--;
  p--;

  /* ER is now completely gone */

  if (p[0] == 'I' && ! CSpellIsVowel(p[-1])) {
    p[0] = 'Y';

    if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->r_flag) {
      wordok = true;
      return;
    }

    p[0] = 'I';
  }

  if ((p[0] != 'E' && p[0] != 'Y') ||
      (p[0] == 'Y' && CSpellIsVowel(p[-1]))) {
    if ((dent = CSpellLookupWord(w, n, 1)) != NULL) {
      if (dent->r_flag)
        wordok = true;

      return;
    }
  }
}

/*------------------------------------------------------------------*
 *
 * CSpellHEnding
 *   Checks whether the supplied word has a "TH" ending.
 *
 *   CSpellHEnding(char *word, int n);
 *
 *     word : The word to be checked.
 *     n    : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellHEnding(char *w, int n)
{
  CDSpellDEnt *dent;

  char *p = w + n - 2;

  if (strcmp(p, "TH") != 0)
    return;

  /* kill "TH" */

  *p = '\0';

  n -= 2;
  p -= 2;

  if (p[1] != 'Y') {
    if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->h_flag)
      wordok = true;
  }

  if (strcmp(p, "IE") != 0)
    return;

  /* change "IE" to "Y" */

  p[0] = 'Y';
  p[1] = '\0';

  n--;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL)
    if (dent->h_flag)
      wordok = true;
}

/*------------------------------------------------------------------*
 *
 * CSpellSEnding
 *   Checks whether the supplied word has a "IONS", "ICATIONS",
 *   "ENS", "INGS", "ERS", "IERS", "IES", "ES", "S", "INESS",
 *   "NESS" or "'S" ending.
 *
 *   CSpellSEnding(char *word, int n);
 *
 *     word : The word to be checked.
 *     n    : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellSEnding(char *w, int n)
{
  CDSpellDEnt *dent;

  char *p = w + n;

  /* kill 'S' */

  p[-1] = '\0';
  n--;

  if (strchr("SXZHY", p[-2]) == NULL ||
      (p[-2] == 'Y' && CSpellIsVowel(p[-3]))) {
    if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->s_flag) {
      wordok = true;
      return;
    }
  }

  switch (p[-2]) {
    /* letter before S */

    case 'N': /* X */
      if (strcmp(p - 4, "ION") == 0) {
        /* change "ION" to "E" */

        p[-4] = 'E';
        p[-3] = '\0';

        n -= 2;

        if ((dent = CSpellLookupWord (w, n, 1)) != NULL &&
            dent->x_flag) {
          wordok = true;
          return;
        }
      }

      if (strcmp (p - 8, "ICATE") == 0) {
        /* change "ICATE" to "Y" */

        p[-8] = 'Y';
        p[-7] = '\0';

        n -= 4;

        if ((dent = CSpellLookupWord(w, n, 1)) != NULL &&
            dent->x_flag)
          wordok = true;

        return;
      }

      if (strcmp (p - 3, "EN") == 0 && p[-4] != 'E' && p[-4] != 'Y') {
        /* kill "EN" */

        p[-3] = '\0';

        n -= 2;

        if ((dent = CSpellLookupWord(w, n, 1)) != NULL &&
            dent->x_flag)
          wordok = true;

        return;
      }

      return;
    case 'G': /* J */
      if (strcmp(p - 4, "ING") != 0)
        return;

      /* change "ING" to "E" */

      p[-4] = 'E';
      p[-3] = '\0';

      n -= 2;

      if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->j_flag) {
        wordok = true;
        return;
      }

      /* This stops CREATEING */

      if (p[-5] == 'E')
        return;

      /* kill 'E' */

      p[-4] = '\0';

      n--;

      if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->j_flag)
        wordok = true;

      return;
    case 'R': /* Z */
      if (strcmp(p - 3, "ER") != 0)
        return;

      /* kill 'R' */

      p[-2] = '\0';

      n--;

      if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->z_flag) {
        wordok = true;
        return;
      }

      if (p[-4] == 'I' && ! CSpellIsVowel(p[-5])) {
        /* change "IE" to "Y" */

        p[-4] = 'Y';
        p[-3] = '\0';

        n--;

        if ((dent = CSpellLookupWord(w, n, 1)) != NULL &&
            dent->z_flag) {
          wordok = true;
          return;
        }

        /* change 'Y' to 'I' */

        p[-4] = 'I';
      }

      if ((p[-4] != 'E' && p[-4] != 'Y') ||
          (p[-4] == 'Y' && CSpellIsVowel(p[-5]))) {
        if (p[-3])
          n--;

        p[-3] = '\0';

        if ((dent = CSpellLookupWord(w, n, 1)) != NULL &&
            dent->z_flag)
          wordok = true;
      }

      return;
    case 'E': /* S (except simple adding of an S) */
      /* drop the E */

      p[-2] = '\0';

      n--;

      if (strchr("SXZH", p[-3]) != NULL) {
        if ((dent = CSpellLookupWord(w, n, 1)) != NULL) {
          if (dent->s_flag)
            wordok = true;

          return;
        }
      }

      if (p[-3] == 'I' && ! CSpellIsVowel(p[-4])) {
        p[-3] = 'Y';

        if ((dent = CSpellLookupWord(w, n, 1)) != NULL &&
            dent->s_flag)
          wordok = true;

        return;
      }

      return;
    case 'S': /* P */
      if (strcmp(p - 4, "NES") != 0)
        return;

      /* kill "NES" */

      p[-4] = '\0';

      n -= 3;

      if (p[-5] != 'Y' || CSpellIsVowel(p[-6])) {
        if ((dent = CSpellLookupWord(w, n, 1)) != NULL &&
            dent->p_flag) {
          wordok = true;
          return;
        }
      }

      if (p[-5] == 'I') {
        p[-5] = 'Y';

        if ((dent = CSpellLookupWord(w, n, 1)) != NULL &&
            dent->p_flag)
          wordok = true;
      }

      return;
    case '\'': /* M */
      /* kill "'" */

      p[-2] = '\0';

      n--;

      if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->m_flag)
        wordok = true;

      return;
  }
}

/*------------------------------------------------------------------*
 *
 * CSpellNEnding
 *   Checks whether the supplied word has a "EN" or "ION"
 *   ending.
 *
 *     CSpellNEnding(char *word, int n);
 *
 *     word : The word to be checked.
 *     n    : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellNEnding(char *w, int n)
{
  CDSpellDEnt  *dent;

  char *p = w + n;

  if (p[-2] == 'E') {
    if (p[-3] == 'E' || p[-3] == 'Y')
      return;

    /* kill "EN" */

    p[-2] = '\0';

    n -= 2;

    if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->n_flag)
      wordok = true;

    return;
  }

  if (strcmp(p - 3, "ION") != 0)
    return;

  /* change "ION" to "E" */

  p[-3] = 'E';
  p[-2] = '\0';

  n -= 2;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL) {
    if (dent->n_flag)
      wordok = true;

    return;
  }

  /* check is really against "ICATION" */

  if (strcmp(p - 7, "ICATE") != 0)
    return;

  /* change "ICATE" to "Y" */

  p[-7] = 'Y';
  p[-6] = '\0';

  n -= 4;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->n_flag)
    wordok = true;
}

/*------------------------------------------------------------------*
 *
 * CSpellEEnding
 *   Checks whether the supplied word has a "IVE" ending.
 *
 *     CSpellEEnding(char *word, int n);
 *
 *       word : The word to be checked.
 *       n    : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellEEnding(char *w, int n)
{
  CDSpellDEnt *dent;

  char *p = w + n;

  if (strcmp(p - 3, "IVE") != 0)
    return;

  /* change "IVE" to "E" */

  p[-3] = 'E';
  p[-2] = '\0';

  n -= 2;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->v_flag) {
    wordok = true;
    return;
  }

  if (p[-4] == 'E')
    return;

  /* kill 'E' */

  p[-3] = '\0';
  n--;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->v_flag)
    wordok = true;
}

/*------------------------------------------------------------------*
 *
 * CSpellYEnding
 *   Checks whether the supplied word has a "LY" ending.
 *
 *   CSpellYEnding(char *word, int n);
 *
 *     word : The word to be checked.
 *     n    : The length of the word to be checked.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static void
CSpellYEnding(char *w, int n)
{
  CDSpellDEnt *dent;

  char *p = w + n;

  if (strcmp(p - 2, "LY") != 0)
    return;

  /* kill "LY" */

  p[-2] = '\0';

  n -= 2;

  if ((dent = CSpellLookupWord(w, n, 1)) != NULL && dent->y_flag)
    wordok = true;
}

/*------------------------------------------------------------------*
 *
 * CSpellIsVowel
 *   Checks whether the supplied character is a vowel.
 *
 *     int flag = CSpellIsVowel(int cc);
 *
 *      c     : The word to be checked.
 *      flag  : Whether the character was a vowel.
 *            :   1 - Vowel, 0 - Not Vowel.
 *
 * NOTES
 *   Global variable 'wordok' is updated if ending found.
 *
 *------------------------------------------------------------------*/

static int
CSpellIsVowel(int c)
{
  return(c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}
