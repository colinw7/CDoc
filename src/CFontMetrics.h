#ifndef CFONT_METRICS_H
#define CFONT_METRICS_H

#include <CFontStyle.h>
#include <iostream>

class CFontMetric;

class CFontMetrics {
 public:
  enum Family {
    COURIER,
    HELVETICA,
    TIMES
  };

 private:
  CFontMetric *courier_normal_;
  CFontMetric *courier_bold_;
  CFontMetric *courier_italic_;
  CFontMetric *courier_boldi_;

  CFontMetric *helvetica_normal_;
  CFontMetric *helvetica_bold_;
  CFontMetric *helvetica_italic_;
  CFontMetric *helvetica_boldi_;

  CFontMetric *times_normal_;
  CFontMetric *times_bold_;
  CFontMetric *times_italic_;
  CFontMetric *times_boldi_;

 public:
  CFontMetrics(const std::string &dirname);
 ~CFontMetrics();

  CFontMetric *getCourierMetric  (CFontStyle font_style);
  CFontMetric *getHelveticaMetric(CFontStyle font_style);
  CFontMetric *getTimesMetric    (CFontStyle font_style);

  void print(std::ostream &os);
};

struct CCharMetric {
  int width;
  int xmin;
  int ymin;
  int xmax;
  int ymax;
};

class CFontMetric {
 private:
  std::string filename_;
  int         xmin_;
  int         ymin_;
  int         xmax_;
  int         ymax_;
  int         descender_;
  CCharMetric chars_[256];

 public:
  CFontMetric(const std::string &filename);
 ~CFontMetric();

  int getXMin() const { return xmin_; }
  int getYMin() const { return ymin_; }
  int getXMax() const { return xmax_; }
  int getYMax() const { return ymax_; }

  int getDescender() const { return descender_; }

  int getCharWidth(uchar c) const { return chars_[c].width; }

  double getAspect() const;

  void print(std::ostream &os);

 private:
  bool read();
};

#endif
