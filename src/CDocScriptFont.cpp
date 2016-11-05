#include "CDocI.h"

/*------------------------------------------------------------------------*/

typedef std::vector<CDFont *> CDFontList;

static CDFont     *current_font  = NULL;
static CDFontList  font_stack;
static CDFontList  font_def_list;

/*------------------------------------------------------------------------*/

static CDFont *CDocScriptGetFontDef(const std::string &);

// Define a mapping between a font name and an actual device specific font
// definition.
extern void
CDocScriptDefineFont(const std::string &font_name, const std::string &font_def)
{
  CDFont *font = new CDFont;

  font->name = font_name;

  uint len = font_def.size();

  if (len >= 2 &&
      (font_def[0      ] == '"' || font_def[0      ] == '\'') &&
      (font_def[len - 1] == '"' || font_def[len - 1] == '\''))
    font->def = font_def.substr(1, len - 2);
  else
    font->def = font_def;

  font_def_list.push_back(font);

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_DEF_FONT_TMPL, font->name.c_str(), font->def.c_str());
}

// Start a new font for the Script Output
//
// Because of the multitude of fonts, which are different on the Mainframe and
// Workstation only 'xmpfont' is really supported in which case a fixed width
// font is output.
extern void
CDocScriptBeginFont(const std::string &font_name)
{
  CDFont *font = new CDFont;

  if      (font_name    != "")
    font->name = font_name;
  else if (current_font != NULL)
    font->name = current_font->name;
  else
    font->name = "default";

  CDFont *font1 = CDocScriptGetFontDef(font->name);

  if (font1 != NULL)
    font->def = font1->def;
  else
    font->def = "";

  font_stack.push_back(font);

  current_font = font;

  /* Inform CDoc of the New Font Name (CDoc will decide
     what to do with it) */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_START_FONT_TMPL, font->name.c_str());

  /* If New Font is 'xmpfont' for Troff then switch to
     fixed width font (other fonts are ignored) */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    if (current_font->name == "xmpfont")
      CDocScriptWriteCommand(".ft 7\n");
    else
      CDocScriptWriteCommand(".ft 1\n");
  }
}

// Return to the previous font for the Script Output
extern void
CDocScriptPreviousFont()
{
  if (current_font == NULL) {
    CDocScriptWarning("No Current Font");
    return;
  }

  CDFontList::iterator p =
    find(font_stack.begin(), font_stack.end(), current_font);

  if (p != font_stack.end())
    font_stack.erase(p);

  delete current_font;

  current_font = NULL;

  if (! font_stack.empty())
    current_font = font_stack.back();

  /* Inform CDoc that the Previous Font Name has
     been terminated */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_END_FONT_TMPL);

  /* Switch Troff back to Standard Font */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    if (current_font != NULL && current_font->name == "xmpfont")
      CDocScriptWriteCommand(".ft 7\n");
    else
      CDocScriptWriteCommand(".ft 1\n");
  }
}

// Check if the supplied font name is valid for the current output device.
//
// Does a very basic job at the moment which means CDoc output formats accept
// all fonts and other formats only accept 'xmpfont'.
//
// This needs more work to check if font_name is a valid X Font.
extern int
CDocScriptIsValidFont(const std::string &font_name)
{
  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    return true;

  if (font_name == "xmpfont")
    return true;

  return false;
}

// Get the device specific font definition for a supplied font name.
static CDFont *
CDocScriptGetFontDef(const std::string &name)
{
  int no_fonts = font_def_list.size();

  for (int i = 1; i <= no_fonts; i++) {
    CDFont *font = font_def_list[i - 1];

    if (font->name == name)
      return font;
  }

  return NULL;
}

// Restart all the fonts on the font stack.
//
// This is done at the top of each page so that the page does not rely on fonts
// defined on previous pages.  This allows us to jump from page to page in any
// order without losing the font stack.
extern void
CDocScriptStartAllFonts()
{
  if (font_stack.empty() || CDocInst->getOutputFormat() != CDOC_OUTPUT_CDOC)
    return;

  int no_fonts = font_stack.size();

  for (int i = 1; i <= no_fonts; i++) {
    CDFont *font = font_stack[i - 1];

    CDocScriptWriteCommand(CDOC_START_FONT_TMPL, font->name.c_str());
  }
}

// Terminate all the fonts on the font stack.
//
// This is done at the bottom of each page so that each page has its own
// independant font stack. See CDocScriptStartAllFonts().
extern void
CDocScriptEndAllFonts()
{
  if (font_stack.empty() || CDocInst->getOutputFormat() != CDOC_OUTPUT_CDOC)
    return;

  int no_fonts = font_stack.size();

  for (int i = 1; i <= no_fonts; i++)
    CDocScriptWriteCommand(CDOC_END_FONT_TMPL);
}

// Delete all resources used by fonts.
extern void
CDocScriptDeleteFonts()
{
  for_each(font_stack.begin(), font_stack.end(), CDeletePointer());

  font_stack.clear();

  for_each(font_def_list.begin(), font_def_list.end(), CDeletePointer());

  font_def_list.clear();

  current_font = NULL;
}
