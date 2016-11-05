#include "CDocI.h"

#define PAGE_OFF 1000000

#define PAGE_NORMAL    0
#define PAGE_START     1
#define PAGE_RANGE     2
#define PAGE_HIGHLIGHT 3
#define PAGE_STRING    4

/*------------------------------------------------------------------------*/

struct CDIndexEntry1;
struct CDIndexEntry2;
struct CDIndexEntry3;
struct CDIndexPageNo;

typedef std::vector<CDIndexEntry1 *> IndexEntry1List;
typedef std::vector<CDIndexEntry2 *> IndexEntry2List;
typedef std::vector<CDIndexEntry3 *> IndexEntry3List;
typedef std::vector<CDIndexPageNo *> PageNumList;

struct CDIndexEntry1 {
  std::string      str;
  std::string      id;
  std::string      print;
  std::string      see;
  std::string      seeid;
  int              no_refs;
  std::string      see_text;
  IndexEntry2List  list2;
  PageNumList      page_no_list;

  ~CDIndexEntry1();
};

struct CDIndexEntry2 {
  std::string      str;
  std::string      id;
  std::string      print;
  std::string      see;
  std::string      seeid;
  int              no_refs;
  std::string      see_text;
  IndexEntry3List  list3;
  PageNumList      page_no_list;
  CDIndexEntry1   *parent;

  ~CDIndexEntry2();
};

struct CDIndexEntry3 {
  std::string    str;
  std::string    id;
  std::string    print;
  std::string    see;
  std::string    seeid;
  int            no_refs;
  std::string    see_text;
  PageNumList    page_no_list;
  CDIndexEntry2 *parent;

 ~CDIndexEntry3();
};

struct CDIndexPageNo {
  int         type;
  int         start;
  int         end;
  std::string str;
};

/*------------------------------------------------------------------------*/

static IndexEntry1List index_entry_list1;
static IndexEntry2List index_entry_list2;
static IndexEntry3List index_entry_list3;

static CDIndexEntry1 *current_index_entry1 = NULL;
static CDIndexEntry2 *current_index_entry2 = NULL;

static std::string see_text;

/*------------------------------------------------------------------------*/

static void            CDocScriptOutputIndexSeeId
                        (const std::string &, int, int);
static CDIndexEntry1  *CDocScriptGetIndexByRef1
                        (const std::string &);
static CDIndexEntry2  *CDocScriptGetIndexByRef2
                        (const std::string &);
static CDIndexEntry3  *CDocScriptGetIndexByRef3
                        (const std::string &);
static CDIndexEntry1  *CDocScriptGetIndexByName1
                        (const std::string &);
static CDIndexEntry2  *CDocScriptGetIndexByName2
                        (CDIndexEntry1 *, const std::string &);
static CDIndexEntry3  *CDocScriptGetIndexByName3
                        (CDIndexEntry2 *, const std::string &);
static void            CDocScriptAddPageNumber
                        (const std::string &, int, PageNumList &);
static std::string     CDocScriptGetIndexPageString
                        (CDIndexPageNo *);

// Output the Index.
//
// According to Script this should only be allowed in the Back Matter section
// but this could should work anywhere so we ignore the restriction.
extern void
CDocScriptOutputIndex()
{
  int            i;
  int            j;
  int            k;
  int            n;
  int            no;
  char           last_char;
  int            no_entries1;
  int            no_entries2;
  int            no_entries3;
  char           current_char;
  CDIndexEntry1 *index_entry1;
  CDIndexEntry2 *index_entry2;
  CDIndexEntry3 *index_entry3;

  CDocScriptWriteCentreJustifiedPageHeader("Index");

  CDocScriptSetTOCPage();

  CDocScriptSkipLine();

  if (index_entry_list1.empty())
    return;

  std::sort(index_entry_list1.begin(), index_entry_list1.end());

  no_entries1 = index_entry_list1.size();

  last_char = '\0';

  for (i = 1; i <= no_entries1; i++) {
    index_entry1 = index_entry_list1[i - 1];

    current_char = index_entry1->str[0];

    if (islower(current_char))
      current_char = toupper(current_char);

    if (current_char != last_char) {
      CDocScriptSkipLine();
      CDocScriptWriteIndent(4);
      CDocScriptWriteText("%s%c%s\n",
        CDocStartBold(), current_char, CDocEndBold());
      CDocScriptSkipLine();

      last_char = current_char;
    }

    CDocScriptWriteIndent(4);

    if (index_entry1->print != "")
      CDocScriptWriteText("%s", index_entry1->print.c_str());
    else
      CDocScriptWriteText("%s", index_entry1->str.c_str());

    if (! index_entry1->page_no_list.empty() &&
        cdoc_page_numbering && CDocIsPagedOutput()) {
      CDocScriptWriteText(" ");

      no = index_entry1->page_no_list.size();

      for (n = 1; n <= no; n++) {
        if (n > 1)
          CDocScriptWriteText(", ");

        CDocScriptWriteText("##cdoc_ih1_pg%d##", n*PAGE_OFF + i);
      }
    }

    CDocScriptWriteText("\n");

    if      (index_entry1->seeid != "") {
      if (! index_entry1->list2.empty() ||
          ! index_entry1->page_no_list.empty())
        CDocScriptOutputIndexSeeId(index_entry1->seeid, 6, true);
      else
        CDocScriptOutputIndexSeeId(index_entry1->seeid, 6, false);
    }
    else if (index_entry1->see != "") {
      CDocScriptWriteIndent(6);

      CDocScriptWriteText("See %s\n", index_entry1->see.c_str());
    }

    if (index_entry1->list2.empty())
      continue;

    std::sort(index_entry1->list2.begin(), index_entry1->list2.end());

    no_entries2 = index_entry1->list2.size();

    for (j = 1; j <= no_entries2; j++) {
      index_entry2 = index_entry1->list2[j - 1];

      CDocScriptWriteIndent(6);

      if (index_entry2->print != "")
        CDocScriptWriteText("%s", index_entry2->print.c_str());
      else
        CDocScriptWriteText("%s", index_entry2->str.c_str());

      if (! index_entry2->page_no_list.empty() &&
          cdoc_page_numbering && CDocIsPagedOutput()) {
        CDocScriptWriteText(" ");

        no = index_entry2->page_no_list.size();

        for (n = 1; n <= no; n++) {
          if (n > 1)
            CDocScriptWriteText(", ");

          CDocScriptWriteText("##cdoc_ih2_pg%d.%d##",
                              i, n*PAGE_OFF + j);
        }
      }

      CDocScriptWriteText("\n");

      if      (index_entry2->seeid != "") {
        if (! index_entry2->list3.empty() ||
            ! index_entry2->page_no_list.empty())
          CDocScriptOutputIndexSeeId(index_entry2->seeid, 8, true);
        else
          CDocScriptOutputIndexSeeId(index_entry2->seeid, 8, false);
      }
      else if (index_entry2->see != "") {
        CDocScriptWriteIndent(8);

        CDocScriptWriteText("See %s\n", index_entry2->see.c_str());
      }

      if (index_entry2->list3.empty())
        continue;

      std::sort(index_entry2->list3.begin(), index_entry2->list3.end());

      no_entries3 = index_entry2->list3.size();

      for (k = 1; k <= no_entries3; k++) {
        index_entry3 = index_entry2->list3[k - 1];

        CDocScriptWriteIndent(8);

        if (index_entry3->print != "")
          CDocScriptWriteText("%s", index_entry3->print.c_str());
        else
          CDocScriptWriteText("%s", index_entry3->str.c_str());

        if (! index_entry3->page_no_list.empty() &&
            cdoc_page_numbering && CDocIsPagedOutput()) {
          CDocScriptWriteText(" ");

          no = index_entry3->page_no_list.size();

          for (n = 1; n <= no; n++) {
            if (n > 1)
              CDocScriptWriteText(", ");

            CDocScriptWriteText("##cdoc_ih3_pg%d.%d.%d##", i, j, n*PAGE_OFF + k);
          }
        }

        CDocScriptWriteText("\n");

        if      (index_entry3->seeid != "") {
          if (! index_entry3->page_no_list.empty())
            CDocScriptOutputIndexSeeId(index_entry3->seeid, 10, true);
          else
            CDocScriptOutputIndexSeeId(index_entry3->seeid, 10, false);
        }
        else if (index_entry3->see != "") {
          CDocScriptWriteIndent(10);

          CDocScriptWriteText("See %s\n", index_entry3->see.c_str());
        }
      }
    }
  }
}

// Output a Index See Reference, keyed by an identifier, at the specified
// indent.
static void
CDocScriptOutputIndexSeeId(const std::string &seeid, int indent, int children)
{
  CDIndexEntry2 *index_entry2;
  CDIndexEntry3 *index_entry3;

  CDIndexEntry1 *index_entry1 = CDocScriptGetIndexByRef1(seeid);

  if (index_entry1 != NULL)
    see_text = index_entry1->str;
  else {
    index_entry2 = CDocScriptGetIndexByRef2(seeid);

    if (index_entry2 != NULL)
      see_text = index_entry2->parent->str + ", " + index_entry2->str;
    else {
      index_entry3 = CDocScriptGetIndexByRef3(seeid);

      if (index_entry3 != NULL)
        see_text = index_entry3->parent->parent->str + ", " +
                   index_entry3->parent->str + ", " + index_entry3->str;
    }
  }

  CDocScriptWriteIndent(indent);

  if (children)
    CDocScriptWriteText("See also %s\n", see_text.c_str());
  else
    CDocScriptWriteText("See %s\n", see_text.c_str());
}

// Add a Level 1 Index Entry (from :i1).
extern void
CDocScriptAddIndex1(const std::string &str, const std::string &id, const std::string &)
{
  CDIndexEntry1 *index_entry = CDocScriptGetIndexByName1(str);

  if (index_entry == NULL) {
    index_entry = new CDIndexEntry1;

    index_entry->str     = str;
    index_entry->id      = "";
    index_entry->print   = "";
    index_entry->see     = "";
    index_entry->seeid   = "";
    index_entry->no_refs = 0;

    if (id != "")
      index_entry->id = id;

    char first_char;

    if (islower(index_entry->str[0]))
      first_char = toupper(index_entry->str[0]);
    else
      first_char = index_entry->str[0];

    see_text = std::string(&first_char, 1) + " = " + str;

    index_entry->see_text = see_text;

    index_entry_list1.push_back(index_entry);
  }

  current_index_entry1 = index_entry;
  current_index_entry2 = NULL;
}

// Add a Level 1 Index Header Entry (from :ih1).
extern void
CDocScriptAddIndexHeader1(const std::string &str, const std::string &id, const std::string &print,
                          const std::string &see, const std::string &seeid)
{
  CDIndexEntry1 *index_entry = CDocScriptGetIndexByName1(str);

  if (index_entry == NULL) {
    index_entry = new CDIndexEntry1;

    index_entry->str     = str;
    index_entry->id      = "";
    index_entry->print   = "";
    index_entry->see     = "";
    index_entry->seeid   = "";
    index_entry->no_refs = 0;

    if (id != "")
      index_entry->id = id;

    if (print != "")
      index_entry->print = print;

    if      (see   != "")
      index_entry->see   = see;
    else if (seeid != "")
      index_entry->seeid = seeid;

    char first_char;

    if (islower(index_entry->str[0]))
      first_char = toupper(index_entry->str[0]);
    else
      first_char = index_entry->str[0];

    see_text = std::string(&first_char, 1) + " - " + str;

    index_entry->see_text = see_text;

    index_entry_list1.push_back(index_entry);
  }

  current_index_entry1 = index_entry;
  current_index_entry2 = NULL;

  return;

}

// Add a Level 2 Index Entry (from :i2).
extern void
CDocScriptAddIndex2(const std::string &str, const std::string &id, const std::string &refid)
{
  CDIndexEntry1 *index_entry1;

  /* Get Parent Level 1 Index */

  if      (refid != "") {
    index_entry1 = CDocScriptGetIndexByRef1(refid);

    if (index_entry1 == NULL) {
      CDocScriptError("No Level 1 Index with Reference %s", refid.c_str());
      return;
    }
  }
  else if (current_index_entry1 != NULL)
    index_entry1 = current_index_entry1;
  else {
    CDocScriptError("No Current Level 1 Index");
    return;
  }

  /* Get Existing Level 2 Index */

  CDIndexEntry2 *index_entry = CDocScriptGetIndexByName2(index_entry1, str);

  /* Create new one if doesn't already exist or increment
     the Level 2 Indexes Reference Count */

  if (index_entry == NULL) {
    index_entry = new CDIndexEntry2;

    index_entry->str     = str;
    index_entry->id      = "";
    index_entry->print   = "";
    index_entry->see     = "";
    index_entry->seeid   = "";
    index_entry->no_refs = 0;
    index_entry->parent  = index_entry1;

    if (id != "")
      index_entry->id = id;

    see_text = index_entry1->see_text + ", " + str;

    index_entry->see_text = see_text;

    index_entry1->list2.push_back(index_entry);

    index_entry_list2.push_back(index_entry);
  }

  current_index_entry2 = index_entry;
}

// Add a Level 2 Index Header Entry (from :ih2).
extern void
CDocScriptAddIndexHeader2(const std::string &str, const std::string &id, const std::string &print,
                          const std::string &see, const std::string &seeid)
{
  CDIndexEntry1 *index_entry1;

  /* Get Parent Level 1 Index */

  if (current_index_entry1 != NULL)
    index_entry1 = current_index_entry1;
  else {
    CDocScriptError("No Current Level 1 Index");
    return;
  }

  /* Get Existing Level 2 Index */

  CDIndexEntry2 *index_entry = CDocScriptGetIndexByName2(index_entry1, str);

  /* Create new one if doesn't already exist or increment
     the Level 2 Indexes Reference Count */

  if (index_entry == NULL) {
    index_entry = new CDIndexEntry2;

    index_entry->str     = str;
    index_entry->id      = "";
    index_entry->print   = "";
    index_entry->see     = "";
    index_entry->seeid   = "";
    index_entry->no_refs = 0;
    index_entry->parent  = index_entry1;

    if (id != "")
      index_entry->id = id;

    if (print != "")
      index_entry->print = print;

    if      (see   != "")
      index_entry->see   = see;
    else if (seeid != "")
      index_entry->seeid = seeid;

    see_text = index_entry1->see_text + ", " + str;

    index_entry->see_text = see_text;

    index_entry1->list2.push_back(index_entry);

    index_entry_list2.push_back(index_entry);
  }

  current_index_entry2 = index_entry;

}

// Add a Level 3 Index Entry (from :i3).
extern void
CDocScriptAddIndex3(const std::string &str, const std::string &id, const std::string &refid)
{
  CDIndexEntry2 *index_entry2;

  /* Get Parent Level 2 Index */

  if      (refid != "") {
    index_entry2 = CDocScriptGetIndexByRef2(refid);

    if (index_entry2 == NULL) {
      CDocScriptError("No Level 2 Index with Reference %s", refid.c_str());
      return;
    }
  }
  else if (current_index_entry2 != NULL)
    index_entry2 = current_index_entry2;
  else {
    CDocScriptError("No Current Level 2 Index");
    return;
  }

  /* Get Existing Level 3 Index */

  CDIndexEntry3 *index_entry = CDocScriptGetIndexByName3(index_entry2, str);

  if (index_entry == NULL) {
    index_entry = new CDIndexEntry3;

    index_entry->str     = str;
    index_entry->id      = "";
    index_entry->print   = "";
    index_entry->see     = "";
    index_entry->seeid   = "";
    index_entry->no_refs = 0;
    index_entry->parent  = index_entry2;

    if (id != "")
      index_entry->id = id;

    see_text = index_entry2->see_text + ", " + str;

    index_entry->see_text = see_text;

    index_entry2->list3.push_back(index_entry);

    index_entry_list3.push_back(index_entry);
  }
}

// Add a Level 3 Index Header Entry (from :ih3).
extern void
CDocScriptAddIndexHeader3(const std::string &str, const std::string &id, const std::string &print,
                          const std::string &see, const std::string &seeid)
{
  CDIndexEntry2 *index_entry2;

  /* Get Parent Level 2 Index */

  if (current_index_entry2 != NULL)
    index_entry2 = current_index_entry2;
  else {
    CDocScriptError("No Current Level 2 Index");
    return;
  }

  /* Get Existing Level 3 Index */

  CDIndexEntry3 *index_entry = CDocScriptGetIndexByName3(index_entry2, str);

  if (index_entry == NULL) {
    index_entry = new CDIndexEntry3;

    index_entry->str     = str;
    index_entry->id      = "";
    index_entry->print   = "";
    index_entry->see     = "";
    index_entry->seeid   = "";
    index_entry->no_refs = 0;
    index_entry->parent  = index_entry2;

    if (id != "")
      index_entry->id = id;

    if (print != "")
      index_entry->print = print;

    if      (see   != "")
      index_entry->see   = see;
    else if (seeid != "")
      index_entry->seeid = seeid;

    see_text = index_entry2->see_text + ", " + str;

    index_entry->see_text = see_text;

    index_entry2->list3.push_back(index_entry);

    index_entry_list3.push_back(index_entry);
  }
}

// Add an Index Entry Reference (from :iref).
extern void
CDocScriptAddIndexRef(const std::string &refid, const std::string &page,
                      const std::string & /*see*/, const std::string & /*seeid*/)
{
  /* Set Page Number */

  int page_no;

  if (cdoc_page_no != -1)
    page_no = cdoc_page_no;
  else
    page_no = cdoc_page_no_offset;

  CDIndexEntry1 *index_entry1 = CDocScriptGetIndexByRef1(refid);

  if (index_entry1 != NULL) {
    /* Add Page Number to List */

    CDocScriptAddPageNumber(page, page_no, index_entry1->page_no_list);

    return;
  }

  CDIndexEntry2 *index_entry2 = CDocScriptGetIndexByRef2(refid);

  if (index_entry2 != NULL) {
    /* Add Page Number to List */

    CDocScriptAddPageNumber(page, page_no, index_entry2->page_no_list);

    return;
  }

  CDIndexEntry3 *index_entry3 = CDocScriptGetIndexByRef3(refid);

  if (index_entry3 != NULL) {
    /* Add Page Number to List */

    CDocScriptAddPageNumber(page, page_no, index_entry3->page_no_list);

    return;
  }
}

// Get Unique String for Specified Level 1 Index Entry
extern std::string
CDocScriptGetIndex1SeeText(const std::string &str, const std::string & /*refid*/)
{
  /* Get Level 1 Index */

  CDIndexEntry1 *index_entry = CDocScriptGetIndexByName1(str);

  /* Build the See String */

  see_text = index_entry->see_text + "(" + CStrUtil::toString(++index_entry->no_refs) + ")";

  /* Set Current Index Entries */

  current_index_entry1 = index_entry;
  current_index_entry2 = NULL;

  return see_text;
}

// Get Unique String for Specified Level 2 Index Entry
extern std::string
CDocScriptGetIndex2SeeText(const std::string &str, const std::string &refid)
{
  CDIndexEntry1 *index_entry1;

  /* Get Parent Level 1 Index */

  if      (refid != "") {
    index_entry1 = CDocScriptGetIndexByRef1(refid);

    if (index_entry1 == NULL)
      return "";
  }
  else if (current_index_entry1 != NULL)
    index_entry1 = current_index_entry1;
  else
    return "";

  /* Get Level 2 Index */

  CDIndexEntry2 *index_entry = CDocScriptGetIndexByName2(index_entry1, str);

  /* Build the See String */

  see_text = index_entry->see_text + "(" + CStrUtil::toString(++index_entry->no_refs) + ")";

  /* Set Current Index Entries */

  current_index_entry2 = index_entry;

  return see_text;
}

// Get Unique String for Specified Level 3 Index Entry
extern std::string
CDocScriptGetIndex3SeeText(const std::string &str, const std::string &refid)
{
  CDIndexEntry2 *index_entry2;

  /* Get Parent Level 2 Index */

  if      (refid != "") {
    index_entry2 = CDocScriptGetIndexByRef2(refid);

    if (index_entry2 == NULL)
      return "";
  }
  else if (current_index_entry2 != NULL)
    index_entry2 = current_index_entry2;
  else
    return "";

  /* Get Level 3 Index */

  CDIndexEntry3 *index_entry = CDocScriptGetIndexByName3(index_entry2, str);

  /* Build the See String */

  see_text = index_entry->see_text + "(" + CStrUtil::toString(++index_entry->no_refs) + ")";

  return see_text;
}

// Get Level 1 Index Entry from Reference.
static CDIndexEntry1 *
CDocScriptGetIndexByRef1(const std::string &refid)
{
  if (index_entry_list1.empty())
    return NULL;

  int no_entries = index_entry_list1.size();

  for (int i = 1; i <= no_entries; i++) {
    CDIndexEntry1 *index_entry = index_entry_list1[i - 1];

    if (index_entry->id != "" && CStrUtil::casecmp(index_entry->id, refid) == 0)
      return(index_entry);
  }

  return NULL;
}

// Get Level 2 Index Entry from Reference.
static CDIndexEntry2 *
CDocScriptGetIndexByRef2(const std::string &refid)
{
  if (index_entry_list2.empty())
    return NULL;

  int no_entries = index_entry_list2.size();

  for (int i = 1; i <= no_entries; i++) {
    CDIndexEntry2 *index_entry = index_entry_list2[i - 1];

    if (index_entry->id != "" && CStrUtil::casecmp(index_entry->id, refid) == 0)
      return index_entry;
  }

  return NULL;
}

// Get Level 3 Index Entry from Reference.
static CDIndexEntry3 *
CDocScriptGetIndexByRef3(const std::string &refid)
{
  if (index_entry_list3.empty())
    return NULL;

  int no_entries = index_entry_list3.size();

  for (int i = 1; i <= no_entries; i++) {
    CDIndexEntry3 *index_entry = index_entry_list3[i - 1];

    if (index_entry->id != "" && CStrUtil::casecmp(index_entry->id, refid) == 0)
      return(index_entry);
  }

  return NULL;
}

// Get Level 1 Index Entry from Name.
static CDIndexEntry1 *
CDocScriptGetIndexByName1(const std::string &name)
{
  if (index_entry_list1.empty())
    return NULL;

  int no_entries = index_entry_list1.size();

  for (int i = 1; i <= no_entries; i++) {
    CDIndexEntry1 *index_entry = index_entry_list1[i - 1];

    if (CStrUtil::casecmp(index_entry->str, name) == 0)
      return(index_entry);
  }

  return NULL;
}

// Get Level 2 Index Entry from Name.
static CDIndexEntry2 *
CDocScriptGetIndexByName2(CDIndexEntry1 *index_entry1, const std::string &name)
{
  if (index_entry1->list2.empty())
    return NULL;

  int no_entries = index_entry1->list2.size();

  for (int i = 1; i <= no_entries; i++) {
    CDIndexEntry2 *index_entry = index_entry1->list2[i - 1];

    if (CStrUtil::casecmp(index_entry->str, name) == 0)
      return(index_entry);
  }

  return NULL;
}

// Get Level 3 Index Entry from Name.
static CDIndexEntry3 *
CDocScriptGetIndexByName3(CDIndexEntry2 *index_entry2, const std::string &name)
{
  if (index_entry2->list3.empty())
    return NULL;

  int no_entries = index_entry2->list3.size();

  for (int i = 1; i <= no_entries; i++) {
    CDIndexEntry3 *index_entry = index_entry2->list3[i - 1];

    if (CStrUtil::casecmp(index_entry->str, name) == 0)
      return(index_entry);
  }

  return NULL;
}

// Set a Level 1 Index Entry's Page Number from the current Page Number.
extern void
CDocScriptSetIndex1Page(const std::string &str, const std::string &page,
                        const std::string & /*refid*/)
{
  /* Get Level 1 Index */

  CDIndexEntry1 *index_entry = CDocScriptGetIndexByName1(str);

  /* Set Page Number */

  int page_no;

  if (cdoc_page_no != -1)
    page_no = cdoc_page_no;
  else
    page_no = cdoc_page_no_offset;

  /* Add Page Number to List */

  CDocScriptAddPageNumber(page, page_no, index_entry->page_no_list);
}

// Set a Level 2 Index Entry's Page Number from the current Page Number.
extern void
CDocScriptSetIndex2Page(const std::string &str, const std::string &page, const std::string &refid)
{
  int            page_no;
  CDIndexEntry1 *index_entry1;

  /* Get Parent Level 1 Index */

  if      (refid != "") {
    index_entry1 = CDocScriptGetIndexByRef1(refid);

    if (index_entry1 == NULL)
      return;
  }
  else if (current_index_entry1 != NULL)
    index_entry1 = current_index_entry1;
  else
    return;

  /* Get Level 2 Index */

  CDIndexEntry2 *index_entry = CDocScriptGetIndexByName2(index_entry1, str);

  /* Set Page Number */

  if (cdoc_page_no != -1)
    page_no = cdoc_page_no;
  else
    page_no = cdoc_page_no_offset;

  /* Add Page Number to List */

  CDocScriptAddPageNumber(page, page_no, index_entry->page_no_list);
}

// Set a Level 3 Index Entry's Page Number from the current Page Number.
extern void
CDocScriptSetIndex3Page(const std::string &str, const std::string &page, const std::string &refid)
{
  int            page_no;
  CDIndexEntry2 *index_entry2;

  /* Get Parent Level 2 Index */

  if      (refid != "") {
    index_entry2 = CDocScriptGetIndexByRef2(refid);

    if (index_entry2 == NULL)
      return;
  }
  else if (current_index_entry2 != NULL)
    index_entry2 = current_index_entry2;
  else
    return;

  /* Get Level 3 Index */

  CDIndexEntry3 *index_entry = CDocScriptGetIndexByName3(index_entry2, str);

  /* Set Page Number */

  if (cdoc_page_no != -1)
    page_no = cdoc_page_no;
  else
    page_no = cdoc_page_no_offset;

  /* Add Page Number to List */

  CDocScriptAddPageNumber(page, page_no, index_entry->page_no_list);
}

// Add a Page Number Reference to an Index Entry's Page Number List.
static void
CDocScriptAddPageNumber(const std::string &page, int no, PageNumList &page_no_list)
{
  CDIndexPageNo *page_no;
  CDIndexPageNo *page_no1;

  /* Get Last Page Number (if any) */

  if (! page_no_list.empty())
    page_no1 = page_no_list.back();
  else
    page_no1 = NULL;

  /* Add Page Number to List (if not already present) */

  if      (page == PG_START_STR) {
    page_no = new CDIndexPageNo;

    page_no->type  = PAGE_START;
    page_no->start = no;
    page_no->end   = -1;
    page_no->str   = "";

    page_no_list.push_back(page_no);
  }
  else if (page == PG_END_STR) {
    if (page_no1 == NULL || page_no1->type != PAGE_START) {
      CDocScriptError("Invalid Index Entry - No Start for End");
      return;
    }

    page_no1->type = PAGE_RANGE;
    page_no1->end  = no;
  }
  else if (page == "" || page == PG_MAJOR_STR) {
    if (page_no1 != NULL && page_no1->type == PAGE_START) {
      CDocScriptError("Invalid Index Entry - In Start/End Block");
      return;
    }

    if (page_no1 != NULL &&
        (page_no1->type == PAGE_HIGHLIGHT ||
         page_no1->type == PAGE_NORMAL) &&
        page_no1->start == no)
      return;

    page_no = new CDIndexPageNo;

    if (page == PG_MAJOR_STR)
      page_no->type = PAGE_HIGHLIGHT;
    else
      page_no->type = PAGE_NORMAL;

    page_no->start = no;
    page_no->end   = -1;
    page_no->str   = "";

    page_no_list.push_back(page_no);
  }
  else {
    page_no = new CDIndexPageNo;

    page_no->type  = PAGE_STRING;
    page_no->start = no;
    page_no->end   = -1;
    page_no->str   = page;

    page_no_list.push_back(page_no);
  }
}

// Get the Page Number String for the Nth Level 1 Index Entry.
extern std::string
CDocScriptGetIndex1Page(int no)
{
  CDIndexPageNo  *page_no;

  int n1 = no/PAGE_OFF;
  int n2 = no - n1*PAGE_OFF;

  if (index_entry_list1.empty())
    return "";

  CDIndexEntry1 *index_entry1 = index_entry_list1[n2 - 1];

  std::string page_string;

  if (index_entry1 != NULL) {
    page_no = index_entry1->page_no_list[n1 - 1];

    page_string = CDocScriptGetIndexPageString(page_no);
  }
  else
    page_string = "";

  return page_string;
}

// Get the Page Number String for the Nth Level 2 Index Entry for
// the Mth Level 1 Index Entry.
extern std::string
CDocScriptGetIndex2Page(int no1, int no2)
{
  CDIndexPageNo  *page_no;
  CDIndexEntry2  *index_entry2;

  if (index_entry_list1.empty())
    return "";

  CDIndexEntry1 *index_entry1 = index_entry_list1[no1 - 1];

  std::string page_string;

  if (index_entry1 != NULL) {
    int n1 = no2/PAGE_OFF;
    int n2 = no2 - n1*PAGE_OFF;

    if (index_entry1->list2.empty())
      return "";

    index_entry2 = index_entry1->list2[n2 - 1];

    if (index_entry2 != NULL) {
      page_no = index_entry2->page_no_list[n1 - 1];

      page_string = CDocScriptGetIndexPageString(page_no);
    }
    else
      page_string = "";
  }
  else
    page_string = "";

  return page_string;
}

// Get the Page Number String for the Nth Level 3 Index Entry for the
// Mth Level 2 Index Entry and the Lth Level 1 Index Entry.
extern std::string
CDocScriptGetIndex3Page(int no1, int no2, int no3)
{
  CDIndexPageNo *page_no;
  CDIndexEntry2 *index_entry2;
  CDIndexEntry3 *index_entry3;

  if (index_entry_list1.empty())
    return "";

  CDIndexEntry1 *index_entry1 = index_entry_list1[no1 - 1];

  std::string page_string;

  if (index_entry1 != NULL) {
    if (index_entry1->list2.empty())
      return "";

    index_entry2 = index_entry1->list2[no2 - 1];

    if (index_entry2 != NULL) {
      int n1 = no3/PAGE_OFF;
      int n2 = no3 - n1*PAGE_OFF;

      if (index_entry2->list3.empty())
        return "";

      index_entry3 = index_entry2->list3[n2 - 1];

      if (index_entry3 != NULL) {
        page_no = index_entry3->page_no_list[n1 - 1];

        page_string = CDocScriptGetIndexPageString(page_no);
      }
      else
        page_string = "";
    }
    else
      page_string = "";
  }
  else
    page_string = "";

  return page_string;
}

// Get the Page Number String for the specified Page Number reference.
static std::string
CDocScriptGetIndexPageString(CDIndexPageNo *page_no)
{
  static char page_string[128];

  strcpy(page_string, "??");

  if (page_no != NULL) {
    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
      if      (page_no->type == PAGE_NORMAL)
        sprintf(page_string, "%s%dB%d%sE", ESCG, page_no->start, page_no->start, ESCG);
      else if (page_no->type == PAGE_START)
        sprintf(page_string, "%s%dB%d-?%sE", ESCG, page_no->start, page_no->start, ESCG);
      else if (page_no->type == PAGE_RANGE)
        sprintf(page_string, "%s%dB%d-%d%sE",
                ESCG, page_no->start, page_no->start, page_no->end, ESCG);
      else if (page_no->type == PAGE_HIGHLIGHT)
        sprintf(page_string, "%s%dB%s%d%s%sE", ESCG, page_no->start,
                CDocStartBold(), page_no->start, CDocEndBold(), ESCG);
      else
        sprintf(page_string, "%s%dB%s%sE",
                ESCG, page_no->start, page_no->str.c_str(), ESCG);
    }
    else {
      if      (page_no->type == PAGE_NORMAL)
        sprintf(page_string, "%d", page_no->start);
      else if (page_no->type == PAGE_START)
        sprintf(page_string, "%d-?", page_no->start);
      else if (page_no->type == PAGE_RANGE)
        sprintf(page_string, "%d-%d", page_no->start, page_no->end);
      else if (page_no->type == PAGE_HIGHLIGHT)
        sprintf(page_string, "%s%d%s", CDocStartBold(), page_no->start, CDocEndBold());
      else
        sprintf(page_string, "%s", page_no->str.c_str());
    }
  }

  return page_string;
}

// Delete all allocated Index resources.
extern void
CDocScriptDeleteIndex()
{
  for_each(index_entry_list1.begin(), index_entry_list1.end(), CDeletePointer());
  for_each(index_entry_list2.begin(), index_entry_list2.end(), CDeletePointer());
  for_each(index_entry_list3.begin(), index_entry_list3.end(), CDeletePointer());

  index_entry_list1.clear();
  index_entry_list2.clear();
  index_entry_list3.clear();
}

CDIndexEntry1::
~CDIndexEntry1()
{
  for_each(page_no_list.begin(), page_no_list.end(), CDeletePointer());

  page_no_list.clear();
}

CDIndexEntry2::
~CDIndexEntry2()
{
  for_each(page_no_list.begin(), page_no_list.end(), CDeletePointer());

  page_no_list.clear();
}

CDIndexEntry3::
~CDIndexEntry3()
{
  for_each(page_no_list.begin(), page_no_list.end(), CDeletePointer());

  page_no_list.clear();
}
