#ifndef CSPELL_GEN_I_H
#define CSPELL_GEN_I_H

#include <CSpellI.h>

/*--------------------------------------------------------------------*/

/* Hash Table Magic Number */

#define CSPELL_MAGIC 2

/* word length allowed in dictionary by buildhash */

#define CSPELL_WORDLEN 30

/*
** Define this if you want the capitalization feature.  This will increase
** the size of the hashed dictionary on most 16-bit and some 32-bit machines.
*/

#ifndef CSPELL_CAPS
#  define CSPELL_CAPS
#endif

/*--------------------------------------------------------------------*/

struct CDSpellHEnt {
  int          next;
  int          word;
  unsigned int used   : 1;
  unsigned int v_flag : 1;
  unsigned int n_flag : 1;
  unsigned int x_flag : 1;
  unsigned int h_flag : 1;
  unsigned int y_flag : 1;
  unsigned int g_flag : 1;
  unsigned int j_flag : 1;
  unsigned int d_flag : 1;
  unsigned int t_flag : 1;
  unsigned int r_flag : 1;
  unsigned int z_flag : 1;
  unsigned int s_flag : 1;
  unsigned int p_flag : 1;
  unsigned int m_flag : 1;
  unsigned int keep : 1;

#ifdef CSPELL_CAPS
  unsigned int allcaps    : 1;
  unsigned int capitalize : 1;
  unsigned int followcase : 1;

  unsigned int k_allcaps    : 1;
  unsigned int k_capitalize : 1;
  unsigned int k_followcase : 1;
#endif
};

struct CDSpellDEnt {
  CDSpellDEnt     *next;
  char            *word;
  unsigned int     used : 1;

  /* bit fields for all of the flags */

  /*******************************************************************/

  /*
   * "V" flag:
   *   ...E --> ...IVE  as in CREATE --> CREATIVE
   *   if # .ne. E, ...# --> ...#IVE  as in PREVENT --> PREVENTIVE
   */

  unsigned int     v_flag : 1;

  /*******************************************************************/

  /*
   * "N" flag:
   *   ...E --> ...ION  as in CREATE --> CREATION
   *   ...Y --> ...ICATION  as in MULTIPLY --> MULTIPLICATION
   *   if # .ne. E or Y, ...# --> ...#EN  as in FALL --> FALLEN
   */

  unsigned int     n_flag : 1;

  /*******************************************************************/

  /*
   * "X" flag:
   *   ...E --> ...IONS  as in CREATE --> CREATIONS
   *   ...Y --> ...ICATIONS  as in MULTIPLY --> MULTIPLICATIONS
   *   if # .ne. E or Y, ...# --> ...#ENS  as in WEAK --> WEAKENS
   */

  unsigned int     x_flag : 1;

  /*******************************************************************/

  /*
   * "H" flag:
   *   ...Y --> ...IETH  as in TWENTY --> TWENTIETH
   *   if # .ne. Y, ...# --> ...#TH  as in HUNDRED --> HUNDREDTH
   */

  unsigned int     h_flag : 1;

  /*******************************************************************/

  /*
   * "Y" FLAG:
   *   ... --> ...LY  as in QUICK --> QUICKLY
   */

  unsigned int     y_flag : 1;

  /*******************************************************************/

  /*
   * "G" FLAG:
   *   ...E --> ...ING  as in FILE --> FILING
   *   if # .ne. E, ...# --> ...#ING  as in CROSS --> CROSSING
   */

  unsigned int     g_flag : 1;

  /*******************************************************************/

  /*
   * "J" FLAG"
   *   ...E --> ...INGS  as in FILE --> FILINGS
   *   if # .ne. E, ...# --> ...#INGS  as in CROSS --> CROSSINGS
   */

  unsigned int     j_flag : 1;

  /*******************************************************************/

  /*
   * "D" FLAG:
   *   ...E --> ...ED  as in CREATE --> CREATED
   *   if @ .ne. A, E, I, O, or U,
   *     ...@Y --> ...@IED  as in IMPLY --> IMPLIED
   *   if # .ne. E or Y, or (# = Y and @ = A, E, I, O, or U)
   *     ...@# --> ...@#ED  as in CROSS --> CROSSED
   *                          or CONVEY --> CONVEYED
   */

  unsigned int     d_flag : 1;

  /*******************************************************************/

  /*
   * "T" FLAG:
   *   ...E --> ...EST  as in LATE --> LATEST
   *   if @ .ne. A, E, I, O, or U,
   *     ...@Y --> ...@IEST  as in DIRTY --> DIRTIEST
   *   if # .ne. E or Y, or (# = Y and @ = A, E, I, O, or U)
   *     ...@# --> ...@#EST  as in SMALL --> SMALLEST
   *                             or GRAY --> GRAYEST
   */

  unsigned int     t_flag : 1;

  /*******************************************************************/

  /*
   * "R" FLAG:
   *   ...E --> ...ER  as in SKATE --> SKATER
   *   if @ .ne. A, E, I, O, or U,
   *     ...@Y --> ...@IER  as in MULTIPLY --> MULTIPLIER
   *   if # .ne. E or Y, or (# = Y and @ = A, E, I, O, or U)
   *     ...@# --> ...@#ER  as in BUILD --> BUILDER
   *                          or CONVEY --> CONVEYER
   */

  unsigned int     r_flag : 1;

  /*******************************************************************/

  /*
   * "Z FLAG:
   *   ...E --> ...ERS  as in SKATE --> SKATERS
   *   if @ .ne. A, E, I, O, or U,
   *     ...@Y --> ...@IERS  as in MULTIPLY --> MULTIPLIERS
   *   if # .ne. E or Y, or (# = Y and @ = A, E, I, O, or U)
   *     ...@# --> ...@#ERS  as in BUILD --> BUILDERS
   *                             or SLAY --> SLAYERS
   */

  unsigned int     z_flag : 1;

  /*******************************************************************/

  /*
   * "S" FLAG:
   *   if @ .ne. A, E, I, O, or U,
   *     ...@Y --> ...@IES  as in IMPLY --> IMPLIES
   *   if # .eq. S, X, Z, or H,
   *     ...# --> ...#ES  as in FIX --> FIXES
   *   if # .ne. S,X,Z,H, or Y, or (# = Y and @ = A, E, I, O, or U)
   *     ...# --> ...#S  as in BAT --> BATS
   *                     or CONVEY --> CONVEYS
   */

  unsigned int     s_flag : 1;

  /*******************************************************************/

  /*
   * "P" FLAG:
   *   if @ .ne. A, E, I, O, or U,
   *     ...@Y --> ...@INESS  as in CLOUDY --> CLOUDINESS
   *   if # .ne. Y, or @ = A, E, I, O, or U,
   *     ...@# --> ...@#NESS  as in LATE --> LATENESS
   *                             or GRAY --> GRAYNESS
   */

  unsigned int     p_flag : 1;

  /*******************************************************************/

  /*
   * "M" FLAG:
   *   ... --> ...'S  as in DOG --> DOG'S
   */

  unsigned int     m_flag : 1;

  /*******************************************************************/

  unsigned int     keep : 1;

#ifdef CSPELL_CAPS
  /*
  ** if followcase is set, the actual word entry (dent->word)
  ** is followed by one or more further strings giving exact
  ** capitalizations.   The first byte after the uppercase word
  ** gives the number of capitalizations.  Each capitalization
  ** is preceded by the character "+" if it is to be kept, or
  ** "-" if it is to be discarded from the personal dictionary.
  ** For example, the entry "ITCORP\0\3+ITcorp\0+ITCorp\0+ItCorp\0"
  ** gives various ways of writing my e-mail address.  If all-lowercase
  ** is acceptable, an all-lower entry must appear.  Simple
  ** capitalization, on the other hand, is signified by the "capitalize"
  ** flag.
  **
  ** Suffixes always match the case of the final character of a word.
  **
  ** If "allcaps" is set, the other two flags must be clear.
  */

  unsigned int     allcaps    : 1;  /* Word must be all capitals */
  unsigned int     capitalize : 1;  /* Capitalize the word */
  unsigned int     followcase : 1;  /* Follow capitalization exactly */

  /*
  ** The following entries denote the flag values that are actually
  ** to be kept for this dictionary entry.  They may differ if the
  ** "a" command is used for a word that differs only in capitalization.
  */

  unsigned int     k_allcaps    : 1;
  unsigned int     k_capitalize : 1;
  unsigned int     k_followcase : 1;
#endif
};

struct CDSpellHashHeader {
  int magic;
  int stringsize;
  int tblsize;
};

/*----------------------------------------------------------------------*/

#include <CSpellCtrl.h>
#include <CSpellGood.h>
#include <CSpellHash.h>
#include <CSpellLookup.h>
#include <CSpellTree.h>

/*----------------------------------------------------------------------*/

extern int                 cspell_aflag;
extern int                 cspell_cflag;
extern int                 cspell_lflag;
extern char                cspell_rootword[];
extern CDSpellDEnt        *cspell_last_dent;
extern char               *cspell_hash_strings;
extern CDSpellHashHeader   cspell_hashheader;
extern char                cspell_hash_file[];
extern CDSpellDEnt        *cspell_hashtbl;
extern int                 cspell_hashsize;

/*----------------------------------------------------------------------*/

#endif
