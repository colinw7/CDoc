#include <CPrint.h>
#include <CStrUtil.h>
#include <CFile.h>
#include <CImageLib.h>
#include <COSTime.h>

#define MAX_DATE_STRING_LEN 256

#define COURIER_N    "Courier"
#define COURIER_B    "Courier-Bold"
#define COURIER_I    "Courier-Oblique"
#define COURIER_BI   "Courier-BoldOblique"
#define HELVETICA_N  "Helvetica"
#define HELVETICA_B  "Helvetica-Bold"
#define HELVETICA_I  "Helvetica-Oblique"
#define HELVETICA_BI "Helvetica-BoldOblique"
#define TIMES_N      "Times-Roman"
#define TIMES_B      "Times-Bold"
#define TIMES_I      "Times-Italic"
#define TIMES_BI     "Times-BoldItalic"
#define SYMBOL_N     "Symbol"

#define FILL_STRING_DEF "\
/fillString {\n\
  /string exch def\n\
  /angle exch def\n\
  /y exch def\n\
  /x exch def\n\
  /fb exch def\n\
  /fg exch def\n\
  /fr exch def\n\
  /bb exch def\n\
  /bg exch def\n\
  /br exch def\n\
\n\
  x y moveto\n\
  string true charpath flattenpath pathbbox\n\
  /ury exch 1 add def\n\
  /urx exch 1 add def\n\
  /lly exch 1 sub def\n\
  /llx exch 1 sub def\n\
\n\
  br bg bb setrgbcolor\n\
\n\
  newpath\n\
  llx lly moveto\n\
  angle rotate\n\
  urx lly lineto\n\
  urx ury lineto\n\
  llx ury lineto\n\
  0 angle sub rotate\n\
  closepath fill\n\
\n\
  fr fg fb setrgbcolor\n\
\n\
  newpath\n\
  x y moveto\n\
  angle rotate\n\
  string show\n\
  0 angle sub rotate\n\
} def\n\
"

#define COLOR_IMAGE_DEF "\
/bwproc {\n\
 rgbproc\n\
 dup length 3 idiv string 0 3 0\n\
 5 -1 roll {\n\
  add 2 1 roll 1 sub dup 0 eq\n\
  {\n\
   pop 3 idiv 3 -1 roll dup 4 -1 roll dup\n\
   3 1 roll 5 -1 roll put 1 add 3 0\n\
  }\n\
  {\n\
   2 1 roll\n\
  } ifelse\n\
 } forall\n\
 pop pop pop\n\
} def\n\
systemdict /colorimage known not {\n\
 /colorimage {\n\
  pop pop\n\
  /rgbproc exch def\n\
  { bwproc } image\n\
 } def\n\
} if\n\
"

CPrint::
CPrint() :
 bg_(0,0,0),
 fg_(1,1,1)
{
  active_ = false;

  filename_ = ".screen.ps";

  creator_ = "Nobody";
  title_   = "Nobody";

  page_width_  = 600;
  page_height_ = 800;

  has_range_ = false;

  xmin_ = 1.0;
  ymin_ = 1.0;
  xmax_ = 2.0;
  ymax_ = 2.0;

  width_  = xmax_ - xmin_;
  height_ = ymax_ - ymin_;

  font_width_   = 8;
  font_ascent_  = 8;
  font_descent_ = 2;
  font_name_    = "FontCN";

  line_width_      =  1.0;
  last_line_width_ = -1.0;

  fill_string_defined_ = false;
  color_image_defined_ = false;

  output_color_  = true;
  output_depth_  = 4;
  output_invert_ = false;
}

CPrint::
~CPrint()
{
}

bool
CPrint::
setFilename(const std::string &filename)
{
  filename_ = filename;

  if (filename == "") {
    file_ = 0;

    return true;
  }

  file_ = std::make_unique<CFile>(filename_);

  return file_->open(CFile::Mode::WRITE);
}

void
CPrint::
setFile(FILE *fp)
{
  file_ = std::make_unique<CFile>(fp);
}

void
CPrint::
setCreator(const std::string &creator)
{
  creator_ = creator;
}

void
CPrint::
setTitle(const std::string &title)
{
  title_ = title;
}

void
CPrint::
setSize(double xmin, double ymin, double xmax, double ymax)
{
  xmin_ = xmin;
  ymin_ = ymin;
  xmax_ = xmax;
  ymax_ = ymax;

  width_  = xmax_ - xmin_;
  height_ = ymax_ - ymin_;

  has_range_ = true;
}

void
CPrint::
init()
{
  if (! file_)
    file_ = std::make_unique<CFile>(filename_);

  writeHeader();
}

void
CPrint::
term()
{
  writeFooter();

  file_ = 0;
}

void
CPrint::
writeHeader()
{
  std::string date_str = COSTime::getTimeString("%Y.%m.%d-%H:%M:%S");

  /*------*/

  std::string s;

  file_->write("%!PS-Adobe-1.0\n");
  file_->write("%%Creator: " + creator_ + "\n");
  file_->write("%%Title: " + title_ + "\n");
  file_->write("%%CreationDate: "+ date_str);
  file_->write("\n");

  file_->write("%%DocumentFonts: ");
  file_->write(s + COURIER_N   + " " + COURIER_B   + " " + COURIER_I   + " " + COURIER_BI   + " ");
  file_->write(s + HELVETICA_N + " " + HELVETICA_B + " " + HELVETICA_I + " " + HELVETICA_BI + " ");
  file_->write(s + TIMES_N     + " " + TIMES_B     + " " + TIMES_I     + " " + TIMES_BI     + " ");
  file_->write(s + SYMBOL_N + "\n");

  file_->write("%%EndComments\n");

  /*------*/

  file_->write(s + "/FontCN  /" + COURIER_N    + " findfont def\n");
  file_->write(s + "/FontCB  /" + COURIER_B    + " findfont def\n");
  file_->write(s + "/FontCI  /" + COURIER_I    + " findfont def\n");
  file_->write(s + "/FontCBI /" + COURIER_BI   + " findfont def\n");
  file_->write(s + "/FontHN  /" + HELVETICA_N  + " findfont def\n");
  file_->write(s + "/FontHB  /" + HELVETICA_B  + " findfont def\n");
  file_->write(s + "/FontHI  /" + HELVETICA_I  + " findfont def\n");
  file_->write(s + "/FontHBI /" + HELVETICA_BI + " findfont def\n");
  file_->write(s + "/FontTN  /" + TIMES_N      + " findfont def\n");
  file_->write(s + "/FontTB  /" + TIMES_B      + " findfont def\n");
  file_->write(s + "/FontTI  /" + TIMES_I      + " findfont def\n");
  file_->write(s + "/FontTBI /" + TIMES_BI     + " findfont def\n");
  file_->write(s + "/FontSN  /" + SYMBOL_N     + " findfont def\n");

  /*------*/

  /* Calculate Page Bounding Box */

  file_->write("clippath pathbbox\n");
  file_->write("/YMax exch def\n");
  file_->write("/XMax exch def\n");
  file_->write("/YMin exch def\n");
  file_->write("/XMin exch def\n");
  file_->write("newpath\n");

  /*------*/

  /* Set Page Scaling Factor */

  double aspect = 1.0;

  if (! has_range_) {
    if      (per_page_ == 4) {
      file_->write("/HSpace XMax YMin sub 20 div def\n");
      file_->write("/VSpace YMax YMin sub 20 div def\n");

      file_->write("/Width  XMax XMin sub HSpace sub 2 div def\n");
      file_->write("/Height YMax YMin sub VSpace sub 2 div def\n");

      if (orient_ == PORTRAIT) {
        file_->write("/XScale Width  " + CStrUtil::toString(page_width_ ) + " div def\n");
        file_->write("/YScale Height " + CStrUtil::toString(page_height_) + " div def\n");
      }
      else {
        file_->write("/XScale Height " + CStrUtil::toString(page_width_ ) + " div def\n");
        file_->write("/YScale Width  " + CStrUtil::toString(page_height_) + " div def\n");
      }

      file_->write("XScale YScale lt\n");
      file_->write(" { /Scale XScale def }\n");
      file_->write(" { /Scale YScale def }\n");
      file_->write("ifelse\n");

      file_->write("Scale 1 gt\n");
      file_->write(" { /Scale 1 def }\n");
      file_->write("if\n");

      if (orient_ == PORTRAIT) {
        file_->write("/XOffset Width  " + CStrUtil::toString(page_width_) +
                     " Scale mul sub 2 div def\n");
        file_->write("/YOffset Height " + CStrUtil::toString(page_height_) +
                     " Scale mul sub 2 div def\n");
      }
      else {
        file_->write("/XOffset Height " + CStrUtil::toString(page_width_) +
                     " Scale mul sub 2 div def\n");
        file_->write("/YOffset Width  " + CStrUtil::toString(page_height_) +
                     " Scale mul sub 2 div def\n");
      }
    }
    else if (per_page_ == 2) {
      file_->write("/VSpace YMax YMin sub 20 div def\n");

      file_->write("/Width  XMax XMin sub def\n");
      file_->write("/Height YMax YMin sub VSpace sub 2 div def\n");

      if (orient_ == PORTRAIT) {
        file_->write("/XScale Height " + CStrUtil::toString(page_width_ ) + " div def\n");
        file_->write("/YScale Width  " + CStrUtil::toString(page_height_) + " div def\n");
      }
      else {
        file_->write("/XScale Width  " + CStrUtil::toString(page_width_ ) + " div def\n");
        file_->write("/YScale Height " + CStrUtil::toString(page_height_) + " div def\n");
      }

      file_->write("XScale YScale lt\n");
      file_->write(" { /Scale XScale def }\n");
      file_->write(" { /Scale YScale def }\n");
      file_->write("ifelse\n");

      file_->write("Scale 1 gt\n");
      file_->write(" { /Scale 1 def }\n");
      file_->write("if\n");

      if (orient_ == PORTRAIT) {
        file_->write("/XOffset Height " + CStrUtil::toString(page_width_) +
                     " Scale mul sub 2 div def\n");
        file_->write("/YOffset Width  " + CStrUtil::toString(page_height_) +
                     " Scale mul sub 2 div def\n");
      }
      else {
        file_->write("/XOffset Width  " + CStrUtil::toString(page_width_) +
                     " Scale mul sub 2 div def\n");
        file_->write("/YOffset Height " + CStrUtil::toString(page_height_) +
                     " Scale mul sub 2 div def\n");
      }
    }
    else {
      file_->write("/Width  XMax XMin sub def\n");
      file_->write("/Height YMax YMin sub def\n");

      if (orient_ == PORTRAIT) {
        file_->write("/XScale Width  " + CStrUtil::toString(page_width_ ) + " div def\n");
        file_->write("/YScale Height " + CStrUtil::toString(page_height_) + " div def\n");
      }
      else {
        file_->write("/XScale Height " + CStrUtil::toString(page_width_ ) + " div def\n");
        file_->write("/YScale Width  " + CStrUtil::toString(page_height_) + " div def\n");
      }

      file_->write("XScale YScale lt\n");
      file_->write(" { /Scale XScale def }\n");
      file_->write(" { /Scale YScale def }\n");
      file_->write("ifelse\n");

      file_->write("Scale 1 gt\n");
      file_->write(" { /Scale 1 def }\n");
      file_->write("if\n");

      /* Set Page Offset */

      if (orient_ == PORTRAIT) {
        file_->write("/XOffset Width  " + CStrUtil::toString(page_width_) +
                     " Scale mul sub 2 div def\n");
        file_->write("/YOffset Height " + CStrUtil::toString(page_height_) +
                     " Scale mul sub 2 div def\n");
      }
      else {
        file_->write("/XOffset Height " + CStrUtil::toString(page_width_) +
                     " Scale mul sub 2 div def\n");
        file_->write("/YOffset Width  " + CStrUtil::toString(page_height_) +
                     " Scale mul sub 2 div def\n");
      }
    }
  }
  else {
    file_->write("/Width  XMax XMin sub def\n");
    file_->write("/Height YMax YMin sub def\n");

    file_->write("/PlotXMin " + CStrUtil::toString(xmin_) + " def\n");
    file_->write("/PlotYMin " + CStrUtil::toString(ymin_) + " def\n");
    file_->write("/PlotXMax " + CStrUtil::toString(xmax_) + " def\n");
    file_->write("/PlotYMax " + CStrUtil::toString(ymax_) + " def\n");

    file_->write("/PlotWidth  PlotXMax PlotXMin sub def\n");
    file_->write("/PlotHeight PlotYMax PlotYMin sub def\n");

    /*------*/

    aspect = width_/height_;

    if (aspect <= 1.0) {
      file_->write("/XScale Width  PlotWidth  div def\n");
      file_->write("/YScale Height PlotHeight div def\n");
    }
    else {
      file_->write("/XScale Height PlotWidth  div def\n");
      file_->write("/YScale Width  PlotHeight div def\n");
    }

    file_->write("XScale YScale lt\n");
    file_->write(" { /Scale XScale def }\n");
    file_->write(" { /Scale YScale def }\n");
    file_->write("ifelse\n");

    if (aspect <= 1.0) {
      file_->write("/XOffset Width  PlotWidth  Scale mul sub 2 div def\n");
      file_->write("/YOffset Height PlotHeight Scale mul sub 2 div def\n");
    }
    else {
      file_->write("/XOffset Height PlotWidth  Scale mul sub 2 div def\n");
      file_->write("/YOffset Width  PlotHeight Scale mul sub 2 div def\n");
    }
  }

  /*------*/

  file_->write("%%EndProlog\n");

  /*------*/

#if 0
  file_->write("%%Page: 1 1\n");
#endif

  /*------*/

  if (has_range_) {
    if (aspect <= 1.0) {
      file_->write("XMin YMin translate\n");
      file_->write("XOffset YOffset translate\n");
      file_->write("Scale dup scale\n");
      file_->write("PlotXMin neg PlotYMin neg translate\n");
    }
    else {
      file_->write("XMax YMin translate\n");
      file_->write("YOffset neg XOffset translate\n");
      file_->write("Scale dup scale\n");
      file_->write("90 rotate\n");
      file_->write("PlotXMin neg PlotYMin neg translate\n");
    }

    /*------*/

    file_->write("PlotXMin PlotYMin moveto\n");
    file_->write("PlotXMax PlotYMin lineto\n");
    file_->write("PlotXMax PlotYMax lineto\n");
    file_->write("PlotXMin PlotYMax lineto\n");
    file_->write("clip newpath\n");
  }

  /*------*/

  file_->write("true setstrokeadjust\n");

  /*------*/

  active_ = true;
}

void
CPrint::
clear()
{
}

void
CPrint::
writeFooter(bool showpage)
{
  if (showpage)
    file_->write("showpage\n");

  file_->write("%%Trailer\n");

  active_ = false;
}

void
CPrint::
startPage(int page_no1, int page_no2)
{
  if      (per_page_ == 4) {
    if (orient_ == PORTRAIT) {
      if     (page_no2 % 4 == 1) {
        if (page_no1 > 1)
          file_->write("showpage\n");

        file_->write("%%%%Page: " + CStrUtil::toString(page_no1) + " " +
                     CStrUtil::toString(page_no1) + "\n");

        file_->write("1 setlinewidth\n");
        file_->write("XMin YMin YMax add 2 div moveto\n");
        file_->write("XMax YMin YMax add 2 div lineto\n");
        file_->write("stroke\n");
        file_->write("XMin XMax add 2 div YMin moveto\n");
        file_->write("XMin XMax add 2 div YMax lineto\n");
        file_->write("stroke\n");

        file_->write("/NewX 0 def\n");
        file_->write("/NewY Height VSpace add def\n");
      }
      else if (page_no2 % 4 == 2) {
        file_->write("initgraphics\n");

        file_->write("/NewX Width HSpace add def\n");
        file_->write("/NewY Height VSpace add def\n");
      }
      else if (page_no2 % 4 == 3) {
        file_->write("initgraphics\n");

        file_->write("/NewX 0 def\n");
        file_->write("/NewY 0 def\n");
      }
      else {
        file_->write("initgraphics\n");

        file_->write("/NewX Width HSpace add def\n");
        file_->write("/NewY 0 def\n");
      }

      file_->write("XMin YMin translate\n");
      file_->write("NewX NewY translate\n");
      file_->write("XOffset YOffset translate\n");
      file_->write("Scale dup scale\n");
    }
    else {
      if     (page_no2 % 4 == 1) {
        if (page_no1 > 1)
          file_->write("showpage\n");

        file_->write("%%%%Page: " + CStrUtil::toString(page_no1) + " " +
                     CStrUtil::toString(page_no1) + "\n");

        file_->write("1 setlinewidth\n");
        file_->write("XMin YMin YMax add 2 div moveto\n");
        file_->write("XMax YMin YMax add 2 div lineto\n");
        file_->write("stroke\n");
        file_->write("XMin XMax add 2 div YMin moveto\n");
        file_->write("XMin XMax add 2 div YMax lineto\n");
        file_->write("stroke\n");

        file_->write("/NewX Width HSpace add def\n");
        file_->write("/NewY 0 def\n");
      }
      else if (page_no2 % 4 == 2) {
        file_->write("initgraphics\n");

        file_->write("/NewX 0 def\n");
        file_->write("/NewY 0 def\n");
      }
      else if (page_no2 % 4 == 3) {
        file_->write("initgraphics\n");

        file_->write("/NewX Width HSpace add def\n");
        file_->write("/NewY Height VSpace add def\n");
      }
      else {
        file_->write("initgraphics\n");

        file_->write("/NewX 0 def\n");
        file_->write("/NewY Height VSpace add def\n");
      }

      file_->write("XMax YMin translate\n");
      file_->write("NewX neg NewY translate\n");
      file_->write("YOffset neg XOffset translate\n");
      file_->write("Scale dup scale\n");
      file_->write("90 rotate\n");
    }
  }
  else if (per_page_ == 2) {
    if (orient_ == PORTRAIT) {
      if (page_no2 & 1) {
        if (page_no1 > 1)
          file_->write("showpage\n");

        file_->write("%%%%Page: " + CStrUtil::toString(page_no1) + " " +
                     CStrUtil::toString(page_no1) + "\n");

        file_->write("1 setlinewidth\n");
        file_->write("XMin YMin YMax add 2 div moveto\n");
        file_->write("XMax YMin YMax add 2 div lineto\n");
        file_->write("stroke\n");

        file_->write("/NewY 0 def\n");
      }
      else {
        file_->write("initgraphics\n");

        file_->write("/NewY Height VSpace add def\n");
      }

      file_->write("XMax YMin translate\n");
      file_->write("0 NewY translate\n");
      file_->write("YOffset neg XOffset translate\n");
      file_->write("Scale dup scale\n");
      file_->write("90 rotate\n");
    }
    else {
      if (page_no2 & 1) {
        if (page_no1 > 1)
          file_->write("showpage\n");

        file_->write("%%%%Page: " + CStrUtil::toString(page_no1) + " " +
                     CStrUtil::toString(page_no1) + "\n");

        file_->write("1 setlinewidth\n");
        file_->write("XMin YMin YMax add 2 div moveto\n");
        file_->write("XMax YMin YMax add 2 div lineto\n");
        file_->write("stroke\n");

        file_->write("/NewY Height VSpace add def\n");
      }
      else {
        file_->write("initgraphics\n");

        file_->write("/NewY 0 def\n");
      }

      file_->write("XMin YMin translate\n");
      file_->write("0 NewY translate\n");
      file_->write("XOffset YOffset translate\n");
      file_->write("Scale dup scale\n");
    }
  }
  else {
    if (page_no1 > 1)
      file_->write("showpage\n");

    file_->write("%%%%Page: " + CStrUtil::toString(page_no1) + " " +
                 CStrUtil::toString(page_no1) + "\n");

    if (orient_ == PORTRAIT) {
      file_->write("XMin YMin translate\n");
      file_->write("XOffset YOffset translate\n");
      file_->write("Scale dup scale\n");
    }
    else {
      file_->write("XMax YMin translate\n");
      file_->write("YOffset neg XOffset translate\n");
      file_->write("Scale dup scale\n");
      file_->write("90 rotate\n");
    }
  }

  file_->write("0 0 moveto\n");
  file_->write(CStrUtil::toString(page_width_) + " 0 lineto\n");
  file_->write(CStrUtil::toString(page_width_) + " " +
               CStrUtil::toString(page_height_) + " lineto\n");
  file_->write("0 " + CStrUtil::toString(page_height_) + " lineto\n");
  file_->write("clip newpath\n");
}

void
CPrint::
drawCircle(double x, double y, double r)
{
  assert(active_);

  drawArc(x, y, r, r, 0.0, 360.0);
}

void
CPrint::
fillCircle(double x, double y, double r)
{
  assert(active_);

  fillArc(x, y, r, r, 0.0, 360.0);
}

void
CPrint::
drawEllipse(double x, double y, double xr, double yr)
{
  assert(active_);

  drawArc(x, y, xr, yr, 0.0, 360.0);
}

void
CPrint::
fillEllipse(double x, double y, double xr, double yr)
{
  assert(active_);

  fillArc(x, y, xr, yr, 0.0, 360.0);
}

void
CPrint::
drawArc(double x, double y, double xr, double, double angle1, double angle2)
{
  assert(active_);

  file_->write("newpath ");
  file_->write(CStrUtil::toString(x));
  file_->write(" ");
  file_->write(CStrUtil::toString(y));
  file_->write(" ");
  file_->write(CStrUtil::toString(xr));
  file_->write(" ");
  file_->write(CStrUtil::toString(angle1));
  file_->write(" ");
  file_->write(CStrUtil::toString(angle1 + angle2));
  file_->write(" ");
  file_->write(angle2 > 0 ? "arc" : "arcn");
  file_->write(" closepath stroke\n");
}

void
CPrint::
fillArc(double x, double y, double xr, double, double angle1, double angle2)
{
  assert(active_);

  file_->write("newpath ");
  file_->write(CStrUtil::toString(x));
  file_->write(" ");
  file_->write(CStrUtil::toString(y));
  file_->write(" ");
  file_->write(CStrUtil::toString(xr));
  file_->write(" ");
  file_->write(CStrUtil::toString(angle1));
  file_->write(" ");
  file_->write(CStrUtil::toString(angle1 + angle2));
  file_->write(" ");
  file_->write(angle2 > 0 ? "arc" : "arcn");
  file_->write(" closepath fill\n");
}

void
CPrint::
drawRectangle(double x1, double y1, double x2, double y2)
{
  assert(active_);

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x1) + " " + CStrUtil::toString(y1) + " moveto\n");
  file_->write(CStrUtil::toString(x2) + " " + CStrUtil::toString(y1) + " lineto\n");
  file_->write(CStrUtil::toString(x2) + " " + CStrUtil::toString(y2) + " lineto\n");
  file_->write(CStrUtil::toString(x1) + " " + CStrUtil::toString(y2) + " lineto\n");

  file_->write("closepath stroke\n");
}

void
CPrint::
fillRectangle(double x1, double y1, double x2, double y2)
{
  assert(active_);

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x1) + " " + CStrUtil::toString(y1) + " moveto\n");
  file_->write(CStrUtil::toString(x2) + " " + CStrUtil::toString(y1) + " lineto\n");
  file_->write(CStrUtil::toString(x2) + " " + CStrUtil::toString(y2) + " lineto\n");
  file_->write(CStrUtil::toString(x1) + " " + CStrUtil::toString(y2) + " lineto\n");

  file_->write("closepath fill\n");
}

void
CPrint::
drawPolygon(double *x, double *y, int n)
{
  assert(active_);

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x[0]));
  file_->write(" ");
  file_->write(CStrUtil::toString(y[0]));
  file_->write(" moveto\n");

  for (int i = 1; i < n; i++) {
    file_->write(CStrUtil::toString(x[i]));
    file_->write(" ");
    file_->write(CStrUtil::toString(y[i]));
    file_->write(" lineto\n");
  }

  file_->write("closepath stroke\n");
}

void
CPrint::
fillPolygon(double *x, double *y, int n)
{
  assert(active_);

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x[0]));
  file_->write(" ");
  file_->write(CStrUtil::toString(y[0]));
  file_->write(" moveto\n");

  for (int i = 1; i < n; i++) {
    file_->write(CStrUtil::toString(x[i]));
    file_->write(" ");
    file_->write(CStrUtil::toString(y[i]));
    file_->write(" lineto\n");
  }

  file_->write("closepath fill\n");
}

void
CPrint::
drawPath(double *x, double *y, int n)
{
  assert(active_);

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x[0]));
  file_->write(" ");
  file_->write(CStrUtil::toString(y[0]));
  file_->write(" moveto\n");

  for (int i = 1; i < n; i++) {
    file_->write(CStrUtil::toString(x[i]));
    file_->write(" ");
    file_->write(CStrUtil::toString(y[i]));
    file_->write(" lineto\n");
  }

  file_->write("stroke\n");
}

void
CPrint::
drawLine(double x1, double y1, double x2, double y2)
{
  assert(active_);

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x1) + " " + CStrUtil::toString(y1) + " moveto\n");
  file_->write(CStrUtil::toString(x2) + " " + CStrUtil::toString(y2) + " lineto\n");

  file_->write("stroke\n");
}

void
CPrint::
drawLines(double *x, double *y, int num_xy)
{
  assert(active_);

  if (num_xy <= 0)
    return;

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x[0]));
  file_->write(" ");
  file_->write(CStrUtil::toString(y[0]));
  file_->write(" moveto\n");

  for (int i = 1; i < num_xy; i++) {
    file_->write(CStrUtil::toString(x[i]));
    file_->write(" ");
    file_->write(CStrUtil::toString(y[i]));
    file_->write(" lineto\n");
  }

  file_->write("stroke\n");
}

void
CPrint::
drawPoint(double x, double y)
{
  assert(active_);

  file_->write("newpath\n");

  file_->write(CStrUtil::toString(x));
  file_->write(" ");
  file_->write(CStrUtil::toString(y));
  file_->write(" moveto\n");
  file_->write(CStrUtil::toString(x));
  file_->write(" ");
  file_->write(CStrUtil::toString(y));
  file_->write(" lineto\n");

  file_->write("stroke\n");
}

void
CPrint::
drawText(double x, double y, const std::string &str, int angle)
{
  assert(active_);

  std::string str1;

  int len = str.size();

  for (int i = 0; i < len; i++) {
    if (str[i] == '(' || str[i] == ')')
      str1 += '\\';

    str1 += str[i];
  }

  /*------*/

  double x1 = x + (font_ascent_ + font_descent_)*sin(angle);
  double y1 = y - (font_ascent_ + font_descent_)*cos(angle);

  file_->write(CStrUtil::toString(x1));
  file_->write(" ");
  file_->write(CStrUtil::toString(y1));
  file_->write(" moveto\n");

  if (angle) {
    file_->write(CStrUtil::toString(angle));
    file_->write(" rotate\n");
  }

  file_->write("(");
  file_->write(str1);
  file_->write(") show\n");

  file_->write("-");
  file_->write(CStrUtil::toString(angle));
  file_->write(" rotate\n");
}

void
CPrint::
showText(double x, double y, const std::string &str)
{
  file_->write(CStrUtil::toString(x) + " " + CStrUtil::toString(y) + " moveto\n");

  file_->write("(" + str + ") show\n");
}

void
CPrint::
fillText(double x, double y, const std::string &str, int angle)
{
  assert(active_);

  if (! fill_string_defined_) {
    file_->write(FILL_STRING_DEF);

    fill_string_defined_ = true;
  }

  /*------*/

  std::string str1;

  int len = str.size();

  for (int i = 0; i < len; i++) {
    if (str[i] == '(' || str[i] == ')')
      str1 += '\\';

    str1 += str[i];
  }

  /*------*/

  double x1 = x + (font_ascent_ + font_descent_)*sin(angle);
  double y1 = y - (font_ascent_ + font_descent_)*cos(angle);

  file_->write(CStrUtil::toString(bg_.getRed()));
  file_->write(" ");
  file_->write(CStrUtil::toString(bg_.getGreen()));
  file_->write(" ");
  file_->write(CStrUtil::toString(bg_.getBlue()));
  file_->write(" ");
  file_->write(CStrUtil::toString(fg_.getRed()));
  file_->write(" ");
  file_->write(CStrUtil::toString(fg_.getGreen()));
  file_->write(" ");
  file_->write(CStrUtil::toString(fg_.getBlue()));
  file_->write(" ");
  file_->write(CStrUtil::toString(x1));
  file_->write(" ");
  file_->write(CStrUtil::toString(y1));
  file_->write(" ");
  file_->write(CStrUtil::toString(angle));
  file_->write(" (");
  file_->write(str1);
  file_->write(") fillString\n");
}

void
CPrint::
drawImage(const CImagePtr &image, double x, double y, int width, int height,
          bool resize, int rotate)
{
  file_->write("save\n");

  if (resize) {
    double scale;

    if (rotate == 90 || rotate == 270)
      scale = double(height)/image->getWidth();
    else
      scale = double(height)/image->getHeight();

    double offset;

    if (rotate == 90 || rotate == 270)
      offset = (width - image->getHeight()*scale)/2.0;
    else
      offset = (width - image->getWidth() *scale)/2.0;

    file_->write(CStrUtil::toString(x + offset) + " " +
                 CStrUtil::toString(page_height_ - y - height) + " translate\n");

    file_->write(CStrUtil::toString(scale) + " dup scale\n");
  }
  else {
    if (rotate == 90 || rotate == 270)
      file_->write(CStrUtil::toString(x) + " " +
                   CStrUtil::toString(page_height_ - y - image->getWidth()) + " translate\n");
    else
      file_->write(CStrUtil::toString(x) + " " +
                   CStrUtil::toString(page_height_ - y - image->getHeight()) + " translate\n");
  }

  file_->write("1 1 scale\n");

  if      (rotate == 90)
    file_->write(CStrUtil::toString(image->getHeight()) + " 0 translate\n");
  else if (rotate == 180)
    file_->write(CStrUtil::toString(image->getWidth()) + " " +
                 CStrUtil::toString(image->getHeight()) + " translate\n");
  else if (rotate == 270)
    file_->write("0 " + CStrUtil::toString(image->getWidth()) + " translate\n");

  if (rotate != 0)
    file_->write(CStrUtil::toString(rotate) + " rotate\n");

  drawImage(image, 0, 0);

  file_->write("restore\n");
}

void
CPrint::
drawImage(const CImagePtr &image, double x, double y)
{
  assert(active_);

  drawSubImage(image, 0, 0, x, y, image->getWidth(), image->getHeight());
}

void
CPrint::
drawSubImage(const CImagePtr &image, double src_x, double src_y,
             double dst_x, double dst_y, double width1, double height1)
{
  assert(active_);

  if (! color_image_defined_) {
    file_->write(COLOR_IMAGE_DEF);

    color_image_defined_ = true;
  }

  /*------*/

  int width2  = image->getWidth ();
  int height2 = image->getHeight();

  if (src_x + width1 > width2)
    width1 = width2 - src_x;

  if (src_y + height1 > height2)
    height1 = height2 - src_y;

  file_->write("save\n");

  file_->write(CStrUtil::toString(dst_x));
  file_->write(" ");
  file_->write(CStrUtil::toString(height_ - dst_y - height2));
  file_->write(" translate\n");
  file_->write(CStrUtil::toString(width2));
  file_->write(" ");
  file_->write(CStrUtil::toString(height2));
  file_->write(" scale\n");

  if (output_color_) {
    file_->write("/buffer ");
    file_->write(CStrUtil::toString((int) (3*width1)));
    file_->write(" string def\n");

    file_->write(CStrUtil::toString((int) width1));
    file_->write(" ");
    file_->write(CStrUtil::toString((int) height1));
    file_->write(" 8\n");
    file_->write("[");
    file_->write(CStrUtil::toString(width2));
    file_->write(" 0 0 ");
    file_->write(CStrUtil::toString(-height2));
    file_->write(" 0 ");
    file_->write(CStrUtil::toString(height2));
    file_->write("]\n");
    file_->write("{currentfile buffer readhexstring pop}\n");

    file_->write("false 3 colorimage\n");

    double r, g, b, a;

    for (int y1 = 0; y1 < (int) height1; y1++) {
      int byte_count = 0;

      for (int x1 = 0; x1 < (int) width1; x1++) {
        image->getRGBAPixel((int) (src_x + x1), (int) (src_y + y1),
                            &r, &g, &b, &a);

        int r1 = (int) (r*255);
        int g1 = (int) (g*255);
        int b1 = (int) (b*255);

        file_->write(CStrUtil::toHexString(r1 & 0xFF, 2));
        file_->write(CStrUtil::toHexString(g1 & 0xFF, 2));
        file_->write(CStrUtil::toHexString(b1 & 0xFF, 2));

        if (byte_count % 19 == 18)
          file_->write("\n");

        byte_count++;
      }

      file_->write("\n");
    }
  }
  else if (output_depth_ == 8) {
    file_->write("/buffer ");
    file_->write(CStrUtil::toString((int) width1));
    file_->write(" string def\n");

    file_->write(CStrUtil::toString((int) width1));
    file_->write(" ");
    file_->write(CStrUtil::toString((int) height1));
    file_->write(" 8\n");
    file_->write("[");
    file_->write(CStrUtil::toString(width2));
    file_->write(" 0 0 ");
    file_->write(CStrUtil::toString(-height2));
    file_->write(" 0 ");
    file_->write(CStrUtil::toString(height2));
    file_->write("]\n");
    file_->write("{currentfile buffer readhexstring pop}\n");

    file_->write("image\n");

    double r, g, b, a;

    for (int y1 = 0; y1 < (int) height1; y1++) {
      int byte_count = 0;

      for (int x1 = 0; x1 < (int) width1; x1++) {
        image->getRGBAPixel((int) (src_x + x1), (int) (src_y + y1),
                            &r, &g, &b, &a);

        double gray = (r + g + b)/3.0;

        int gray1 = (int) (gray*255);

        if (output_invert_)
          gray1 = ~gray1;

        file_->write(CStrUtil::toHexString(gray1 & 0xFF, 2));

        if (byte_count % 19 == 18)
          file_->write("\n");

        byte_count++;
      }

      file_->write("\n");
    }
  }
  else if (output_depth_ == 4) {
    file_->write("/buffer ");
    file_->write(CStrUtil::toString((int) ((width1 + 1)/2)));
    file_->write(" string def\n");

    file_->write(CStrUtil::toString((int) width1));
    file_->write(" ");
    file_->write(CStrUtil::toString((int) height1));
    file_->write(" 4\n");
    file_->write("[");
    file_->write(CStrUtil::toString(width2));
    file_->write(" 0 0 ");
    file_->write(CStrUtil::toString(-height2));
    file_->write(" 0 ");
    file_->write(CStrUtil::toString(height2));
    file_->write("]\n");
    file_->write("{currentfile buffer readhexstring pop}\n");

    file_->write("image\n");

    double r, g, b, a;

    for (int y1 = 0; y1 < (int) height1; y1++) {
      int byte_count = 0;

      int pos  = 7;
      int byte = 0;

      for (int x1 = 0; x1 < (int) width1; x1++) {
        image->getRGBAPixel((int) (src_x + x1), (int) (src_y + y1),
                            &r, &g, &b, &a);

        double gray = (r + g + b)/3.0;

        int gray1 = (int) (gray*255);

        if      (gray1 >= 0xF0) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 1);
          byte |= 1 << (pos - 2);
          byte |= 1 << (pos - 3);
        }
        else if (gray1 >= 0xE0) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 1);
          byte |= 1 << (pos - 2);
        }
        else if (gray1 >= 0xD0) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 1);
          byte |= 1 << (pos - 3);
        }
        else if (gray1 >= 0xC0) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 1);
        }
        else if (gray1 >= 0xB0) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 2);
          byte |= 1 << (pos - 3);
        }
        else if (gray1 >= 0xA0) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 2);
        }
        else if (gray1 >= 0x90) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 3);
        }
        else if (gray1 >= 0x80) {
          byte |= 1 << (pos    );
        }
        else if (gray1 >= 0x70) {
          byte |= 1 << (pos - 1);
          byte |= 1 << (pos - 2);
          byte |= 1 << (pos - 3);
        }
        else if (gray1 >= 0x60) {
          byte |= 1 << (pos - 1);
          byte |= 1 << (pos - 2);
        }
        else if (gray1 >= 0x50) {
          byte |= 1 << (pos - 1);
          byte |= 1 << (pos - 3);
        }
        else if (gray1 >= 0x40) {
          byte |= 1 << (pos - 1);
        }
        else if (gray1 >= 0x30) {
          byte |= 1 << (pos - 2);
          byte |= 1 << (pos - 3);
        }
        else if (gray1 >= 0x20) {
          byte |= 1 << (pos - 2);
        }
        else if (gray1 >= 0x10) {
          byte |= 1 << (pos - 3);
        }

        pos -= 4;

        if (pos < 0) {
          if (output_invert_)
            byte = ~byte;

          file_->write(CStrUtil::toHexString(byte & 0xFF, 2));

          if (byte_count % 19 == 18)
            file_->write("\n");

          byte_count++;

          pos  = 7;
          byte = 0;
        }
      }

      if (pos != 7) {
        if (output_invert_)
          byte = ~byte;

        file_->write(CStrUtil::toHexString(byte & 0xFF, 2));
      }

      file_->write("\n");
    }
  }
  else if (output_depth_ == 2) {
    file_->write("/buffer ");
    file_->write(CStrUtil::toString((int) ((width1 + 3)/4)));
    file_->write(" string def\n");

    file_->write(CStrUtil::toString((int) width1));
    file_->write(" ");
    file_->write(CStrUtil::toString((int) height1));
    file_->write(" 2\n");
    file_->write("[");
    file_->write(CStrUtil::toString(width2));
    file_->write(" 0 0 ");
    file_->write(CStrUtil::toString(-height2));
    file_->write(" 0 ");
    file_->write(CStrUtil::toString(height2));
    file_->write("]\n");
    file_->write("{currentfile buffer readhexstring pop}\n");

    file_->write("image\n");

    double r, g, b, a;

    for (int y1 = 0; y1 < (int) height1; y1++) {
      int byte_count = 0;

      int pos  = 7;
      int byte = 0;

      for (int x1 = 0; x1 < (int) width1; x1++) {
        image->getRGBAPixel((int) (src_x + x1), (int) (src_y + y1),
                            &r, &g, &b, &a);

        double gray = (r + g + b)/3.0;

        int gray1 = (int) (gray*255);

        if      (gray1 >= 0xC0) {
          byte |= 1 << (pos    );
          byte |= 1 << (pos - 1);
        }
        else if (gray1 >= 0x80)
          byte |= 1 << (pos    );
        else if (gray1 >= 0x40)
          byte |= 1 << (pos - 1);

        pos -= 2;

        if (pos < 0) {
          if (output_invert_)
            byte = ~byte;

          file_->write(CStrUtil::toHexString(byte & 0xFF, 2));

          if (byte_count % 19 == 18)
            file_->write("\n");

          byte_count++;

          pos  = 7;
          byte = 0;
        }
      }

      if (pos != 7) {
        if (output_invert_)
          byte = ~byte;

        file_->write(CStrUtil::toHexString(byte & 0xFF, 2));
      }

      file_->write("\n");
    }
  }
  else if (output_depth_ == 1) {
    file_->write("/buffer ");
    file_->write(CStrUtil::toString((int) ((width1 + 7)/8)));
    file_->write(" string def\n");

    file_->write(CStrUtil::toString((int) width1));
    file_->write(" ");
    file_->write(CStrUtil::toString((int) height1));
    file_->write(" 1\n");
    file_->write("[");
    file_->write(CStrUtil::toString(width2));
    file_->write(" 0 0 ");
    file_->write(CStrUtil::toString(-height2));
    file_->write(" 0 ");
    file_->write(CStrUtil::toString(height2));
    file_->write("]\n");
    file_->write("{currentfile buffer readhexstring pop}\n");

    file_->write("image\n");

    double r, g, b, a;

    for (int y1 = 0; y1 < (int) height1; y1++) {
      int byte_count = 0;

      int pos  = 7;
      int byte = 0;

      for (int x1 = 0; x1 < (int) width1; x1++) {
        image->getRGBAPixel((int) (src_x + x1), (int) (src_y + y1),
                            &r, &g, &b, &a);

        double gray = (r + g + b)/3.0;

        int gray1 = (int) (gray*255);

        if (gray1 >= 0x80)
          byte |= 1 << pos;

        pos--;

        if (pos < 0) {
          if (output_invert_)
            byte = ~byte;

          file_->write(CStrUtil::toHexString(byte & 0xFF, 2));

          if (byte_count % 19 == 18)
            file_->write("\n");

          byte_count++;

          pos  = 7;
          byte = 0;
        }
      }

      if (pos != 7) {
        if (output_invert_)
          byte = ~byte;

        file_->write(CStrUtil::toHexString(byte & 0xFF, 2));
      }

      file_->write("\n");
    }
  }

  file_->write("restore\n");
}

void
CPrint::
setForeground(const CRGBA &rgb)
{
  fg_ = rgb;

  if (active_) {
    file_->write(CStrUtil::toString(fg_.getRed()));
    file_->write(" ");
    file_->write(CStrUtil::toString(fg_.getGreen()));
    file_->write(" ");
    file_->write(CStrUtil::toString(fg_.getBlue()));
    file_->write(" setrgbcolor\n");
  }
}

void
CPrint::
getForeground(CRGBA &rgb)
{
  rgb = fg_;
}

void
CPrint::
setBackground(const CRGBA &rgb)
{
  bg_ = rgb;
}

void
CPrint::
getBackground(CRGBA &rgb)
{
  rgb = bg_;
}

void
CPrint::
setGray(double gray)
{
  fg_ = CRGBA(gray, gray, gray);

  if (active_) {
    file_->write(CStrUtil::toString(gray));
    file_->write(" setgray\n");
  }
}

void
CPrint::
setLineDash(const CILineDash &line_dash)
{
  line_dash_ = line_dash;

  if (active_) {
    file_->write("[");

    int num_dashes = line_dash_.getNumLengths();

    for (int i = 0; i < num_dashes; i++) {
      file_->write(" ");
      file_->write(CStrUtil::toString(line_dash_.getLength(i)));
    }

    file_->write(" ]");

    file_->write(" ");
    file_->write(CStrUtil::toString(line_dash_.getOffset()));
    file_->write(" setdash\n");
  }
}

void
CPrint::
setLineWidth(double line_width)
{
  line_width_ = line_width;

  if (line_width_ < 0.0)
    line_width_ = 0.0;

  /*------*/

  if (active_) {
    if (last_line_width_ < 0 || last_line_width_ != line_width_) {
      file_->write(CStrUtil::toString(line_width_));
      file_->write(" setlinewidth\n");
    }

    last_line_width_ = line_width_;
  }
}

void
CPrint::
setFont(CFontPtr font)
{
  font_ = font;

  //------

  std::string name    = font->getFamily();
  CFontStyle  style   = font->getStyle ();
  int         ascent  = font->getICharAscent ();
  int         descent = font->getICharDescent();
  double      aspect  = font->getCharAspect ();

  //------

  if      (CStrUtil::casefind(name, "courier") != std::string::npos) {
    if      (style == CFONT_STYLE_NORMAL)
      font_name_ = "FontCN";
    else if (style == CFONT_STYLE_BOLD)
      font_name_ = "FontCB";
    else if (style == CFONT_STYLE_ITALIC)
      font_name_ = "FontCI";
    else if (style == CFONT_STYLE_BOLD_ITALIC)
      font_name_ = "FontCBI";
  }
  else if (CStrUtil::casefind(name, "times") != std::string::npos) {
    if      (style == CFONT_STYLE_NORMAL)
      font_name_ = "FontTN";
    else if (style == CFONT_STYLE_BOLD)
      font_name_ = "FontTB";
    else if (style == CFONT_STYLE_ITALIC)
      font_name_ = "FontTI";
    else if (style == CFONT_STYLE_BOLD_ITALIC)
      font_name_ = "FontTBI";
  }

  else if (CStrUtil::casefind(name, "helvetica") != std::string::npos) {
    if      (style == CFONT_STYLE_NORMAL)
      font_name_ = "FontHN";
    else if (style == CFONT_STYLE_BOLD)
      font_name_ = "FontHB";
    else if (style == CFONT_STYLE_ITALIC)
      font_name_ = "FontHI";
    else if (style == CFONT_STYLE_BOLD_ITALIC)
      font_name_ = "FontHBI";
  }
  else {
    if      (style == CFONT_STYLE_NORMAL)
      font_name_ = "FontCN";
    else if (style == CFONT_STYLE_BOLD)
      font_name_ = "FontCB";
    else if (style == CFONT_STYLE_ITALIC)
      font_name_ = "FontCI";
    else if (style == CFONT_STYLE_BOLD_ITALIC)
      font_name_ = "FontCBI";
  }

  /*------*/

  font_ascent_  = ascent;
  font_descent_ = descent;

  /*------*/

  if (active_) {
    int font_height = font_ascent_ + font_descent_;

    double font_aspect = aspect;

    setFont(font_name_, font_height/font_aspect, font_height);
  }
}

void
CPrint::
setFont(const std::string &name, double x_size, double y_size)
{
  if (! active_) return;

  file_->write(name);

  file_->write(" [" + CStrUtil::toString(x_size) + " 0 0 " +
               CStrUtil::toString(y_size) + " 0 0] makefont setfont\n");
}

void
CPrint::
getFont(CFontPtr &font)
{
  font = font_;
}
