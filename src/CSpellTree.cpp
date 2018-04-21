#include <CSpellTree.h>
#include <cstring>
#include <unistd.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

/* Defines */

/* environment variable for user's word list */

#ifndef PDICTVAR
# define PDICTVAR "CUSER_WORDLIST"
#endif

/* default word list */

#ifndef DEFPDICT
# define DEFPDICT ".cuser_wordlist"
#endif

/* Approximate number of words in the full dictionary, after munching.
** Err on the high side unless you are very short on memory, in which
** case you might want to change the tables in tree.c and also increase
** MAXPCT.
**
** (Note:  dict.191 is a bit over 15000 words.  dict.191 munched with
** /usr/dict/words is a little over 28000).
*/

#ifndef BIG_DICT
# define BIG_DICT 29000
#endif

/*
** Maximum hash table fullness percentage.  Larger numbers trade space
** for time.
**/

#ifndef MAXPCT
# define MAXPCT 70  /* Expand table when 70% full */
#endif

/*
** Define this if you want your personal dictionary sorted.  This may take
** a long time for very large dictionaries.  Dictionaries larger than
** SORTPERSONAL words will not be sorted.
*/

/* Commented out because 'I' doesn't work the second time if you have
   this - clobbers dictionary leaving only latest word there (why?)
#define SORTPERSONAL 1000
*/
#ifndef SORTPERSONAL
# undef SORTPERSONAL
#endif

/* Internal Data */

static int          cantexpand = 0;    /* NZ if an expansion fails */
static CDSpellDEnt *htab       = NULL; /* Hash table for our stuff */
static int          hsize      = 0;    /* Space available in hash table */
static int          hcount     = 0;    /* Number of items in hash table */

static char         personaldict[MAXPATHLEN] = "";
static FILE        *dictf                    = NULL;
static int          newwords                 = 0;

#if 0
static int          hasslash                 = false;
#endif

/*
 * Hash table sizes.  Prime is probably a good idea, though in truth I
 * whipped the algorithm up on the spot rather than looking it up, so
 * who knows what's really best?  If we overflow the table, we just
 * use a double-and-add-1 algorithm.
 *
 * The strange pattern in the table is because this table is sometimes
 * used with huge dictionaries, and we want to get the table bigger fast.
 * 23003 just happens to be such that the original dict.191 will fill
 * the table to just under 70%.  31469 is similarly selected for dict.191
 * combined with /usr/dict/words.  The other numbers are on 10000-word
 * intervals starting at 30000.  (The table is still valid if MAXPCT
 * is changed, but the dictionary sizes will no longer fall on neat
 * boundaries).
 */

static int
goodsizes[] = {
  53, 223, 907,

#if ((BIG_DICT * 100) / MAXPCT) <= 23003
  23003, /* ~16000 words */
#endif

#if ((BIG_DICT * 100) / MAXPCT) <= 31469
  31469, /* ~22000 words */
#endif

#if ((BIG_DICT * 100) / MAXPCT) <= 42859
  42859, /* ~30000 words */
#endif

#if ((BIG_DICT * 100) / MAXPCT) <= 57143
  57143, /* ~40000 words */
#endif

  71429  /* ~50000 words */
};

/* Routines */

static CDSpellDEnt *CSpellTreeInsert
                     (char *, CDSpellDEnt *, int);
static char        *CSpellToUpper
                     (char *);
#ifdef CSPELL_CAPS
static char        *CSpellToLower
                     (char *);
#endif
#ifdef NEVER
#if SORTPERSONAL != 0
static int          CScriptPDictCompare
                     (CDSpellDEnt **, CDSpellDEnt **);
#endif
static void         CSpellOutputTreeEntry
                     (CDSpellDEnt *);
static void         CSpellOutputTreeWord
                     (char *, CDSpellDEnt *);
static void         CSpellOutputFlag
                     (int);
#endif

/*------------------------------------------------------------------*
 *
 * #NAME CSpellInitTree
 *
 * #FUNCTION
 *       Initialise List of Words in the User's Personal
 *       Dictionary by reading them in from a File.
 *
 * #CALL_DETAILS
 *       CSpellInitTree(p);
 *
 * #INPUT_ARGS
 *       p       : Name of File to Read (NULL implies
 *        char * : look in default places).
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
CSpellInitTree(char *p)
{
  char        *h;
  CDSpellDEnt *dp;
  char        *orig;
  char         buf[BUFSIZ];

  /*
  ** if p exists and begins with '/' we don't really need HOME,
  ** but it's not very likely that HOME isn't set anyway.
  */

  orig = p;

  if (p == NULL)
    p = getenv(PDICTVAR);

  if ((h = getenv("HOME")) == NULL)
    h = (char *) ".";

  if (p == NULL)
    sprintf(personaldict, "%s/%s", h, DEFPDICT);
  else {
    if (*p == '/')
      strcpy(personaldict, p);
    else {
      /*
      ** The user gave us a relative pathname.  How we
      ** interpret it depends on how it was given:
      **
      ** -p switch:  as-is first, then $HOME/name
      ** PDICTVAR:   $HOME/name first, then as-is
      **/

      if (orig == NULL)
        sprintf(personaldict, "%s/%s", h, p);
      else /* -p switch */
        strcpy(personaldict, p);
    }
  }

  if ((dictf = fopen(personaldict, "r")) == NULL) {
    /* The file doesn't exist. */

    if (p != NULL) {
      /* If pathname is relative, try another place */

      if (*p != '/') {
        if (orig == NULL)
          strcpy (personaldict, p);
        else /* -p switch */
          sprintf(personaldict, "%s/%s", h, p);

        dictf = fopen (personaldict, "r");
      }

      if (dictf == NULL) {
        fprintf(stderr, "Couldn't open ");
        perror(p);

        if (*p != '/') {
          /*
          ** Restore the preferred default, so
          ** that output will go to the right
          ** place.
          */

          if (orig == NULL)
            sprintf(personaldict, "%s/%s", h, p);
          else /* -p switch */
            strcpy(personaldict, p);
        }
      }
    }

    /* If the name wasn't specified explicitly, we don't object */

    return;
  }

  while (fgets(buf, sizeof buf, dictf) != NULL) {
    int len = strlen(buf) - 1;

    if (buf[len] == '\n')
      buf[len] = '\0';

    if ((h = strchr(buf, '/')) != NULL)
      *h++ = '\0';

    dp = CSpellInsertInTree(buf, 1);

    if (h != NULL) {
      while (*h != '\0' && *h != '\n') {
        if (islower (*h))
          *h = toupper (*h);

        switch (*h++) {
          case 'D':
            dp->d_flag = true;
            break;
          case 'G':
            dp->g_flag = true;
            break;
          case 'H':
            dp->h_flag = true;
            break;
          case 'J':
            dp->j_flag = true;
            break;
          case 'M':
            dp->m_flag = true;
            break;
          case 'N':
            dp->n_flag = true;
            break;
          case 'P':
            dp->p_flag = true;
            break;
          case 'R':
            dp->r_flag = true;
            break;
          case 'S':
            dp->s_flag = true;
            break;
          case 'T':
            dp->t_flag = true;
            break;
          case 'V':
            dp->v_flag = true;
            break;
          case 'X':
            dp->x_flag = true;
            break;
          case 'Y':
            dp->y_flag = true;
            break;
          case 'Z':
            dp->z_flag = true;
            break;
          default:
            fprintf(stderr, "Illegal flag in personal dictionary");
            fprintf(stderr, " - %c (word %s)\n",
                    h[-1], buf);

            break;
        }

        /* Accept old-format dicts with extra slashes */

        if (*h == '/')
          h++;
      }
    }
  }

  fclose(dictf);

  newwords = 0;

  if (! cspell_lflag && ! cspell_aflag &&
      access(personaldict, W_OK) < 0)
    fprintf(stderr,
            "Warning: Cannot update personal dictionary (%s)\n",
            personaldict);
}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellInsertInTree
 *
 * #FUNCTION
 *       Insert Word in List of Personal and added Words.
 *
 * #CALL_DETAILS
 *       CDSpellDEnt *dent =
 *        CSpellInsertInTree(const char *word, int keep);
 *
 * #INPUT_ARGS
 *       word : The Word to be added.
 *
 *       keep : Whether the word should be retained (??).
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       dent : The Dictionary Entry Structure of the Added
 *            : Word (NULL if not added)
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern CDSpellDEnt *
CSpellInsertInTree(const char *word, int keep)
{
  int           i;
  CDSpellDEnt  *dp;
  int           len;
  CDSpellDEnt  *olddp;
  CDSpellDEnt  *oldhtab;
  int           oldhsize;
  char          nword[BUFSIZ];
#ifdef CSPELL_CAPS
  int           capspace;
  char         *saveword;
#endif

  strcpy(nword, word);

  CSpellToUpper(nword);

  len = strlen(nword);

  if ((dp = CSpellLookupWord(nword, len, 0)) != NULL)
    dp->keep = keep;

  /*
   * Expand hash table when it is MAXPCT % full.
   */

  else if (! cantexpand && (hcount * 100) / MAXPCT >= hsize) {
    oldhsize = hsize;
    oldhtab  = htab;

    for (i = 0; i < int(sizeof(goodsizes)/sizeof(goodsizes[0])); i++)
      if (goodsizes[i] > hsize)
        break;

    if (i >= int(sizeof(goodsizes)/sizeof(goodsizes[0])))
      hsize += hsize + 1;
    else
      hsize = goodsizes[i];

    htab = (CDSpellDEnt *) calloc(hsize, sizeof(CDSpellDEnt));

    if (htab == NULL) {
      fprintf(stderr, "Ran out of space for personal dictionary\n");

      /*
       * Try to continue anyway, since our overflow
       * algorithm can handle an overfull (100%+) table,
       * and the malloc very likely failed because we
       * already have such a huge table, so small mallocs
       * for overflow entries will still work.
       */

      if (oldhtab == NULL)
        exit(1);    /* No old table, can't go on */

      fprintf(stderr, "Continuing anyway (with reduced performance).\n");

      cantexpand = 1;        /* Suppress further messages */
      hsize      = oldhsize; /* Put this back how the were */
      htab       = oldhtab;  /* ... */
      newwords   = 1;        /* And pretend it worked */

      return(CSpellTreeInsert(nword, (CDSpellDEnt *) NULL, keep));
    }
    else {
      /*
       * Re-insert old entries into new table
       */

      for (i = 0; i < oldhsize; i++) {
        dp = &oldhtab[i];

        if (oldhtab[i].used) {
          CSpellTreeInsert((char *) NULL, dp, 0);

          dp = dp->next;

          while (dp != NULL) {
            CSpellTreeInsert((char *) NULL, dp, 0);

            olddp = dp;
            dp    = dp->next;

            free((char *) olddp);
          }
        }
      }

      if (oldhtab != NULL)
        free((char *) oldhtab);

      dp = NULL;    /* This will force the insert below */
    }
  }

  newwords |= keep;

  if (dp == NULL)
    dp = CSpellTreeInsert(nword, (CDSpellDEnt *) NULL, keep);

#ifdef CSPELL_CAPS
  if (dp == NULL)
    return NULL;

  /*
  ** Figure out the capitalization rules from the
  ** capitalization of the sample entry.  If the sample is
  ** all caps, we don't change the existing flags, since
  ** all-caps gives us no information.  Tinsert initializes
  ** new entries with "allcaps" set, so if the word is truly
  ** required to appear in capitals, the correct result
  ** will be achieved.
  */

  char *word1 = CStrUtil::strdup(word);

  char *cp = word1;

  for ( ; *cp; cp++) {
    if (islower(*cp))
      break;
  }

  if (*cp) {
    /*
    ** Sample entry has at least some lowercase.  See if
    ** the case is mixed.
    */

    for (cp = word1; *cp; cp++) {
      if (isupper(*cp))
        break;
    }

    if (*cp == '\0' && ! dp->followcase) {
      /*
      ** Sample entry is all lowercase, and word is not
      ** followcase.  Clear all of the capitalization flags.
      */

      dp->allcaps    = 0;
      dp->capitalize = 0;

      if (keep) {
        dp->k_allcaps    = 0;
        dp->k_capitalize = 0;
        dp->k_followcase = 0;
      }
    }
    else {
      /*
      ** The sample entry is mixed case (or all-lower and the
      ** entry is already followcase).  If it's simply
      ** capitalized, set the capitalize flag and that's that.
      */

      for (cp = word1 + 1; *cp && ! isupper(*cp);)
        cp++;

      if (*cp == 0 && isupper(*word1)) {
        dp->allcaps    = 0;
        dp->capitalize = 1;

        if (keep) {
          dp->k_allcaps    = 0;
          dp->k_capitalize = 1;
        }
      }
      else {
        /*
        ** The sample entry is followcase.  Make the
        ** dictionary entry followcase if necessary.
        */

        if (! dp->followcase) {
          dp->followcase = 1;

          if (keep)
            dp->k_followcase = 1;

          capspace = 2 * len + 4;

          if (dp->word >= cspell_hash_strings &&
              dp->word <= cspell_hash_strings +
                          cspell_hashheader.stringsize) {
            cp       = dp->word;
            dp->word = (char *) malloc(capspace);

            if (dp->word)
              strcpy(dp->word, cp);
          }
          else
            dp->word = (char *) realloc(dp->word, capspace);

          if (dp->word == NULL) {
            fprintf(stderr,
                    "Ran out of space for personal dictionary\n");
            exit (1);
          }

          cp = dp->word + len + 1;

          if (dp->capitalize || dp->allcaps)
            *cp++ = 0;
          else {
            *cp++ = 1;

            strcpy(cp + 1, dp->word);

            CSpellToLower(cp + 1);
          }

          *cp = dp->keep ? '+' : '-';
        }

        dp->allcaps = 0;

        if (keep)
          dp->k_allcaps = 0;

        cp = dp->word + len + 1;

        /* See if the capitalization is already there */

        for (i = 0, saveword = cp + 1; i < (*cp & 0xFF); i++) {
          if (strcmp(saveword + 1, word) == 0)
            break;

          saveword += len + 2;
        }

        if (i != (*cp & 0xFF)) {
          if (keep)
            *saveword = '+';
        }
        else {
          /* Add a new capitalization */

          (*cp)++;

          capspace = (cp - dp->word + 1)*((*cp & 0xFF) + 1);

          if (dp->word >= cspell_hash_strings &&
              dp->word <= cspell_hash_strings +
                          cspell_hashheader.stringsize) {
            saveword = dp->word;
            dp->word = (char *) malloc(capspace);

            if (dp->word) {
              cp = dp->word;

              while (--capspace >= 0)
                *cp++ = *saveword++;
            }
          }
          else
            dp->word = (char *) realloc(dp->word, capspace);

          if (dp->word == NULL) {
            fprintf(stderr, "Ran out of space for personal dictionary\n");
            exit (1);
          }

          cp = dp->word + len + 1;

          cp += ((*cp & 0xFF) - 1)*(cp - dp->word + 1) + 1;

          *cp++ = keep ? '+' : '-';

          strcpy(cp, word);
        }
      }
    }
  }

  delete [] word1;

#endif

  return dp;
}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellTreeInsert
 *
 * #FUNCTION
 *       Insert Word or Dictionary Entry in List of Personal
 *       and added Words.
 *
 * #CALL_DETAILS
 *       dent = CSpellTreeInsert(word, proto, keep);
 *
 * #INPUT_ARGS
 *       word           : The Word to be added.
 *        char *        :
 *
 *       proto          : The Dictionary Entry Structure
 *        CDSpellDEnt * : to be added.
 *                      : (if word is NULL)
 *
 *       keep           : Whether the word should be
 *        int           : retained (??).
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       dent           : The Dictionary Entry Structure
 *        CDSpellDEnt * : of the Added Word.
 *                      : (NULL if not added)
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

static CDSpellDEnt *
CSpellTreeInsert(char *word, CDSpellDEnt *proto, int keep)
{
  CDSpellDEnt *hp;   /* Next trial entry in hash table */
  CDSpellDEnt *php;  /* Previous value of hp, for chaining */
  int          hcode;

  if (word == NULL)
    word = proto->word;

  hcode = CSpellHash(word, strlen (word), hsize);

  php = NULL;

  hp = &htab[hcode];

  if (hp->used) {
    while (hp != NULL) {
      if (strcmp (word, hp->word) == 0) {
        if (keep)
          hp->keep = 1;

        return(hp);
      }

      php = hp;
      hp  = hp->next;
    }

    hp = (CDSpellDEnt *) calloc(1, sizeof(CDSpellDEnt));

    if (hp == NULL) {
      fprintf(stderr, "Ran out of space for personal dictionary\n");
      exit(1);
    }
  }

  if (proto != NULL) {
    *hp = *proto;

    if (php != NULL)
      php->next = hp;

    hp->next = NULL;

    return(&htab[hcode]);
  }
  else {
    if (php != NULL)
      php->next = hp;

    hp->word = (char *) malloc(strlen(word) + 1);

    if (hp->word == NULL) {
      fprintf(stderr, "Ran out of space for personal dictionary\n");
      exit(1);
    }

    strcpy(hp->word, word);

    hp->used = 1;
    hp->next = NULL;

    hp->d_flag = 0;
    hp->g_flag = 0;
    hp->h_flag = 0;
    hp->j_flag = 0;
    hp->m_flag = 0;
    hp->n_flag = 0;
    hp->p_flag = 0;
    hp->r_flag = 0;
    hp->s_flag = 0;
    hp->t_flag = 0;
    hp->v_flag = 0;
    hp->x_flag = 0;
    hp->y_flag = 0;
    hp->z_flag = 0;

#ifdef CSPELL_CAPS
    hp->allcaps      = 1;    /* Assume word is all-caps */
    hp->k_allcaps    = 1;
    hp->capitalize   = 0;
    hp->k_capitalize = 0;
    hp->followcase   = 0;
    hp->k_followcase = 0;
#endif

    hp->keep = keep;

    hcount++;

    return hp;
  }
}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellTreeLookup
 *
 * #FUNCTION
 *       Lookup Word in Dictionary of Personal or added Words.
 *
 * #CALL_DETAILS
 *       dent = CSpellTreeLookup(word);
 *
 * #INPUT_ARGS
 *       word              : The Word to be Looked up.
 *        char *           :
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       dent              : The Dictionary Entry Structure
 *        CDSpellDEnt * : of the Word. (NULL if not found)
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern CDSpellDEnt *
CSpellTreeLookup(char *word)
{
  CDSpellDEnt *hp;
  int          hcode;
  char         nword[BUFSIZ];

  if (hsize <= 0)
    return(NULL);

  strcpy(nword, word);

  hcode = CSpellHash(nword, strlen(nword), hsize);

  hp = &htab[hcode];

  while (hp != NULL && hp->used) {
    if (strcmp(nword, hp->word) == 0)
      break;

    hp = hp->next;
  }

  if (hp != NULL && hp->used)
    return(hp);
  else
    return(NULL);

}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellToUpper
 *
 * #FUNCTION
 *       Convert any lower case characters in the supplied
 *       str to upper case.
 *
 * #CALL_DETAILS
 *       CSpellToUpper(str);
 *
 * #INPUT_ARGS
 *       str     : The str whose characters are to
 *        char * : be converted to upper case.
 *
 * #OUTPUT_ARGS
 *       str     : The str with all lower case
 *        char * : characters converted to upper case.
 *
 * #RETURN_ARG
 *       None
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

static char *
CSpellToUpper(char *s)
{
  char *os = s;

  while (*s) {
    if (islower(*s))
      *s = toupper(*s);

    s++;
  }

  return os;
}

#ifdef CSPELL_CAPS
/*------------------------------------------------------------------*
 *
 * #NAME CSpellToLower
 *
 * #FUNCTION
 *       Convert any upper case characters in the supplied
 *       str to lower case.
 *
 * #CALL_DETAILS
 *       CSpellToLower(str);
 *
 * #INPUT_ARGS
 *       str     : The str whose characters are to
 *        char * : be converted to lower case.
 *
 * #OUTPUT_ARGS
 *       str     : The str with all upper case
 *        char * : characters converted to lower case.
 *
 * #RETURN_ARG
 *       None
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

static char *
CSpellToLower(char *s)
{
  char *os = s;

  while (*s) {
    if (isupper(*s))
      *s = tolower(*s);

    s++;
  }

  return os;
}
#endif

/*---------------------------------------------------------------------*
 *
 *  Rest of File can be used to output personal dictionary to file.
 *  It is left in for possible future use.
 *
 *---------------------------------------------------------------------*/

#ifdef NEVER
extern void
CScriptTreeOutput(void)
{
  CDSpellDEnt   *cent;      /* Current entry */
  CDSpellDEnt   *lent;      /* Linked entry */
#if SORTPERSONAL != 0
  int            pdictsize; /* Number of entries to write */
  CDSpellDEnt  **sortptr;   /* Handy pointer into sortlist */
  CDSpellDEnt  **sortlist;  /* List of entries to be sorted */
#endif
  CDSpellDEnt   *ehtab;     /* End of htab, for quick looping */

  if (newwords == 0)
    return;

  if ((dictf = fopen(personaldict, "w")) == NULL) {
    fprintf(stderr, "Can't create %s\n", personaldict);
    return;
  }

#if SORTPERSONAL != 0
  /*
  ** If we are going to sort the personal dictionary, we must know
  ** how many items are going to be sorted.
  */

  if (hcount >= SORTPERSONAL)
    sortlist = NULL;
  else {
    pdictsize = 0;

    for (cent = htab, ehtab = htab + hsize; cent < ehtab; cent++) {
      for (lent = cent; lent != NULL; lent = lent->next) {
        if (lent->used && lent->keep)
          pdictsize++;
      }
    }

    for (cent = cspell_hashtbl, ehtab = cspell_hashtbl + cspell_hashsize; cent < ehtab; cent++) {
      if (cent->keep && cent->used)
        pdictsize++;
    }

    sortlist = new CDSpellDEnt * [pdictsize];
  }

  if (sortlist == NULL) {
#endif
    for (cent = htab, ehtab = htab + hsize; cent < ehtab; cent++) {
      for (lent = cent; lent != NULL; lent = lent->next) {
        if (lent->used && lent->keep)
          CSpellOutputTreeEntry(lent);
      }
    }

    for (cent = cspell_hashtbl, ehtab = cspell_hashtbl + cspell_hashsize; cent < ehtab; cent++) {
      if (cent->used && cent->keep)
        CSpellOutputTreeEntry(cent);
    }

#if SORTPERSONAL != 0
    return;
  }

  /*
  ** Produce dictionary in sorted order.  We used to do this
  ** destructively, but that turns out to fail because in some modes
  ** the dictionary is written more than once.  So we build an
  ** auxiliary pointer table (in sortlist) and sort that.  This
  ** is faster anyway, though it uses more memory.
  */

  sortptr = sortlist;

  for (cent = htab, ehtab = htab + hsize; cent < ehtab; cent++) {
    for (lent = cent; lent != NULL; lent = lent->next) {
      if (lent->used && lent->keep)
        *sortptr++ = lent;
    }
  }

  for (cent = cspell_hashtbl, ehtab = cspell_hashtbl + cspell_hashsize; cent < ehtab; cent++) {
    if (cent->used && cent->keep)
      *sortptr++ = cent;
  }

  /* Sort the list */

  qsort((char *) sortlist, pdictsize, sizeof(sortlist[0]),
        CScriptPDictCompare);

  /* Write it out */

  for (sortptr = sortlist; --pdictsize >= 0;)
    CSpellOutputTreeEntry(*sortptr++);

  delete [] sortlist;
#endif

  newwords = 0;

  fclose(dictf);

}

/* Comparison routine for sorting the personal dictionary with qsort. */

#if SORTPERSONAL != 0
static int
CScriptPDictCompare(CDSpellDEnt **enta, CDSpellDEnt **entb)
{
  return (casecmp((*enta)->word, (*entb)->word));
}
#endif

static void
CSpellOutputTreeEntry(CDSpellDEnt *cent)
{
#ifdef CSPELL_CAPS
  char  *cp;
  int    len;
  int    wcount;
  char   wbuf[CSPELL_WORDLEN + 1];

  strcpy(wbuf, cent->word);

  if (cent->k_followcase) {
    if (cent->k_capitalize) {
      CSpellToLower(wbuf);

      if (islower(wbuf[0]))
        wbuf[0] = toupper(wbuf[0]);

      CSpellOutputTreeWord(wbuf, cent);
    }

    len = strlen(wbuf) + 1;

    cp = cent->word + len;

    wcount = *cp++ & 0xFF;

    while (--wcount >= 0) {
      if (*cp++ == '+')
        CSpellOutputTreeWord(cp, cent);

      cp += len;
    }
  }
  else {
    if (! cent->k_allcaps)
      CSpellToLower(wbuf);

    if (cent->k_capitalize && islower(wbuf[0]))
      wbuf[0] = toupper(wbuf[0]);

    CSpellOutputTreeWord(wbuf, cent);
  }
#else
  CSpellOutputTreeWord(cent->word, cent);
#endif
}

static void
CSpellOutputTreeWord(char *word, CDSpellDEnt *cent)
{
  hasslash = false;

  fprintf(dictf, "%s", word);

  if (cent->d_flag)
    CSpellOutputFlag('D');
  if (cent->g_flag)
    CSpellOutputFlag('G');
  if (cent->h_flag)
    CSpellOutputFlag('H');
  if (cent->j_flag)
    CSpellOutputFlag('J');
  if (cent->m_flag)
    CSpellOutputFlag('M');
  if (cent->n_flag)
    CSpellOutputFlag('N');
  if (cent->p_flag)
    CSpellOutputFlag('P');
  if (cent->r_flag)
    CSpellOutputFlag('R');
  if (cent->s_flag)
    CSpellOutputFlag('S');
  if (cent->t_flag)
    CSpellOutputFlag('T');
  if (cent->v_flag)
    CSpellOutputFlag('V');
  if (cent->x_flag)
    CSpellOutputFlag('X');
  if (cent->y_flag)
    CSpellOutputFlag('Y');
  if (cent->z_flag)
    CSpellOutputFlag('Z');

  fprintf(dictf, "\n");
}

static void
CSpellOutputFlag(int flag)
{
  if (! hasslash)
    putc('/', dictf);

  hasslash = true;

  putc(flag, dictf);
}
#endif
