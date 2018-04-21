#include <CSpellLookup.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

/* Internal Data */

static int inited = 0;

/*------------------------------------------------------------------*
 *
 * #NAME CSpellInitWords
 *
 * #FUNCTION
 *       Initialise List of Words in the Dictionary by
 *       reading them in from the Hash File.
 *
 * #CALL_DETAILS
 *       flag = CSpellInitWords();
 *
 * #INPUT_ARGS
 *       None
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       flag  : Flag to indicate whether the Dictionary
 *        int  : was successfully initialised
 *             :  1 - OK, 0 - Failed.
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern int
CSpellInitWords()
{
  int          i;
  CDSpellHEnt *hp;
  CDSpellDEnt *dp;
  int          hashfd;

  if (inited)
    return 1;

  if ((hashfd = open(cspell_hash_file, 0)) < 0) {
    fprintf(stderr, "can't open %s\n", cspell_hash_file);
    return -1;
  }

  cspell_hashsize = read(hashfd, &cspell_hashheader, sizeof(cspell_hashheader));

  if (cspell_hashsize == 0) {
    /*
     * Empty file - create an empty dummy table.  We
     * actually have to have one entry since the hash
     * algorithm involves a divide by the table size
     * (actually modulo, but zero is still unacceptable).
     * So we create an entry with a word of all lowercase,
     * which can't match because the comparison str has
     * been converted to uppercase by then.
     */

    close(hashfd);

    cspell_hashsize = 1;  /* This prevents divides by zero */

    cspell_hashtbl = new CDSpellDEnt [cspell_hashsize];

    if (! cspell_hashtbl) {
      fprintf(stderr, "Couldn't allocate space for hash table\n");
      return -1;
    }

    cspell_hashtbl[0].word = (char *) "xxxxxxxxxxx";
    cspell_hashtbl[0].next = NULL;
    cspell_hashtbl[0].keep = 0;
    cspell_hashtbl[0].used = 1;

    /* The flag bits don't matter, but calloc cleared them. */

    inited = true;

    return(0);
  }
  else if (cspell_hashsize < 0 || cspell_hashheader.magic != CSPELL_MAGIC) {
    fprintf(stderr, "Illegal format hash table\n");
    return(-1);
  }

  assert(sizeof(CDSpellDEnt) == sizeof(CDSpellHEnt));

  cspell_hash_strings = new char [cspell_hashheader.stringsize];

  cspell_hashtbl = new CDSpellDEnt [cspell_hashheader.tblsize];

  CDSpellHEnt *hashtbl = new CDSpellHEnt [cspell_hashheader.tblsize];

  if (! cspell_hashtbl || ! cspell_hash_strings || ! hashtbl) {
    fprintf(stderr, "Couldn't allocate space for hash table\n");
    return(-1);
  }

  cspell_hashsize = cspell_hashheader.tblsize;

  read(hashfd, cspell_hash_strings, cspell_hashheader.stringsize);
  read(hashfd, hashtbl            , cspell_hashheader.tblsize*sizeof(CDSpellDEnt));

  close(hashfd);

  for (i = cspell_hashsize, hp = hashtbl, dp = cspell_hashtbl; --i >= 0; ++hp, ++dp) {
    assert(hp->word >= 0 && hp->word < cspell_hashheader.stringsize);

    dp->word = &cspell_hash_strings[hp->word];

    if (hp->next == -1)
      dp->next = NULL;
    else
      dp->next = &cspell_hashtbl[hp->next];

    dp->used   = hp->used;
    dp->v_flag = hp->v_flag;
    dp->n_flag = hp->n_flag;
    dp->x_flag = hp->x_flag;
    dp->h_flag = hp->h_flag;
    dp->y_flag = hp->y_flag;
    dp->g_flag = hp->g_flag;
    dp->j_flag = hp->j_flag;
    dp->d_flag = hp->d_flag;
    dp->t_flag = hp->t_flag;
    dp->r_flag = hp->r_flag;
    dp->z_flag = hp->z_flag;
    dp->s_flag = hp->s_flag;
    dp->p_flag = hp->p_flag;
    dp->m_flag = hp->m_flag;
    dp->keep   = hp->keep;

#ifdef CSPELL_CAPS
    dp->allcaps    = hp->allcaps;
    dp->capitalize = hp->capitalize;
    dp->followcase = hp->followcase;

    dp->k_allcaps    = hp->k_allcaps;
    dp->k_capitalize = hp->k_capitalize;
    dp->k_followcase = hp->k_followcase;
#endif
  }

  delete [] hashtbl;

  inited = true;

  return 0;
}

/*------------------------------------------------------------------*
 *
 * #NAME CSpellLookupWord
 *
 * #FUNCTION
 *       Lookup Word in Dictionary and Return Dictionary
 *       Entry Structure if found.
 *
 * #CALL_DETAILS
 *       CDSpellDEnt *dent = CSpellLookupWord(char *s, int n, int dotree);
 *
 * #INPUT_ARGS
 *       s      : The Word to be Looked up.
 *
 *       n      : The length of the Word.
 *
 *       dotree : Whether the word should be
 *              : looked for in the list of
 *              : added words or not.
 *              :  1 - Lookup, 0 - Don't Lookup
 *
 * #OUTPUT_ARGS
 *       None
 *
 * #RETURN_ARG
 *       dent   : Dictionary Entry Structure
 *              : (NULL if not found).
 *
 * #NOTES
 *       None
 *
 *------------------------------------------------------------------*/

extern CDSpellDEnt *
CSpellLookupWord(char *s, int n, int dotree)
{
  CDSpellDEnt *dp = &cspell_hashtbl[CSpellHash(s, n, cspell_hashsize)];

  for (; dp != NULL; dp = dp->next) {
    /* quick strcmp, but only for equality */

    char *s1 = dp->word;
    char *s2 = s;

    while (*s1 == *s2++)
      if (*s1++ == '\0') {
        cspell_last_dent = dp;

        return(cspell_last_dent);
      }
  }

  if (dotree) {
    char c = s[n];

    s[n] = '\0';

    if ((dp = CSpellTreeLookup(s)) != NULL)
      cspell_last_dent = dp;

    s[n] = c;

    return(dp);
  }
  else
    return(NULL);
}
