#include <CDocI.h>
#include <COSTime.h>

/*---------------------------------------------------------------------------*/

#define MAX_LISTS 50

/*---------------------------------------------------------------------------*/

/* Define Header Parameter Definition */

#define DH_OFFSET(a) CDocOffset(CDHeaderControl*,a)

struct CDHeaderControl {
  int    new_page;
  int    section_breaks;
  int    alignment;
  int    space_before;
  int    skip_before;
  int    space_after;
  int    underscored;
  int    capitalised;
  int    break_header;
  int    toc_entry;
  int    toc_only;
  int    skip_before_toc_entry;
  int    toc_indentation;
  int    number_header;
  char  *font;
  char  *toc_font;
};

static CDParameterData
dh_parameter_data[] = {
  {"br"     , PARM_FLAG   , NULL, DH_OFFSET(break_header         ),},
  {"nbr"    , PARM_NFLAG  , NULL, DH_OFFSET(break_header         ),},
  {"pa"     , PARM_FLAG   , NULL, DH_OFFSET(new_page             ),},
  {"npa"    , PARM_NFLAG  , NULL, DH_OFFSET(new_page             ),},
  {"tc"     , PARM_FLAG   , NULL, DH_OFFSET(toc_entry            ),},
  {"ntc"    , PARM_NFLAG  , NULL, DH_OFFSET(toc_entry            ),},
  {"to"     , PARM_FLAG   , NULL, DH_OFFSET(toc_only             ),},
  {"nto"    , PARM_NFLAG  , NULL, DH_OFFSET(toc_only             ),},
  {"ts"     , PARM_FLAG   , NULL, DH_OFFSET(skip_before_toc_entry),},
  {"nts"    , PARM_NFLAG  , NULL, DH_OFFSET(skip_before_toc_entry),},
  {"up"     , PARM_FLAG   , NULL, DH_OFFSET(capitalised          ),},
  {"nup"    , PARM_NFLAG  , NULL, DH_OFFSET(capitalised          ),},
  {"us"     , PARM_FLAG   , NULL, DH_OFFSET(underscored          ),},
  {"nus"    , PARM_NFLAG  , NULL, DH_OFFSET(underscored          ),},
  {"num"    , PARM_FLAG   , NULL, DH_OFFSET(number_header        ),},
  {"nonum"  , PARM_NFLAG  , NULL, DH_OFFSET(number_header        ),},
  {"sect"   , PARM_FLAG   , NULL, DH_OFFSET(section_breaks       ),},
  {"nosect" , PARM_NFLAG  , NULL, DH_OFFSET(section_breaks       ),},
  {"dot"    , PARM_IGNORE , NULL, 0                               ,},
  {"ndot"   , PARM_IGNORE , NULL, 0                               ,},
  {"autohy" , PARM_IGNORE , NULL, 0                               ,},
  {"nohy"   , PARM_IGNORE , NULL, 0                               ,},
  {"nohang" , PARM_IGNORE , NULL, 0                               ,},
  {"hang"   , PARM_IGNORE , NULL, 0                               ,},
  {"font"   , PARM_NEW_STR, NULL, DH_OFFSET(font                 ),},
  {"spbf"   , PARM_INT    , NULL, DH_OFFSET(space_before         ),},
  {"skbf"   , PARM_INT    , NULL, DH_OFFSET(skip_before          ),},
  {"spaf"   , PARM_INT    , NULL, DH_OFFSET(space_after          ),},
  {"tcin"   , PARM_INT    , NULL, DH_OFFSET(toc_indentation      ),},
  {"tfont"  , PARM_NEW_STR, NULL, DH_OFFSET(toc_font             ),},

  {"center" , PARM_VALUE  , reinterpret_cast<char *>(CHALIGN_TYPE_CENTRE ), DH_OFFSET(alignment),},
  {"centre" , PARM_VALUE  , reinterpret_cast<char *>(CHALIGN_TYPE_CENTRE ), DH_OFFSET(alignment),},
  {"left"   , PARM_VALUE  , reinterpret_cast<char *>(CHALIGN_TYPE_LEFT   ), DH_OFFSET(alignment),},
  {"inside" , PARM_VALUE  , reinterpret_cast<char *>(CHALIGN_TYPE_INSIDE ), DH_OFFSET(alignment),},
  {"outside", PARM_VALUE  , reinterpret_cast<char *>(CHALIGN_TYPE_OUTSIDE), DH_OFFSET(alignment),},
  {"right"  , PARM_VALUE  , reinterpret_cast<char *>(CHALIGN_TYPE_RIGHT  ), DH_OFFSET(alignment),},

  {NULL, 0, NULL, 0,},
};

/*---------------------------------------------------------------------------*/

/* Definition List Parameter Definition */

#define DL_OFFSET(a) CDocOffset(CDDefinitionList*,a)

static CDParameterData
dl_parameter_data[] = {
  {"break"  , PARM_FLAG   , NULL, DL_OFFSET(break_up         ),},
  {"compact", PARM_FLAG   , NULL, DL_OFFSET(compact          ),},
  {"headhi" , PARM_INT    , NULL, DL_OFFSET(heading_highlight),},
  {"termhi" , PARM_INT    , NULL, DL_OFFSET(term_highlight   ),},
  {"tsize"  , PARM_CLENSTR, NULL, DL_OFFSET(depth            ),},
  {NULL     , 0           , NULL, 0                           ,},
};

/*---------------------------------------------------------------------------*/

/* Figure Parameter Definition */

#define FIG_OFFSET(a) CDocOffset(CDFigureData*,a)

struct CDFigureData {
  int   depth;
  char *frame;
  char *ident;
  int   indent;
  int   place;
  int   width;
};

static CDParameterChoiceData
fig_frame_data[] = {
  {"rule", reinterpret_cast<char *>(FRAME_RULE  ) },
  {"box" , reinterpret_cast<char *>(FRAME_BOX   ) },
  {"none", reinterpret_cast<char *>(FRAME_NONE  ) },
  {""    , reinterpret_cast<char *>(PARM_NEW_STR) },
  {NULL  , nullptr                                },
};

static CDParameterChoiceData
fig_place_data[] = {
  {"top"   , reinterpret_cast<char *>(PLACE_TOP   ) },
  {"bottom", reinterpret_cast<char *>(PLACE_BOTTOM) },
  {"inline", reinterpret_cast<char *>(PLACE_INLINE) },
  {NULL    , nullptr                                },
};

static CDParameterChoiceData
fig_width_data[] = {
  {"page"  , reinterpret_cast<char *>(WIDTH_PAGE  ) },
  {"column", reinterpret_cast<char *>(WIDTH_COLUMN) },
  {""      , reinterpret_cast<char *>(PARM_CLENSTR) },
  {NULL    , nullptr                                },
};

static CDParameterData
fig_parameter_data[] = {
  {"depth" , PARM_LLENSTR, NULL                                    , FIG_OFFSET(depth ),},
  {"frame" , PARM_CHOICE , reinterpret_cast<char *>(fig_frame_data), FIG_OFFSET(frame ),},
  {"id"    , PARM_NEW_STR, NULL                                    , FIG_OFFSET(ident ),},
  {"indent", PARM_CLENSTR, NULL                                    , FIG_OFFSET(indent),},
  {"place" , PARM_CHOICE , reinterpret_cast<char *>(fig_place_data), FIG_OFFSET(place ),},
  {"width" , PARM_CHOICE , reinterpret_cast<char *>(fig_width_data), FIG_OFFSET(width ),},
  {NULL    , 0           , NULL                                    , 0                 ,},
};

/*---------------------------------------------------------------------------*/

/* Footnote Parameter Definition */

#define FN_OFFSET(a) CDocOffset(CDFootnoteData*,a)

struct CDFootnoteData {
  char *ident;
};

static CDParameterData
fn_parameter_data[] = {
  {"id", PARM_NEW_STR, NULL, FN_OFFSET(ident),},
  {NULL, 0           , NULL, 0               ,},
};

/*---------------------------------------------------------------------------*/

/* General and Stairs Document Parameter Definition */

#define DOC_OFFSET(a) CDocOffset(CDDocData*,a)

struct CDDocData {
  char *security;
  char *language;
};

static CDParameterData
gdoc_parameter_data[] = {
  {"sec"     , PARM_NEW_STR, NULL, DOC_OFFSET(security),},
  {"language", PARM_NEW_STR, NULL, DOC_OFFSET(language),},
  {NULL      , 0           , NULL, 0                   ,},
};

static CDParameterData
sdoc_parameter_data[] = {
  {"sec"     , PARM_NEW_STR, NULL, DOC_OFFSET(security),},
  {"language", PARM_NEW_STR, NULL, DOC_OFFSET(language),},
  {NULL      , 0           , NULL, 0                   ,},
};

/*---------------------------------------------------------------------------*/

/* Glossary List Parameter Definition */

#define GL_OFFSET(a) CDocOffset(CDGlossaryList*,a)

static CDParameterData
gl_parameter_data[] = {
  {"compact", PARM_FLAG, NULL, GL_OFFSET(compact       ),},
  {"termhi" , PARM_INT , NULL, GL_OFFSET(term_highlight),},
  {NULL     , 0        , NULL, 0                        ,},
};

/*---------------------------------------------------------------------------*/

/* Headings Parameter Definition */

#define HX_OFFSET(a) CDocOffset(CDHeading*,a)

struct CDHeading {
  char *ident;
  char *stitle;
};

static CDParameterData
h01_parameter_data[] = {
  {"id"    , PARM_STR, NULL, HX_OFFSET(ident ),},
  {"stitle", PARM_STR, NULL, HX_OFFSET(stitle),},
  {NULL    , 0       , NULL, 0                ,},
};

static CDParameterData
h26_parameter_data[] = {
  {"id", PARM_STR, NULL, HX_OFFSET(ident),},
  {NULL, 0       , NULL, 0               ,},
};

/*---------------------------------------------------------------------------*/

/* Index Heading Parameter Definition */

#define IHX_OFFSET(a) CDocOffset(CDIndexEntryHeading*,a)

struct CDIndexEntryHeading {
  char *ident;
  char *print;
  char *see;
  char *seeid;
};

static CDParameterData
ih_parameter_data[] = {
  {"id"   , PARM_STR, NULL, IHX_OFFSET(ident),},
  {"print", PARM_STR, NULL, IHX_OFFSET(print),},
  {"see"  , PARM_STR, NULL, IHX_OFFSET(see  ),},
  {"seeid", PARM_STR, NULL, IHX_OFFSET(seeid),},
  {NULL   , 0       , NULL, 0                ,},
};

/*---------------------------------------------------------------------------*/

/* Index Heading Parameter Definition */

#define IX_OFFSET(a)   CDocOffset(CDIndexEntry*,a)
#define IREF_OFFSET(a) CDocOffset(CDIndexRef*,a)

struct CDIndexEntry {
  char *ident;
  char *refid;
  char *page;
};

struct CDIndexRef {
  char *refid;
  char *pg;
  char *see;
  char *seeid;
};

static CDParameterChoiceData
index_pg_data[] = {
  {"start", reinterpret_cast<char *>(PG_START    ) },
  {"end"  , reinterpret_cast<char *>(PG_END      ) },
  {"major", reinterpret_cast<char *>(PG_MAJOR    ) },
  {""     , reinterpret_cast<char *>(PARM_NEW_STR) },
  {NULL   , nullptr                                },
};

static CDParameterData
i1_parameter_data[] = {
  {"id"   , PARM_STR    , NULL                                   , IX_OFFSET(ident)  ,},
  {"pg"   , PARM_CHOICE , reinterpret_cast<char *>(index_pg_data), IX_OFFSET(page )  ,},
  {NULL   , 0           , NULL                                   , 0                 ,},
};

static CDParameterData
i23_parameter_data[] = {
  {"id"   , PARM_STR    , NULL                                   , IX_OFFSET(ident)  ,},
  {"refid", PARM_STR    , NULL                                   , IX_OFFSET(refid)  ,},
  {"pg"   , PARM_CHOICE , reinterpret_cast<char *>(index_pg_data), IX_OFFSET(page )  ,},
  {NULL   , 0           , NULL                                   , 0                 ,},
};

static CDParameterData
iref_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL                                   , IREF_OFFSET(refid),},
  {"pg"   , PARM_CHOICE , reinterpret_cast<char *>(index_pg_data), IREF_OFFSET(pg   ),},
  {"see"  , PARM_NEW_STR, NULL                                   , IREF_OFFSET(see  ),},
  {"seeid", PARM_NEW_STR, NULL                                   , IREF_OFFSET(seeid),},
  {NULL   , 0           , NULL                                   , 0                 ,},
};

/*---------------------------------------------------------------------------*/

/* List Item Parameter Definition */

#define LI_OFFSET(a) CDocOffset(CDListItem*,a)

struct CDListItem {
  char *ident;
};

static CDParameterData
li_parameter_data[] = {
  {"id", PARM_STR, NULL, LI_OFFSET(ident),},
  {NULL, 0       , NULL, 0               ,},
};

/*---------------------------------------------------------------------------*/

/* List Parameter Definition */

#define LIST_OFFSET(a) CDocOffset(CDGeneralList*,a)

static CDParameterData
ol_parameter_data[] = {
  {"compact", PARM_FLAG, NULL, LIST_OFFSET(compact),},
  {NULL     , 0        , NULL, 0                   ,},
};

static CDParameterData
sl_parameter_data[] = {
  {"compact", PARM_FLAG, NULL, LIST_OFFSET(compact),},
  {NULL     , 0        , NULL, 0                   ,},
};

static CDParameterData
ul_parameter_data[] = {
  {"compact", PARM_FLAG, NULL, LIST_OFFSET(compact),},
  {NULL     , 0        , NULL, 0                   ,},
};

/*---------------------------------------------------------------------------*/

/* Row Parameter Definition */

#define ROW_OFFSET(a) CDocOffset(CDTableRowData*,a)

struct CDTableRowData {
  char *refid;
  int   split;
};

static CDParameterData
row_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, ROW_OFFSET(refid),},
  {"split", PARM_BOOLEAN, NULL, ROW_OFFSET(split),},
  {NULL   , 0           , NULL, 0                ,},
};

/*---------------------------------------------------------------------------*/

/* Table Parameter Definition */

#define TABLE_OFFSET(a) CDocOffset(CDTableData*,a)

struct CDTableData {
  int   column;
  char *id;
  int   page;
  char *refid;
  int   split;
  int   rotate;
  int   width;
};

static CDParameterData
table_parameter_data[] = {
  {"column", PARM_VALUE  , reinterpret_cast<char *>(WIDTH_COLUMN), TABLE_OFFSET(column),},
  {"id"    , PARM_NEW_STR, NULL                                  , TABLE_OFFSET(id    ),},
  {"page"  , PARM_VALUE  , reinterpret_cast<char *>(WIDTH_PAGE  ), TABLE_OFFSET(page  ),},
  {"refid" , PARM_NEW_STR, NULL                                  , TABLE_OFFSET(refid ),},
  {"split" , PARM_BOOLEAN, NULL                                  , TABLE_OFFSET(split ),},
  {"rotate", PARM_INT    , NULL                                  , TABLE_OFFSET(rotate),},
  {"width" , PARM_CLENSTR, NULL                                  , TABLE_OFFSET(width ),},
  {NULL    , 0           , NULL                                  , 0                   ,},
};

/*---------------------------------------------------------------------------*/

/* Table Footer Parameter Definition */

static CDParameterData
tft_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, ROW_OFFSET(refid),},
  {NULL   , 0           , NULL, 0                ,},
};

/*---------------------------------------------------------------------------*/

/* Table Header Parameter Definition */

static CDParameterData
thd_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, ROW_OFFSET(refid),},
  {NULL   , 0           , NULL, 0                ,},
};

/*---------------------------------------------------------------------------*/

/* Example Parameter Definition */

#define XMP_OFFSET(a) CDocOffset(CDExample*,a)

static CDParameterData
xmp_parameter_data[] = {
  {"depth", PARM_LLENSTR, NULL, XMP_OFFSET(depth),},
  {NULL   , 0           , NULL, 0                ,},
};

/*---------------------------------------------------------------------------*/

/* Cross Reference Parameter Definitions */

#define CREF_OFFSET(a) CDocOffset(CDCrossRef*,a)

static CDParameterData
figref_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, CREF_OFFSET(refid),},
  {"page" , PARM_BOOLEAN, NULL, CREF_OFFSET(page ),},
  {NULL   , 0           , NULL, 0                 ,},
};

static CDParameterData
fnref_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, CREF_OFFSET(refid),},
  {NULL   , 0           , NULL, 0                 ,},
};

static CDParameterData
hdref_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, CREF_OFFSET(refid),},
  {"page" , PARM_BOOLEAN, NULL, CREF_OFFSET(page ),},
  {NULL   , 0           , NULL, 0                 ,},
};

static CDParameterData
liref_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, CREF_OFFSET(refid),},
  {"page" , PARM_BOOLEAN, NULL, CREF_OFFSET(page ),},
  {NULL   , 0           , NULL, 0                 ,},
};

static CDParameterData
tref_parameter_data[] = {
  {"refid", PARM_NEW_STR, NULL, CREF_OFFSET(refid),},
  {"page" , PARM_BOOLEAN, NULL, CREF_OFFSET(page ),},
  {NULL   , 0           , NULL, 0                 ,},
};

/*---------------------------------------------------------------------------*/

/* Hook Parameter Definitions */

struct CDHookParameterData {
  char *id;
  char *data;
  char *text;
};

#define HOOK_OFFSET(a) CDocOffset(CDHookParameterData*,a)

static CDParameterData
hook_parameter_data[] = {
  {"id"  , PARM_NEW_STR, NULL, HOOK_OFFSET(id  ),},
  {"data", PARM_NEW_STR, NULL, HOOK_OFFSET(data),},
  {"text", PARM_NEW_STR, NULL, HOOK_OFFSET(text),},
  {NULL  , 0           , NULL, 0                ,},
};

/*---------------------------------------------------------------------------*/

static CDHeaderControl
default_header_control[CDOC_MAX_HEADERS] = {
  /* Header 0 */

  {
    /* New Page              */ true,
    /* Section Breaks        */ true,
    /* Alignment             */ CHALIGN_TYPE_LEFT,
    /* Space Before          */ 0,
    /* Skip Before           */ 0,
    /* Space after           */ 0,
    /* Underscored           */ true,
    /* Capitalised           */ true,
    /* Break header          */ true,
    /* TOC entry             */ true,
    /* TOC only              */ false,
    /* Skip before TOC Entry */ true,
    /* TOC Indentation       */ 0,
    /* Number Header         */ false,
    /* Font                  */ NULL,
    /* TOC Font              */ NULL,
  },

  /* Header 1 */

  {
    /* New Page              */ true,
    /* Section Breaks        */ true,
    /* Alignment             */ CHALIGN_TYPE_LEFT,
    /* Space Before          */ 0,
    /* Skip Before           */ 0,
    /* Space after           */ 5,
    /* Underscored           */ false,
    /* Capitalised           */ true,
    /* Break header          */ true,
    /* TOC entry             */ true,
    /* TOC only              */ false,
    /* Skip before TOC Entry */ true,
    /* TOC Indentation       */ 0,
    /* Number Header         */ true,
    /* Font                  */ NULL,
    /* TOC Font              */ NULL,
  },

  /* Header 2 */

  {
    /* New Page              */ false,
    /* Section Breaks        */ false,
    /* Alignment             */ CHALIGN_TYPE_LEFT,
    /* Space Before          */ 0,
    /* Skip Before           */ 3,
    /* Space after           */ 2,
    /* Underscored           */ false,
    /* Capitalised           */ true,
    /* Break header          */ true,
    /* TOC entry             */ true,
    /* TOC only              */ false,
    /* Skip before TOC Entry */ false,
    /* TOC Indentation       */ 0,
    /* Number Header         */ true,
    /* Font                  */ NULL,
    /* TOC Font              */ NULL,
  },

  /* Header 3 */

  {
    /* New Page              */ false,
    /* Section Breaks        */ false,
    /* Alignment             */ CHALIGN_TYPE_LEFT,
    /* Space Before          */ 0,
    /* Skip Before           */ 3,
    /* Space after           */ 2,
    /* Underscored           */ false,
    /* Capitalised           */ false,
    /* Break header          */ true,
    /* TOC entry             */ true,
    /* TOC only              */ false,
    /* Skip before TOC Entry */ false,
    /* TOC Indentation       */ 2,
    /* Number Header         */ true,
    /* Font                  */ NULL,
    /* TOC Font              */ NULL,
  },

  /* Header 4 */

  {
    /* New Page              */ false,
    /* Section Breaks        */ false,
    /* Alignment             */ CHALIGN_TYPE_LEFT,
    /* Space Before          */ 0,
    /* Skip Before           */ 3,
    /* Space after           */ 2,
    /* Underscored           */ true,
    /* Capitalised           */ false,
    /* Break header          */ true,
    /* TOC entry             */ false,
    /* TOC only              */ false,
    /* Skip before TOC Entry */ false,
    /* TOC Indentation       */ 4,
    /* Number Header         */ true,
    /* Font                  */ NULL,
    /* TOC Font              */ NULL,
  },

  /* Header 5 */

  {
    /* New Page              */ false,
    /* Section Breaks        */ false,
    /* Alignment             */ CHALIGN_TYPE_LEFT,
    /* Space Before          */ 0,
    /* Skip Before           */ 1,
    /* Space after           */ 0,
    /* Underscored           */ false,
    /* Capitalised           */ true,
    /* Break header          */ false,
    /* TOC entry             */ false,
    /* TOC only              */ false,
    /* Skip before TOC Entry */ false,
    /* TOC Indentation       */ 6,
    /* Number Header         */ false,
    /* Font                  */ NULL,
    /* TOC Font              */ NULL,
  },

  /* Header 6 */

  {
    /* New Page              */ false,
    /* Section Breaks        */ false,
    /* Alignment             */ CHALIGN_TYPE_LEFT,
    /* Space Before          */ 0,
    /* Skip Before           */ 1,
    /* Space after           */ 0,
    /* Underscored           */ true,
    /* Capitalised           */ false,
    /* Break header          */ false,
    /* TOC entry             */ false,
    /* TOC only              */ false,
    /* Skip before TOC Entry */ false,
    /* TOC Indentation       */ 8,
    /* Number Header         */ false,
    /* Font                  */ NULL,
    /* TOC Font              */ NULL,
  },
};


static const char *
ileaf_header[] = {
"<!OPS, Version = 8.0>",
"",
"<!Page Number Stream,",
" 	Name =			\"page\",",
"	Starting Page # = 	Inherit>",
"",
"<!Document,",
"	Final Output Device =	\"ileaf\",",
"	Default Printer =	\"nearest-ileaf\",",
"	Default Page Stream Name = \"page\">",
"",
"<!Font Definitions,",
"	F2 = Thames 12,",
"	F3 = Times 12,",
"	F4 = Times 6,",
"	F5 = Times 10,",
"	F6 = Times 12 Bold,",
"	F7 = Helvetica 14 Bold,",
"	F8 = Symbols 12,",
"	F9 = Thames 6,",
"	F10 = Times 12 Italic,",
"	F11 = Times 12 Bold Italic>",
"",
"<!Page,",
"	Bottom Margin =		1 Inches,",
"	Left Margin =		1 Inches,",
"	Right Margin =		1 Inches,",
"	Hyphenation =		on,",
"	Consecutive Hyphens =	3,",
"	Revision Bar Placement = Left,",
"	Feathering =		off>",
"",
"<!Autonumber Stream, \"footnote\", 1,",
"	Level 1 Suffix =	\"\">",
"",
"<!Autonumber Stream, \"list\", 1>",
"",
"<!Class, \"bullet\",",
"	Top Margin =		0.04 Inches,",
"	Bottom Margin =		0.04 Inches,",
"	Left Margin =		0.75 Inches,",
"	Right Margin =		0.75 Inches,",
"	First Indent =		-0.25 Inches,",
"	Alignment =		Left,",
"	Font =			F3@Z7@Lam,",
"	Line Spacing =		1.1606 lines,",
"	Left Tab =		0/0.5/1/2/3 Inches,",
"	Composition =		Optimum,",
"	Contents =		Prefix>",
"",
"<\"|:bullet\",",
"	Hidden =		yes,",
"	Font =			@i*,",
"	Subcomponent =		yes,",
"	Contents =		Shared><F8@Z7@Lam>S<F0><Tab>",
"<End Sub><F0>",
"",
"<!Class, \"head\",",
"	Top Margin =		0.1777775 Inches,",
"	Bottom Margin =		0.05 Inches,",
"	Alignment =		Left,",
"	Font =			F7@Z7@Lam,",
"	Line Spacing =		1.1434 lines,",
"	Allow Page Break Within = no,",
"	Allow Page Break After = no,",
"	Left Tab =		0/1*3 Inches,",
"	Composition =		Optimum>",
"",
"<!Class, \"list\",",
"	Top Margin =		0.04 Inches,",
"	Bottom Margin =		0.04 Inches,",
"	Left Margin =		0.75 Inches,",
"	Right Margin =		0.75 Inches,",
"	First Indent =		-0.50 Inches,",
"	Alignment =		Left,",
"	Font =			F3@Z7@Lam,",
"	Line Spacing =		1.1606 lines,",
"	Left Tab =		0/1*3 Inches,",
"	Decimal Tab =		-0.15 Inches,",
"	Composition =		Optimum,",
"	Contents =		Prefix>",
"",
"<\"|:list\",",
"	Hidden =		yes,",
"	Font =			@i*,",
"	Subcomponent =		yes,",
"	Contents =		Shared><F0><Tab><Autonum, \"list\", 1><Tab>",
"<End Sub><F0>",
"",
"<!Class, \"micro:caption\",",
"	Top Margin =		0.03 Inches,",
"	Bottom Margin =		0.03 Inches,",
"	Alignment =		Left,",
"	Font =			F5@Z7@Lam,",
"	Line Spacing =		1.1124 lines,",
"	Left Tab =		0/0.5*3 Inches,",
"	Composition =		Optimum>",
"",
"<!Class, \"micro:ftnote\",",
"	Top Margin =		0.0295272 Inches,",
"	Bottom Margin =		0.0295272 Inches,",
"	Alignment =		Left,",
"	Font =			F5@Z7@Lam,",
"	Line Spacing =		1.1124 lines,",
"	Left Tab =		0/0.25 Inches,",
"	Composition =		Optimum>",
"",
"<!Class, \"para\",",
"	Top Margin =		0.04 Inches,",
"	Bottom Margin =		0.04 Inches,",
"	Font =			F3@Z7@Lam,",
"	Line Spacing =		1.1606 lines,",
"	Left Tab =		0/1*3 Inches,",
"	Composition =		Optimum>",
"",
"<!Class, \"subhead\",",
"	Top Margin =		0.06 Inches,",
"	Bottom Margin =		0.05 Inches,",
"	Alignment =		Left,",
"	Font =			F6@Z7@Lam,",
"	Line Spacing =		1.1606 lines,",
"	Allow Page Break Within = no,",
"	Allow Page Break After = no,",
"	Left Tab =		0/1*3 Inches,",
"	Composition =		Optimum>",
"",
"<!Class, \"|:bullet\",",
"	Top Margin =		0.0266667 Inches,",
"	Bottom Margin =		0.0266667 Inches,",
"	Font =			F3@Z7@Lnl@i*,",
"	Line Spacing =		1.162 lines,",
"	Left Tab =		0/1*3 Inches,",
"	Composition =		Optimum,",
"	Contents =		Shared>",
"",
"<F8@Z7@Lnl>S<F0><Tab>",
"",
"<!Class, \"|:list\",",
"	Top Margin =		0.0266667 Inches,",
"	Bottom Margin =		0.0266667 Inches,",
"	Font =			F3@Z7@Lnl@i*,",
"	Line Spacing =		1.162 lines,",
"	Left Tab =		0/1*3 Inches,",
"	Composition =		Optimum,",
"	Contents =		Shared>",
"",
"<Tab><Autonum, \"list\", 1><Tab>",
"",
"<!Master Frame,",
"	Name =			\"floating\",",
"	Placement =		Following Anchor,",
"	Horizontal Alignment =	Center,",
"	Width =			2 Inches,",
"	Width =			Page Without Margins,",
"	Height =		1 Inches,",
"	Height =		Page Without Margins * 0.33,",
"	Diagram =",
"V11,",
"(g9,0,0,",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,0,0,0,1,5,127,7,0,0,7,0,1,1,0.0666667,0.06",
"  66667,6,6,0,0.0666667,6))>",
"",
"<!Master Frame,",
"	Name =			\"footnote:numbered\",",
"	Placement =		Bottom of Page,",
"	Horizontal Alignment =	Left,",
"	Same Page =		yes,",
"	Width =			6.2992125 Inches,",
"	Width =			Column,",
"	Size Contents To Width = yes,",
"	Height =		0.20 Inches,",
"	Height =		Contents,",
"	Auto Edit =		yes,",
"	Numbered = 		'<Autonum, \"footnote\", 1, Tagname = \"EiXYY2e4vr\">',",
"	Superscript =		yes,",
"	Diagram =",
"V11,",
"(g9,1,0,",
" (T15,1,12,,0,0,5,127,5,7,127,1,0,2,",
"<!Page, Width = 6.28 Inches, Height = 0.19837 Inches,",
"	Top Margin =		0.03 Inches,",
"	Bottom Margin =		0.03 Inches,",
"	Revision Bar Placement = Left>",
"<\"micro:ftnote\",",
"	Hidden =		yes>",
"",
"<Ref, Auto #, Tag = \"EiXYY2e4vr\">.<Tab>",
"",
"<End Text>)",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,0,0,0,1,5,127,7,0,0,7,0,1,1,0.0666667,0.06",
"  66667,6,6,0,0.0666667,6))>",
"",
"<!Master Frame,",
"	Name =			\"footnote:unnumbered\",",
"	Placement =		Bottom of Page,",
"	Horizontal Alignment =	Left,",
"	Same Page =		yes,",
"	Width =			6.2992125 Inches,",
"	Width =			Column,",
"	Size Contents To Width = yes,",
"	Height =		0.1466667 Inches,",
"	Height =		Contents,",
"	Auto Edit =		yes,",
"	Diagram =",
"V11,",
"(g9,1,0,",
" (T15,1,12,,0.0266667,0.0266667,5,127,5,7,127,1,0,2,",
"<!Page, Width = 6.28 Inches, Height = 0.1383699 Inches,",
"	Revision Bar Placement = Left>",
"<\"micro:ftnote\",",
"	Hidden =		yes>",
"",
"<End Text>)",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,-0.0266667,-0.0266667,0,1,5,127,7,0,0,7,0,",
"  1,1,0.0666667,0.0666667,6,6,0,0.0666667,6))>",
"",
"<!Master Frame,",
"	Name =			\"fullpage\",",
"	Placement =		Overlay,",
"	Horizontal Alignment =	Center,",
"	Vertical Alignment =	Center,",
"	Horizontal Reference =	Page With Both Margins,",
"	Vertical Reference =	Page With Both Margins,",
"	Width =			2 Inches,",
"	Width =			Page With Both Margins,",
"	Height =		1 Inches,",
"	Height =		Page With Both Margins,",
"	Diagram =",
"V11,",
"(g9,0,0,",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,0,0,0,1,5,127,7,0,0,7,0,1,1,0.0666667,0.06",
"  66667,6,6,0,0.0666667,6))>",
"",
"<!Master Frame,",
"	Name =			\"inline\",",
"	Placement =		At Anchor,",
"	Width =			0.50 Inches,",
"	Height =		0.1655558 Inches,",
"	Diagram =",
"V11,",
"(g9,0,0,",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,0,0,0,1,5,127,7,0,0,7,0,1,1,0.0666667,0.06",
"  66667,6,6,0,0.0666667,6))>",
"",
"<!Master Frame,",
"	Name =			\"page\",",
"	Placement =		At Anchor,",
"	Vertical Alignment =	Top,",
"	Width =			2 Inches,",
"	Width =			Page Without Margins,",
"	Height =		1 Inches,",
"	Height =		Page Without Margins,",
"	Diagram =",
"V11,",
"(g9,0,0,",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,0,0,0,1,5,127,7,0,0,7,0,1,1,0.0666667,0.06",
"  66667,6,6,0,0.0666667,6))>",
"",
"<Page Header, Frame =",
"V11,",
"(g9,1,0,",
" (t14,1,4,,6.4933333,0.4387817,2,7,0,0,,wst:timsps10,)",
" (t14,2,4,,3.2466667,0.4387817,1,7,0,0,,wst:timsps10,)",
" (t14,3,4,,0,0.4387817,0,7,0,0,,wst:timsps10,)",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,0,0,0,1,5,127,7,0,0,7,0,1,1,0.0666667,0.06",
"  66667,6,6,0,0.0666667,6))>",
"",
"<Page Footer, Frame =",
"V11,",
"(g9,1,0,",
" (t14,1,4,,6.4933333,0.478772,2,7,0,0,,wst:timsps10,)",
" (t14,2,4,,3.2466667,0.478772,1,7,0,0,,wst:timsps10,\\X80a0)",
" (t14,3,4,,0,0.478772,0,7,0,0,,wst:timsps10,)",
" (E16,0,0,,5,1,1,0.0533333,1,15,0,0,1,0,0,0,1,5,127,7,0,0,7,0,1,1,0.0666667,0.06",
"  66667,6,6,0,0.0666667,6))>",
"",
"<!End Declarations>",
};

/*---------------------------------------------------------------------------*/

CDDocument         cdoc_document;
int                cdoc_pass_no             = 0;
FILE              *cdoc_input_fp            = NULL;
FILEList           cdoc_input_fp_list;
int                cdoc_input_line_no       = 0;
FILE              *cdoc_output_fp           = NULL;
std::string        cdoc_current_section;
int                cdoc_indent              = 0;
int                cdoc_no_centred          = 0;
int                cdoc_no_uscore           = 0;
int                cdoc_no_cap              = 0;
int                cdoc_formatting          = true;
CDDefinitionList  *cdoc_definition_list     = NULL;
CDGeneralList     *cdoc_general_list        = NULL;
CDGlossaryList    *cdoc_glossary_list       = NULL;
std::string        cdoc_date;
std::string        cdoc_footer;
std::string        cdoc_line;
int                cdoc_line_len;
int                cdoc_continuation_char   = int('\0');

/*---------------------------------------------------------------------------*/

static char cdoc_a_to_z[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int               long_quotation_depth    = 0;
static int               appendix_depth          = 0;

static int               header_number[CDOC_MAX_HEADERS];
static CDHeaderControl   header_control[CDOC_MAX_HEADERS];

/* Lists */

static char              definition_prefix[CDOC_MAX_LINE] = "";
static char              list_prefix[CDOC_MAX_LINE]       = "";
static std::string       glossary_prefix;

/* Tables */

static CDTable          *current_table = NULL;
static int               table_number  = 0;
static CDTableRow       *current_row   = NULL;
static CDTableCell      *current_cell  = NULL;

/* Figures */

static CDFigure         *current_figure = NULL;
static int               figure_number  = 0;

/* Examples */

static CDExample        *current_example = NULL;

/* Footnotes */

static CDFootnote       *current_footnote        = NULL;
static int               current_footnote_number = 0;

/*---------------------------------------------------------------------------*/

static void        CDocScriptInitPass1
                    ();
static int         CDocProcessFilePass1
                    (const std::string &);
static void        CDocProcessCommentLinePass1
                    (const std::string &);
static void        CDocProcessDotCommandPass1
                    (CDDotCommand *);
static void        CDocProcessColonCommandPass1
                    (CDColonCommand *);
static void        CDocScriptInitPass2
                    ();
static void        CDocProcessFilePass2
                    (const std::string &);
static void        CDocInitialiseOutputPass2
                    ();
static void        CDocProcessCommentLinePass2
                    (const std::string &);
static void        CDocProcessPass2Line
                    (CDScriptLine *);
static void        CDocProcessDotCommandPass2
                    (CDDotCommand *);
static void        CDocProcessColonCommandPass2
                    (CDColonCommand *);
static void        CDocReplaceEmbeddedColonCommands
                    (CDScriptLine *);
static void        CDocScriptTerm
                    ();
static void        CDocScriptInitHeaders
                    ();
static int         CDocScriptHighlightToFont
                    (int);
static std::string CDocScriptAddHighlightEscapes
                    (const std::string &, int);

// Converts the input file containing IBM Script commands to the output format
// defined using the CDoc::setOutputFormat() routine and to the file defined
// by CDoc::setOutputFilename().
//
// The output can be customized using the control routines :-
//
//   CDocSetScriptMargins()
//   CDocSetScriptHeaderNumbering()
//   CDocSetScriptWarnings()
//   CDocSetScriptParagraphIndent()
extern void
CDocScriptProcessFile(const std::string &filename)
{
  /* Save Script Options */

  CDocScriptSaveOptions();

  /* Initialise State for Pass 1 Processing */

  CDocScriptInitPass1();

  /* Do Pass 1 of the Script File Processing to Add References
     and Set up the Table of Contents. */

  int error = CDocProcessFilePass1(filename);

  if (error != 0)
    goto ProcessFile_1;

  /* Initialise State for Pass 2 Processing */

  CDocScriptInitPass2();

  /* Do Pass 2 of the Script File Processing to actually
   * output the processed Script File text */

  CDocProcessFilePass2(filename);

  /* Terminate Script Processing */

  CDocScriptTerm();

 ProcessFile_1:
  CDocScriptRestoreOptions();
}

// Initialises CDoc ready for Pass 1 Processing of the Script File.
//
// Should set all Static Variables to their Initial Values.
static void
CDocScriptInitPass1()
{
  /* Initialise the Document */

  cdoc_document.security = "";
  cdoc_document.language = "";
  cdoc_document.type     = CDOC_NO_DOCUMENT;
  cdoc_document.part     = CDOC_NO_PART;
  cdoc_document.sub_part = CDOC_NO_SUB_PART;

  /* Initialise Spell Checker if Required */

  if (cdoc_spell_check)
    cdoc_spell_check  = CSpellInit();

  if (cdoc_spell_check)
    cdoc_spell_active = true;

  /* Set Pass Number */

  cdoc_pass_no = 1;

  /* Initialise Current Input Line Number */

  cdoc_input_line_no = 0;

  /* Initialise Output File Pointer */

  cdoc_output_fp = stdout;

  /* Initialise Current Section */

  cdoc_current_section = "";

  /* Initialise Indent and Formatting */

  cdoc_indent     = 0;
  cdoc_formatting = true;

  /* Initialise Lists */

  cdoc_definition_list = NULL;
  cdoc_general_list    = NULL;
  cdoc_glossary_list   = NULL;

  /* Initialise Footer and Current Line */

  cdoc_footer   = "";
  cdoc_line     = "";
  cdoc_line_len = 0;

  /* Initialise Long Quotation Depth */

  long_quotation_depth = 0;

  /* If Output Filename has been set then open the file
     (if possible) and set the Output File Pointer to it */

  if (CDocInst->getOutputFilename() != "") {
    FILE *fp = fopen(CDocInst->getOutputFilename().c_str(), "w");

    if (fp != NULL)
      cdoc_output_fp = fp;
  }

  /* If Page Numbers are required then write to a temporary
     file first */

  if (cdoc_page_numbering)
    CDocScriptPageNoInit();

  /* Initialise the References defined in an Extra File */

  CDocScriptInitReferences();

  /* Initialise Page/Line and Character Counts */

  cdoc_page_no            = 1;
  cdoc_page_no_offset     = 0;
  cdoc_line_no            = 0;
  cdoc_char_no            = 0;
  cdoc_line_fiddle        = 0;
  cdoc_page_header_output = false;

  /* Initialise Processing Control */

  CDocScriptInitPCtrl();

  /* Reset Dot Command Control Variables */

  cdoc_no_centred = 0;
  cdoc_no_cap     = 0;
  cdoc_no_uscore  = 0;

  /* Initialise Paragraph Data */

  CDocScriptInitParagraphs();

  /* Initialise the Footnote Variables */

  current_footnote_number = 0;

  /* Initialise Current Header Level for each of the Levels */

  CDocScriptInitHeaders();

  /* Initialise Current Figure and Table Details */

  current_figure = NULL;
  current_table  = NULL;
  current_row    = NULL;
  current_cell   = NULL;

  table_number   = 0;
  figure_number  = 0;

  /* Set Current Date String */

  time_t current_time = time(&current_time);

  cdoc_date = COSTime::getTimeString(current_time, "%d %h %y");

  /* Initialise Current Appendix Depth */

  appendix_depth = 0;

  /* Set Header Control from Default Header Control */

  memcpy(header_control, default_header_control,
         CDOC_MAX_HEADERS*sizeof(CDHeaderControl));

  /* Set Continuation Character */

  cdoc_continuation_char = int('\0');

  /* Initialise Translations */

  CDocScriptInitTranslations();
}

// Perform Pass 1 of the processing of the Script File.
//
// This performs the following functions :-
//
//   . Records Information about Commands with Identifiers
//   . Builds the Table of Contents
//   . Records Information about Figures
//   . Process Commands which alter the Initial State
static int
CDocProcessFilePass1(const std::string &filename)
{
  CDScriptLine   *script_line;
  CDDotCommand   *dot_command;
  CDColonCommand *colon_command;

  /* Initialise Error Return Code and Current List Depth */

  int error = 0;

  /* Open the Script File (Fail if can't be opened) */

  cdoc_input_fp = fopen(filename.c_str(), "r");

  if (cdoc_input_fp == NULL) {
    cdoc_input_fp = stdin;

    CDocScriptError("Dataset '%s' cannot be read", filename.c_str());

    error = 1;

    return(error);
  }

  /* Read and Process each Line from the File */

  while ((script_line = CDocScriptGetNextLine()) != NULL) {
    if      (script_line->getType() == CDOC_DOT_COMMAND)
      dot_command = script_line->getDotCommand();
    else if (script_line->getType() == CDOC_COLON_COMMAND)
      colon_command = script_line->getColonCommand();

    /* Output Line if Debug */

    if (CDocInst->getDebug() & CDOC_DEBUG_PASS_1) {
      if (! CDocScriptProcessing())
        printf("# ");

      script_line->print();
    }

    if (CDocScriptProcessing()) {
      /* Check for Comment Line (not a Normal Dot Command)
         in which we hide CDoc Specific Commands */

      if (script_line->getType() == CDOC_DOT_COMMAND &&
          dot_command->getCommand() == "*") {
        CDocProcessCommentLinePass1(dot_command->getText());

        delete script_line;

        continue;
      }

      /* Replace embedded Symbols in Text */

      if      (script_line->getType() == CDOC_DOT_COMMAND) {
        std::string text = CDocScriptReplaceSymbolsInString(dot_command->getText());

        dot_command->setText(text);
      }
      else if (script_line->getType() == CDOC_COLON_COMMAND) {
        std::string text = CDocScriptReplaceSymbolsInString(colon_command->getText());

        colon_command->setText(text);
      }

      /* Process a Line consisting of a Dot Command */

      if      (script_line->getType() == CDOC_DOT_COMMAND)
        CDocProcessDotCommandPass1(dot_command);

      /* Process Colon Command */

      else if (script_line->getType() == CDOC_COLON_COMMAND)
        CDocProcessColonCommandPass1(colon_command);
    }
    else {
      if (script_line->getType() == CDOC_COLON_COMMAND &&
          colon_command->getCommand() == "epsc")
        CDocScriptEndPCtrl();
    }

    delete script_line;
  }

  /* Close File */

  fclose(cdoc_input_fp);

  cdoc_input_fp_list.clear();

  return error;
}

// Process a Script Comment Line on the First Pass through the input IBM Script File.
//
// Looks for CDoc specific commands embedded in the comment and processes them accordingly.
static void
CDocProcessCommentLinePass1(const std::string &comment)
{
  if (comment == "")
    return;

  char *p1 = const_cast<char *>(comment.c_str());

  CStrUtil::skipSpace(&p1);

  /* Process CDoc Script Options */

  if (strncmp(p1, "cdocopts ", 9) == 0) {
    /* Save Initial State of Page Numbering and Spell Check */

    int save_page_numbering = cdoc_page_numbering;
    int save_spell_check    = cdoc_spell_check;

    /**********/

    /* Parse Options String */

    std::vector<std::string> words;

    CDocStringToWords(&p1[9], words);

    uint no_words = words.size();

    if (no_words > 0) {
      int          no_words1 = no_words;
      const char **words1    = new const char * [no_words1];

      for (int i = 0; i < no_words1; i++)
        words1[i] = words[i].c_str();

      CDocScriptProcessOptions(words1, &no_words1);

      if (no_words1 != 0)
        CDocScriptWarning("Some Options have not been Processed");

      delete [] words1;
    }

    /**********/

    /* Initialise Page Numbering and Spell Checking if they have
       been turned on inside the document */

    if (! save_page_numbering && cdoc_page_numbering)
      CDocScriptPageNoInit();

    if (! save_spell_check && cdoc_spell_check) {
      cdoc_spell_check = CSpellInit();

      if (cdoc_spell_check)
        cdoc_spell_active = true;
    }
  }
}

// Process a parsed Dot Command on the First Pass through the input IBM Script File.
static void
CDocProcessDotCommandPass1(CDDotCommand *dot_command)
{
  int  i;
  int  j;
  int  level;
  /* If not Processing then ignore command */

  if (! CDocScriptProcessing())
    return;

  /* Process a Header Command (for Table of Contents) */

  if      (dot_command->getCommand() == "h0" || dot_command->getCommand() == "h1" ||
           dot_command->getCommand() == "h2" || dot_command->getCommand() == "h3" ||
           dot_command->getCommand() == "h4" || dot_command->getCommand() == "h5" ||
           dot_command->getCommand() == "h6") {
    /* Create the Header String depending on Header Level and
       position in the Document */

    std::string temp_string;

    /* Calculate the Header Level */

    level = (dot_command->getCommand().c_str()[1] - '0') % CDOC_MAX_HEADERS;

    /* If header of this level is numbered then precede header
       with its header number */

    if (header_control[level].number_header) {
      int  header_no;

      CDocScriptStartHeader(level);

      /* Create Header Number String */

      for (j = 1; j <= level; j++) {
        header_no = CDocScriptGetHeader(j);

        /* If in Appendix then first Level is a letter otherwise just use a number */

        std::string temp_string1;

        if (j == 1) {
          if (appendix_depth > 0)
            CStrUtil::sprintf(temp_string1, "Appendix %c",
                              cdoc_a_to_z[(header_no - appendix_depth - 1) % 26]);
          else
            temp_string1 = CStrUtil::toString(header_no);
        }

        /* All other levels are numbers */

        else
          temp_string1 = "." + CStrUtil::toString(header_no);

        temp_string += temp_string1;
      }

      /* Add '.0' for First Level Normal Headers and '.'
         for First Level Appendix Headers */

      if (level == 1) {
        if (appendix_depth == 0)
          temp_string += ".0";
        else
          temp_string += ".";
      }

      /* Add blanks to separate number and header string */

      if (level > 0)
        temp_string += "  ";
    }

    /* If heading requires a line before it in table of contents
       then add it */

    if (header_control[level].skip_before_toc_entry)
      CDocScriptAddTOCSkip();

    /* Add any preceding blanks */

    std::string temp_string1;

    temp_string1 = temp_string;

    temp_string = "";

    for (i = 0; i < header_control[level].toc_indentation; i++)
      temp_string += " ";

    /* If heading causes a section break then make it bold */

    if (header_control[level].section_breaks)
      temp_string += CDocStartBold();

    temp_string += temp_string1;

    /* Use Header Number if Global Switch is On */

    if (cdoc_number_headers)
      temp_string1 = temp_string + dot_command->getText();
    else
      temp_string1 = dot_command->getText();

    if (header_control[level].section_breaks)
      temp_string1 + CDocEndBold();

    /* Add header string as a content in the table of contents if required */

    if (header_control[level].toc_entry)
      CDocScriptAddTOCEntry(temp_string1);
  }

  /* Process a Heading Definition */

  else if (dot_command->getCommand() == "dh") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words >= 2) {
      level = CStrUtil::toInteger(words[0]) % CDOC_MAX_HEADERS;

      CStrUtil::toLower(words[1]);

      std::vector<std::string>::iterator p = words.begin();

      std::vector<std::string> words1(++p, words.end());

      CDocExtractParameters(words1, dh_parameter_data,
                            reinterpret_cast<char *>(&header_control[level]));
    }
    else if (no_words == 1) {
      /* Set Header Control from Default Header Control */

      level = CStrUtil::toInteger(words[0]) % CDOC_MAX_HEADERS;

      memcpy(&header_control[level], &default_header_control[level],
             sizeof(CDHeaderControl));
    }
    else {
      /* Set All Header Controls from Default Header Control */

      memcpy(header_control, default_header_control,
             CDOC_MAX_HEADERS*sizeof(CDHeaderControl));
    }
  }

  /* Macros */

  else if (dot_command->getCommand() == "dm") {
    CDScriptLine *script_line;

    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0)
      CDocScriptError("End Macro when not in Macro");
    else if (no_words == 1 || (no_words == 2 && CStrUtil::casecmp(words[1], "on") == 0)) {
      /* Set to Null State (no processing of input test) */

      int save_no_uscore  = cdoc_no_uscore;
      int save_no_cap     = cdoc_no_cap;
      int save_no_centred = cdoc_no_centred;
      int save_formatting = cdoc_formatting;

      cdoc_no_uscore  = 0;
      cdoc_no_cap     = 0;
      cdoc_no_centred = 0;
      cdoc_formatting = false;

      CDMacro *macro = CDocScriptCreateMacro(words[0]);

      while ((script_line = CDocScriptGetNextLine()) != NULL) {
        if (script_line->getType() == CDOC_DOT_COMMAND) {
          dot_command = script_line->getDotCommand();

          if (dot_command->getCommand() == "dm") {
            std::vector<std::string> words1;

            CDocStringToWords(dot_command->getText(), words1);

            uint no_words1 = words1.size();

            if (no_words1 != 1 || CStrUtil::casecmp(words1[0], "off") != 0)
              CDocScriptError("Nested Macro Definitions not allowed");

            delete script_line;

            break;
          }

          CDocScriptAddMacroLine(macro, script_line);
        }
        else
          CDocScriptAddMacroLine(macro, script_line);
      }

      /* Restore Original State */

      cdoc_no_uscore  = save_no_uscore;
      cdoc_no_cap     = save_no_cap;
      cdoc_no_centred = save_no_centred;
      cdoc_formatting = save_formatting;
    }
  }

  /* Process Label */

  else if (dot_command->getCommand() == "..")
    CDocScriptAddLabel(dot_command->getText());

  /* Symbols */

  else if (dot_command->getCommand() == "se")
    CDocScriptParseSymbol(dot_command->getText());

  else if (dot_command->getCommand() == "dv")
    CDocScriptParseDefineVariable(dot_command->getText());

  /* Translations */

  else if (dot_command->getCommand() == "ti")
    CDocScriptParseTranslation(dot_command->getText());

  /* Define Character */

  else if (dot_command->getCommand() == "dc") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if (no_words > 0) {
      CStrUtil::toLower(words[0]);

      if (words[0] == "cont") {
        if (no_words == 2) {
          if      (CStrUtil::casecmp(words[1], "off") == 0)
            cdoc_continuation_char = int('\0');
          else if (words[1].size() == 1)
            cdoc_continuation_char = int(words[1][0]);
          else
            CDocScriptError("Invalid Continuation Character '%s'", words[1].c_str());
        }
        else
          CDocScriptError("Invalid Continuation Character Definition");
      }
      else
        CDocScriptError("Define Character not Supported for '%s'", words[0].c_str());
    }
    else
      CDocScriptError("Invalid Define Character Command");
  }

  /* External Datasets */

  else if (dot_command->getCommand() == "dd") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if (no_words != 2) {
      CDocScriptError("Invalid CDoc Define Dataset Command .%s %s",
                      dot_command->getCommand().c_str(), dot_command->getText().c_str());

      return;
    }

    CDocScriptDefineDataset(words[0], words[1]);
  }
  else if (dot_command->getCommand() == "im") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if (no_words != 1) {
      CDocScriptError("Invalid CDoc Imbed Dataset Command .%s %s",
                      dot_command->getCommand().c_str(), dot_command->getText().c_str());

      return;
    }

    FILE *fp = CDocScriptImbedDataset(words[0]);

    if (fp != NULL) {
      cdoc_input_fp_list.push_back(cdoc_input_fp);

      cdoc_input_fp = fp;
    }
    else
      CDocScriptError("Failed to Open Imbed File '%s'", words[0].c_str());
  }
}

// Process a parsed Colon Command on the First Pass through the input IBM Script File.
static void
CDocProcessColonCommandPass1(CDColonCommand *colon_command)
{
  static int list_no = 0;
  static int list_type[MAX_LISTS];
  static int list_depth[MAX_LISTS];

  int  i;
  int  j;
  int  level;

  /* Process Specific Control Blocks */

  if      (colon_command->getCommand() == "psc")
    CDocScriptStartPCtrl(colon_command);
  else if (colon_command->getCommand() == "epsc")
    CDocScriptEndPCtrl();

  /* Process a Header Command */

  else if (colon_command->getCommand()[0] == 'h' &&
           isdigit(colon_command->getCommand()[1])) {
    CDHeading heading;

    heading.ident  = NULL;
    heading.stitle = NULL;

    /* Calculate the Header Level */

    level = (colon_command->getCommand()[1] - '0') % CDOC_MAX_HEADERS;

    /* Get Parameter/Value pairs */

    if (level == 0 || level == 1)
      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 h01_parameter_data,
                                 reinterpret_cast<char *>(&heading));
    else
      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 h26_parameter_data,
                                 reinterpret_cast<char *>(&heading));

    /* If we have a reference identifier then add it to the
       reference list */

    if (heading.ident != NULL)
      CDocScriptAddReference(HEADER_REF, heading.ident, colon_command->getText(), "");

    /* Create the Header String depending on Header Level and
       position in the Document */

    std::string temp_string;

    /* If header of this level is numbered and in the body
       or appendix section then precede header with its header number */

    if (header_control[level].number_header &&
        (cdoc_document.part == CDOC_BODY_PART || cdoc_document.part == CDOC_APPENDIX_PART)) {
      int  header_no;

      CDocScriptStartHeader(level);

      /* Create Header Number String */

      for (j = 1; j <= level; j++) {
        header_no = CDocScriptGetHeader(j);

        /* If in Appendix then first Level is a letter
           otherwise just use a number */

        std::string temp_string1;

        if (j == 1) {
          if (appendix_depth > 0)
            CStrUtil::sprintf(temp_string1, "Appendix %c",
                              cdoc_a_to_z[(header_no - appendix_depth - 1) % 26]);
          else
            temp_string1 = CStrUtil::toString(header_no);
        }

        /* All other levels are numbers */

        else
          temp_string1 = "." + CStrUtil::toString(header_no);

        temp_string += temp_string1;
      }

      /* Add '.0' for First Level Normal Headers and '.'
         for First Level Appendix Headers */

      if (level == 1) {
        if (appendix_depth == 0)
          temp_string += ".0";
        else
          temp_string += ".";
      }

      /* Add blanks to separate number and header string */

      if (level > 0)
        temp_string += "  ";
    }

    /* If heading requires a line before it in table of contents
       then add it */

    if (header_control[level].skip_before_toc_entry)
      CDocScriptAddTOCSkip();

    /* Add any preceding blanks */

    std::string temp_string1;

    temp_string1 = temp_string;

    temp_string = "";

    for (i = 0; i < header_control[level].toc_indentation; i++)
      temp_string += " ";

    /* If heading causes a section break then make it bold */

    if (header_control[level].section_breaks)
      temp_string += CDocStartBold();

    temp_string += temp_string1;

    /* Use Header Number if Global Switch is On */

    if (cdoc_number_headers)
      temp_string1 = temp_string + colon_command->getText();
    else
      temp_string1 = colon_command->getText();

    if (header_control[level].section_breaks)
      temp_string1 += CDocEndBold();

    /* Add header string as a content in the table of contents if required */

    if (header_control[level].toc_entry)
      CDocScriptAddTOCEntry(temp_string1);
  }

  /* Footnotes */

  else if (colon_command->getCommand() == "fn") {
    /* Get Footnote Parameter Values */

    CDFootnoteData footnote_data;

    footnote_data.ident = NULL;

    CDocExtractParameterValues(colon_command->getParameters(),
                               colon_command->getValues(),
                               fn_parameter_data,
                               reinterpret_cast<char *>(&footnote_data));

    CDFootnote footnote;

    footnote.ident  = (footnote_data.ident ? std::string(footnote_data.ident) : "");
    footnote.number = ++current_footnote_number;

    std::string temp_string;

    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
      CStrUtil::sprintf(temp_string, "{%d}", footnote.number);
    else
      CStrUtil::sprintf(temp_string, "%s{%d}%s",
                        CDocStartBold(), footnote.number, CDocEndBold());

    if (footnote.ident != "")
      CDocScriptAddReference(FOOTNOTE_REF, footnote.ident, temp_string, "");
  }

  /* Set the list type and current item number in this list
     for the start of an ordered, simple or unordered list
     and increment the list number */

  else if (colon_command->getCommand() == "ol") {
    list_type [list_no] = ORDERED_LIST;
    list_depth[list_no] = 0;

    list_no++;
  }
  else if (colon_command->getCommand() == "sl") {
    list_type [list_no] = SIMPLE_LIST;
    list_depth[list_no] = 0;

    list_no++;
  }
  else if (colon_command->getCommand() == "ul") {
    list_type [list_no] = UNORDERED_LIST;
    list_depth[list_no] = 0;

    list_no++;
  }

  /* Decrement the list number for the end of an ordered,
     simple or unordered list */

  else if (colon_command->getCommand() == "eol") {
    if (list_no > 0)
      list_no--;
  }
  else if (colon_command->getCommand() == "esl") {
    if (list_no > 0)
      list_no--;
  }
  else if (colon_command->getCommand() == "eul") {
    if (list_no > 0)
      list_no--;
  }

  /* Increment the item number of the current list and add a
     reference to this list item if a reference identifier is present */

  else if (colon_command->getCommand() == "li") {
    CDListItem  list_item;

    if (list_no > 0)
      list_depth[list_no - 1]++;

    /* Get Parameter/Value pairs */

    list_item.ident = NULL;

    CDocExtractParameterValues(colon_command->getParameters(),
                               colon_command->getValues(),
                               li_parameter_data,
                               reinterpret_cast<char *>(&list_item));

    /* If we have a reference identifier and an ordered list then
       use the list item number (created from the depths of each
       item in each ordered list currently active), otherwise use
       a NULL reference to indicate we have no str to use to
       reference this item */

    if (list_item.ident != NULL) {
      std::string temp_string;

      if (list_no > 0) {
        if (list_type[list_no - 1] == ORDERED_LIST) {
          for (j = 0; j < list_no; j++) {
            if (list_type[j] == ORDERED_LIST) {
              char temp_string1[33];

              sprintf(temp_string1, "%d.", list_depth[j]);

              temp_string += temp_string1;
            }
          }
        }
      }

      /* Add the reference to the list */

      CDocScriptAddReference(LIST_ITEM_REF, list_item.ident, temp_string,
                             colon_command->getText());
    }
  }

  /* If we have a Figure List then add it to the Table of
     Contents for Stairs Document */

  else if (colon_command->getCommand() == "figlist") {
    if (cdoc_document.type == CDOC_STAIRS_DOCUMENT) {
      CDocScriptStartHeader(1);

      std::string temp_string;

      CStrUtil::sprintf(temp_string,
                        "%s%d.0  List of Illustrations%s",
                        CDocStartBold(), CDocScriptGetHeader(1), CDocEndBold());

      CDocScriptAddTOCSkip();

      CDocScriptAddTOCEntry(temp_string);
    }
  }

  /* If we have a figure then save its details in a structure
     added to the figure list (for later output in the figure
     list in Pass 2). */

  else if (colon_command->getCommand() == "fig") {
    /* Create the figure details structure to store
       the details in */

    current_figure = CDocScriptCreateFigure();

    CDFigureData figure_data;

    figure_data.depth  = 0;
    figure_data.frame  = NULL;
    figure_data.ident  = NULL;
    figure_data.indent = 0;
    figure_data.place  = 0;
    figure_data.width  = 0;

    /* Process the Parameter/Value pairs to fill in the
       rest of the data in the figure details structure */

    CDocExtractParameterValues(colon_command->getParameters(),
                               colon_command->getValues(),
                               fig_parameter_data,
                               reinterpret_cast<char *>(&figure_data));

    current_figure->depth  = figure_data.depth;
    current_figure->frame  = figure_data.frame;
    current_figure->ident  = (figure_data.ident ? std::string(figure_data.ident) : "");
    current_figure->indent = figure_data.indent;
    current_figure->place  = figure_data.place;
    current_figure->width  = figure_data.width;
  }

  /* If we have a figure caption then set this value in the
     figure details structure (if we have one) and if there
     if a reference identification for this figure use the
     caption as the reference str */

  else if (colon_command->getCommand() == "figcap") {
    if (current_figure != NULL) {
      current_figure->caption = colon_command->getText();

      if (current_figure->count == -1)
        current_figure->count = ++figure_number;

      if (current_figure->ident != "") {
        /* Create the reference string */

        std::string temp_string;

        CStrUtil::sprintf(temp_string, "Figure %d.", current_figure->count);

        /* Create the reference structure */

        CDocScriptAddReference(FIGURE_REF, current_figure->ident, temp_string, "");
      }
    }
  }

  /* If we have a figure description then set this value in the
     figure details structure (if we have one). */

  else if (colon_command->getCommand() == "figdesc") {
    if (current_figure != NULL) {
      if (current_figure->count == -1)
        current_figure->count = ++figure_number;

      current_figure->description = colon_command->getText();
    }
  }

  /* If we have reached the end of the figure deactivate it */

  else if (colon_command->getCommand() == "efig") {
    current_figure = NULL;
  }

  /* If we have a Table List then add it to the Table of
     Contents for Stairs Document */

  else if (colon_command->getCommand() == "tlist") {
    if (cdoc_document.type == CDOC_STAIRS_DOCUMENT) {
      CDocScriptStartHeader(1);

      std::string temp_string;

      CStrUtil::sprintf(temp_string,
                        "%s%d.0  List of Tables%s",
                        CDocStartBold(), CDocScriptGetHeader(1), CDocEndBold());

      CDocScriptAddTOCSkip();

      CDocScriptAddTOCEntry(temp_string);
    }
  }

  /* If we have a table then save its details in a structure
     added to the table list (for later output in the table
     list in Pass 2). */

  else if (colon_command->getCommand() == "table") {
    /* Create the table details structure to store the details in */

    current_table = CDocScriptCreateTable();

    /* Process the Parameter/Value pairs to fill in the
       rest of the data in the table details structure */

    CDTableData table_data;

    table_data.column = 0;
    table_data.id     = NULL;
    table_data.page   = 0;
    table_data.refid  = NULL;
    table_data.split  = 0;
    table_data.rotate = 0;
    table_data.width  = 0;

    CDocExtractParameterValues(colon_command->getParameters(),
                               colon_command->getValues(),
                               table_parameter_data,
                               reinterpret_cast<char *>(&table_data));

    current_table->refid = (table_data.refid ? table_data.refid : "");
    current_table->id    = (table_data.id    ? table_data.id    : "");

  //current_table->column = table_data.column;
  //current_table->page   = table_data.page;
    current_table->split  = table_data.split;
    current_table->rotate = table_data.rotate;
    current_table->width  = table_data.width;

    /* Check Parameter/Value pairs */

    if (current_table->rotate !=  0   && current_table->rotate !=  90  &&
        current_table->rotate !=  180 && current_table->rotate !=  270 &&
        current_table->rotate != -90  && current_table->rotate != -180 &&
        current_table->rotate != -270) {
      CDocScriptWarning("Invalid Value for Table Rotation %d", current_table->rotate);

      current_table->rotate = 0;
    }
  }

  /* If we have a table caption then set this value in the
     table details structure (if we have one) and if there
     if a reference identification for this table use the
     caption as the reference str */

  else if (colon_command->getCommand() == "tcap") {
    if (current_table != NULL) {
      current_table->caption = CStrUtil::strdup(colon_command->getText().c_str());

      if (current_table->count == -1)
        current_table->count = ++table_number;

      if (current_table->id != "") {
        /* Create the reference string */

        std::string temp_string;

        CStrUtil::sprintf(temp_string, "Table %d.", current_table->count);

        /* Create the reference structure */

        CDocScriptAddReference(TABLE_REF, current_table->id, temp_string, "");
      }
    }
  }

  /* If we have a table description then set this value in the
     table details structure (if we have one). */

  else if (colon_command->getCommand() == "tdesc") {
    if (current_table != NULL) {
      if (current_table->count == -1)
        current_table->count = ++table_number;

      current_table->description = CStrUtil::strdup(colon_command->getText().c_str());
    }
  }

  /* If we have reached the end of the table deactivate it */

  else if (colon_command->getCommand() == "etable") {
    current_table = NULL;
  }

  /* Index */

  else if (colon_command->getCommand() == "index") {
    if (cdoc_index) {
      CDocScriptAddTOCSkip();

      std::string temp_string;

      CStrUtil::sprintf(temp_string, "%sIndex%s", CDocStartBold(), CDocEndBold());

      CDocScriptAddTOCEntry(temp_string);
    }
  }

  else if (colon_command->getCommand()[0] == 'i' &&
           (colon_command->getCommand()[1] == '1' ||
            colon_command->getCommand()[1] == '2' ||
            colon_command->getCommand()[1] == '3')) {
    if (cdoc_index) {
      CDIndexEntry index_entry;

      index_entry.ident = NULL;
      index_entry.page  = NULL;
      index_entry.refid = NULL;

      if (colon_command->getCommand()[1] == '1')
        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   i1_parameter_data,
                                   reinterpret_cast<char *>(&index_entry));
      else
        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   i23_parameter_data,
                                   reinterpret_cast<char *>(&index_entry));

      std::string ident = (index_entry.ident ? std::string(index_entry.ident) : "");
      std::string refid = (index_entry.refid ? std::string(index_entry.refid) : "");

      if      (colon_command->getCommand()[1] == '1')
        CDocScriptAddIndex1(colon_command->getText(), ident, refid);
      else if (colon_command->getCommand()[1] == '2')
        CDocScriptAddIndex2(colon_command->getText(), ident, refid);
      else
        CDocScriptAddIndex3(colon_command->getText(), ident, refid);
    }
  }

  else if (colon_command->getCommand()[0] == 'i' &&
           colon_command->getCommand()[1] == 'h' &&
           (colon_command->getCommand()[2] == '1' ||
            colon_command->getCommand()[2] == '2' ||
            colon_command->getCommand()[2] == '3')) {
    if (cdoc_index) {
      CDIndexEntryHeading  index_entry_heading;

      index_entry_heading.ident = NULL;
      index_entry_heading.print = NULL;
      index_entry_heading.see   = NULL;
      index_entry_heading.seeid = NULL;

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 ih_parameter_data,
                                 reinterpret_cast<char *>(&index_entry_heading));

      std::string ident = (index_entry_heading.ident ? std::string(index_entry_heading.ident) : "");
      std::string print = (index_entry_heading.print ? std::string(index_entry_heading.print) : "");
      std::string see   = (index_entry_heading.see   ? std::string(index_entry_heading.see  ) : "");
      std::string seeid = (index_entry_heading.seeid ? std::string(index_entry_heading.seeid) : "");

      if      (colon_command->getCommand()[2] == '1')
        CDocScriptAddIndexHeader1(colon_command->getText(), ident, print, see, seeid);
      else if (colon_command->getCommand()[2] == '2')
        CDocScriptAddIndexHeader2(colon_command->getText(), ident, print, see, seeid);
      else
        CDocScriptAddIndexHeader3(colon_command->getText(), ident, print, see, seeid);
    }
  }

  /* If we are in the appendix then record the current header
     depth as the appendix header start depth */

  else if (colon_command->getCommand() == "appendix") {
    cdoc_document.part = CDOC_APPENDIX_PART;

    appendix_depth = CDocScriptGetHeader(1);
  }

  /* If we are out of the appendix then reset the appendix
     header start depth to unset */

  else if (colon_command->getCommand() == "eappendix") {
    cdoc_document.part = CDOC_NO_PART;

    appendix_depth = 0;
  }

  /* If we are starting a new document section (effectively
     ending the appendix then reset the appendix header
     start depth to unset */

  else if (colon_command->getCommand() == "frontm") {
    cdoc_document.part = CDOC_FRONT_MATTER_PART;

    appendix_depth = 0;
  }
  else if (colon_command->getCommand() == "body") {
    cdoc_document.part = CDOC_BODY_PART;

    appendix_depth = 0;
  }
  else if (colon_command->getCommand() == "backm") {
    cdoc_document.part = CDOC_BACK_MATTER_PART;

    appendix_depth = 0;
  }

  /* If we are setting the document type then save it */

  else if (colon_command->getCommand() == "gdoc")
    cdoc_document.type = CDOC_GENERAL_DOCUMENT;
  else if (colon_command->getCommand() == "sdoc")
    cdoc_document.type = CDOC_STAIRS_DOCUMENT;
  else if (colon_command->getCommand() == "memo")
    cdoc_document.type = CDOC_MEMO_DOCUMENT;
}

// Initialises CDoc ready for Pass 2 Processing of the Script File.
// This routines restores variables corrupted by Pass 1.
static void
CDocScriptInitPass2()
{
  /* Set Pass Number */

  cdoc_pass_no = 2;

  /* Initialise Current Input Line Number */

  cdoc_input_line_no = 0;

  /* Initialise Page/Line and Character Counts */

  cdoc_page_no            = 1;
  cdoc_page_no_offset     = 0;
  cdoc_line_no            = 0;
  cdoc_char_no            = 0;
  cdoc_line_fiddle        = 0;
  cdoc_page_header_output = false;

  /* Initialise Processing Control */

  CDocScriptInitPCtrl();

  /* Reset Dot Command Control Variables */

  cdoc_no_centred = 0;
  cdoc_no_cap     = 0;
  cdoc_no_uscore  = 0;

  /* Re-Initialise Current Header Level for each of the Levels */

  CDocScriptInitHeaders();

  /* Initialise Footnote Variables */

  current_footnote = NULL;

  /* Re-Initialise Current Figure and Table Details */

  current_figure = NULL;
  current_table  = NULL;
  current_cell   = NULL;

  figure_number  = 0;
  table_number   = 0;

  /* Set Header Control from Default Header Control */

  memcpy(header_control, default_header_control,
         CDOC_MAX_HEADERS*sizeof(CDHeaderControl));

  /* Set Continuation Character */

  cdoc_continuation_char = int('\0');

  /* Initialise Translations */

  CDocScriptInitTranslations();
}

// Perform Pass 2 of the processing of the Script File.
//
// This pass actually processes and outputs the text built from the Script
// commands using the information collected in Pass 1.
static void
CDocProcessFilePass2(const std::string &filename)
{
  /* Open File (fail if can't be opened) */

  cdoc_input_fp = fopen(filename.c_str(), "r");

  if (cdoc_input_fp == NULL) {
    cdoc_input_fp = stdin;
    return;
  }

  /* Output Initialisation Code required by the current output format */

  CDocInitialiseOutputPass2();

  /* Read and process each line from the dataset */

  CDScriptLine *script_line = NULL;

  while ((script_line = CDocScriptGetNextLine()) != NULL) {
    CDDotCommand   *dot_command   = NULL;
    CDColonCommand *colon_command = NULL;

    if      (script_line->getType() == CDOC_DOT_COMMAND)
      dot_command   = script_line->getDotCommand();
    else if (script_line->getType() == CDOC_COLON_COMMAND)
      colon_command = script_line->getColonCommand();

    /* Output Line if Debug */

    if (CDocInst->getDebug() & CDOC_DEBUG_PASS_2) {
      if (! CDocScriptProcessing())
        printf("# ");

      script_line->print();
    }

    if (CDocScriptProcessing()) {
      /* Check for Comment Line (not a Normal Dot Command)
         in which we hide CDoc Specific Commands */

      if (script_line->getType() == CDOC_DOT_COMMAND &&
          dot_command->getCommand() == "*") {
        CDocProcessCommentLinePass2(dot_command->getText());

        delete script_line;

        continue;
      }

      /* Replace embedded Colon Commands with the required Text */

      CDocReplaceEmbeddedColonCommands(script_line);

      /* Replace embedded Symbols in Text */

      if      (script_line->getType() == CDOC_DOT_COMMAND) {
        std::string text = CDocScriptReplaceSymbolsInString(dot_command->getText());

        dot_command->setText(text);
      }
      else if (script_line->getType() == CDOC_COLON_COMMAND) {
        std::string text = CDocScriptReplaceSymbolsInString(colon_command->getText());

        colon_command->setText(text);
      }
      else
        script_line->setData(script_line->getType(),
          CDocScriptReplaceSymbolsInString(script_line->getData()));

      /* Process the converted line */

      CDocProcessPass2Line(script_line);
    }
    else {
      if (script_line->getType() == CDOC_COLON_COMMAND &&
          colon_command->getCommand() == "epsc")
        CDocScriptEndPCtrl();
    }

    /* Delete the Script Line */

    delete script_line;
  }

  /* Make Sure Script Block Commands are Terminated */

  if      (cdoc_document.part == CDOC_FRONT_SHEET_PART) {
    CDocScriptWarning("Unterminated Front Sheet");

    CDocScriptOutputFrontSheet();
  }
  else if (cdoc_document.part == CDOC_MEMO_HEADER_PART) {
    CDocScriptWarning("Unterminated Memo Header");

    CDocScriptOutputMemoHeader();
  }
  else if (cdoc_document.part == CDOC_AMENDMENTS_PART) {
    CDocScriptWarning("Unterminated Amendments");

    CDocScriptTermAmendments();
  }

  if (current_table != NULL) {
    CDocScriptWarning("Unterminated Table");

    CDocScriptOutputTable(current_table);

    if (current_table->id != "")
      CDocScriptSetReferencePageNumber(TABLE_REF, current_table->id);

    CDocScriptSetTablePage(current_table);
  }

  if (current_figure != NULL) {
    CDocScriptWarning("Unterminated Figure");

    CDocScriptFigureEnd(current_figure);
  }

  if (current_footnote != NULL) {
    CDocScriptWarning("Unterminated Footnote");

    CDocScriptEndFootnote(current_footnote);

    if (current_footnote->ident != "")
      CDocScriptSetReferencePageNumber(FOOTNOTE_REF, current_footnote->ident);
  }

  if (long_quotation_depth != 0)
    CDocScriptWarning("Unterminated Long Quotation");

  /* Ensure any unfinished paragraphs are output as we have reached
     the end of the file */

  if (cdoc_in_paragraph)
    CDocScriptOutputParagraph();

  /* Output any Footnotes */

  CDocScriptOutputFootnotes(true);

  /* Output any Top Figures */

  while (CDocScriptIsTopFigure()) {
    CDocScriptStartNewPage();

    CDocScriptOutputTopFigure();
  }

  /* Close the File */

  fclose(cdoc_input_fp);

  cdoc_input_fp_list.clear();

  /* Free off allocated Paragraph Buffer */

  cdoc_paragraph->setText("");

  /* If we have any unterminated ordered, simple or unordered
     lists then output a warning message */

  if (CDocScriptIsCurrentList()) {
    if      (CDocScriptIsCurrentListType(ORDERED_LIST))
      CDocScriptWarning("Unterminated Ordered List");
    else if (CDocScriptIsCurrentListType(SIMPLE_LIST))
      CDocScriptWarning("Unterminated Simple List");
    else if (CDocScriptIsCurrentListType(UNORDERED_LIST))
      CDocScriptWarning("Unterminated Unordered List");
    else if (CDocScriptIsCurrentListType(GLOSSARY_LIST))
      CDocScriptWarning("Unterminated Glossary List");
    else
      CDocScriptWarning("Unterminated List");
  }

  /* If we have any unterminated definition lists then
     output a warning message */

  if (cdoc_definition_list != NULL)
    CDocScriptWarning("Unterminated Definition List");
}

// Output setup commands required to correctly initialise the output for the
// specified format.
static void
CDocInitialiseOutputPass2()
{
  /* Initialise Troff */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    /* Set Line Length, Page Length and Point Size */

    CDocScriptWriteCommand(".ll 5.8i\n");
    CDocScriptWriteCommand(".pl 10.0i\n");
    CDocScriptWriteCommand(".ps 11\n");

    /* Define Underscore Macro */

    CDocScriptWriteCommand(".de US\n");
    CDocScriptWriteCommand("\\\\$1\\l\'|0\\(ul\'\n");
    CDocScriptWriteCommand("..\n");

    /* Define Bold Macro */

    CDocScriptWriteCommand(".de BD\n");
    CDocScriptWriteCommand("\\fB\\\\$1\\fP\n");
    CDocScriptWriteCommand("..\n");

    /* Define Bold Underscore Macro */

    CDocScriptWriteCommand(".de BU\n");
    CDocScriptWriteCommand("\\fB\\\\$1\\l\'|0\\(ul\'\\fP\n");
    CDocScriptWriteCommand("..\n");

    /* Define Right Justify Macro */

    CDocScriptWriteCommand(".de RN\n");
    CDocScriptWriteCommand(".ti 5.8i\n");
    CDocScriptWriteCommand("\\h\'-(\\w\'\\\\$1\'u)'\\\\$1\n");
    CDocScriptWriteCommand("..\n");

    /* Define Right Justify Underscore Macro */

    CDocScriptWriteCommand(".de RU\n");
    CDocScriptWriteCommand(".ti 5.8i\n");
    CDocScriptWriteCommand(
      "\\h\'-(\\w\'\\\\$1\'u)\'\\\\$1\\l\'-(\\w\'\\\\$1\'u)\\(ul\'\n");
    CDocScriptWriteCommand("..\n");

    /* Define Right Justify Bold Macro */

    CDocScriptWriteCommand(".de RB\n");
    CDocScriptWriteCommand(".ti 5.8i\n");
    CDocScriptWriteCommand("\\fB\\h\'-(\\w\'\\\\$1\'u)\'\\\\$1\\fP\n");
    CDocScriptWriteCommand("..\n");

    /* Define Right Justify Bold Underscore Macro */

    CDocScriptWriteCommand(".de RE\n");
    CDocScriptWriteCommand(".ti 5.8i\n");
    CDocScriptWriteCommand(
  "\\fB\\h\'-(\\w\'\\\\$1\'u)\'\\\\$1\\l\'-(\\w\'\\\\$1\'u)\\(ul\'\\fP\n");
    CDocScriptWriteCommand("..\n");
    CDocScriptWriteCommand("\n");
  }

  /* Initialise CDoc */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptWriteCommand(CDOC_LEFT_MARGIN_TMPL , cdoc_left_margin   );
    CDocScriptWriteCommand(CDOC_RIGHT_MARGIN_TMPL, cdoc_right_margin  );
    CDocScriptWriteCommand(CDOC_PAGE_LENGTH_TMPL , cdoc_lines_per_page);
    CDocScriptWriteCommand(CDOC_DEF_FONT_TMPL    , "cdocsym",
                           "-*-symbol-*-*-*-*-13-*-*-*-*-*-*-*");
  }

  /* Initialise HTML */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<html>\n");
    CDocScriptWriteCommand("<head>\n");
    CDocScriptWriteCommand("<title>%s</title>\n", "HTML Document");
    CDocScriptWriteCommand("</head>\n");
    CDocScriptWriteCommand("<body>\n");
  }

  /* Initialise Interleaf */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    for (uint i = 0; i < sizeof(ileaf_header)/sizeof(char *); i++)
      CDocScriptWriteCommand("%s\n", ileaf_header[i]);
  }
}

// Process a Script Comment Line on the Second Pass through the input IBM
// Script File.
//
// Looks for CDoc specific commands embedded in the comment and processes
// them accordingly.
static void
CDocProcessCommentLinePass2(const std::string &comment)
{
  char *p1 = const_cast<char *>(comment.c_str());

  if (*p1 == '\0')
    return;

  CStrUtil::skipSpace(&p1);

  /* Check for CDoc Control Word */

  if (strncmp(p1, "cdoc", 4) == 0) {
    /* Flush Paragraph to ensure any CDoc Controlled
       output appears in the correct position */

    if (cdoc_in_paragraph)
      CDocScriptOutputParagraph();

    /* Output CDoc specific control line */

    if      (strncmp(&p1[4], "cdoc ", 5) == 0) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
        CDocScriptWriteCommand("%s\n", &p1[9]);
    }

    /* Output Troff specific control line */

    else if (strncmp(&p1[4], "troff ", 6) == 0) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
        CDocScriptWriteCommand("%s\n", &p1[10]);
    }

    /* Output HTML specific control line */

    else if (strncmp(&p1[4], "html ", 5) == 0) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
        CDocScriptWriteCommand("%s\n", &p1[9]);
    }

    /* Output Interleaf specific control line */

    else if (strncmp(&p1[4], "ileaf ", 6) == 0) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
        CDocScriptWriteCommand("%s\n", &p1[9]);
    }

    /* Output Raw with Control Codes specific control line */

    else if (strncmp(&p1[4], "rawcc ", 6) == 0) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC)
        CDocScriptWriteCommand("%s\n", &p1[10]);
    }

    /* Output Raw specific control line */

    else if (strncmp(&p1[4], "raw ", 4) == 0) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
        CDocScriptWriteCommand("%s\n", &p1[8]);
    }

    /* Process CDoc Script Options */

    else if (strncmp(&p1[4], "opts ", 5) == 0) {
      std::vector<std::string> words;

      CDocStringToWords(&p1[9], words);

      uint no_words = words.size();

      if (no_words > 0) {
        int           no_words1 = no_words;
        const char  **words1    = new const char * [no_words1];

        for (int i = 0; i < no_words1; i++)
          words1[i] = words[i].c_str();

        CDocScriptProcessOptions(words1, &no_words1);

        delete [] words1;
      }
    }

    /* Output Generic control line */

    else if (p1[4] != '\0' && isspace(p1[4])) {
      CDocScriptWriteCommand("%s\n", &p1[5]);
    }

    /* Any other Format is an Error */

    else
      CDocScriptError("Invalid CDoc Control Line %s", p1);
  }
}

// Process the Commands defined by the supplied Script Line structure.
static void
CDocProcessPass2Line(CDScriptLine *script_line)
{
  /* Process Dot Command */

  if      (script_line->getType() == CDOC_DOT_COMMAND)
    CDocProcessDotCommandPass2(script_line->getDotCommand());

  /* Process Colon Command */

  else if (script_line->getType() == CDOC_COLON_COMMAND)
    CDocProcessColonCommandPass2(script_line->getColonCommand());

  /* Process Ordinary Text */

  else if (script_line->getType() == CDOC_CENTRED_TEXT) {
    if (cdoc_in_paragraph) {
      if (cdoc_paragraph->getFormatted() ||
          cdoc_paragraph->getJustification() != CENTRE_JUSTIFICATION) {
        CDocScriptOutputParagraph();

        if (script_line->getData()[0] != '\0') {
          if (CDocScriptIsCurrentList())
            CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                                   false, CENTRE_JUSTIFICATION);
          else
            CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                                   false, CENTRE_JUSTIFICATION);
        }
      }
      else {
        CDocScriptAddStringToParagraph("\n");
        CDocScriptAddStringToParagraph(script_line->getData());
      }
    }
    else {
      if (CDocScriptIsCurrentList())
        CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                               false, CENTRE_JUSTIFICATION);
      else
        CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                               false, CENTRE_JUSTIFICATION);
    }
  }
  else if (script_line->getType() == CDOC_RAW_TEXT) {
    /* If in a paragraph then add as a new line */

    if (cdoc_in_paragraph) {
      if (cdoc_paragraph->getFormatted() ||
          cdoc_paragraph->getJustification() != LEFT_JUSTIFICATION) {
        CDocScriptOutputParagraph();

        if (script_line->getData()[0] != '\0') {
          if (CDocScriptIsCurrentList())
            CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                                   false, LEFT_JUSTIFICATION);
          else
            CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                                   false, LEFT_JUSTIFICATION);
        }
      }
      else {
        CDocScriptAddStringToParagraph("\n");
        CDocScriptAddStringToParagraph(script_line->getData());
      }
    }

    /* If not in a paragraph then start a new unformatted
       paragraph */

    else {
      if (CDocScriptIsCurrentList())
        CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                               false, LEFT_JUSTIFICATION);
      else
        CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                               false, LEFT_JUSTIFICATION);
    }
  }
  else if (cdoc_formatting) {
    /* If in a paragraph and formatting then add to end of
       paragraph, otherwise add as a new line */

    if (cdoc_in_paragraph) {
      if (! cdoc_paragraph->getFormatted() ||
          cdoc_paragraph->getJustification() != LEFT_JUSTIFICATION) {
        CDocScriptOutputParagraph();

        if (script_line->getData()[0] != '\0') {
          if (CDocScriptIsCurrentList())
            CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                                   true, LEFT_JUSTIFICATION);
          else
            CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                                   true, LEFT_JUSTIFICATION);
        }
      }
      else {
        int len = cdoc_paragraph->getTextLen();

        if (script_line->getData()[0] != '\0' &&
            len > 0 && cdoc_paragraph->getTextChar(len - 1) != '\n')
          CDocScriptAddStringToParagraph(" ");

        CDocScriptAddStringToParagraph(script_line->getData());
      }
    }

    /* If not in a paragraph then start a new normal paragraph */

    else {
      if (CDocScriptIsCurrentList())
        CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                               true, LEFT_JUSTIFICATION);
      else
        CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                               true, LEFT_JUSTIFICATION);
    }
  }
  else {
    /* If in a paragraph then add as a new line */

    if (cdoc_in_paragraph) {
      if (cdoc_paragraph->getFormatted() ||
          cdoc_paragraph->getJustification() != LEFT_JUSTIFICATION) {
        CDocScriptOutputParagraph();

        if (script_line->getData()[0] != '\0') {
          if (CDocScriptIsCurrentList())
            CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                                   false, LEFT_JUSTIFICATION);
          else
            CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                                   false, LEFT_JUSTIFICATION);
        }
      }
      else {
        CDocScriptAddStringToParagraph("\n");

        CDocScriptAddStringToParagraph(script_line->getData());
      }
    }

    /* If not in a paragraph then start a new unformatted
       paragraph */

    else {
      if (CDocScriptIsCurrentList())
        CDocScriptNewParagraph(script_line->getData(), SUB_LIST_PARAGRAPH1,
                               false, LEFT_JUSTIFICATION);
      else
        CDocScriptNewParagraph(script_line->getData(), NORMAL_PARAGRAPH,
                               false, LEFT_JUSTIFICATION);
    }
  }
}

// Process a Dot Command
static void
CDocProcessDotCommandPass2(CDDotCommand *dot_command)
{
  int  i;
  int  level;
  int  no_spaces;
  int  ignore_error;
  char temp_string[256];

  if (dot_command->getCommandData() == NULL ||
      dot_command->getCommandData()->flush) {
    if (cdoc_in_paragraph)
      CDocScriptOutputParagraph();
  }

  /* Fonts */

  if (dot_command->getCommand() == "bf") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if (no_words > 1 && words[no_words - 1] == "=") {
      no_words--;

      ignore_error = true;
    }
    else
      ignore_error = false;

    if (no_words == 0)
      CDocScriptBeginFont(NULL);
    else {
      uint i = 0;

      for (i = 0; i < no_words; i++)
        if (CDocScriptIsValidFont(words[i])) {
          CDocScriptBeginFont(words[i]);

          break;
        }

      if (! ignore_error && i == no_words)
        CDocScriptError("Font List is Invalid - '%s'", dot_command->getText().c_str());
    }
  }
  else if (dot_command->getCommand() == "df") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
      if (no_words != 2) {
        CDocScriptError("Invalid CDoc Define Font Command .%s %s",
                        dot_command->getCommand().c_str(), dot_command->getText().c_str());
        return;
      }

      CDocScriptDefineFont(words[0], words[1]);
    }
    else
      CDocScriptError("Define Font not supported for this Format");
  }
  else if (dot_command->getCommand() == "pf")
    CDocScriptPreviousFont();

  /* Macros */

  else if (dot_command->getCommand() == "dm") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0)
      CDocScriptWarning("Not in Macro");
    else if (no_words == 1 || (no_words == 2 && CStrUtil::casecmp(words[1], "on") == 0)) {
      CDScriptLine *script_line;

      while ((script_line = CDocScriptGetNextLine()) != NULL) {
        if (script_line->getType() == CDOC_DOT_COMMAND) {
          dot_command = script_line->getDotCommand();

          std::vector<std::string> words1;

          CDocStringToWords(dot_command->getText(), words1);

          uint no_words1 = words1.size();

          delete script_line;

          if (no_words1 == 1 && CStrUtil::casecmp(words1[0], "off") == 0)
            break;
        }
        else {
          delete script_line;
        }
      }
    }
  }

  /* External Datasets */

  else if (dot_command->getCommand() == "dd")
    ;
  else if (dot_command->getCommand() == "im") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if (no_words != 1) {
      CDocScriptError("Invalid CDoc Imbed Dataset Command .%s %s",
                      dot_command->getCommand().c_str(), dot_command->getText().c_str());

      return;
    }

    FILE *fp = CDocScriptImbedDataset(words[0]);

    if (fp != NULL) {
      cdoc_input_fp_list.push_back(cdoc_input_fp);

      cdoc_input_fp = fp;
    }
  }

  /* Formatting */

  else if (dot_command->getCommand() == "fo") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    for (uint i = 0; i < no_words; i++) {
      if      (CStrUtil::casecmp(words[i], "on") == 0)
        CDocScriptFormattingOn();
      else if (CStrUtil::casecmp(words[i], "off") == 0)
        CDocScriptFormattingOff();
      else
        CDocScriptWarning("Formatting Type '%s' not Supported", words[i].c_str());
    }
  }

  /* Page Throws */

  else if (dot_command->getCommand() == "pa")
    CDocScriptNewPage();

  /* Indenting */

  else if (dot_command->getCommand() == "in") {
    int sign = 0;

    std::string str = dot_command->getText();

    if      (str[0] == '+') {
      str  = str.substr(1);
      sign = -1;
    }
    else if (str[0] == '-') {
      str  = str.substr(1);
      sign = -1;
    }

    int indent = 0;

    if (CStrUtil::isInteger(str))
      indent -= CStrUtil::toInteger(str);

    if (sign == 0)
      cdoc_indent = indent + sign*indent;
    else
      cdoc_indent += sign*indent;

    if (cdoc_left_margin + cdoc_indent < 1) {
      CDocScriptWarning("Indent %d Puts Left Margin Outside Page Bounds", cdoc_indent);

      cdoc_indent = -cdoc_left_margin + 1;
    }
  }

  /* Spacing */

  else if (dot_command->getCommand() == "bl")
    CDocScriptSkipLine();
  else if (dot_command->getCommand() == "sp") {
    if (dot_command->getText() == "")
      no_spaces = 1;
    else {
      no_spaces = CStrUtil::toInteger(dot_command->getText());

      if (no_spaces == 0 || no_spaces > cdoc_lines_per_page) {
        CDocScriptWarning("Invalid Number of Spaces '%s'", dot_command->getText().c_str());

        no_spaces = 1;
      }
    }

    if (cdoc_in_paragraph) {
      if (cdoc_paragraph->getFormatted())
        CDocScriptAddStringToParagraph("\n");

      for (i = 0; i < no_spaces; i++)
        CDocScriptAddStringToParagraph("\n");
    }
    else {
      for (i = 0; i < no_spaces; i++)
        CDocScriptSkipLine();
    }
  }

  /* Revision Control */

  else if (dot_command->getCommand() == "rc") {
    if (isdigit(dot_command->getText()[0])) {
      int i = 0;
      int j = 0;

      char no_string[32];

      while (dot_command->getText()[i] != '\0' && isdigit(dot_command->getText()[i]))
        no_string[j++] = dot_command->getText()[i++];

      no_string[j] = '\0';

      int no = CStrUtil::toInteger(no_string);

      if (no <= 0) {
        CDocScriptWarning("Invalid Revision Control Line '%s'",
                          dot_command->getText().c_str());
        return;
      }

      CStrUtil::skipSpace(dot_command->getText(), &i);

      if      (CStrUtil::casecmp(&dot_command->getText()[i], "on") == 0)
        CDocScriptStartRevisionControl(no);
      else if (CStrUtil::casecmp(&dot_command->getText()[i], "off") == 0)
        CDocScriptEndRevisionControl(no);
      else
        CDocScriptSetRevisionControlChar(no, dot_command->getText()[i]);
    }
    else if (dot_command->getText()[0] == '*') {
      CDocScriptSetRevisionControlChar(0, dot_command->getText()[0]);
      CDocScriptStartRevisionControl(0);
    }
    else {
      CDocScriptWarning("Invalid Revision Control Line '%s'",
                        dot_command->getText().c_str());
      return;
    }
  }

  /* Line Breaks */

  else if (strncmp(dot_command->getCommand().c_str(), "br", 2) == 0) {
    if (cdoc_in_paragraph) {
      CDocScriptAddStringToParagraph("\n");
      CDocScriptAddStringToParagraph(dot_command->getText());
    }
    else
      CDocScriptNewParagraph(dot_command->getText(), NORMAL_PARAGRAPH, true, LEFT_JUSTIFICATION);
  }

  /* If ... Then ... Else */

  else if (dot_command->getCommand() == "if")
    CDocScriptWarning("If not Supported");
  else if (dot_command->getCommand() == "th")
    CDocScriptWarning("Then not Supported");
  else if (dot_command->getCommand() == "el")
    CDocScriptWarning("Else not Supported");

  /* Page Control */

  else if (dot_command->getCommand() == "cp")
    CDocScriptWarning("Condition Page Eject not Supported");

  /* Comments */

  else if (dot_command->getCommand() == "cm")
    ;

  /* Centering */

  else if (dot_command->getCommand() == "ce") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words == 1 && CStrUtil::casecmp(words[0], "on") == 0)
      cdoc_no_centred = -1;
    else if (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0)
      cdoc_no_centred = 0;
    else if (no_words == 1 && isdigit(words[0][0]))
      cdoc_no_centred = CStrUtil::toInteger(words[0]);
    else
      cdoc_no_centred = 1;
  }

  /* Underscores and Capitalization */

  else if (dot_command->getCommand() == "uc") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words == 1 && CStrUtil::casecmp(words[0], "on") == 0)
      cdoc_no_uscore = -1;
    else if (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0)
      cdoc_no_uscore = 0;
    else if (no_words == 1 && isdigit(words[0][0]))
      cdoc_no_uscore = CStrUtil::toInteger(words[0]);
    else
      cdoc_no_uscore = 1;

    cdoc_no_cap = cdoc_no_uscore;
  }

  else if (dot_command->getCommand() == "up") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words == 1 && CStrUtil::casecmp(words[0], "on") == 0)
      cdoc_no_cap = -1;
    else if (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0)
      cdoc_no_cap = 0;
    else if (no_words == 1 && isdigit(words[0][0]))
      cdoc_no_cap = CStrUtil::toInteger(words[0]);
    else
      cdoc_no_cap = 1;
  }

  else if (dot_command->getCommand() == "us") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words == 1 && CStrUtil::casecmp(words[0], "on") == 0)
      cdoc_no_uscore = -1;
    else if (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0)
      cdoc_no_uscore = 0;
    else if (no_words == 1) {
      std::string str;

      int i   = 0;
      int len = words[0].size();

      while (i < len && isdigit(words[0][i]))
        str += words[0][i++];

      if (str != "")
        cdoc_no_uscore = CStrUtil::toInteger(str);
      else
        cdoc_no_uscore = 1;
    }
    else
      cdoc_no_uscore = 1;
  }

  /* Process a Heading Definition */

  else if (dot_command->getCommand() == "dh") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words >= 2) {
      level = CStrUtil::toInteger(words[0]) % CDOC_MAX_HEADERS;

      CStrUtil::toLower(words[1]);

      std::vector<std::string>::iterator p = words.begin();

      std::vector<std::string> words1(++p, words.end());

      CDocExtractParameters(words1, dh_parameter_data,
                            reinterpret_cast<char *>(&header_control[level]));
    }
    else if (no_words == 1) {
      /* Set Header Control from Default Header Control */

      level = CStrUtil::toInteger(words[0]) % CDOC_MAX_HEADERS;

      memcpy(&header_control[level], &default_header_control[level],
             sizeof(CDHeaderControl));
    }
    else {
      /* Set All Header Controls from Default Header Control */

      memcpy(header_control, default_header_control,
             CDOC_MAX_HEADERS*sizeof(CDHeaderControl));
    }
  }

  /* Headings */

  else if (dot_command->getCommand() == "h0" ||
           dot_command->getCommand() == "h1" ||
           dot_command->getCommand() == "h2" ||
           dot_command->getCommand() == "h3" ||
           dot_command->getCommand() == "h4" ||
           dot_command->getCommand() == "h5" ||
           dot_command->getCommand() == "h6") {
    /* Spell Check if Required */

    if (cdoc_spell_check && cdoc_spell_active)
      CSpellCheckString(dot_command->getText());

    /* Calculate the Heading Level */

    level = (dot_command->getCommand()[1] - '0') % CDOC_MAX_HEADERS;

    /* Set the left margin for this Heading Level if the
       Heading will be output */

    if (! header_control[level].toc_only) {
      if      (level <= 1)
        cdoc_left_margin = cdoc_save_left_margin;
      else if (level < 5)
        cdoc_left_margin = cdoc_save_left_margin + 2*(level - 1);
    }

    /* Add Space before Header Text */

    strcpy(temp_string, "");

    for (i = 0; i < header_control[level].space_before; i++)
      strcat(temp_string, " ");

    /* If the Header is numbered then add the number text */

    if (header_control[level].number_header) {
      int  header_no;

      CDocScriptStartHeader(level);

      /* Create Header number from the Depth of each Header Level
         below and equal to the current Header Level's Depth */

      for (i = 1; i <= level; i++) {
        header_no = CDocScriptGetHeader(i);

        /* Use Alphabetic Characters for the Appendix */

        char temp_string1[256];

        if (i == 1) {
          if (cdoc_document.part == CDOC_APPENDIX_PART)
            sprintf(temp_string1, "Appendix %c",
                    cdoc_a_to_z[(header_no - appendix_depth - 1) % 26]);
          else
            sprintf(temp_string1, "%d", header_no);
        }
        else
          sprintf(temp_string1, ".%d", header_no);

        strcat(temp_string, temp_string1);
      }

      /* Add '.0' for First Level Normal Headers and '.'
         for First Level Appendix Headers */

      if (level == 1) {
        if (cdoc_document.part != CDOC_APPENDIX_PART)
          strcat(temp_string, ".0");
        else
          strcat(temp_string, ".");
      }

      /* Add spacing between Number Text and Header Text */

      if (level > 0)
        strcat(temp_string, "  ");
    }

    /* Convert to Upper Case if Header is Capitalised */

    if (header_control[level].capitalised)
      CStrUtil::toUpper(dot_command->getText());

    /* Use Header Number if Global Switch is On */

    std::string temp_string1;

    if (cdoc_number_headers)
      temp_string1 = temp_string + dot_command->getText();
    else
      temp_string1 = dot_command->getText();

    /* Add any space required after header */

    for (i = 0; i < header_control[level].space_after; i++)
      temp_string1 += " ";

    /* If Header appears on its own then output as a title,
       otherwise add to the current text */

    if (header_control[level].break_header) {
      /* If new page required start a new section of the
         document (also causes a page throw), otherwise
         output header on separate line as a sub-section */

      if (header_control[level].new_page)
        CDocScriptStartSection(dot_command->getText());
      else {
        if (CDocIsPagedOutput()) {
          //int lines_per_page = CDocScriptGetLinesPerPage();

          int new_page = CDocScriptFindSpace(header_control[level].skip_before + 3);

          if (! new_page) {
            if (cdoc_paragraph_done)
              CDocScriptSkipLine();

            /* Output required number of blank lines
               before header */

            for (i = 0; i < header_control[level].skip_before - 1; i++)
              CDocScriptSkipLine();
          }
        }
        else {
          if (cdoc_paragraph_done)
            CDocScriptSkipLine();

          /* Output required number of blank lines before header */

          for (i = 0; i < header_control[level].skip_before - 1; i++)
            CDocScriptSkipLine();
        }

        std::string temp_string = "  " + dot_command->getText();

        CDocScriptStartSubSection(temp_string);
      }

      /* If header only appears in table of contents then
         don't output the header in the main text */

      if (! header_control[level].toc_only) {
        CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

        if (header_control[level].underscored)
          CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(),
                              temp_string1.c_str(), CDocEndUnderline());
        else
          CDocScriptWriteText("%s\n", temp_string1.c_str());

        CDocScriptSkipLine();

        cdoc_paragraph_done = false;

        cdoc_left_margin++;
      }
    }
    else {
      /* If header only appears in table of contents then
         don't add the header as a new paragraph */

      if (! header_control[level].toc_only) {
        std::string temp_string = temp_string1 + "  ";

        if (header_control[level].underscored)
          CDocScriptSetParagraphPrefix(temp_string, CDOC_BOLD_UNDERLINE_FONT, false);
        else
          CDocScriptSetParagraphPrefix(temp_string, CDOC_BOLD_FONT, false);
      }
    }

    CDocScriptSetTOCPage();
  }

  /* Running Heading */

  else if (dot_command->getCommand() == "rh")
    CDocScriptWarning("Running Heading not Supported");

  /* Postscript Inclusion */

  else if (dot_command->getCommand() == "po")
    CDocScriptWarning("Postscript Inclusion not Supported");

  /* Goto */

  else if (dot_command->getCommand() == "go")
    CDocScriptGotoLabel(dot_command->getText());

  /* Label (Processed in Pass 1) */

  else if (dot_command->getCommand() == "..")
    ;

  /* Symbol (Processed in Pass 1) */

  else if (dot_command->getCommand() == "se")
    ;

  else if (dot_command->getCommand() == "dv")
    ;

  /* Translations */

  else if (dot_command->getCommand() == "ti")
    CDocScriptParseTranslation(dot_command->getText());

  /* Dictionary */

  else if (dot_command->getCommand() == "du") {
    if (cdoc_spell_check) {
      std::vector<std::string> words;

      CDocStringToWords(dot_command->getText(), words);

      uint no_words = words.size();

      if (no_words > 1) {
        if      (CStrUtil::casecmp(words[0], "add") == 0) {
          for (uint i = 1; i < no_words; i++)
            CSpellAddWord(words[i]);
        }
        else if (CStrUtil::casecmp(words[0], "del") == 0)
          CDocScriptWarning("Dictionary Deletion not Supported");
        else
          CDocScriptWarning("Invalid Dictionary Update Command");
      }
    }
  }

  /* Spell Check */

  else if (dot_command->getCommand() == "sv") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if      (no_words == 1 && CStrUtil::casecmp(words[0], "on") == 0)
      cdoc_spell_active = true;
    else if (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0)
      cdoc_spell_active = false;
    else
      CDocScriptWarning("Invalid Spelling Verification Options");
  }

  /* Hyphenation */

  else if (dot_command->getCommand() == "hy")
    CDocScriptWarning("Hyphenation not Supported");

  /* Define Character (Error Checking Done in Pass 1) */

  else if (dot_command->getCommand() == "dc") {
    std::vector<std::string> words;

    CDocStringToWords(dot_command->getText(), words);

    uint no_words = words.size();

    if (no_words > 0) {
      CStrUtil::toLower(words[0]);

      if (words[0] == "cont") {
        if (no_words == 2) {
          if      (CStrUtil::casecmp(words[1], "off") == 0)
            cdoc_continuation_char = int('\0');
          else if (words[1].size() == 1)
            cdoc_continuation_char = int(words[1][0]);
        }
      }
    }
  }

  /* Invalid or Unsupported Commands */

  else {
    if (dot_command->getCommandData() != NULL)
      CDocScriptWarning("Unsupported Dot Command - %s %s",
                        dot_command->getCommand().c_str(), dot_command->getText().c_str());
    else
      CDocScriptWarning("Invalid Dot Command - %s %s",
                        dot_command->getCommand().c_str(), dot_command->getText().c_str());
  }
}

// Process a Colon Command
static void
CDocProcessColonCommandPass2(CDColonCommand *colon_command)
{
  int  i;
  int  j;
  int  level;
  int  no_spaces;
  char temp_string[CDOC_MAX_LINE];

  if (colon_command->getCommandData() == NULL ||
      colon_command->getCommandData()->flush ||
      (colon_command->getCommandData()->end_command != NULL &&
       colon_command->getCommand() == colon_command->getCommandData()->end_command)) {
    if (cdoc_in_paragraph)
      CDocScriptOutputParagraph();
  }

  /* Process Specific Control Blocks */

  if      (colon_command->getCommand() == "psc")
    CDocScriptStartPCtrl(colon_command);
  else if (colon_command->getCommand() == "epsc")
    CDocScriptEndPCtrl();

  /* Process Front Sheet Colon Commands */

  else if (cdoc_document.part == CDOC_FRONT_SHEET_PART) {
    if (! CDocScriptProcessFrontSheetCommand(colon_command)) {
      /* Set the Document Part to Unspecified while processing
         the Colon Command otherwise we will end up in an endless
         loop */

      cdoc_document.part = CDOC_NO_PART;

      CDocProcessColonCommandPass2(colon_command);

      cdoc_document.part = CDOC_FRONT_SHEET_PART;
    }
  }

  /* Process Amendments Colon Commands */

  else if (cdoc_document.part == CDOC_AMENDMENTS_PART)
    CDocScriptProcessAmendmentsCommand(colon_command);

  /* Process Memo Header Colon Commands */

  else if (cdoc_document.part == CDOC_MEMO_HEADER_PART)
    CDocScriptProcessMemoHeaderCommand(colon_command);

  /* Process Title Page Colon Commands */

  else if (cdoc_document.sub_part == CDOC_TITLE_PAGE_SUB_PART)
    CDocScriptProcessTitlePageCommand(colon_command);

  /* Process Basic Colon Commands */

  else if ((cdoc_document.part == CDOC_FRONT_MATTER_PART &&
            (cdoc_document.sub_part == CDOC_ABSTRACT_SUB_PART ||
             cdoc_document.sub_part == CDOC_PREFACE_SUB_PART  ||
             cdoc_document.sub_part == CDOC_NO_SUB_PART      )) ||
           cdoc_document.part == CDOC_BODY_PART ||
           cdoc_document.part == CDOC_APPENDIX_PART ||
           cdoc_document.part == CDOC_BACK_MATTER_PART ||
           cdoc_document.part == CDOC_NO_PART) {
    /* Document Types */

    if      (colon_command->getCommand() == "sdoc") {
      CDDocData doc_data;

      doc_data.security = NULL;
      doc_data.language = NULL;

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 sdoc_parameter_data,
                                 reinterpret_cast<char *>(&doc_data));

      if (doc_data.security) cdoc_document.security = doc_data.security;
      if (doc_data.language) cdoc_document.language = doc_data.language;

      CStrUtil::toUpper(cdoc_document.security);

      cdoc_document.type     = CDOC_STAIRS_DOCUMENT;
      cdoc_document.part     = CDOC_NO_PART;
      cdoc_document.sub_part = CDOC_NO_SUB_PART;
    }
    else if (colon_command->getCommand() == "esdoc")
      cdoc_document.type = CDOC_NO_DOCUMENT;
    else if (colon_command->getCommand() == "gdoc") {
      CDDocData doc_data;

      doc_data.security = NULL;
      doc_data.language = NULL;

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 gdoc_parameter_data,
                                 reinterpret_cast<char *>(&doc_data));

      if (doc_data.security) cdoc_document.security = doc_data.security;
      if (doc_data.language) cdoc_document.language = doc_data.language;

      cdoc_document.type     = CDOC_GENERAL_DOCUMENT;
      cdoc_document.part     = CDOC_NO_PART;
      cdoc_document.sub_part = CDOC_NO_SUB_PART;
    }
    else if (colon_command->getCommand() == "egdoc")
      cdoc_document.type = CDOC_NO_DOCUMENT;
    else if (colon_command->getCommand() == "memo")
      cdoc_document.type = CDOC_MEMO_DOCUMENT;

    /* Document Parts */

    else if (colon_command->getCommand() == "frontm") {
      cdoc_document.part     = CDOC_FRONT_MATTER_PART;
      cdoc_document.sub_part = CDOC_NO_SUB_PART;
    }
    else if (colon_command->getCommand() == "body") {
      cdoc_document.part     = CDOC_BODY_PART;
      cdoc_document.sub_part = CDOC_NO_SUB_PART;
    }
    else if (colon_command->getCommand() == "appendix") {
      cdoc_document.part     = CDOC_APPENDIX_PART;
      cdoc_document.sub_part = CDOC_NO_SUB_PART;

      appendix_depth = CDocScriptGetHeader(1);
    }
    else if (colon_command->getCommand() == "eappendix") {
      cdoc_document.part     = CDOC_NO_PART;
      cdoc_document.sub_part = CDOC_NO_SUB_PART;

      appendix_depth = 0;
    }
    else if (colon_command->getCommand() == "backm") {
      cdoc_document.part     = CDOC_BACK_MATTER_PART;
      cdoc_document.sub_part = CDOC_NO_SUB_PART;
    }
    else if (colon_command->getCommand() == "fronts")
      CDocScriptInitFrontSheet();
    else if (colon_command->getCommand() == "amends")
      CDocScriptInitAmendments();
    else if (colon_command->getCommand() == "mh")
      CDocScriptInitMemoHeader();

    /* Front Matter Sub Parts */

    else if (colon_command->getCommand() == "titlep")
      CDocScriptInitTitlePage();
    else if (colon_command->getCommand() == "abstract") {
      cdoc_document.part     = CDOC_FRONT_MATTER_PART;
      cdoc_document.sub_part = CDOC_ABSTRACT_SUB_PART;

      CDocScriptWriteLeftJustifiedPageHeader("Abstract");
    }
    else if (colon_command->getCommand() == "preface") {
      cdoc_document.part     = CDOC_FRONT_MATTER_PART;
      cdoc_document.sub_part = CDOC_PREFACE_SUB_PART;

      CDocScriptWriteLeftJustifiedPageHeader("Preface");
    }

    /* Table of Contents */

    else if (colon_command->getCommand() == "toc")
      CDocScriptOutputTableOfContents();

    /* Figure List */

    else if (colon_command->getCommand() == "figlist")
      CDocScriptOutputFigureList();

    /* Table List */

    else if (colon_command->getCommand() == "tlist")
      CDocScriptOutputTableList();

    /* Index */

    else if (colon_command->getCommand() == "index") {
      if (cdoc_index)
        CDocScriptOutputIndex();
    }

    else if (colon_command->getCommand()[0] == 'i' &&
             (colon_command->getCommand()[1] == '1' ||
              colon_command->getCommand()[1] == '2' ||
              colon_command->getCommand()[1] == '3')) {
      if (cdoc_index) {
        CDIndexEntry index_entry;

        index_entry.ident = NULL;
        index_entry.page  = NULL;
        index_entry.refid = NULL;

        if (colon_command->getCommand()[1] == '1')
          CDocExtractParameterValues(colon_command->getParameters(),
                                     colon_command->getValues(),
                                     i1_parameter_data,
                                     reinterpret_cast<char *>(&index_entry));
        else
          CDocExtractParameterValues(colon_command->getParameters(),
                                     colon_command->getValues(),
                                     i23_parameter_data,
                                     reinterpret_cast<char *>(&index_entry));

        std::string see_text;

        std::string page;

        if      (index_entry.page == PG_START)
          page = PG_START_STR;
        else if (index_entry.page == PG_END)
          page = PG_END_STR;
        else if (index_entry.page == PG_MAJOR)
          page = PG_MAJOR_STR;
        else
          page  = (index_entry.page  ? std::string(index_entry.page ) : "");

        std::string refid = (index_entry.refid ? std::string(index_entry.refid) : "");

        if      (colon_command->getCommand()[1] == '1') {
          see_text = CDocScriptGetIndex1SeeText(colon_command->getText(), refid);

          CDocScriptSetIndex1Page(colon_command->getText(), page, refid);
        }
        else if (colon_command->getCommand()[1] == '2') {
          see_text = CDocScriptGetIndex2SeeText(colon_command->getText(), refid);

          CDocScriptSetIndex2Page(colon_command->getText(), page, refid);
        }
        else {
          see_text = CDocScriptGetIndex3SeeText(colon_command->getText(), refid);

          CDocScriptSetIndex3Page(colon_command->getText(), page, refid);
        }

        if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("<a name='%s'></a>\n", see_text.c_str());
      }
    }
    else if (colon_command->getCommand()[0] == 'i' &&
             colon_command->getCommand()[1] == 'h' &&
             (colon_command->getCommand()[2] == '1' ||
              colon_command->getCommand()[2] == '2' ||
              colon_command->getCommand()[2] == '3')) {
      if (cdoc_index) {
        CDIndexEntryHeading index_entry_heading;

        index_entry_heading.ident = NULL;
        index_entry_heading.print = NULL;
        index_entry_heading.see   = NULL;
        index_entry_heading.seeid = NULL;

        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   ih_parameter_data,
                                   reinterpret_cast<char *>(&index_entry_heading));

        std::string see_text;

        if      (colon_command->getCommand()[2] == '1')
          see_text = CDocScriptGetIndex1SeeText(colon_command->getText(), "");
        else if (colon_command->getCommand()[2] == '2')
          see_text = CDocScriptGetIndex2SeeText(colon_command->getText(), "");
        else
          see_text = CDocScriptGetIndex3SeeText(colon_command->getText(), "");

        if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("<a name='%s'></a>\n", see_text.c_str());
      }
    }

    /* Title */

    else if (colon_command->getCommand() == "title") {
      /* Spell Check if Required */

      if (cdoc_spell_check && cdoc_spell_active)
        CSpellCheckString(colon_command->getText());

      /* Output Title Line */

      if (CDocStringDisplayLength(colon_command->getText()) > 0)
        CDocScriptWriteLeftJustifiedHeader(colon_command->getText());
    }

    /* Signature */

    else if (colon_command->getCommand() == "sig")
      CDocScriptOutputMemoSignature();

    /* Process Table Commands */

    /* Table Row */

    else if (colon_command->getCommand() == "row") {
      if (current_table != NULL) {
        /* Set structure values from Parameter/Value pairs */

        CDTableRowData row_data;

        row_data.refid = NULL;
        row_data.split = false;

        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   row_parameter_data,
                                   reinterpret_cast<char *>(&row_data));

        CDTableRow *row = CDocScriptCreateTableRow(current_table);

        row->refid = (row_data.refid ? row_data.refid : "");
        row->split = row_data.split;

        CDocScriptSetTableRowRowDef(row);

        current_row = row;

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<tr>\n");
          return;
        }
      }
      else
        CDocScriptError("Table Row outside Table");
    }

    /* Table End Row */

    else if (colon_command->getCommand() == "erow") {
      if (current_table != NULL) {
        current_row = NULL;

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("</tr>\n");
          return;
        }
      }
      else
        CDocScriptError("Table End Row outside Table");
    }

    /* Table Cell */

    else if (colon_command->getCommand() == "c") {
      /* Spell Check if Required */

      if (cdoc_spell_check && cdoc_spell_active)
        CSpellCheckString(colon_command->getText());

      if (current_table != NULL) {
        if (current_row != NULL) {
          CDTableCell *cell = CDocScriptCreateTableCell(current_row);

          if      (colon_command->getNumParameters() == 0)
            ;
          else if (colon_command->getNumParameters() == 1 && colon_command->getValue(0) == "") {
            cell->number = CStrUtil::toInteger(colon_command->getParameter(0));

            if (cell->number <= 0) {
              cell->number = 0;

              CDocScriptError("Invalid Cell Number '%s'",
                              colon_command->getParameter(0).c_str());
            }
            else
              current_row->cell_number = cell->number;
          }
          else
            CDocScriptError("Invalid Cell Parameter List");

          cell->text = CStrUtil::strdup(colon_command->getText().c_str());

          cell->no_breaks = colon_command->getNumBreaks();

          if (cell->no_breaks > 0) {
            cell->breaks = new int [cell->no_breaks];

            for (j = 0; j < cell->no_breaks; j++)
              cell->breaks[j] = colon_command->getBreak(j);
          }
          else
            cell->breaks = NULL;

          if (cell->number == 0)
            cell->number = ++current_row->cell_number;

          if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
            CDocScriptWriteCommand("<td>%s</td>\n", CDocEncodeHtmlString(cell->text));
            return;
          }
          else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
            CDocScriptWriteCommand("<\"para\">\n");

            CDocScriptWriteCommand("%s\n",
              CDocEncodeILeafString(cell->text));
            return;
          }
        }
        else
          CDocScriptError("Cell outside Row Block");
      }
      else
        CDocScriptError("Table End Row outside Table");
    }

    /* Table Caption */

    else if (colon_command->getCommand() == "tcap") {
      if (current_table == NULL)
        CDocScriptError("Table Caption outside Table");
    }

    /* Table Description */

    else if (colon_command->getCommand() == "tdesc") {
      if (current_table == NULL)
        CDocScriptError("Table Description outside Table");
    }

    /* Table Footer */

    else if (colon_command->getCommand() == "tft") {
      if (current_table != NULL) {
        CDTableRow *row = CDocScriptCreateTableFooterRow(current_table);

        /* Set structure values from Parameter/Value pairs */

        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   tft_parameter_data,
                                   reinterpret_cast<char *>(row));

        CDocScriptSetTableRowRowDef(row);

        current_row = row;
      }
      else
        CDocScriptError("Table Footer outside Table");
    }

    /* Table End Footer */

    else if (colon_command->getCommand() == "etft") {
      if (current_table != NULL)
        current_row = NULL;
      else
        CDocScriptError("Table End Footer outside Table");
    }

    /* Table Heading */

    else if (colon_command->getCommand() == "thd") {
      if (current_table != NULL) {
        CDTableRow *row = CDocScriptCreateTableHeaderRow(current_table);

        /* Set structure values from Parameter/Value pairs */

        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   thd_parameter_data,
                                   reinterpret_cast<char *>(row));

        CDocScriptSetTableRowRowDef(row);

        current_row = row;
      }
      else
        CDocScriptError("Table Header outside Table");
    }

    /* Table End Heading */

    else if (colon_command->getCommand() == "ethd") {
      if (current_table != NULL)
        current_row = NULL;
      else
        CDocScriptError("Table End Header outside Table");
    }

    /* Table Note */

    else if (colon_command->getCommand() == "tnote") {
      /* Spell Check if Required */

      if (cdoc_spell_check && cdoc_spell_active)
        CSpellCheckString(colon_command->getText());

      if (current_table != NULL) {
        current_row = NULL;

        CDocScriptAddTableNote(current_table, colon_command->getText());
      }
      else
        CDocScriptError("Table Note outside Table");
    }
    else if (colon_command->getCommand() == "etnote") {
      if (current_table != NULL)
        ;
      else
        CDocScriptError("End Table Note outside Table");
    }

    /* End Table */

    else if (colon_command->getCommand() == "etable") {
      if (current_table != NULL) {
        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("</table>\n");
        else
          CDocScriptOutputTable(current_table);

        if (current_table->id != "")
          CDocScriptSetReferencePageNumber(TABLE_REF, current_table->id);

        CDocScriptSetTablePage(current_table);
      }
      else
        CDocScriptWarning("Invalid Table Colon Command - %s %s",
                          colon_command->getCommand().c_str(), colon_command->getText().c_str());

      current_table = NULL;
    }

    /* Figures */

    else if (colon_command->getCommand() == "fig") {
      if (current_figure != NULL) {
        CDocScriptWarning("Unterminated Figure");

        CDocScriptFigureEnd(current_figure);
      }

      /* Process Figure (Details already recorded in Pass 1) */

      figure_number++;

      /* Get Pass 1 Figure Details */

      current_figure = CDocScriptGetFigure(figure_number);

      if (current_figure != NULL)
        CDocScriptFigureBegin(current_figure);
      else
        CDocScriptError("Figure Not Found - Internal Error");
    }
    else if (colon_command->getCommand() == "figcap") {
      if (current_figure == NULL)
        CDocScriptWarning("Figure Caption outside Figure");
    }
    else if (colon_command->getCommand() == "figdesc") {
      if (current_figure == NULL)
        CDocScriptWarning("Figure Description outside Figure");
    }
    else if (colon_command->getCommand() == "efig") {
      /* End of Figure */

      if (current_figure != NULL)
        CDocScriptFigureEnd(current_figure);
      else
        CDocScriptWarning("End Figure outside Figure");

      current_figure = NULL;
    }

    /* Definition Lists */

    else if (colon_command->getCommand() == "dl") {
      if (CDocInst->getOutputFormat() != CDOC_OUTPUT_HTML &&
          CDocInst->getOutputFormat() != CDOC_OUTPUT_ILEAF) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptSkipLine();
      }

      /* Create and store Definition List Structure */

      cdoc_definition_list = CDocScriptCreateDefinitionList();

      /* Set structure values from Parameter/Value pairs */

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 dl_parameter_data,
                                 reinterpret_cast<char *>(cdoc_definition_list));

      cdoc_definition_list->heading_highlight %= 4;
      cdoc_definition_list->term_highlight    %= 4;

      /* Add Definition List Structure as the Current List */

      CDocScriptStartDefinitionList(cdoc_definition_list);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptWriteCommand("<dl compact>\n");
        else
          CDocScriptWriteCommand("<dl>\n");
      }
    }

    /* Definition Term Header */

    else if (colon_command->getCommand() == "dthd") {
      /* Spell Check if Required */

      if (cdoc_spell_check && cdoc_spell_active)
        CSpellCheckString(colon_command->getText());

      if (cdoc_definition_list != NULL) {
        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<dt><b>%s</b></dt>\n",
           CDocEncodeHtmlString(colon_command->getText()));
          return;
        }
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
          CDocScriptWriteCommand("<\"para\">\n");

          CDocScriptWriteCommand("%s\n",
           CDocEncodeILeafString(colon_command->getText()));
          return;
        }

        strcpy(temp_string, "");

        /* If term exceeds space allowed */

        if (CDocStringDisplayLength(colon_command->getText()) + 2 >
             cdoc_definition_list->depth) {
          /* If break up flag set then output on a separate
             line as the definition text using the paragraph
             prefix, otherwise output on the same line as the
             definition text */

          if (cdoc_definition_list->break_up) {
            std::string temp_string;

            if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
                     CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
                     CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
              temp_string =
                CDocScriptAddHighlightEscapes(colon_command->getText(),
                                              cdoc_definition_list->heading_highlight) + "\n";
            }
/*
            else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
              if (cdoc_definition_list->heading_highlight == 0)
                temp_string = "";
              else
                temp_string = ".ft B\n";

              temp_string += colon_command->getText();
              temp_string += "\n";

              if (cdoc_definition_list->heading_highlight != 0)
                temp_string += ".ft P\n";
            }
*/
            else
              temp_string = colon_command->getText() + "\n";

            cdoc_paragraph->setHeader(temp_string);

            int i = 0;

            for (i = 0; i < cdoc_definition_list->depth; i++)
              definition_prefix[i] = ' ';

            definition_prefix[i] = '\0';

            CDocScriptSetParagraphPrefix(definition_prefix, CDOC_NORMAL_FONT, true);
          }
          else {
            uint i = 0;

            for (i = 0; i < colon_command->getText().size(); i++)
              definition_prefix[i] = colon_command->getText()[i];

            definition_prefix[i++] = ' ';
            definition_prefix[i++] = ' ';

            definition_prefix[i] = '\0';

            CDocScriptSetParagraphPrefix(definition_prefix,
              CDocScriptHighlightToFont(cdoc_definition_list->heading_highlight),
                                        true);
          }
        }

        /* If term is within space allowed then pad out to the
           required length with spaces and set the paragraph
           prefix */

        else {
          uint i = 0;

          for (i = 0; i < colon_command->getText().size(); i++)
            definition_prefix[i] = colon_command->getText()[i];

          for ( ; i < uint(cdoc_definition_list->depth); i++)
            definition_prefix[i] = ' ';

          definition_prefix[i] = '\0';

          CDocScriptSetParagraphPrefix(definition_prefix,
            CDocScriptHighlightToFont(cdoc_definition_list->heading_highlight),
                                      true);
        }

        cdoc_definition_list->prefix_length = CDocStringDisplayLength(definition_prefix);
      }
    }

    /* Definition Description Header */

    else if (colon_command->getCommand() == "ddhd") {
      if (cdoc_definition_list != NULL) {
        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<dd>%s</dd>\n",
            CDocEncodeHtmlString(colon_command->getText().c_str()));
          return;
        }
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
          CDocScriptWriteCommand("<\"para\">\n");

          CDocScriptWriteCommand("%s\n", CDocEncodeILeafString(colon_command->getText().c_str()));
          return;
        }

        /* Add the text as a New Paragraph */

        std::string temp_string =
          CDocScriptAddHighlightEscapes(colon_command->getText(),
                                        cdoc_definition_list->heading_highlight);

        CDocScriptNewParagraph(temp_string, DEFINITION_PARAGRAPH,
                               true, LEFT_JUSTIFICATION);
      }
      else
        CDocScriptWarning("Definition Data outside Definition List - %s %s",
                          colon_command->getCommand().c_str(), colon_command->getText().c_str());
    }

    /* Definition Term */

    else if (colon_command->getCommand() == "dt") {
      /* Spell Check if Required */

      if (cdoc_spell_check && cdoc_spell_active)
        CSpellCheckString(colon_command->getText());

      if (cdoc_definition_list != NULL) {
        if (cdoc_definition_list->term_flag)
          CDocScriptWarning("No Definition Data for Definition Term");
        else
          cdoc_definition_list->term_flag = true;

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<dt><b>%s</b></dt>\n",
            CDocEncodeHtmlString(colon_command->getText().c_str()));

          return;
        }
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
          CDocScriptWriteCommand("<\"para\">\n");

          CDocScriptWriteCommand("%s%s%s\n", ILEAF_BOLD_FONT,
                                 CDocEncodeILeafString(colon_command->getText()),
                                 ILEAF_NORMAL_FONT);

          return;
        }

        strcpy(temp_string, "");

        /* If term exceeds space allowed */

        if (CDocStringDisplayLength(colon_command->getText()) + 2 >
              cdoc_definition_list->depth) {
          /* If break up flag set then output on a separate
             line as the definition text using the paragraph
             prefix, otherwise output on the same line as the
             definition text */

          if (cdoc_definition_list->break_up) {
            std::string temp_string;

            if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
                     CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
                     CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
              temp_string =
                CDocScriptAddHighlightEscapes(colon_command->getText(),
                                              cdoc_definition_list->term_highlight) + "\n";
            }
/*
            else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
              if (cdoc_definition_list->term_highlight == 0)
                temp_string = "";
              else
                temp_string = ".ft B\n";

              temp_string += colon_command->getText();
              temp_string += "\n";

              if (cdoc_definition_list->term_highlight != 0)
                temp_string += ".ft P\n";
            }
*/
            else
              temp_string = colon_command->getText() + "\n";

            cdoc_paragraph->setHeader(temp_string);

            int i = 0;

            for (i = 0; i < cdoc_definition_list->depth; i++)
              definition_prefix[i] = ' ';

            definition_prefix[i] = '\0';

            CDocScriptSetParagraphPrefix(definition_prefix, CDOC_NORMAL_FONT, true);
          }
          else {
            uint i = 0;

            for (i = 0; i < colon_command->getText().size(); i++)
              definition_prefix[i] = colon_command->getText()[i];

            definition_prefix[i++] = ' ';
            definition_prefix[i++] = ' ';

            definition_prefix[i] = '\0';

            CDocScriptSetParagraphPrefix(definition_prefix,
              CDocScriptHighlightToFont(cdoc_definition_list->term_highlight),
                                         true);
          }
        }

        /* If term is within space allowed then pad out to the
           required length with spaces and set the paragraph
           prefix */

        else {
          int i = 0;

          for (i = 0; i < int(colon_command->getText().size()); i++)
            definition_prefix[i] = colon_command->getText()[i];

          for (; i < cdoc_definition_list->depth; i++)
            definition_prefix[i] = ' ';

          definition_prefix[i] = '\0';

          CDocScriptSetParagraphPrefix(definition_prefix,
            CDocScriptHighlightToFont(cdoc_definition_list->term_highlight),
                                       true);
        }

        cdoc_definition_list->prefix_length = CDocStringDisplayLength(definition_prefix);
      }
    }

    /* Definition Description */

    else if (colon_command->getCommand() == "dd") {
      /* Add the text as a New Paragraph */

      if (cdoc_definition_list != NULL) {
        if (! cdoc_definition_list->term_flag)
          CDocScriptWarning("Definition Data without Definition Term");
        else
          cdoc_definition_list->term_flag = false;

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<dd>%s</dd>\n", CDocEncodeHtmlString(colon_command->getText()));
          return;
        }
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
          CDocScriptWriteCommand("<\"para\">\n");

          CDocScriptWriteCommand("%s\n", CDocEncodeILeafString(colon_command->getText()));
          return;
        }

        /* Add the text as a New Paragraph */

        CDocScriptNewParagraph(colon_command->getText(), DEFINITION_PARAGRAPH,
                               true, LEFT_JUSTIFICATION);
      }
      else
        CDocScriptWarning("Definition Data outside Definition List - %s %s",
                          colon_command->getCommand().c_str(), colon_command->getText().c_str());
    }

    /* End of Definition List */

    else if (colon_command->getCommand() == "edl") {
      if (cdoc_definition_list != NULL &&
          CDocScriptIsCurrentListType(DEFINITION_LIST)) {
        /* If the list was compact then add a Newline
           at then end */

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("</dl>\n");
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
          ;
        else {
          if (cdoc_definition_list->compact)
            CDocScriptSkipLine();
        }

        /* Remove the Definition List and reset the current
           Definition List */

        cdoc_definition_list = CDocScriptEndDefinitionList();
      }
      else
        CDocScriptWarning("Invalid End Definition List");
    }

    /* Lists */

    /* Ordered List */

    else if (colon_command->getCommand() == "ol") {
      if (CDocInst->getOutputFormat() != CDOC_OUTPUT_HTML &&
          CDocInst->getOutputFormat() != CDOC_OUTPUT_ILEAF) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptSkipLine();
      }

      /* Create and Store List Structure */

      cdoc_general_list = CDocScriptCreateGeneralList(ORDERED_LIST);

      /* Set structure values from Parameter/Value pairs */

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 ol_parameter_data,
                                 reinterpret_cast<char *>(cdoc_general_list));

      /* Add List Structure as the Current List */

      CDocScriptStartGeneralList(cdoc_general_list);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptWriteCommand("<ol compact>\n");
        else
          CDocScriptWriteCommand("<ol>\n");
      }
    }

    /* Simple List */

    else if (colon_command->getCommand() == "sl") {
      if (CDocInst->getOutputFormat() != CDOC_OUTPUT_HTML &&
          CDocInst->getOutputFormat() != CDOC_OUTPUT_ILEAF) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptSkipLine();
      }

      /* Create and Store List Structure */

      cdoc_general_list = CDocScriptCreateGeneralList(SIMPLE_LIST);

      /* Set structure values from Parameter/Value pairs */

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 sl_parameter_data,
                                 reinterpret_cast<char *>(cdoc_general_list));

      /* Add List Structure as the Current List */

      CDocScriptStartGeneralList(cdoc_general_list);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptWriteCommand("<ul compact>\n");
        else
          CDocScriptWriteCommand("<ul>\n");
      }
    }

    /* Unordered List */

    else if (colon_command->getCommand() == "ul") {
      if (CDocInst->getOutputFormat() != CDOC_OUTPUT_HTML &&
          CDocInst->getOutputFormat() != CDOC_OUTPUT_ILEAF) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptSkipLine();
      }

      /* Create and Store List Structure */

      cdoc_general_list = CDocScriptCreateGeneralList(UNORDERED_LIST);

      /* Set structure values from Parameter/Value pairs */

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 ul_parameter_data,
                                 reinterpret_cast<char *>(cdoc_general_list));

      /* Add List Structure as the Current List */

      CDocScriptStartGeneralList(cdoc_general_list);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptWriteCommand("<ul compact>\n");
        else
          CDocScriptWriteCommand("<ul>\n");
      }
    }

    /* List Item */

    else if (colon_command->getCommand() == "li") {
      CDListItem  list_item;

      /* Process Parameter/Value pairs */

      list_item.ident = NULL;

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 li_parameter_data,
                                 reinterpret_cast<char *>(&list_item));

      /* Add reference if List has Identifier */

      if (list_item.ident != NULL) {
        if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("<a name='%s'></a>\n",
                                 colon_command->getText().c_str());
      }

      /* Output Text dependant on List Type */

      if (cdoc_general_list != NULL) {
        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<li>%s\n",
            CDocEncodeHtmlString(colon_command->getText().c_str()));
          return;
        }
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
          if (cdoc_general_list->type == ORDERED_LIST) {
            CDocScriptWriteCommand("<\"list\">\n");

            CDocScriptWriteCommand("<\"|:list\", Font = @i*, Subcomponent = yes><F0><Tab>"
                                   "<Autonum, \"list\", 1, Value = \"%d.\"><Tab><End Sub><F0>%s\n",
                                   cdoc_general_list->number,
                                   CDocEncodeILeafString(colon_command->getText().c_str()));
          }
          else {
            CDocScriptWriteCommand("<\"bullet\">\n");

            CDocScriptWriteCommand("<\"|:bullet\", Font = @i*, Subcomponent = yes>"
                                   "<F8@Z7@Lam>S<F0><Tab><Tab><End Sub><F0>%s\n",
                                   CDocEncodeILeafString(colon_command->getText().c_str()));
          }

          return;
        }

        /* Set current Paragraph to the List Item Text */

        CDocScriptNewParagraph(colon_command->getText(), LIST_PARAGRAPH,
                               true, LEFT_JUSTIFICATION);

        /* Ordered List requires preceding Number */

        if (cdoc_general_list->type == ORDERED_LIST) {
          CDGeneralList  *temp_general_list;

          /* Set current list item Number */

          cdoc_general_list->number++;

          /* Create List Item Number from a concatenation
             of the depth of this and all lower level
             Ordered List Items */

          strcpy(list_prefix, "");

          for (i = 1; i <= CDocScriptGetNoGeneralLists(); i++) {
            temp_general_list = CDocScriptGetNthGeneralList(i);

            if (temp_general_list->number > 0) {
              char temp_string1[33];

              sprintf(temp_string1, "%d.", temp_general_list->number);

              strcat(list_prefix, temp_string1);
            }
          }

          strcat(list_prefix, " ");

          /* Set the Number String as the Paragraph Prefix */

          CDocScriptSetParagraphPrefix(list_prefix, CDOC_NORMAL_FONT, true);
        }

        /* Simple List requires preceding Spaces */

        else if (cdoc_general_list->type == SIMPLE_LIST) {
          /* Set the Space String as the Paragraph Prefix */

          if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
            strcpy(list_prefix, "\\ \\  ");
          else
            strcpy(list_prefix, "   ");

          CDocScriptSetParagraphPrefix(list_prefix, CDOC_NORMAL_FONT, true);
        }

        /* Unordered List requires preceding Bullet */

        else if (cdoc_general_list->type == UNORDERED_LIST) {
          /* Set the Bullet String as the Paragraph Prefix */

          if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
            strcpy(list_prefix, "\\(bu  ");
          else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
            strcpy(list_prefix, "B@cdocsym@\267E   ");
          else
            strcpy(list_prefix, ".  ");

          CDocScriptSetParagraphPrefix(list_prefix, CDOC_NORMAL_FONT, true);
        }
      }
      else
        CDocScriptWarning("List Item outside List - %s %s",
                          colon_command->getCommand().c_str(), colon_command->getText().c_str());

      if (list_item.ident != NULL)
        CDocScriptSetReferencePageNumber(LIST_ITEM_REF, list_item.ident);
    }

    /* End Ordered List */

    else if (colon_command->getCommand() == "eol") {
      if (cdoc_general_list != NULL && cdoc_general_list->type == ORDERED_LIST &&
          CDocScriptIsCurrentListType(ORDERED_LIST)) {
        /* If the list was compact then add a Newline
           at then end */

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("</ol>\n");
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
          ;
        else {
          if (cdoc_general_list->compact)
            CDocScriptSkipLine();
        }

        /* Remove the List and reset the current List */

        cdoc_general_list = CDocScriptEndGeneralList(ORDERED_LIST);
      }
      else
        CDocScriptWarning("Invalid End Ordered List");
    }

    /* End Simple List */

    else if (colon_command->getCommand() == "esl") {
      if (cdoc_general_list != NULL && cdoc_general_list->type == SIMPLE_LIST &&
          CDocScriptIsCurrentListType(SIMPLE_LIST)) {
        /* If the list was compact then add a Newline
           at then end */

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("</ul>\n");
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
          ;
        else {
          if (cdoc_general_list->compact)
            CDocScriptSkipLine();
        }

        /* Remove the List and reset the current List */

        cdoc_general_list = CDocScriptEndGeneralList(SIMPLE_LIST);
      }
      else
        CDocScriptWarning("Invalid End Ordered List");
    }

    /* End Unordered List */

    else if (colon_command->getCommand() == "eul") {
      if (cdoc_general_list != NULL && cdoc_general_list->type == UNORDERED_LIST &&
          CDocScriptIsCurrentListType(UNORDERED_LIST)) {
        /* If the list was compact then add a Newline
           at then end */

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("</ul>\n");
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
          ;
        else {
          if (cdoc_general_list->compact)
            CDocScriptSkipLine();
        }

        /* Remove the List and reset the current List */

        cdoc_general_list =
          CDocScriptEndGeneralList(UNORDERED_LIST);
      }
      else
        CDocScriptWarning("Invalid End Unordered List");
    }

    /* Glossary List */

    else if (colon_command->getCommand() == "gl") {
      if (CDocInst->getOutputFormat() != CDOC_OUTPUT_HTML &&
          CDocInst->getOutputFormat() != CDOC_OUTPUT_ILEAF) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptSkipLine();
      }

      /* Create and Store Glossary List Structure */

      cdoc_glossary_list = CDocScriptCreateGlossaryList();

      /* Set structure values from Parameter/Value pairs */

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 gl_parameter_data,
                                 reinterpret_cast<char *>(cdoc_glossary_list));

      cdoc_glossary_list->term_highlight %= 4;

      /* Add Glossary List Structure as the Current List */

      CDocScriptStartGlossaryList(cdoc_glossary_list);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        if (CDocScriptGetCurrentListCompact())
          CDocScriptWriteCommand("<dl compact>\n");
        else
          CDocScriptWriteCommand("<dl>\n");
      }
    }

    /* Glossary Term */

    else if (colon_command->getCommand() == "gt") {
      /* Spell Check if Required */

      if (cdoc_spell_check && cdoc_spell_active)
        CSpellCheckString(colon_command->getText());

      /* Set Glossary Prefix to Text String */

      if (cdoc_glossary_list != NULL) {
        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<dt><b>%s</b></dt>\n",
            CDocEncodeHtmlString(colon_command->getText()));
          return;
        }
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
          CDocScriptWriteCommand("<\"para\">\n");

          CDocScriptWriteCommand("%s%s%s\n",
            ILEAF_BOLD_FONT,
            CDocEncodeILeafString(colon_command->getText()),
            ILEAF_NORMAL_FONT);

          return;
        }

        glossary_prefix =
          CDocScriptAddHighlightEscapes(colon_command->getText(),
                                        cdoc_glossary_list->term_highlight);
      }
      else
        CDocScriptWarning("Glossary Term outside Glossary List - %s %s",
                          colon_command->getCommand().c_str(), colon_command->getText().c_str());
    }

    /* Glossary Description */

    else if (colon_command->getCommand() == "gd") {
      if (cdoc_glossary_list != NULL) {
        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<dd>%s</dd>\n",
            CDocEncodeHtmlString(colon_command->getText()));
          return;
        }
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
          CDocScriptWriteCommand("<\"para\">\n");

          CDocScriptWriteCommand("%s\n",
           CDocEncodeILeafString(colon_command->getText()));
          return;
        }

        /* Set paragraph to Glossary Term (if specified) and
           Glossary Description Text */

        if (glossary_prefix != "") {
          CDocScriptNewParagraph(glossary_prefix, GLOSSARY_PARAGRAPH, true, LEFT_JUSTIFICATION);

          CDocScriptAddStringToParagraph(":  ");
          CDocScriptAddStringToParagraph(colon_command->getText());

          glossary_prefix = "";
        }
        else {
          CDocScriptNewParagraph("???:  ", GLOSSARY_PARAGRAPH, true, LEFT_JUSTIFICATION);

          CDocScriptAddStringToParagraph(colon_command->getText());
        }
      }
      else
        CDocScriptWarning("Glossary Data outside Glossary List - %s %s",
                          colon_command->getCommand().c_str(), colon_command->getText().c_str());
    }

    /* End Glossary List */

    else if (colon_command->getCommand() == "egl") {
      if (cdoc_glossary_list != NULL &&
          CDocScriptIsCurrentListType(GLOSSARY_LIST)) {
        /* If the list was compact then add a Newline
           at then end */

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
          CDocScriptWriteCommand("</dl>\n");
        else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
          ;
        else {
          if (cdoc_glossary_list->compact)
            CDocScriptSkipLine();
        }

        /* Remove the Glossary List and reset the current
           Glossary List */

        cdoc_glossary_list = CDocScriptEndGlossaryList();
      }
      else
        CDocScriptWarning("Invalid End Glossary List");
    }

    /* Headings */

    else if (colon_command->getCommand()[0] == 'h' &&
             isdigit(colon_command->getCommand()[1])) {
      CDHeading  heading;

      heading.ident  = NULL;
      heading.stitle = NULL;

      /* Spell Check if Required */

      if (cdoc_spell_check && cdoc_spell_active)
        CSpellCheckString(colon_command->getText());

      /* Calculate the Heading Level */

      level = (colon_command->getCommand()[1] - '0') % CDOC_MAX_HEADERS;

      /* Process Parameter List */

      heading.ident  = NULL;
      heading.stitle = NULL;

      if (level == 0 || level == 1)
        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   h01_parameter_data,
                                   reinterpret_cast<char *>(&heading));
      else
        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   h26_parameter_data,
                                   reinterpret_cast<char *>(&heading));

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        if (header_control[level].new_page)
          CDocScriptStartSection(colon_command->getText());

        CDocScriptWriteCommand("<h%d>%s</h%d>\n",
          level, CDocEncodeHtmlString(colon_command->getText()), level);

        return;
      }
      else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        if (header_control[level].new_page)
          CDocScriptStartSection(colon_command->getText());

        CDocScriptWriteCommand("<\"head\">\n");

        CDocScriptWriteCommand("<|,\"%d\">%s\n", level,
          CDocEncodeILeafString(colon_command->getText()));

        return;
      }

      /* Set Left Margin */

      if (! header_control[level].toc_only) {
        if      (level <= 1)
          cdoc_left_margin = cdoc_save_left_margin;
        else if (level < 5)
          cdoc_left_margin = cdoc_save_left_margin + 2*(level - 1);
      }

      /* Add Space before Header Text */

      strcpy(temp_string, "");

      for (i = 0; i < header_control[level].space_before; i++)
        strcat(temp_string, " ");

      /* If the Header is numbered and is in the body or
         appendix section then add the number text */

      if (header_control[level].number_header &&
          (cdoc_document.part == CDOC_BODY_PART ||
           cdoc_document.part == CDOC_APPENDIX_PART)) {
        int  header_no;

        CDocScriptStartHeader(level);

        /* Create Header number from the Depth of each Header Level
           below and equal to the current Header Level's Depth */

        for (i = 1; i <= level; i++) {
          header_no = CDocScriptGetHeader(i);

          /* Use Alphabetic Characters for the Appendix */

          char temp_string1[256];

          if (i == 1) {
            if (cdoc_document.part == CDOC_APPENDIX_PART)
              sprintf(temp_string1, "Appendix %c",
                      cdoc_a_to_z[(header_no - appendix_depth - 1) % 26]);
            else
              sprintf(temp_string1, "%d", header_no);
          }
          else
            sprintf(temp_string1, ".%d", header_no);

          strcat(temp_string, temp_string1);
        }

        /* Add '.0' for First Level Normal Headers and '.'
           for First Level Appendix Headers */

        if (level == 1) {
          if (cdoc_document.part != CDOC_APPENDIX_PART)
            strcat(temp_string, ".0");
          else
            strcat(temp_string, ".");
        }

        /* Add spacing between Number Text and Header Text */

        if (level > 0)
          strcat(temp_string, "  ");
      }

      /* Convert to Upper Case if Header is Capitalised */

      if (header_control[level].capitalised)
        CStrUtil::toUpper(colon_command->getText());

      /* Use Header Number if Global Switch is On */

      std::string temp_string1;

      if (cdoc_number_headers)
        temp_string1 = temp_string + colon_command->getText();
      else
        temp_string1 = colon_command->getText();

      /* Add any space required after header */

      for (i = 0; i < header_control[level].space_after; i++)
        temp_string1 += " ";

      /* If Header appears on its own then output as a title,
         otherwise add to the current text */

      if (header_control[level].break_header) {
        /* Reset Indentation */

        cdoc_indent = 0;

        /* If new page required start a new section of the
           document (also causes a page throw), otherwise
           output header on separate line as a sub-section */

        if (header_control[level].new_page)
          CDocScriptStartSection(colon_command->getText());
        else {
          if (CDocIsPagedOutput()) {
            //int lines_per_page = CDocScriptGetLinesPerPage();

            int new_page = CDocScriptFindSpace(header_control[level].skip_before + 3);

            if (! new_page) {
              if (cdoc_paragraph_done)
                CDocScriptSkipLine();

              /* Output required number of blank lines
                 before header */

              for (i = 0; i < header_control[level].skip_before - 1; i++)
                CDocScriptSkipLine();
            }
          }
          else {
            if (cdoc_paragraph_done)
              CDocScriptSkipLine();

            /* Output required number of blank lines before header */

            for (i = 0; i < header_control[level].skip_before - 1; i++)
              CDocScriptSkipLine();
          }

          std::string temp_string = "  " + colon_command->getText();

          CDocScriptStartSubSection(temp_string);
        }

        /* If header only appears in table of contents then
           don't output the header in the main text */

        if (! header_control[level].toc_only) {
          CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

          CStrUtil::stripSpaces(temp_string1);

          if (header_control[level].underscored)
            CDocScriptWriteText("%s%s%s\n",
              CDocStartUnderline(), temp_string1.c_str(), CDocEndUnderline());
          else
            CDocScriptWriteText("%s%s%s\n",
              CDocStartBold(), temp_string1.c_str(), CDocEndBold());

          if (header_control[level].underscored &&
              CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
            CDocScriptDrawHLine(cdoc_indent,
             CDocStringDisplayLength(temp_string1) - cdoc_indent - 1);

          CDocScriptSkipLine();

          cdoc_paragraph_done = false;
        }

        cdoc_left_margin++;
      }
      else {
        /* If header only appears in table of contents then
           don't add the header as a new paragraph */

        if (! header_control[level].toc_only) {
          std::string temp_string = temp_string1 + "  ";

          if (header_control[level].underscored)
            CDocScriptSetParagraphPrefix(temp_string, CDOC_BOLD_UNDERLINE_FONT, false);
          else
            CDocScriptSetParagraphPrefix(temp_string, CDOC_BOLD_FONT, false);
        }
      }

      /* If we have a reference identifier then set its
         Page Number */

      if (heading.ident != NULL)
        CDocScriptSetReferencePageNumber(HEADER_REF, heading.ident);

      CDocScriptSetTOCPage();
    }

    /* Notes */

    else if (colon_command->getCommand() == "note") {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        CDocScriptWriteCommand("<p><b>Note:</b> %s\n",
          CDocEncodeHtmlString(colon_command->getText()));
        return;
      }
      else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        CDocScriptWriteCommand("<\"para\">\n");

        CDocScriptWriteCommand("%s\n",
          CDocEncodeILeafString(colon_command->getText()));

        return;
      }

      /* Set Paragraph Prefix to 'Note:' Text */

      strcpy(temp_string, "Note: ");

      CDocScriptSetParagraphPrefix(temp_string, CDOC_BOLD_FONT, true);

      /* Set Paragraph to Note Text */

      if (CDocScriptGetCurrentListCompact())
        CDocScriptSkipLine();

      if (CDocScriptIsCurrentList())
        CDocScriptNewParagraph(colon_command->getText(), SUB_LIST_PARAGRAPH1,
                               true, LEFT_JUSTIFICATION);
      else
        CDocScriptNewParagraph(colon_command->getText(), NORMAL_PARAGRAPH,
                               true, LEFT_JUSTIFICATION);
    }

    /* Table Row Definition (does not use CDocExtractParameterValues
       as this cannot handle arrays of values) */

    else if (colon_command->getCommand() == "rdef") {
      /* Allocate Row Definition Structure */

      CDTableRowDef *row_definition = CDocScriptCreateTableRowDef();

      /* Set structure values from Parameter/Value pairs */

      for (uint i = 0; i < colon_command->getNumParameters(); i++) {
        if      (colon_command->getParameter(i) == "id")
          row_definition->name = CStrUtil::strdup(colon_command->getValue(i));
        else if (colon_command->getParameter(i) == "hp") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0)
            row_definition->highlights = new int [no_words];
          else
            row_definition->highlights = NULL;

          row_definition->no_highlights = no_words;

          for (uint j = 0; j < no_words; j++)
            row_definition->highlights[j] = CStrUtil::toInteger(words[j]) % 4;
        }
        else if (colon_command->getParameter(i) == "align") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0)
            row_definition->aligns = new int [no_words];
          else
            row_definition->aligns = NULL;

          row_definition->no_aligns = no_words;

          for (uint j = 0; j < no_words; j++) {
            CStrUtil::toLower(words[j]);

            if      (words[j] == "left")
              row_definition->aligns[j] = CHALIGN_TYPE_LEFT;
            else if (words[j] == "center")
              row_definition->aligns[j] = CHALIGN_TYPE_CENTRE;
            else if (words[j] == "centre")
              row_definition->aligns[j] = CHALIGN_TYPE_CENTRE;
            else if (words[j] == "right")
              row_definition->aligns[j] = CHALIGN_TYPE_RIGHT;
            else if (words[j] == "inside")
              row_definition->aligns[j] = CHALIGN_TYPE_INSIDE;
            else if (words[j] == "outside")
              row_definition->aligns[j] = CHALIGN_TYPE_OUTSIDE;
            else if (words[j] == "justify")
              row_definition->aligns[j] = CHALIGN_TYPE_JUSTIFY;
            else {
              CDocScriptWarning("Invalid Value for Row Definition Align '%s'", words[j].c_str());

              row_definition->aligns[j] = CHALIGN_TYPE_LEFT;
            }
          }
        }
        else if (colon_command->getParameter(i) == "concat") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0)
            row_definition->concats = new int [no_words];
          else
            row_definition->concats = NULL;

          row_definition->no_concats = no_words;

          for (uint j = 0; j < no_words; j++) {
            CStrUtil::toLower(words[j]);

            if      (words[j] == "yes")
              row_definition->concats[j] = true;
            else if (words[j] == "no")
              row_definition->concats[j] = false;
            else {
              CDocScriptWarning( "Invalid Value for Row Definition Concat '%s'", words[j].c_str());

              row_definition->concats[j] = true;
            }
          }
        }
        else if (colon_command->getParameter(i) == "valign") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0)
            row_definition->valigns = new int [no_words];
          else
            row_definition->valigns = NULL;

          row_definition->no_valigns = no_words;

          for (uint j = 0; j < no_words; j++) {
            CStrUtil::toLower(words[j]);

            if      (words[j] == "top")
              row_definition->valigns[j] = CVALIGN_TYPE_TOP;
            else if (words[j] == "center")
              row_definition->valigns[j] = CVALIGN_TYPE_CENTRE;
            else if (words[j] == "centre")
              row_definition->valigns[j] = CVALIGN_TYPE_CENTRE;
            else if (words[j] == "bottom")
              row_definition->valigns[j] = CVALIGN_TYPE_BOTTOM;
            else {
              CDocScriptWarning( "Invalid Value for Row Definition Align '%s'", words[j].c_str());

              row_definition->valigns[j] = CVALIGN_TYPE_TOP;
            }
          }
        }
        else if (colon_command->getParameter(i) == "rotate") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0)
            row_definition->rotates = new int [no_words];
          else
            row_definition->rotates = NULL;

          row_definition->no_rotates = no_words;

          for (uint j = 0; j < no_words; j++) {
            row_definition->rotates[j] = CStrUtil::toInteger(words[j]) % 360;

            if (row_definition->rotates[j] !=  0   && row_definition->rotates[j] !=  90  &&
                row_definition->rotates[j] !=  180 && row_definition->rotates[j] !=  270 &&
                row_definition->rotates[j] != -90  && row_definition->rotates[j] != -180 &&
                row_definition->rotates[j] != -270) {
              CDocScriptWarning("Invalid Value for Row Definition Rotation '%s'",
                                words[j].c_str());

              row_definition->rotates[j] = 0;
            }
          }
        }
        else if (colon_command->getParameter(i) == "mindepth") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0)
            row_definition->mindepths = new int [no_words];
          else
            row_definition->mindepths = NULL;

          row_definition->no_mindepths = no_words;

          for (uint j = 0; j < no_words; j++)
            row_definition->mindepths[j] = CStrUtil::toInteger(words[j]);
        }
        else if (colon_command->getParameter(i) == "arrange") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0) {
            int  no_arrange = 0;
            int *arrange    = new int [no_words];

            for (uint j = 0; j < no_words; j++) {
              if (words[j] == "/") {
                row_definition->arrange_list   .push_back(arrange);
                row_definition->no_arrange_list.push_back(no_arrange);

                no_arrange = 0;
                arrange    = new int [no_words - j - 1];
              }
              else
                arrange[no_arrange++] = CStrUtil::toInteger(words[j]);
            }

            row_definition->arrange_list   .push_back(arrange);
            row_definition->no_arrange_list.push_back(no_arrange);
          }
        }
        else if (colon_command->getParameter(i) == "cwidths") {
          std::vector<std::string> words;

          CDocStringToWords(colon_command->getValue(i), words);

          uint no_words = words.size();

          if (no_words > 0)
            row_definition->cwidths = new int [no_words];
          else
            row_definition->cwidths = NULL;

          row_definition->no_cwidths = no_words;

          for (uint j = 0; j < no_words; j++) {
            if (words[j][words[j].size() - 1] == '*') {
              words[j] = words[j].substr(0, words[j].size() - 1);

              if (words[j].size() == 0)
                row_definition->cwidths[j] = -1;
              else
                row_definition->cwidths[j] = -CStrUtil::toInteger(words[j]);

              if (row_definition->cwidths[j] == 0)
                row_definition->cwidths[j] = -1;
            }
            else
              row_definition->cwidths[j] =
               CDocLengthStringToChars(words[j]);
          }
        }
        else if (colon_command->getParameter(i) == "shade") {
          if      (CStrUtil::casecmp(colon_command->getValue(i), "xlight") == 0)
            row_definition->shade = SHADE_XLIGHT;
          else if (CStrUtil::casecmp(colon_command->getValue(i), "light") == 0)
            row_definition->shade = SHADE_LIGHT;
          else if (CStrUtil::casecmp(colon_command->getValue(i), "medium") == 0)
            row_definition->shade = SHADE_MEDIUM;
          else if (CStrUtil::casecmp(colon_command->getValue(i), "dark") == 0)
            row_definition->shade = SHADE_DARK;
          else if (CStrUtil::casecmp(colon_command->getValue(i), "xdark") == 0)
            row_definition->shade = SHADE_XDARK;
          else
            CDocScriptWarning("Invalid Value '%s' for Shade",
                              colon_command->getValue(i).c_str());
        }
        else
          CDocScriptWarning("Invalid Parameter '%s' for Command '%s'",
                            colon_command->getParameter(i).c_str(),
                            colon_command->getCommand().c_str());
      }

      if (row_definition->name != "" && row_definition->no_cwidths > 0) {
        /* Set Width of Non-Free Cells and Count of Free Cells
           and Total Cells in Free Cells */

        row_definition->width       = 1;
        row_definition->free_cells1 = 0;
        row_definition->free_cells2 = 0;

        for (j = 0; j < row_definition->no_cwidths; j++) {
          if (row_definition->cwidths[j] < 0) {
            row_definition->free_cells1++;
            row_definition->free_cells2 += -row_definition->cwidths[j];

            row_definition->width += 2;
          }
          else
            row_definition->width += row_definition->cwidths[j] + 1;
        }

        CDocScriptAddTableRowDef(row_definition);
      }
      else {
        delete row_definition;

        if (row_definition->name == "")
          CDocScriptError("Invalid Row Definition - No Name");
        else
          CDocScriptError("Invalid Row Definition - No Cell Widths");
      }
    }

    /* Table */

    else if (colon_command->getCommand() == "table") {
      if (current_table != NULL) {
        CDocScriptWarning("Unterminated Table");

        CDocScriptOutputTable(current_table);

        if (current_table->id != "")
          CDocScriptSetReferencePageNumber(TABLE_REF, current_table->id);

        CDocScriptSetTablePage(current_table);
      }

      /* Process Table (Details already recorded in Pass 1) */

      table_number++;

      /* Get Pass 1 Table Details */

      current_table = CDocScriptGetTable(table_number);

      if (current_table != NULL) {
        /* Set the Table's Row Definition */

        CDocScriptSetTableRowDef(current_table);

        if (current_table->row_definition == NULL) {
          CDocScriptError("Table has no Row Definition");

          current_table = NULL;
        }

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
          CDocScriptWriteCommand("<table border>\n");
          return;
        }
      }
      else
        CDocScriptError("Table Not Found - Internal Error");
    }

    /* Long Quote */

    else if (colon_command->getCommand() == "lq") {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        CDocScriptWriteCommand("<blockquote>\n");
        return;
      }

      long_quotation_depth++;

      if (cdoc_right_margin - cdoc_left_margin >= 8) {
        cdoc_left_margin  += 2;
        cdoc_right_margin -= 2;
      }
    }
    else if (colon_command->getCommand() == "elq") {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        CDocScriptWriteCommand("</blockquote>\n");
        return;
      }

      if (long_quotation_depth == 0)
        CDocScriptWarning("End Long Quotation outside Long Quotation");
      else {
        long_quotation_depth--;

        if (cdoc_right_margin - cdoc_left_margin >= 4) {
          cdoc_left_margin  -= 2;
          cdoc_right_margin += 2;
        }
      }
    }

    /* Paragraph */

    else if (colon_command->getCommand() == "p") {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        CDocScriptWriteCommand("<p>%s\n",
         CDocEncodeHtmlString(colon_command->getText()));
        return;
      }
      else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        CDocScriptWriteCommand("<\"para\">\n");

        CDocScriptWriteCommand("%s\n",
          CDocEncodeILeafString(colon_command->getText()));

        return;
      }

      if (cdoc_paragraph->getPrefix() != "") {
        std::string temp_string = cdoc_paragraph->getPrefix();

        no_spaces = 0;

        j = temp_string.size() - 1;

        while (j >= 0 && isspace(temp_string[j])) {
          ++no_spaces;

          temp_string = temp_string.substr(0, j--);
        }

        temp_string += ":";

        for (j = 0; j < no_spaces; j++)
          temp_string += " ";

        cdoc_paragraph->setPrefix(temp_string);
      }

      if (CDocScriptIsCurrentList())
        CDocScriptNewParagraph(colon_command->getText(), SUB_LIST_PARAGRAPH1,
                               true, LEFT_JUSTIFICATION);
      else
        CDocScriptNewParagraph(colon_command->getText(), NORMAL_PARAGRAPH,
                               true, LEFT_JUSTIFICATION);
    }

    /* Paragraph Continuation */

    else if (colon_command->getCommand() == "pc") {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        CDocScriptWriteCommand("<p>%s\n",
          CDocEncodeHtmlString(colon_command->getText()));
        return;
      }
      else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        CDocScriptWriteCommand("<\"para\">\n");

        CDocScriptWriteCommand("%s\n",
          CDocEncodeILeafString(colon_command->getText()));

        return;
      }

      CDocScriptNewParagraph(colon_command->getText(), NORMAL_PARAGRAPH,
                             true, LEFT_JUSTIFICATION);
    }

    /* List Paragraph */

    else if (colon_command->getCommand() == "lp") {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
        CDocScriptWriteCommand("<p>%s\n",
         CDocEncodeHtmlString(colon_command->getText()));
        return;
      }
      else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        CDocScriptWriteCommand("<\"para\">\n");

        CDocScriptWriteCommand("%s\n",
          CDocEncodeILeafString(colon_command->getText()));

        return;
      }

      /* Label this Paragraph as a List Paragraph so it
         can be formatted correctly */

      if (CDocScriptIsCurrentList())
        CDocScriptNewParagraph(colon_command->getText(), SUB_LIST_PARAGRAPH2,
                               true, LEFT_JUSTIFICATION);
      else {
        CDocScriptError("List Paragraph Outside List");

        CDocScriptNewParagraph(colon_command->getText(), NORMAL_PARAGRAPH,
                               true, LEFT_JUSTIFICATION);
      }
    }

    /* Footnotes */

    else if (colon_command->getCommand() == "fn") {
      if (current_footnote != NULL) {
        CDocScriptWarning("Unterminated Footnote");

        CDocScriptEndFootnote(current_footnote);

        if (current_footnote->ident != "")
          CDocScriptSetReferencePageNumber(FOOTNOTE_REF, current_footnote->ident);
      }

      current_footnote = CDocScriptCreateFootnote();

      /* Get Footnote Parameter Values */

      CDocExtractParameterValues(colon_command->getParameters(),
                                 colon_command->getValues(),
                                 fn_parameter_data,
                                 reinterpret_cast<char *>(current_footnote));

      /* Start Footnote Processing */

      CDocScriptStartFootnote(current_footnote);

      /* Add Command Text (if any) */

      CDocScriptAddStringToParagraph(colon_command->getText());
    }
    else if (colon_command->getCommand() == "efn") {
      if (current_footnote != NULL) {
        /* End Footnote Processing */

        CDocScriptEndFootnote(current_footnote);

        if (current_footnote->ident != "")
          CDocScriptSetReferencePageNumber(FOOTNOTE_REF, current_footnote->ident);

        current_footnote = NULL;
      }
      else
        CDocScriptError("End Footnote outside Footnote");
    }

    /* Examples */

    else if (colon_command->getCommand() == "xmp") {
      if (current_example == NULL) {
        current_example = CDocScriptCreateExample();

        /* Get Parameter Values */

        CDocExtractParameterValues(colon_command->getParameters(),
                                   colon_command->getValues(),
                                   xmp_parameter_data,
                                   reinterpret_cast<char *>(current_example));

        /* Start Example */

        CDocScriptExampleBegin(current_example);
      }
      else
        CDocScriptError("Cannot have Example inside Example");
    }
    else if (colon_command->getCommand() == "exmp") {
      if (current_example != NULL) {
        /* End Example */

        CDocScriptExampleEnd(current_example);

        current_example = NULL;
      }
      else
        CDocScriptError("End Example when not in Example");
    }

    /* Page Throws */

    else if (colon_command->getCommand() == "pa")
      CDocScriptNewPage();

    /* Ignore EDS setup tag */

    else if (colon_command->getCommand() == "doceds")
      ;

    else
      CDocScriptWarning("Invalid Colon Command - %s %s",
                        colon_command->getCommand().c_str(), colon_command->getText().c_str());
  }

  /* Ignore EDS setup tag */

  else if (colon_command->getCommand() == "doceds")
    ;

  /* Unrecognized Command */

  else
    CDocScriptWarning("Invalid Colon Command - %s %s",
                      colon_command->getCommand().c_str(), colon_command->getText().c_str());
}

// Process any inline colon commands and replace then with the required text
// or control characters.
static void
CDocReplaceEmbeddedColonCommands(CDScriptLine *script_line)
{
  /* Get Script Line's Text */

  std::string text;

  if      (script_line->getType() == CDOC_DOT_COMMAND)
    text = script_line->getDotCommand()->getText();
  else if (script_line->getType() == CDOC_COLON_COMMAND)
    text = script_line->getColonCommand()->getText();
  else
    text = script_line->getData();

  const char *p1 = text.c_str();

  /* Get Length of New Line */

  bool changed = false;

  int i = 0;

  while (*p1 != '\0') {
    /* Citation */

    if      (CStrUtil::casencmp(p1, ":cit", 4) == 0 &&
             (p1[4] == '.' || isspace(p1[4]) || p1[4] == '\0')) {
      /* Insert Bold Escape Code if Output Format can handle it */

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
        i += strlen(CDocStartBold());

      /* Skip Command */

      if (p1[4] == '.')
        p1 += 5;
      else
        p1 += 4;

      changed = true;
    }

    /* End Citation */

    else if (CStrUtil::casencmp(p1, ":ecit", 5) == 0 &&
             (p1[5] == '.' || isspace(p1[5]) || p1[5] == '\0')) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
        i += strlen(CDocEndBold());

      /* Skip Command */

      if (p1[5] == '.')
        p1 += 6;
      else
        p1 += 5;

      changed = true;
    }

    /* Highlighting */

    else if (CStrUtil::casencmp(p1, ":hp", 3) == 0 && isdigit(p1[3]) &&
             (p1[4] == '.' || isspace(p1[4]) || p1[4] == '\0')) {
      int level = (p1[3] - '0') % 4;

      const char *str = CDocScriptStartParagraphHighlight(level);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
        i += strlen(str);

      /* Skip Command */

      if (p1[4] == '.')
        p1 += 5;
      else
        p1 += 4;

      changed = true;
    }

    /* End Highlighting */

    else if (CStrUtil::casencmp(p1, ":ehp", 4) == 0 && isdigit(p1[4]) &&
             (p1[5] == '.' || isspace(p1[5]) || p1[5] == '\0')) {
      int level = (p1[4] - '0') % 4;

      const char *str = CDocScriptEndParagraphHighlight(level);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
        i += strlen(str);

      /* Skip Command */

      if (p1[5] == '.')
        p1 += 6;
      else
        p1 += 5;

      changed = true;
    }

    /* Cross References */

    else if (CStrUtil::casencmp(p1, ":figref", 7) == 0 ||
             CStrUtil::casencmp(p1, ":fnref" , 6) == 0 ||
             CStrUtil::casencmp(p1, ":hdref" , 6) == 0 ||
             CStrUtil::casencmp(p1, ":iref"  , 5) == 0 ||
             CStrUtil::casencmp(p1, ":liref" , 6) == 0 ||
             CStrUtil::casencmp(p1, ":tref"  , 5) == 0) {
      int              type = 0;
      CDParameterData *parameter_data;

      const char *p2 = p1;

      if      (CStrUtil::casencmp(p1, ":figref", 7) == 0) {
        type           = FIGURE_REF;
        parameter_data = figref_parameter_data;

        p1 += 7;
      }
      else if (CStrUtil::casencmp(p1, ":fnref" , 6) == 0) {
        type           = FOOTNOTE_REF;
        parameter_data = fnref_parameter_data;

        p1 += 6;
      }
      else if (CStrUtil::casencmp(p1, ":hdref" , 6) == 0) {
        type           = HEADER_REF;
        parameter_data = hdref_parameter_data;

        p1 += 6;
      }
      else if (CStrUtil::casencmp(p1, ":iref"  , 5) == 0) {
        type           = INDEX_REF;
        parameter_data = iref_parameter_data;

        p1 += 5;
      }
      else if (CStrUtil::casencmp(p1, ":liref" , 6) == 0) {
        type           = LIST_ITEM_REF;
        parameter_data = liref_parameter_data;

        p1 += 6;
      }
      else if (CStrUtil::casencmp(p1, ":tref"  , 5) == 0) {
        type           = TABLE_REF;
        parameter_data = tref_parameter_data;

        p1 += 5;
      }

      /* Skip Spaces after Command */

      CStrUtil::skipSpace(&p1);

      /* Extract Reference Values */

      bool reference_done = false;

      CDCrossRef cross_ref;
      CDIndexRef index_ref;

      if (type == INDEX_REF) {
        index_ref.refid = NULL;
        index_ref.pg    = NULL;
        index_ref.see   = NULL;
        index_ref.seeid = NULL;
      }
      else {
        cross_ref.type  = type;
        cross_ref.refid = NULL;
        cross_ref.page  = true;
      }

      int j = 0;

      while (! reference_done && *p1 != '\0' &&
             parameter_data[j].name != NULL) {
        int len = strlen(parameter_data[j].name);

        if (CStrUtil::casencmp(p1, parameter_data[j].name, len) == 0 &&
            p1[len] == '=') {
          p1 += len + 1;

          /* Get Reference Value (may be quoted) */

          int k = 0;

          char temp_string[256];

          if (*p1 == '\'') {
            p1++;

            while (*p1 != '\0' && *p1 != '\'')
              temp_string[k++] = *p1++;

            if (*p1 == '\'')
              p1++;
          }
          else {
            while (*p1 != '\0' && *p1 != '.' && isalnum(*p1))
              temp_string[k++] = *p1++;
          }

          temp_string[k] = '\0';

          std::string temp_string1 = CDocScriptReplaceSymbolsInString(temp_string);

          if (type == INDEX_REF)
            CDocExtractParameterValue(temp_string1, &parameter_data[j],
                                      reinterpret_cast<char *>(&index_ref));
          else
            CDocExtractParameterValue(temp_string1, &parameter_data[j],
                                      reinterpret_cast<char *>(&cross_ref));

          /* Skip Space after Reference Value */

          CStrUtil::skipSpace(&p1);

          /* Reference Terminated */

          if (*p1 == '\0' || *p1 == '.') {
            reference_done = true;

            if (*p1 == '.')
              p1++;
          }

          j = 0;
        }
        else
          j++;
      }

      if      (type == INDEX_REF && index_ref.refid != NULL) {
        changed = true;

        delete [] index_ref.refid;
      }
      else if (cross_ref.refid != NULL) {
        /* Make sure Reference Identifier is in Lower Case */

        CStrUtil::toLower(cross_ref.refid);

        /* Get Reference Text for this Identifier */

        std::string reference = CDocScriptGetParagraphReferenceText(&cross_ref);

        /* Add Reference Text to Paragraph */

        i += reference.size();

        if (! reference_done)
          i++;

        changed = true;

        delete [] cross_ref.refid;
      }
      else {
        p1 = p2;

        i++;

        p1++;
      }
    }

    /* Hook */

    else if (CStrUtil::casencmp(p1, ":hook", 5) == 0) {
      CDHookParameterData hook_data;

      const char *p2 = p1;

      p1 += 5;

      /* Skip Spaces after Command */

      CStrUtil::skipSpace(&p1);

      /* Extract Hook Values */

      bool done = false;

      int j = 0;

      while (! done && *p1 != '\0' &&
             hook_parameter_data[j].name != NULL) {
        int len = strlen(hook_parameter_data[j].name);

        if (CStrUtil::casencmp(p1, hook_parameter_data[j].name, len) == 0 &&
            p1[len] == '=') {
          p1 += len + 1;

          /* Get Hook Value (may be quoted) */

          int k = 0;

          char temp_string[256];

          if (*p1 == '\'') {
            p1++;

            while (*p1 != '\0' && *p1 != '\'')
              temp_string[k++] = *p1++;

            if (*p1 == '\'')
              p1++;
          }
          else {
            while (*p1 != '\0' && *p1 != '.' && isalnum(*p1))
              temp_string[k++] = *p1++;
          }

          temp_string[k] = '\0';

          std::string temp_string1 = CDocScriptReplaceSymbolsInString(temp_string);

          CDocExtractParameterValue(temp_string1, &hook_parameter_data[j],
                                    reinterpret_cast<char *>(&hook_data));

          /* Skip Space after Reference Value */

          CStrUtil::skipSpace(&p1);

          /* Hook Terminated */

          if (*p1 == '\0' || *p1 == '.') {
            done = true;

            if (*p1 == '.')
              p1++;
          }

          j = 0;
        }
        else
          j++;
      }

      if (hook_data.id != NULL) {
        CDHookData hook_data1(hook_data.id, hook_data.data, hook_data.text);

        /* Add Hook Text to Paragraph */

        std::string text = CDocScriptGetHookText(&hook_data1);

        i += text.size();

        if (! done)
          i++;

        changed = true;
      }
      else {
        p1 = p2;

        i++;

        p1++;
      }
    }

    /* Start Quotation */

    else if (CStrUtil::casencmp(p1, ":q", 2) == 0 &&
             (p1[2] == '.' || isspace(p1[2]) || p1[2] == '\0')) {
      /* Opening Quotation Mark */

      i++;

      /* Skip Command */

      if (p1[2] == '.')
        p1 += 3;
      else
        p1 += 2;

      changed = true;
    }

    /* End Citation */

    else if (CStrUtil::casencmp(p1, ":eq", 3) == 0 &&
             (p1[3] == '.' || isspace(p1[3]) || p1[3] == '\0')) {
      /* Closing Quotation Mark */

      i++;

      /* Skip Command */

      if (p1[3] == '.')
        p1 += 4;
      else
        p1 += 3;

      changed = true;
    }

    /* New Line */

    else if (CStrUtil::casencmp(p1, ":nl", 3) == 0 &&
             (p1[3] == '.' || isspace(p1[3]) || p1[3] == '\0')) {
      /* Newline Character */

      i++;

      /* Skip Command */

      if (p1[3] == '.')
        p1 += 4;
      else
        p1 += 3;

      changed = true;
    }

    /* Break */

    else if (CStrUtil::casencmp(p1, ":br", 3) == 0 &&
             (p1[3] == '.' || isspace(p1[3]) || p1[3] == '\0')) {
      /* Newline Character */

      i++;

      /* Skip Command */

      if (p1[3] == '.')
        p1 += 4;
      else
        p1 += 3;

      changed = true;
    }

    else {
      i++;

      p1++;
    }
  }

  /* If the line hasn't changed then just return */

  if (! changed)
    return;

  /* Allocate line big enough to take new text */

  char *line1 = new char [i + 2];

  /* Get Script Line's Text */

  if      (script_line->getType() == CDOC_DOT_COMMAND)
    p1 = const_cast<char *>(script_line->getDotCommand()->getText().c_str());
  else if (script_line->getType() == CDOC_COLON_COMMAND)
    p1 = const_cast<char *>(script_line->getColonCommand()->getText().c_str());
  else
    p1 = const_cast<char *>(script_line->getData().c_str());

  /* Set New Line */

  i = 0;

  while (*p1 != '\0') {
    /* Citation */

    if      (CStrUtil::casencmp(p1, ":cit", 4) == 0 &&
             (p1[4] == '.' || isspace(p1[4]) || p1[4] == '\0')) {
      /* Insert Bold Escape Code if Output Format can handle it */

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        strcpy(&line1[i], CDocStartBold());

        i += strlen(CDocStartBold());
      }

      /* Skip Command */

      if (p1[4] == '.')
        p1 += 5;
      else
        p1 += 4;
    }

    /* End Citation */

    else if (CStrUtil::casencmp(p1, ":ecit", 5) == 0 &&
             (p1[5] == '.' || isspace(p1[5]) || p1[5] == '\0')) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        strcpy(&line1[i], CDocEndBold());

        i += strlen(CDocEndBold());
      }

      /* Skip Command */

      if (p1[5] == '.')
        p1 += 6;
      else
        p1 += 5;
    }

    /* Highlighting */

    else if (CStrUtil::casencmp(p1, ":hp", 3) == 0 && isdigit(p1[3]) &&
             (p1[4] == '.' || isspace(p1[4]) || p1[4] == '\0')) {
      int level = (p1[3] - '0') % 4;

      const char *str = CDocScriptStartParagraphHighlight(level);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        for (uint j = 0; j < strlen(str); j++)
          line1[i++] = str[j];
      }

      /* Skip Command */

      if (p1[4] == '.')
        p1 += 5;
      else
        p1 += 4;
    }

    /* End Highlighting */

    else if (CStrUtil::casencmp(p1, ":ehp", 4) == 0 && isdigit(p1[4]) &&
             (p1[5] == '.' || isspace(p1[5]) || p1[5] == '\0')) {
      int level = (p1[4] - '0') % 4;

      const char *str = CDocScriptEndParagraphHighlight(level);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML ||
          CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
        for (uint j = 0; j < strlen(str); j++)
          line1[i++] = str[j];
      }

      /* Skip Command */

      if (p1[5] == '.')
        p1 += 6;
      else
        p1 += 5;
    }

    /* Cross References */

    else if (CStrUtil::casencmp(p1, ":figref", 7) == 0 ||
             CStrUtil::casencmp(p1, ":fnref" , 6) == 0 ||
             CStrUtil::casencmp(p1, ":hdref" , 6) == 0 ||
             CStrUtil::casencmp(p1, ":iref"  , 5) == 0 ||
             CStrUtil::casencmp(p1, ":liref" , 6) == 0 ||
             CStrUtil::casencmp(p1, ":tref"  , 5) == 0) {
      int              type = 0;
      CDParameterData *parameter_data;

      const char *p2 = p1;

      if      (CStrUtil::casencmp(p1, ":figref", 7) == 0) {
        type           = FIGURE_REF;
        parameter_data = figref_parameter_data;

        p1 += 7;
      }
      else if (CStrUtil::casencmp(p1, ":fnref" , 6) == 0) {
        type           = FOOTNOTE_REF;
        parameter_data = fnref_parameter_data;

        p1 += 6;
      }
      else if (CStrUtil::casencmp(p1, ":hdref" , 6) == 0) {
        type           = HEADER_REF;
        parameter_data = hdref_parameter_data;

        p1 += 6;
      }
      else if (CStrUtil::casencmp(p1, ":iref"  , 5) == 0) {
        type           = INDEX_REF;
        parameter_data = iref_parameter_data;

        p1 += 5;
      }
      else if (CStrUtil::casencmp(p1, ":liref" , 6) == 0) {
        type           = LIST_ITEM_REF;
        parameter_data = liref_parameter_data;

        p1 += 6;
      }
      else if (CStrUtil::casencmp(p1, ":tref"  , 5) == 0) {
        type           = TABLE_REF;
        parameter_data = tref_parameter_data;

        p1 += 5;
      }

      /* Skip Spaces after Command */

      CStrUtil::skipSpace(&p1);

      /* Extract Reference Values */

      bool reference_done = false;

      CDCrossRef cross_ref;
      CDIndexRef index_ref;

      if (type == INDEX_REF) {
        index_ref.refid = NULL;
        index_ref.pg    = NULL;
        index_ref.see   = NULL;
        index_ref.seeid = NULL;
      }
      else {
        cross_ref.type  = type;
        cross_ref.refid = NULL;
        cross_ref.page  = true;
      }

      int j = 0;

      while (! reference_done && *p1 != '\0' &&
             parameter_data[j].name != NULL) {
        int len = strlen(parameter_data[j].name);

        if (CStrUtil::casencmp(p1, parameter_data[j].name, len) == 0 &&
            p1[len] == '=') {
          p1 += len + 1;

          /* Get Reference Value (may be quoted) */

          int k = 0;

          char temp_string[256];

          if (*p1 == '\'') {
            p1++;

            while (*p1 != '\0' && *p1 != '\'')
              temp_string[k++] = *p1++;

            if (*p1 == '\'')
              p1++;
          }
          else {
            while (*p1 != '\0' && *p1 != '.' && isalnum(*p1))
              temp_string[k++] = *p1++;
          }

          temp_string[k] = '\0';

          std::string temp_string1 = CDocScriptReplaceSymbolsInString(temp_string);

          if (type == INDEX_REF)
            CDocExtractParameterValue(temp_string1, &parameter_data[j],
                                      reinterpret_cast<char *>(&index_ref));
          else
            CDocExtractParameterValue(temp_string1, &parameter_data[j],
                                      reinterpret_cast<char *>(&cross_ref));

          /* Skip Space after Reference Value */

          CStrUtil::skipSpace(&p1);

          /* Reference Terminated */

          if (*p1 == '\0' || *p1 == '.') {
            reference_done = true;

            if (*p1 == '.')
              p1++;
          }

          j = 0;
        }
        else
          j++;
      }

      if      (type == INDEX_REF && index_ref.refid != NULL) {
        CDocScriptAddIndexRef(index_ref.refid, index_ref.pg, index_ref.see, index_ref.seeid);

        delete [] index_ref.refid;
      }
      else if (cross_ref.refid != NULL) {
        /* Make sure Reference Identifier is in Lower Case */

        CStrUtil::toLower(cross_ref.refid);

        /* Get Reference Text for Paragraph for this Identifier */

        std::string reference = CDocScriptGetParagraphReferenceText(&cross_ref);

        /* Add Reference Text to the Paragraph */

        int len = reference.size();

        for (int k = 0; k < len; k++)
          line1[i++] = reference[k];

        if (! reference_done)
          line1[i++] = ' ';

        /* Update the Reference Command List to be output
           at the end of the File for CDoc Format */

        CDocScriptUpdateReferenceCommands(&cross_ref);

        delete [] cross_ref.refid;
      }
      else {
        p1 = p2;

        line1[i++] = *p1++;
      }
    }

    else if (CStrUtil::casencmp(p1, ":hook", 5) == 0) {
      CDHookParameterData hook_data;

      const char *p2 = p1;

      p1 += 5;

      /* Skip Spaces after Command */

      CStrUtil::skipSpace(&p1);

      /* Extract Hook Values */

      bool done = false;

      int j = 0;

      while (! done && *p1 != '\0' && hook_parameter_data[j].name != NULL) {
        int len = strlen(hook_parameter_data[j].name);

        if (CStrUtil::casencmp(p1, hook_parameter_data[j].name, len) == 0 &&
            p1[len] == '=') {
          p1 += len + 1;

          /* Get Hook Value (may be quoted) */

          int k = 0;

          char temp_string[256];

          if (*p1 == '\'') {
            p1++;

            while (*p1 != '\0' && *p1 != '\'')
              temp_string[k++] = *p1++;

            if (*p1 == '\'')
              p1++;
          }
          else {
            while (*p1 != '\0' && *p1 != '.' && isalnum(*p1))
              temp_string[k++] = *p1++;
          }

          temp_string[k] = '\0';

          std::string temp_string1 = CDocScriptReplaceSymbolsInString(temp_string);

          CDocExtractParameterValue(temp_string1, &hook_parameter_data[j],
                                    reinterpret_cast<char *>(&hook_data));

          /* Skip Space after Hook Value */

          CStrUtil::skipSpace(&p1);

          /* Hook Terminated */

          if (*p1 == '\0' || *p1 == '.') {
            done = true;

            if (*p1 == '.')
              p1++;
          }

          j = 0;
        }
        else
          j++;
      }

      if (hook_data.id != NULL) {
        CDHookData hook_data1(hook_data.id, hook_data.data, hook_data.text);

        /* Add Reference Text to the Paragraph */

        std::string text = CDocScriptGetHookText(&hook_data1);

        int len = text.size();

        for (int k = 0; k < len; k++)
          line1[i++] = text[k];

        if (! done)
          line1[i++] = ' ';
      }
      else {
        p1 = p2;

        line1[i++] = *p1++;
      }
    }

    /* Start Quotation */

    else if (CStrUtil::casencmp(p1, ":q", 2) == 0 &&
             (p1[2] == '.' || isspace(p1[2]) || p1[2] == '\0')) {
      /* Add Opening Quotation Mark */

      line1[i++] = '"';

      /* Skip Command */

      if (p1[2] == '.')
        p1 += 3;
      else
        p1 += 2;
    }

    /* End Citation */

    else if (CStrUtil::casencmp(p1, ":eq", 3) == 0 &&
             (p1[3] == '.' || isspace(p1[3]) || p1[3] == '\0')) {
      /* Add Closing Quotation Mark */

      line1[i++] = '"';

      /* Skip Command */

      if (p1[3] == '.')
        p1 += 4;
      else
        p1 += 3;
    }

    /* New Line */

    else if (CStrUtil::casencmp(p1, ":nl", 3) == 0 &&
             (p1[3] == '.' || isspace(p1[3]) || p1[3] == '\0')) {
      /* Remove Previous Spaces */

      while (i > 0 && isspace(line1[i - 1]))
        i--;

      /* Add Newline Character */

      line1[i++] = '\n';

      /* Skip Command */

      if (p1[3] == '.')
        p1 += 4;
      else
        p1 += 3;
    }

    /* Break */

    else if (CStrUtil::casencmp(p1, ":br", 3) == 0 &&
             (p1[3] == '.' || isspace(p1[3]) || p1[3] == '\0')) {
      /* Add Newline Character */

      line1[i++] = '\n';

      /* Skip Command */

      if (p1[3] == '.')
        p1 += 4;
      else
        p1 += 3;
    }

    else
      line1[i++] = *p1++;
  }

  line1[i] = '\0';

  /* Reset Script Line's Text */

  script_line->setData(script_line->getType(), line1);

  delete [] line1;
}

// Terminates Script Processing by Outputting Reference Commands and Closing Output File.
static void
CDocScriptTerm()
{
  /* Write Last Page Footer */

  if (cdoc_line_no != 0)
    CDocScriptWritePageFooter();

  /* If we are adding Page Numbers then Replace Forward
     references with real values */

  CDocScriptPageNoTerm();

  /* Terminate the References (this will output any reference
     commands used by CDoc to the end of the output) */

  CDocScriptTermReferences();

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("</body>\n");
    CDocScriptWriteCommand("</html>\n");
  }

  /* Close Output File if it was redirected to a Dataset */

  if (cdoc_output_fp != stdout) {
    fclose(cdoc_output_fp);

    cdoc_output_fp = stdout;
  }

  /* Clean Up */

  CDocScriptDeleteTableRowDefs();
  CDocScriptDeleteTables();
  CDocScriptDeleteFigures();
  CDocScriptDeleteFootnotes();
  CDocScriptDeleteMacros();
  CDocScriptDeleteTOC();
  CDocScriptDeleteIndex();
  CDocScriptDeleteFonts();

  CDocScriptDeleteSymbols();

  CDocScriptTermParagraphs();

  cdoc_current_section = "";

  cdoc_document.security = "";
  cdoc_document.language = "";
}

// Initialise Headers ready for the input IBM Script file to be processed.
static void
CDocScriptInitHeaders()
{
  for (int i = 0; i < CDOC_MAX_HEADERS; i++)
    header_number[i] = cdoc_header_number[i];
}

// Start a Header at the specified level.
//
// Ensures lower and higher level numbers are kept up to date.
extern void
CDocScriptStartHeader(int level)
{
  int i;

  /* If previous levels have not appeared then set them to 1 */

  for (i = 0; i < level; i++)
    if (header_number[i] == 0)
      header_number[i] = 1;

  /* Increment Count for current Header Level */

  header_number[level]++;

  /* Set higher levels to unset (0) */

  for (i = level + 1; i < CDOC_MAX_HEADERS; i++)
    header_number[i] = 0;
}

// Get the Current Header Number for the specified Level.
extern int
CDocScriptGetHeader(int level)
{
  if (level >= 0 && level < CDOC_MAX_HEADERS)
    return(header_number[level]);
  else
    return(-1);
}

// Turn formatting (i.e. text collection and justification) on.
extern void
CDocScriptFormattingOn()
{
  /* Set formatting flag */

  cdoc_formatting = true;

  /* Inform Troff that formatting is Active */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".fi\n");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_FORMAT_TMPL, "on");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("</pre>\n");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    CDocScriptWriteCommand("\n");
}

// Turn formatting (i.e. text collection and justification) off.
// Text will now appear as entered.
extern void
CDocScriptFormattingOff()
{
  /* Reset formatting flag */

  cdoc_formatting = false;

  /* Inform Troff that formatting is not Active */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".nf\n");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_FORMAT_TMPL, "off");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("<pre>\n");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    CDocScriptWriteCommand("\n");
}

// Convert an IBM Script highlight level to a CDoc font type.
static int
CDocScriptHighlightToFont(int highlight)
{
  int  font;

  if      (highlight == 0) font = CDOC_NORMAL_FONT;
  else if (highlight == 1) font = CDOC_UNDERLINE_FONT;
  else if (highlight == 2) font = CDOC_BOLD_FONT;
  else if (highlight == 3) font = CDOC_BOLD_UNDERLINE_FONT;
  else                     font = CDOC_NORMAL_FONT;

  return font;
}

// Surround supplied str with the Escape Codes required to display it in the
// specified IBM Script highlight.
static std::string
CDocScriptAddHighlightEscapes(const std::string &str, int highlight)
{
  std::string highlight_string;

  if      (highlight == 0)
    highlight_string = str;
  else if (highlight == 1)
    highlight_string = CDocStartUnderline() + str + CDocEndUnderline();
  else if (highlight == 2)
    highlight_string = CDocStartBold() + str + CDocEndBold();
  else if (highlight == 3)
    highlight_string = CDocStartBoldUnderline() + str + CDocEndBoldUnderline();
  else
    highlight_string = str;

  return highlight_string;
}
