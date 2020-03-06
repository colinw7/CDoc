#ifndef CPRINT_H
#define CPRINT_H

class CFile;

#include <CRGBA.h>
#include <CILineDash.h>
#include <CFont.h>
#include <CImage.h>
#include <memory>

class CPrint {
 public:
  enum Orientation {
    PORTRAIT,
    LANDSCAPE
  };

 private:
  enum FontType {
    FONT_TYPE_NORMAL,
    FONT_TYPE_BOLD,
    FONT_TYPE_ITALIC,
    FONT_TYPE_BOLD_ITALIC
  };

 public:
  CPrint();
 ~CPrint();

  bool setFilename(const std::string &filename);

  void setFile(FILE *fp);

  void setCreator(const std::string &creator);

  void setTitle(const std::string &title);

  void setPageSize(int width, int height) { page_width_ = width; page_height_ = height; }

  void setSize(double xmin, double ymin, double xmax, double ymax);

  void setPerPage(int per_page) { per_page_ = per_page; }

  void setOrientation(Orientation orient) { orient_ = orient; }

  void setOutputDepth(uint depth) { output_depth_ = depth; }

  void init();
  void term();

  void writeHeader();
  void writeFooter(bool showpage=true);

  void startPage(int page_no1, int page_no2);

  void clear();

  void drawCircle(double x, double y, double r);
  void fillCircle(double x, double y, double r);

  void drawEllipse(double x, double y, double xr, double yr);
  void fillEllipse(double x, double y, double xr, double yr);

  void drawArc(double x, double y, double xr, double yr,
               double angle1, double angle2);
  void fillArc(double x, double y, double xr, double yr,
               double angle1, double angle2);

  void drawRectangle(double x1, double y1, double x2, double y2);
  void fillRectangle(double x1, double y1, double x2, double y2);

  void drawPolygon(double *x, double *y, int n);
  void fillPolygon(double *x, double *y, int n);

  void drawPath(double *x, double *y, int n);

  void drawLine(double x1, double y1, double x2, double y2);
  void drawLines(double *x, double *y, int num_xy);

  void drawPoint(double x, double y);

  void drawText(double x, double y, const std::string &str, int angle = 0);
  void fillText(double x, double y, const std::string &str, int angle = 0);

  void showText(double x, double y, const std::string &str);

  void drawImage(const CImagePtr &image, double x, double y, int width, int height,
                 bool resize=false, int rotate=0);

  void drawImage(const CImagePtr &image, double x, double y);
  void drawSubImage(const CImagePtr &image, double src_x, double src_y,
                    double dst_x, double dst_y, double width, double height);

  void setForeground(const CRGBA &rgb);
  void setBackground(const CRGBA &rgb);

  void getForeground(CRGBA &rgb);
  void getBackground(CRGBA &rgb);

  void setGray(double gray);

  void setLineDash(const CILineDash &line_dash);
  void setLineWidth(double width);

  void setFont(CFontPtr font);
  void getFont(CFontPtr &font);

  void setFont(const std::string &name, double x_size, double y_size);

  std::string timeToDateString(time_t t);

 private:
  using FileP = std::unique_ptr<CFile>;

  bool        active_              { false };
  std::string filename_            { ".screen.ps" };
  FileP       file_;
  std::string creator_             { "Nobody" };
  std::string title_               { "Nobody" };
  int         page_width_          { 600 };
  int         page_height_         { 800 };
  bool        has_range_           { false };
  double      xmin_                { 1.0 };
  double      ymin_                { 1.0 };
  double      xmax_                { 2.0 };
  double      ymax_                { 2.0 };
  double      width_;
  double      height_;
  int         per_page_;
  Orientation orient_;
  CFontPtr    font_;
  int         font_width_          { 8 };
  int         font_ascent_         { 8 };
  int         font_descent_        { 2 };
  std::string font_name_           { "FontCN" };
  CRGBA       bg_                  { 0, 0, 0 };
  CRGBA       fg_                  { 1, 1, 1 };
  double      line_width_          { 1.0 };
  double      last_line_width_     { -1.0 };
  CILineDash  line_dash_;
  bool        fill_string_defined_ { false };
  bool        color_image_defined_ { false };
  bool        output_color_        { true };
  uint        output_depth_        { 4 };
  bool        output_invert_       { false };
};

#endif
