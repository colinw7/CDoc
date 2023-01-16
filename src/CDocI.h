#ifndef CDOC_I_H
#define CDOC_I_H

/* Includes */

#include <std_tmpl++.h>
#include <CFuncs.h>
#include <CAlignType.h>
#include <CSpell.h>
#include <deque>

/* Defines */

#define ESC   '\033'
#define ESCF  "\033F"
#define ESCG  "\033G"
#define ESCH  "\033H"
#define ESC0m "\033[0m"
#define ESC1m "\033[1m"
#define ESC2m "\033[2m"
#define ESC3m "\033[3m"
#define ESC4m "\033[4m"

#define FILL_TMPL "\033F%d;%d;%c"

#define NEW_PAGE '\014'

#define AMP_MARK_C '\001'
#define AMP_MARK_S "\001"

#define CMD_MARK_C '\002'
#define CMD_MARK_S "\002"

#define KEEP_MARK_C '\003'
#define KEEP_MARK_S "\003"

#define BOX_TL__C '\377'
#define BOX_TR__C '\376'
#define BOX_BL__C '\375'
#define BOX_BR__C '\374'
#define BOX_TPR_C '\373'
#define BOX_BPR_C '\372'
#define BOX_LPR_C '\371'
#define BOX_RPR_C '\370'
#define BOX_MID_C '\367'
#define BOX_VRT_C '\366'
#define BOX_HOR_C '\365'

#define BOX_TL__S "\377"
#define BOX_TR__S "\376"
#define BOX_BL__S "\375"
#define BOX_BR__S "\374"
#define BOX_TPR_S "\373"
#define BOX_BPR_S "\372"
#define BOX_LPR_S "\371"
#define BOX_RPR_S "\370"
#define BOX_MID_S "\367"
#define BOX_VRT_S "\366"
#define BOX_HOR_S "\365"

/*---------------*/

#define CDOC_CMD_ID "**cdoc**"
#define CDOC_CMD_ID_LEN 8

#define CDOC_LEFT_MARGIN_CMD   "**cdoc**left_margin"
#define CDOC_LEFT_MARGIN_TMPL  "**cdoc**left_margin %d\n"
#define CDOC_RIGHT_MARGIN_CMD  "**cdoc**right_margin"
#define CDOC_RIGHT_MARGIN_TMPL "**cdoc**right_margin %d\n"
#define CDOC_PAGE_LENGTH_CMD   "**cdoc**page_length"
#define CDOC_PAGE_LENGTH_TMPL  "**cdoc**page_length %d\n"

#define CDOC_START_HEADER_CMD  "**cdoc**start_header"
#define CDOC_START_HEADER_TMPL "**cdoc**start_header\n"
#define CDOC_END_HEADER_CMD    "**cdoc**end_header"
#define CDOC_END_HEADER_TMPL   "**cdoc**end_header\n"

#define CDOC_START_FOOTER_CMD  "**cdoc**start_footer"
#define CDOC_START_FOOTER_TMPL "**cdoc**start_footer\n"
#define CDOC_END_FOOTER_CMD    "**cdoc**end_footer"
#define CDOC_END_FOOTER_TMPL   "**cdoc**end_footer\n"

#define CDOC_SECTION_CMD      "**cdoc**section"
#define CDOC_SECTION_TMPL     "**cdoc**section %s\n"
#define CDOC_SUB_SECTION_CMD  "**cdoc**sub_section"
#define CDOC_SUB_SECTION_TMPL "**cdoc**sub_section %s\n"

#define CDOC_DEF_FONT_CMD    "**cdoc**def_font"
#define CDOC_DEF_FONT_TMPL   "**cdoc**def_font %s=%s\n"
#define CDOC_START_FONT_CMD  "**cdoc**start_font"
#define CDOC_START_FONT_TMPL "**cdoc**start_font %s\n"
#define CDOC_END_FONT_CMD    "**cdoc**end_font"
#define CDOC_END_FONT_TMPL   "**cdoc**end_font\n"

#define CDOC_PARAGRAPH_BEGIN_CMD  "**cdoc**paragraph_begin"
#define CDOC_PARAGRAPH_BEGIN_TMPL "**cdoc**paragraph_begin %d\n"
#define CDOC_PARAGRAPH_END_CMD    "**cdoc**paragraph_end"
#define CDOC_PARAGRAPH_END_TMPL   "**cdoc**paragraph_end\n"
#define CDOC_PARAGRAPH_START_CMD  "**cdoc**paragraph_start"
#define CDOC_PARAGRAPH_START_TMPL "**cdoc**paragraph_start %d\n"
#define CDOC_PARAGRAPH_STOP_CMD   "**cdoc**paragraph_stop"
#define CDOC_PARAGRAPH_STOP_TMPL  "**cdoc**paragraph_stop\n"

#define CDOC_GOTO_START_CMD  "**cdoc**goto_start"
#define CDOC_GOTO_START_TMPL "**cdoc**goto_start %s\n"
#define CDOC_GOTO_END_CMD    "**cdoc**goto_end"
#define CDOC_GOTO_END_TMPL   "**cdoc**goto_end\n"

#define CDOC_COLOUR_START_CMD  "**cdoc**colour_start"
#define CDOC_COLOUR_START_TMPL "**cdoc**colour_start %d\n"
#define CDOC_FONT_START_CMD    "**cdoc**font_start"
#define CDOC_FONT_START_TMPL   "**cdoc**font_start %d\n"

#define CDOC_INDENT_CMD  "**cdoc**indent"
#define CDOC_INDENT_TMPL "**cdoc**indent %d\n"

#define CDOC_REV_CNTRL_CMD  "**cdoc**rev_cntrl"
#define CDOC_REV_CNTRL_TMPL "**cdoc**rev_cntrl %c\n"

#define CDOC_SET_LINE_CMD  "**cdoc**set_line"
#define CDOC_SET_LINE_TMPL "**cdoc**set_line %d\n"

#define CDOC_LEFT_CMD     "**cdoc**left"
#define CDOC_LEFT_TMPL    "**cdoc**left\n"
#define CDOC_CENTRE_CMD   "**cdoc**centre"
#define CDOC_CENTRE_TMPL  "**cdoc**centre\n"
#define CDOC_RIGHT_CMD    "**cdoc**right"
#define CDOC_RIGHT_TMPL   "**cdoc**right\n"
#define CDOC_JUSTIFY_CMD  "**cdoc**justify"
#define CDOC_JUSTIFY_TMPL "**cdoc**justify\n"

#define CDOC_SET_FILL_PIXMAP_CMD  "**cdoc**set_fill_pixmap"
#define CDOC_SET_FILL_PIXMAP_TMPL "**cdoc**set_fill_pixmap %s\n"
#define CDOC_FILL_BOX_CMD         "**cdoc**fill_box"
#define CDOC_FILL_BOX_TMPL        "**cdoc**fill_box %d %d %d %d\n"
#define CDOC_DRAW_LINE_CMD        "**cdoc**draw_line"
#define CDOC_DRAW_LINE_TMPL       "**cdoc**draw_line %d %d %d %d\n"

#define CDOC_COMMAND_CMD  "**cdoc**command"
#define CDOC_COMMAND_TMPL "**cdoc**command %s\t%s\n"

#define CDOC_FORMAT_CMD  "**cdoc**format"
#define CDOC_FORMAT_TMPL "**cdoc**format %s\n"

#define CDOC_BITMAP_CMD   "**cdoc**bitmap"
#define CDOC_XWD_CMD      "**cdoc**xwd"
#define CDOC_XWD_Z_CMD    "**cdoc**xwd.Z"
#define CDOC_IMAGE_CMD    "**cdoc**image"

#define CDOC_IS_CMD(s,c) \
(strncmp(s, c, strlen(c)) == 0 && \
 ((s)[strlen(c)] == '\0' || isspace((s)[strlen(c)])))

#define CDOC_IS_FONT_ESC(s) \
  ((s)[0]==ESC && (s)[1]=='[' && isdigit((s)[2]) && (s)[3]=='m')

#define CDOC_IS_INLINE_FONT_ESC(s) \
  ((s)[0]=='' && ((s)[1]=='B' || (s)[1]=='E'))

#define CDOC_IS_FOREGROUND_ESC(s) \
  ((s)[0]==ESC && (s)[1]=='[' && (s)[2]=='3' && \
   isdigit((s)[3]) && (s)[4]=='m')

#define CDOC_IS_FILL_ESC(s) ((s)[0]==ESC && (s)[1]=='F')
#define CDOC_IS_GOTO_ESC(s) ((s)[0]==ESC && (s)[1]=='G')
#define CDOC_IS_HOOK_ESC(s) ((s)[0]==ESC && (s)[1]=='H')

#define HIGHLIGHT_COLOUR "blue"

#define CDOC_MEAN_CHAR 'x'

#define SPACE_CHAR  'x'
#define INDENT_CHAR 'm'

#define CDOC_BUTTON_IN  1
#define CDOC_BUTTON_OUT 2

/*---------------*/

#define CDOC_MAX_LINE 1024

#define CDOC_MAX_HEADERS 20

#define CDocOffset(t,f) (ulong(&((static_cast<t>(nullptr))->f)))

#define CDOC_SHELL_WIDGET_CLASS1 vendorShellWidgetClass
#define CDOC_SHELL_WIDGET_CLASS2 transientShellWidgetClass

#define CDOC_DEBUG_PASS_1 1
#define CDOC_DEBUG_PASS_2 2
#define CDOC_DEBUG_OUTPUT 4

#define CDocMax(a,b) ((a)>(b)?(a):(b))
#define CDocMin(a,b) ((a)<(b)?(a):(b))

enum CDocFontType {
  CDOC_NO_FONT             = 0,
  CDOC_NORMAL_FONT         = 1,
  CDOC_UNDERLINE_FONT      = 2,
  CDOC_BOLD_FONT           = 3,
  CDOC_BOLD_UNDERLINE_FONT = 4
};

#define CDOC_PRINT_AUTOMATIC 0
#define CDOC_PRINT_PORTRAIT  1
#define CDOC_PRINT_LANDSCAPE 2

/* Includes */

#include <CStrUtil.h>

#include "CDocErrorI.h"
#include "CDocControlI.h"
#include "CDocHelpI.h"
#include "CDocHelpHookI.h"
#include "CDocHelpPanelI.h"
#include "CDocHelpDatasetDataI.h"
#include "CDocStringI.h"
#include "CDocScriptErrorI.h"
#include "CDocScriptWriteI.h"
#include "CDocScriptTempFileI.h"
#include "CDocScriptDeviceI.h"
#include "CDocScriptLineI.h"
#include "CDocScriptReferenceI.h"
#include "CDocScriptParagraphI.h"
#include "CDocScriptPageI.h"
#include "CDocScriptListI.h"
#include "CDocScriptTocI.h"
#include "CDocScriptIndexI.h"
#include "CDocScriptFigureI.h"
#include "CDocScriptExampleI.h"
#include "CDocScriptFootnoteI.h"
#include "CDocScriptTableI.h"
#include "CDocScriptPCtrlI.h"
#include "CDocScriptProcessI.h"
#include "CDocScriptMacroI.h"
#include "CDocScriptOptionsI.h"
#include "CDocScriptTitlePageI.h"
#include "CDocScriptAmendmentsI.h"
#include "CDocScriptFrontSheetI.h"
#include "CDocScriptMemoI.h"
#include "CDocScriptFontI.h"
#include "CDocScriptLabelI.h"
#include "CDocScriptSymbolI.h"
#include "CDocScriptTranslateI.h"
#include "CDocScriptRevCtrlI.h"
#include "CDocScriptDatasetI.h"

#endif
