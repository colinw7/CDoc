#ifndef CQT_PIXEL_RENDERER_H
#define CQT_PIXEL_RENDERER_H

#include <CAutoPtr.h>
#include <QColor>

#ifndef CPIXEL_RENDERER_VIRTUAL
#define CPIXEL_RENDERER_VIRTUAL
#endif

#include <CPixelRenderer.h>

class QPainter;
class QPen;
class QBrush;
class QPixmap;
class QPainterPath;
class QImage;
class CQFont;

class CQPixelRenderer : public CPixelRenderer {
 protected:
  CAutoPtr<QPainter> painter_;
  CAutoPtr<QPen>     pen_;
  CAutoPtr<QBrush>   brush_;
  CAutoPtr<QPixmap>  pixmap_;
  int                pixmap_width_, pixmap_height_;
  QPainter*          rep_painter_;
  QColor             qfg_, qbg_;
  uint               font_id_;
  bool               drawing_;

 public:
  CQPixelRenderer();

  virtual ~CQPixelRenderer();

  QPainter *getQPainter() const;
  void setQPainter(QPainter *painter);

  QPen   *getQPen  () const { return pen_    ; }
  QBrush *getQBrush() const { return brush_  ; }

  virtual uint getWidth () const = 0;
  virtual uint getHeight() const = 0;

  virtual void updateSize(int width, int height) = 0;

  virtual void beginDraw();
  virtual void endDraw();

  virtual void startDoubleBuffer(bool clear=true);
  virtual void endDoubleBuffer  (bool copy=true);
  virtual void copyDoubleBuffer ();

  virtual void setForeground(const CRGBA &fg);
  virtual void setBackground(const CRGBA &bg);

  virtual void setLineDash(const CILineDash &line_dash);
  virtual void setLineCap(CLineCapType cap);
  virtual void setLineJoin(CLineJoinType join);

  virtual void clear();
  virtual void fill();

  virtual void drawClippedPoint(const CIPoint2D &point);

  virtual void drawClippedChar(const CIPoint2D &point, char c);

  virtual void drawClippedString(const CIPoint2D &point,
                                 const std::string &str);

  virtual void drawStringInRect(const CIBBox2D &bbox, const std::string &str);

  void drawScaledStringInRect(const CIBBox2D &bbox, const std::string &str);

  virtual void drawClippedLine(const CIPoint2D &point1,
                               const CIPoint2D &point2);

  virtual void drawClippedRectangle(const CIBBox2D &bbox);

  virtual void fillClippedRectangle(const CIBBox2D &bbox);

  virtual void fillClippedPolygon(const IPointList &points);

  virtual void fillImageClippedPolygon(const IPointList &points,
                                       CImagePtr image);

  virtual void fillGradientClippedPolygon(const IPointList &points,
                                          CRefPtr<CGenGradient> gradient);

  virtual void drawClippedArc(const CIPoint2D &center, int xr, int yr,
                              double a1, double a2);

  virtual void fillClippedArc(const CIPoint2D &center, int xr, int yr,
                              double a1, double a2);

  virtual void drawClippedImage(const CIPoint2D &point, CImagePtr image);

  virtual void drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image);

  virtual void drawQImage(const CIPoint2D &point, QImage &image);

  virtual void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  virtual void strokePath(QPainterPath *path);

  virtual void fillClippedTriangle(const CIPoint2D &point1,
                                   const CIPoint2D &point2,
                                   const CIPoint2D &point3);

  virtual void drawClippedPolygon(const IPointList &points);

  virtual void flush();

  virtual int getCharWidth();
  virtual int getCharAscent();
  virtual int getCharDescent();
  virtual int getStringWidth(const std::string &str);

  virtual bool mmToPixel(double mm, double *pixel);

  virtual CQFont *getQFont() const;

 private:
  void setQFont();
};

#endif
