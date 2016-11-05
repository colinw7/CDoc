#include "CDocI.h"

/*----------*/

struct CDRevCntrl {
  int no;
  int c;
};

/*----------*/

typedef std::vector<CDRevCntrl *> CDRevCntrlList;

static CDRevCntrlList  rev_ctrl_list;
static CDRevCntrl     *current_rev_ctrl      = NULL;
static CDRevCntrlList  current_rev_ctrl_list;

/*----------*/

static CDRevCntrl *CDocScriptCreateRevisionControl
                    (int, int);
static CDRevCntrl *CDocScriptGetRevisionControl
                    (int);

// Set the specified revision number's control character.
extern void
CDocScriptSetRevisionControlChar(int no, int c)
{
  CDRevCntrl *rev_ctrl = CDocScriptGetRevisionControl(no);

  if (rev_ctrl == NULL)
    rev_ctrl = CDocScriptCreateRevisionControl(no, '|');

  rev_ctrl->c = c;
}

// Get the current revision control character.
extern int
CDocScriptGetRevisionControlChar()
{
  if (current_rev_ctrl == NULL)
    return(' ');

  int c = current_rev_ctrl->c;

  if (current_rev_ctrl->no == 0)
    CDocScriptEndRevisionControl(0);

  return c;
}

// Start the revision control for the specified number.
extern void
CDocScriptStartRevisionControl(int no)
{
  CDRevCntrl *rev_ctrl = CDocScriptGetRevisionControl(no);

  if (rev_ctrl == NULL)
    rev_ctrl = CDocScriptCreateRevisionControl(no, '|');

  current_rev_ctrl_list.push_back(rev_ctrl);

  current_rev_ctrl = rev_ctrl;
}

// End the revision control for the specified number.
extern void
CDocScriptEndRevisionControl(int no)
{
  CDRevCntrl *rev_ctrl;

  if (! current_rev_ctrl_list.empty())
    rev_ctrl = current_rev_ctrl_list.back();
  else
    rev_ctrl = NULL;

  if (rev_ctrl == NULL || rev_ctrl->no != no) {
    CDocScriptError("Invalid End Revision Control No %d", no);
    return;
  }

  current_rev_ctrl_list.pop_back();

  if (! current_rev_ctrl_list.empty())
    current_rev_ctrl = current_rev_ctrl_list.back();
  else
    current_rev_ctrl = NULL;
}

// Create a revision control structure for the specified number and character.
static CDRevCntrl *
CDocScriptCreateRevisionControl(int no, int c)
{
  CDRevCntrl *rev_ctrl = new CDRevCntrl;

  rev_ctrl->no = no;
  rev_ctrl->c  = c;

  rev_ctrl_list.push_back(rev_ctrl);

  return rev_ctrl;
}

// Get the revision control structure for the specified number.
static CDRevCntrl *
CDocScriptGetRevisionControl(int no)
{
  int no1 = rev_ctrl_list.size();

  for (int i = 1; i <= no1; i++) {
    CDRevCntrl *rev_ctrl = rev_ctrl_list[i - 1];

    if (rev_ctrl->no == no)
      return rev_ctrl;
  }

  return NULL;
}
