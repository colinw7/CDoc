#include <CStrUtil.h>
#include <CFile.h>
#include <CFontMetrics.h>

static const char *COURIER_AFM          = "Courier.afm";
static const char *COURIER_BOLD_AFM     = "Courier-Bold.afm";
static const char *COURIER_ITALIC_AFM   = "Courier-Oblique.afm";
static const char *COURIER_BOLDI_AFM    = "Courier-BoldOblique.afm";
static const char *HELVETICA_AFM        = "Helvetica.afm";
static const char *HELVETICA_BOLD_AFM   = "Helvetica-Bold.afm";
static const char *HELVETICA_ITALIC_AFM = "Helvetica-Oblique.afm";
static const char *HELVETICA_BOLDI_AFM  = "Helvetica-BoldOblique.afm";
static const char *TIMES_AFM            = "Times-Roman.afm";
static const char *TIMES_BOLD_AFM       = "Times-Bold.afm";
static const char *TIMES_ITALIC_AFM     = "Times-Italic.afm";
static const char *TIMES_BOLDI_AFM      = "Times-BoldItalic.afm";

CFontMetrics::
CFontMetrics(const std::string &dirname)
{
  std::string dirname1 = dirname;

  if (dirname1.empty() || dirname1[dirname1.size() - 1] != '/')
    dirname1 += "/";

  courier_normal_ = new CFontMetric(dirname1 + COURIER_AFM);
  courier_bold_   = new CFontMetric(dirname1 + COURIER_BOLD_AFM);
  courier_italic_ = new CFontMetric(dirname1 + COURIER_ITALIC_AFM);
  courier_boldi_  = new CFontMetric(dirname1 + COURIER_BOLDI_AFM);

  helvetica_normal_ = new CFontMetric(dirname1 + HELVETICA_AFM);
  helvetica_bold_   = new CFontMetric(dirname1 + HELVETICA_BOLD_AFM);
  helvetica_italic_ = new CFontMetric(dirname1 + HELVETICA_ITALIC_AFM);
  helvetica_boldi_  = new CFontMetric(dirname1 + HELVETICA_BOLDI_AFM);

  times_normal_ = new CFontMetric(dirname1 + TIMES_AFM);
  times_bold_   = new CFontMetric(dirname1 + TIMES_BOLD_AFM);
  times_italic_ = new CFontMetric(dirname1 + TIMES_ITALIC_AFM);
  times_boldi_  = new CFontMetric(dirname1 + TIMES_BOLDI_AFM);
}

CFontMetrics::
~CFontMetrics()
{
  delete courier_normal_;
  delete courier_bold_;
  delete courier_italic_;
  delete courier_boldi_;

  delete helvetica_normal_;
  delete helvetica_bold_;
  delete helvetica_italic_;
  delete helvetica_boldi_;

  delete times_normal_;
  delete times_bold_;
  delete times_italic_;
  delete times_boldi_;
}

CFontMetric *
CFontMetrics::
getCourierMetric(CFontStyle font_style)
{
  if      (font_style == CFONT_STYLE_NORMAL)
    return courier_normal_;
  else if (font_style == CFONT_STYLE_BOLD)
    return courier_bold_;
  else if (font_style == CFONT_STYLE_ITALIC)
    return courier_italic_;
  else if (font_style == CFONT_STYLE_BOLD_ITALIC)
    return courier_boldi_;
  else
    return courier_normal_;
}

CFontMetric *
CFontMetrics::
getHelveticaMetric(CFontStyle font_style)
{
  if      (font_style == CFONT_STYLE_NORMAL)
    return helvetica_normal_;
  else if (font_style == CFONT_STYLE_BOLD)
    return helvetica_bold_;
  else if (font_style == CFONT_STYLE_ITALIC)
    return helvetica_italic_;
  else if (font_style == CFONT_STYLE_BOLD_ITALIC)
    return helvetica_boldi_;
  else
    return helvetica_normal_;
}

CFontMetric *
CFontMetrics::
getTimesMetric(CFontStyle font_style)
{
  if      (font_style == CFONT_STYLE_NORMAL)
    return times_normal_;
  else if (font_style == CFONT_STYLE_BOLD)
    return times_bold_;
  else if (font_style == CFONT_STYLE_ITALIC)
    return times_italic_;
  else if (font_style == CFONT_STYLE_BOLD_ITALIC)
    return times_boldi_;
  else
    return times_normal_;
}

void
CFontMetrics::
print(std::ostream &os)
{
  courier_normal_->print(os);
  courier_bold_  ->print(os);
  courier_italic_->print(os);
  courier_boldi_ ->print(os);

  helvetica_normal_->print(os);
  helvetica_bold_  ->print(os);
  helvetica_italic_->print(os);
  helvetica_boldi_ ->print(os);

  times_normal_->print(os);
  times_bold_  ->print(os);
  times_italic_->print(os);
  times_boldi_ ->print(os);
}

CFontMetric::
CFontMetric(const std::string &filename) :
 filename_(filename)
{
  read();
}

CFontMetric::
~CFontMetric()
{
}

bool
CFontMetric::
read()
{
  CFile file(filename_);

  if (! file.isReadable() || ! file.isRegular()) {
    std::cerr << "Failed to read file " << filename_ << std::endl;
    return false ;
  }

  xmin_      = 0;
  ymin_      = 0;
  xmax_      = 0;
  ymax_      = 0;
  descender_ = 0;

  for (int i = 0; i < 256; ++i) {
    chars_[i].width = 0;
    chars_[i].xmin  = 0;
    chars_[i].ymin  = 0;
    chars_[i].xmax  = 0;
    chars_[i].ymax  = 0;
  }

  bool in_char_metrics = false;

  std::string line;

  while (file.readLine(line)) {
    std::vector<std::string> words;

    CStrUtil::addWords(line, words);

    if (words.size() == 0)
      continue;

    if      (words[0]  == "FontBBox") {
      if (words.size() != 5)
        continue;

      if (! CStrUtil::isInteger(words[1]) ||
          ! CStrUtil::isInteger(words[2]) ||
          ! CStrUtil::isInteger(words[3]) ||
          ! CStrUtil::isInteger(words[4]))
        continue;

      xmin_ = CStrUtil::toInteger(words[1]);
      ymin_ = CStrUtil::toInteger(words[2]);
      xmax_ = CStrUtil::toInteger(words[3]);
      ymax_ = CStrUtil::toInteger(words[4]);
    }
    else if (words[0] == "Descender") {
      if (words.size() != 2)
        continue;

      if (! CStrUtil::isInteger(words[1]))
        continue;

      descender_ = CStrUtil::toInteger(words[1]);
    }
    else if (words[0] == "StartCharMetrics")
      in_char_metrics = true;
    else if (words[0] == "EndCharMetrics")
      in_char_metrics = false;
    else if (in_char_metrics) {
      std::vector<std::string> fields;

      CStrUtil::addFields(line, fields, ";");

      if (fields.size() != 5)
        continue;

      /* Get Character Number */

      std::vector<std::string> words1;

      CStrUtil::addWords(fields[0], words1);

      if (words1.size() != 2)
        continue;

      if (words1[0] != "C")
        continue;

      if (! CStrUtil::isInteger(words1[1]))
        continue;

      int char_num = CStrUtil::toInteger(words1[1]);

      if (char_num < 0 || char_num > 255)
        continue;

      /* Get Width */

      words1.clear();

      CStrUtil::addWords(fields[1], words1);

      if (words1.size() != 2)
        continue;

      if (words1[0] != "WX")
        continue;

      if (! CStrUtil::isInteger(words1[1]))
        continue;

      chars_[char_num].width = CStrUtil::toInteger(words1[1]);

      /* Get Bounding Box */

      words1.clear();

      CStrUtil::addWords(fields[3], words1);

      if (words1.size() != 5)
        continue;

      if (words1[0] != "B")
        continue;

      if (! CStrUtil::isInteger(words1[1]) ||
          ! CStrUtil::isInteger(words1[2]) ||
          ! CStrUtil::isInteger(words1[3]) ||
          ! CStrUtil::isInteger(words1[4]))
        continue;

      chars_[char_num].xmin = CStrUtil::toInteger(words1[1]);;
      chars_[char_num].ymin = CStrUtil::toInteger(words1[2]);;
      chars_[char_num].xmax = CStrUtil::toInteger(words1[3]);;
      chars_[char_num].ymax = CStrUtil::toInteger(words1[4]);;
    }
  }

  return true;
}

double
CFontMetric::
getAspect() const
{
  return double(ymax_ - ymin_)/(xmax_ - xmin_);
}

void
CFontMetric::
print(std::ostream &os)
{
  os << "(" << xmin_ << ", " << ymin_ << ", " <<
               xmax_ << ", " << ymax_ << ")"  << std::endl;

  os << descender_ << std::endl;
}
