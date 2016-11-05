#ifndef CDOC_SCRIPT_TABLE_I_H
#define CDOC_SCRIPT_TABLE_I_H

#define SHADE_XLIGHT 0
#define SHADE_LIGHT  1
#define SHADE_MEDIUM 2
#define SHADE_DARK   3
#define SHADE_XDARK  4

/*---------------------------------------------------------------------------*/

struct CDTableRowDef {
  typedef std::vector<int  > NoArrangeList;
  typedef std::vector<int *> ArrangeList;

  std::string    name;
  int            no_highlights;
  int           *highlights;
  int            no_aligns;
  int           *aligns;
  int            no_concats;
  int           *concats;
  int            no_valigns;
  int           *valigns;
  int            no_rotates;
  int           *rotates;
  int            no_mindepths;
  int           *mindepths;
  NoArrangeList  no_arrange_list;
  ArrangeList    arrange_list;
  int            no_cwidths;
  int           *cwidths;
  int            shade;
  int            width;
  int            free_cells1;
  int            free_cells2;

 ~CDTableRowDef();
};

/*---------------------------------------------------------------------------*/

#define WIDTH_COLUMN -1
#define WIDTH_PAGE   -2

struct CDTableNote;

struct CDTable {
  typedef std::vector<CDTableRow *>  RowList;
  typedef std::vector<CDTableNote *> NoteList;

  CDTableRowDef    *row_definition;
  std::string       refid;
  std::string       id;
  int               width;
  int               split;
  int               rotate;
  int               number;
  int               count;
  std::string       caption;
  std::string       description;
  CDTableRow       *header_row;
  CDTableRow       *footer_row;
  RowList           row_list;
  NoteList          note_list;
  int               page_no;
  int               full_width;
  CDScriptTempFile *temp_file;

 ~CDTable();
};

/*---------------------------------------------------------------------------*/

struct CDTableCell;

struct CDTableRow {
  typedef std::vector<CDTableCell *> CellList;

  CDTable       *table;
  std::string    refid;
  int            split;
  CDTableRowDef *row_definition;
  int            cell_number;
  CellList       cell_list;
  int            no_cwidths;
  int           *cwidths;

 ~CDTableRow();
};

/*---------------------------------------------------------------------------*/

struct CDTableNote {
  std::string text;
  int         row_no;
};

/*---------------------------------------------------------------------------*/

struct CDTableCell {
  CDTableRow  *row;
  int          number;
  std::string  text;
  int          no_breaks;
  int         *breaks;

 ~CDTableCell();
};

/*---------------------------------------------------------------------------*/

extern void            CDocScriptOutputTableList
                        ();
extern void            CDocScriptSetTablePage
                        (CDTable *);
extern std::string     CDocScriptGetTablePage
                        (int);
extern void            CDocScriptOutputTable
                        (CDTable *);
extern CDTable        *CDocScriptCreateTable
                        ();
extern void            CDocScriptSetTableRowDef
                        (CDTable *);
extern CDTable        *CDocScriptGetTable
                        (int);
extern void            CDocScriptDeleteTables
                        ();
extern CDTableRow     *CDocScriptCreateTableRow
                        (CDTable *);
extern CDTableRow     *CDocScriptCreateTableFooterRow
                        (CDTable *);
extern CDTableRow     *CDocScriptCreateTableHeaderRow
                        (CDTable *);
extern void            CDocScriptSetTableRowRowDef
                        (CDTableRow *);
extern void            CDocScriptAddTableNote
                        (CDTable *, const std::string &);
extern CDTableCell    *CDocScriptCreateTableCell
                        (CDTableRow *);
extern CDTableRowDef  *CDocScriptCreateTableRowDef
                        ();
extern void            CDocScriptAddTableRowDef
                        (CDTableRowDef *);
extern CDTableRowDef  *CDocScriptGetTableRowDef
                        (const std::string &);
extern void            CDocScriptDeleteTableRowDefs
                        ();

#endif
