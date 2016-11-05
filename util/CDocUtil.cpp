#include <CDocUtil.h>

#define ESC0m "[0m"
#define ESC1m "[1m"
#define ESC2m "[2m"
#define ESC3m "[3m"
#define ESC4m "[4m"

#define ILEAF_NORMAL_FONT         "<F0>"
#define ILEAF_UNDERLINE_FONT      "<F10@Z7@Lam>"
#define ILEAF_BOLD_FONT           "<F6@Z7@Lam>"
#define ILEAF_BOLD_UNDERLINE_FONT "<F11@Z7@Lam>"

const char *
CDocCDocFormat::
startNormal() const
{
  return ESC0m;
}

const char *
CDocCDocFormat::
endNormal() const
{
  return ESC0m;
}

const char *
CDocCDocFormat::
startBold() const
{
  return ESC1m;
}

const char *
CDocCDocFormat::
endBold() const
{
  return ESC0m;
}

const char *
CDocCDocFormat::
startUnderline() const
{
  return ESC4m;
}

const char *
CDocCDocFormat::
endUnderline() const
{
  return ESC0m;
}

const char *
CDocCDocFormat::
startBoldUnderline() const
{
  return ESC3m;
}

const char *
CDocCDocFormat::
endBoldUnderline() const
{
  return ESC0m;
}

//----------------

const char *
CDocRawCCFormat::
startNormal() const
{
  return ESC0m;
}

const char *
CDocRawCCFormat::
endNormal() const
{
  return ESC0m;
}

const char *
CDocRawCCFormat::
startBold() const
{
  return ESC1m;
}

const char *
CDocRawCCFormat::
endBold() const
{
  return ESC0m;
}

const char *
CDocRawCCFormat::
startUnderline() const
{
  return ESC4m;
}

const char *
CDocRawCCFormat::
endUnderline() const
{
  return ESC0m;
}

const char *
CDocRawCCFormat::
startBoldUnderline() const
{
  return ESC3m;
}

const char *
CDocRawCCFormat::
endBoldUnderline() const
{
  return ESC0m;
}

//----------------

const char *
CDocRawFormat::
startNormal() const
{
  return ESC0m;
}

const char *
CDocRawFormat::
endNormal() const
{
  return ESC0m;
}

const char *
CDocRawFormat::
startBold() const
{
  return ESC1m;
}

const char *
CDocRawFormat::
endBold() const
{
  return ESC0m;
}

const char *
CDocRawFormat::
startUnderline() const
{
  return ESC4m;
}

const char *
CDocRawFormat::
endUnderline() const
{
  return ESC0m;
}

const char *
CDocRawFormat::
startBoldUnderline() const
{
  return ESC3m;
}

const char *
CDocRawFormat::
endBoldUnderline() const
{
  return ESC0m;
}

//----------------

const char *
CDocHtmlFormat::
startNormal() const
{
  return "";
}

const char *
CDocHtmlFormat::
endNormal() const
{
  return "";
}

const char *
CDocHtmlFormat::
startBold() const
{
  return "<b>";
}

const char *
CDocHtmlFormat::
endBold() const
{
  return "</b>";
}

const char *
CDocHtmlFormat::
startUnderline() const
{
  return "<u>";
}

const char *
CDocHtmlFormat::
endUnderline() const
{
  return "</u>";
}

const char *
CDocHtmlFormat::
startBoldUnderline() const
{
  return "<b><u>";
}

const char *
CDocHtmlFormat::
endBoldUnderline() const
{
  return "</u></b>";
}

//----------------

const char *
CDocILeafFormat::
startNormal() const
{
  return "";
}

const char *
CDocILeafFormat::
endNormal() const
{
  return "";
}

const char *
CDocILeafFormat::
startBold() const
{
  return ILEAF_BOLD_FONT;
}

const char *
CDocILeafFormat::
endBold() const
{
  return ILEAF_NORMAL_FONT;
}

const char *
CDocILeafFormat::
startUnderline() const
{
  return ILEAF_UNDERLINE_FONT;
}

const char *
CDocILeafFormat::
endUnderline() const
{
  return ILEAF_NORMAL_FONT;
}

const char *
CDocILeafFormat::
startBoldUnderline() const
{
  return ILEAF_BOLD_UNDERLINE_FONT;
}

const char *
CDocILeafFormat::
endBoldUnderline() const
{
  return ILEAF_NORMAL_FONT;
}

//----------------

const char *
CDocTextFormat::
startNormal() const
{
  return "";
}

const char *
CDocTextFormat::
endNormal() const
{
  return "";
}

const char *
CDocTextFormat::
startBold() const
{
  return "";
}

const char *
CDocTextFormat::
endBold() const
{
  return "";
}

const char *
CDocTextFormat::
startUnderline() const
{
  return "";
}

const char *
CDocTextFormat::
endUnderline() const
{
  return "";
}

const char *
CDocTextFormat::
startBoldUnderline() const
{
  return "";
}

const char *
CDocTextFormat::
endBoldUnderline() const
{
  return "";
}
