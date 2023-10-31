#ifndef CDOC_SCRIPT_FIGURE_I_H
#define CDOC_SCRIPT_FIGURE_I_H

/*---------------------------------------------------------------------*/

constexpr ulong FRAME_NONE = 0;
constexpr ulong FRAME_RULE = 1;
constexpr ulong FRAME_BOX  = 2;

#define PLACE_TOP     1
#define PLACE_BOTTOM  2
#define PLACE_INLINE  3
#define PLACE_TOP_NOW 101

struct CDFigure {
  int               depth;
  char             *frame;
  std::string       ident;
  int               indent;
  int               place;
  int               width;
  int               number;
  int               count;
  std::string       caption;
  std::string       description;
  int               page_no;
  CDScriptTempFile *temp_file;

 ~CDFigure();
};

/*---------------------------------------------------------------------*/

extern CDFigure *  CDocScriptCreateFigure         ();
extern CDFigure *  CDocScriptGetFigure            (int);
extern void        CDocScriptFigureBegin          (CDFigure *);
extern void        CDocScriptFigureEnd            (CDFigure *);
extern int         CDocScriptIsTopFigure          ();
extern void        CDocScriptOutputTopFigure      ();
extern void        CDocScriptOutputFigureList     ();
extern void        CDocScriptSetFigurePage        (CDFigure *);
extern std::string CDocScriptGetFigurePage        (int);
extern void        CDocScriptDeleteFigures        ();
extern void        CDocScriptSetCurrentFigureDepth(int);

#endif
