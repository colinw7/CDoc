#include <CQPixelRenderer.h>
#include <CQImage.h>
#include <CQFont.h>
#include <CQUtil.h>
#include <CLinearGradient.h>
#include <CRadialGradient.h>

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QDesktopWidget>

CQPixelRenderer::
CQPixelRenderer() :
 CPixelRenderer(), rep_painter_(0), font_id_(INT_MAX), drawing_(false)
{
  painter_ = new QPainter;
  pen_     = new QPen;
  brush_   = new QBrush;

  pixmap_width_  = 0;
  pixmap_height_ = 0;

  brush_->setStyle(Qt::SolidPattern);

  CFontPtr font = CFontMgrInst->lookupFont("helvetica", CFONT_STYLE_NORMAL, 8);

  setFont(font);
}

CQPixelRenderer::
~CQPixelRenderer()
{
}

QPainter *
CQPixelRenderer::
getQPainter() const
{
  if (rep_painter_)
    return rep_painter_;

  return painter_;
}

void
CQPixelRenderer::
setQPainter(QPainter *painter)
{
  rep_painter_ = painter;
}

void
CQPixelRenderer::
beginDraw()
{
  drawing_ = true;

  font_id_ = INT_MAX;
}

void
CQPixelRenderer::
endDraw()
{
  drawing_ = false;
}

void
CQPixelRenderer::
startDoubleBuffer(bool do_clear)
{
  int width  = getWidth ();
  int height = getHeight();

  if (width != pixmap_width_ || height != pixmap_height_) {
    pixmap_width_  = width;
    pixmap_height_ = height;

    pixmap_ = new QPixmap(pixmap_width_, pixmap_height_);

    pixmap_->fill(Qt::black);
  }

  getQPainter()->begin(pixmap_);

  if (do_clear)
    clear();

  font_id_ = INT_MAX;
}

void
CQPixelRenderer::
endDoubleBuffer(bool copy)
{
  getQPainter()->end();

  if (copy)
    copyDoubleBuffer();
}

void
CQPixelRenderer::
copyDoubleBuffer()
{
  if (pixmap_) {
    beginDraw();

    getQPainter()->drawPixmap(0, 0, *pixmap_);

    endDraw();
  }
}

void
CQPixelRenderer::
setForeground(const CRGBA &fg)
{
  qfg_ = CQUtil::rgbaToColor(fg);

  pen_  ->setColor(qfg_);
  brush_->setColor(qfg_);
}

void
CQPixelRenderer::
setBackground(const CRGBA &bg)
{
  qbg_ = CQUtil::rgbaToColor(bg);
}

void
CQPixelRenderer::
setLineDash(const CILineDash &line_dash)
{
  if (line_dash.isSolid())
    pen_->setStyle(Qt::SolidLine);
  else {
    QVector<qreal> dashes;

    pen_->setDashOffset(line_dash.getOffset());

    for (int i = 0; i < line_dash.getNumLengths(); ++i)
      dashes << line_dash.getLength(i);

    if (line_dash.getNumLengths() & 1)
      dashes << line_dash.getLength(line_dash.getNumLengths() - 1);

    pen_->setDashPattern(dashes);
  }
}

void
CQPixelRenderer::
setLineJoin(CLineJoinType join)
{
  switch (join) {
    case LINE_JOIN_TYPE_MITRE:
      pen_->setJoinStyle(Qt::MiterJoin);
      break;
    case LINE_JOIN_TYPE_ROUND:
      pen_->setJoinStyle(Qt::RoundJoin);
      break;
    case LINE_JOIN_TYPE_BEVEL:
      pen_->setJoinStyle(Qt::BevelJoin);
      break;
    default:
      break;
  }
}

void
CQPixelRenderer::
setLineCap(CLineCapType cap)
{
  switch (cap) {
    case LINE_CAP_TYPE_BUTT:
      pen_->setCapStyle(Qt::FlatCap);
      break;
    case LINE_CAP_TYPE_ROUND:
      pen_->setCapStyle(Qt::RoundCap);
      break;
    case LINE_CAP_TYPE_SQUARE:
      pen_->setCapStyle(Qt::SquareCap);
      break;
    default:
      break;
  }
}

void
CQPixelRenderer::
clear()
{
  QBrush brush;

  brush.setColor(qbg_);
  brush.setStyle(Qt::SolidPattern);

  if (pixmap_)
    getQPainter()->fillRect(0, 0, pixmap_width_, pixmap_height_, brush);
  else
    getQPainter()->fillRect(0, 0, getWidth(), getHeight(), brush);
}

void
CQPixelRenderer::
fill()
{
  QBrush brush;

  brush.setColor(qfg_);
  brush.setStyle(Qt::SolidPattern);

  if (pixmap_)
    getQPainter()->fillRect(0, 0, pixmap_width_, pixmap_height_, brush);
  else
    getQPainter()->fillRect(0, 0, getWidth(), getHeight(), brush);
}

void
CQPixelRenderer::
drawClippedPoint(const CIPoint2D &point)
{
  getQPainter()->setPen(*pen_);

  getQPainter()->drawPoint(point.x, point.y);
}

void
CQPixelRenderer::
drawClippedChar(const CIPoint2D &point, char c)
{
  std::string str;

  str += c;

  drawClippedString(point, str);
}

void
CQPixelRenderer::
drawClippedString(const CIPoint2D &point, const std::string &str)
{
  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_  );

  setQFont();

  CQFont *qfont = getQFont();

  int ascent = getCharAscent();

  QString qstr(str.c_str());

  QTransform old_trans = getQPainter()->worldTransform();

  double angle = qfont->getAngle();

  if (! REAL_EQ(angle, 0.0)) {
    QTransform trans1, trans2, trans3;

    trans1.translate(point.x, point.y);
    trans2.rotate   (angle);
    trans3.translate(-point.x, -point.y);

    getQPainter()->setWorldTransform(trans1, true);
    getQPainter()->setWorldTransform(trans2, true);
    getQPainter()->setWorldTransform(trans3, true);

    getQPainter()->drawText(point.x, point.y + ascent, qstr);

    getQPainter()->setWorldTransform(old_trans);
  }
  else
    getQPainter()->drawText(point.x, point.y + ascent, qstr);
}

void
CQPixelRenderer::
drawStringInRect(const CIBBox2D &bbox, const std::string &str)
{
  QString qstr(str.c_str());

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_  );

  setQFont();

  QFont qfont = getQFont()->getQFont();

  QFontMetrics fm(qfont);

  QRect rect = fm.boundingRect(qstr);

  int dx = (bbox.getWidth () - rect.width ())/2;
  int dy = (bbox.getHeight() - rect.height())/2;

  QPointF p(bbox.getXMin() + dx, bbox.getYMin() + dy + fm.ascent());

  getQPainter()->drawText(p, qstr);
}

void
CQPixelRenderer::
drawScaledStringInRect(const CIBBox2D &bbox, const std::string &str)
{
  QString qstr(str.c_str());

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_  );

  setQFont();

  double angle = getQFont()->getAngle();

  QFont qfont = getQFont()->getQFont();

  QFontMetrics fm(qfont);

  QRect rect = fm.boundingRect(qstr);
  //QRect rect = fm.tightBoundingRect(qstr);

  int width  = rect.width();
  int height = rect.height();

  double xscale = (1.0*bbox.getWidth ())/width ;
  double yscale = (1.0*bbox.getHeight())/height;

  double scale = std::min(xscale, yscale);

  qfont.setPointSizeF(scale*qfont.pointSizeF());

  getQPainter()->setFont(qfont);

  QFontMetrics fm1(qfont);

  rect = fm1.boundingRect(qstr);
  //rect = fm1.tightBoundingRect(qstr);

  width  = rect.width();
  height = rect.height();

  QTransform old_trans = getQPainter()->worldTransform();

  if (! REAL_EQ(angle, 0.0)) {
    QTransform trans1, trans2, trans3;

    CIPoint2D origin = bbox.getCenter();

    trans1.translate(origin.x, origin.y);
    trans2.rotate   (-angle);
    trans3.translate(-origin.x, -origin.y);

    getQPainter()->setWorldTransform(trans1, true);
    getQPainter()->setWorldTransform(trans2, true);
    getQPainter()->setWorldTransform(trans3, true);
  }

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_  );

  QRect rect1(bbox.getXMin(), bbox.getYMin(),
              bbox.getWidth(), bbox.getHeight());

  //int flags = Qt::AlignHCenter | Qt::AlignVCenter;

  //getQPainter()->drawText(rect1, flags, qstr);

  QPointF p(bbox.getXMin() + (bbox.getWidth () - width )/2,
            bbox.getYMin() + (bbox.getHeight() - height)/2 +
            fm1.ascent());

  getQPainter()->drawText(p, qstr);

  //brush_->setStyle(Qt::NoBrush);

  //getQPainter()->setBrush(*brush_);

  //getQPainter()->drawRect(rect1);

  getQPainter()->setWorldTransform(old_trans);
}

void
CQPixelRenderer::
drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2)
{
  getQPainter()->setPen(*pen_);

  if (getLineWidth() > 0) {
    QPainterPathStroker stroker;

    stroker.setWidth      (getLineWidth());
    stroker.setCapStyle   (pen_->capStyle());
    stroker.setJoinStyle  (pen_->joinStyle());
    stroker.setDashPattern(pen_->dashPattern());

    QPainterPath path;

    path.moveTo(point1.x, point1.y);
    path.lineTo(point2.x, point2.y);

    getQPainter()->fillPath(stroker.createStroke(path), *brush_);
  }
  else
    getQPainter()->drawLine(point1.x, point1.y, point2.x, point2.y);
}

void
CQPixelRenderer::
drawClippedRectangle(const CIBBox2D &bbox)
{
  brush_->setStyle(Qt::NoBrush);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  getQPainter()->drawRect(bbox.getXMin(), bbox.getYMin(),
                          bbox.getWidth() + 1, bbox.getHeight() + 1);
}

void
CQPixelRenderer::
fillClippedRectangle(const CIBBox2D &bbox)
{
  brush_->setStyle(Qt::SolidPattern);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  getQPainter()->fillRect(bbox.getXMin(), bbox.getYMin(),
                          bbox.getWidth() + 1, bbox.getHeight() + 1,
                          *brush_);
}

void
CQPixelRenderer::
drawClippedPolygon(const IPointList &points)
{
  getQPainter()->setPen(*pen_);

  QPainterPath path;

  uint num_points = points.size();

  path.moveTo(points[0].x, points[0].y);

  for (uint i = 1; i < num_points; ++i)
    path.lineTo(points[i].x, points[i].y);

  path.closeSubpath();

  getQPainter()->strokePath(path, *pen_);
}

void
CQPixelRenderer::
fillClippedPolygon(const IPointList &points)
{
  brush_->setStyle(Qt::SolidPattern);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  QPainterPath path;

  uint num_points = points.size();

  if (num_points < 1) return;

  path.moveTo(points[0].x, points[0].y);

  for (uint i = 1; i < num_points; ++i)
    path.lineTo(points[i].x, points[i].y);

  path.closeSubpath();

  getQPainter()->fillPath(path, *brush_);
}

void
CQPixelRenderer::
fillImageClippedPolygon(const IPointList &points, CImagePtr image)
{
  if (! image.isValid()) return;

  QImage &qimage = image.cast<CQImage>()->getQImage();

  brush_->setTextureImage(qimage);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  QPainterPath path;

  uint num_points = points.size();

  if (num_points < 1) return;

  path.moveTo(points[0].x, points[0].y);

  for (uint i = 1; i < num_points; ++i)
    path.lineTo(points[i].x, points[i].y);

  path.closeSubpath();

  getQPainter()->fillPath(path, *brush_);

  brush_->setStyle(Qt::SolidPattern);
}

void
CQPixelRenderer::
fillGradientClippedPolygon(const IPointList &points,
                           CRefPtr<CGenGradient> gradient)
{
  CLinearGradient *lg;
  CRadialGradient *rg;

  if      ((lg = dynamic_cast<CLinearGradient*>(gradient.getPtr())) != 0) {
    QLinearGradient qlg = CQUtil::toQGradient(lg);

    getQPainter()->setBrush(QBrush(qlg));
  }
  else if ((rg = dynamic_cast<CRadialGradient*>(gradient.getPtr())) != 0) {
    QRadialGradient qrg = CQUtil::toQGradient(rg);

    getQPainter()->setBrush(QBrush(qrg));
  }

  QPainterPath path;

  uint num_points = points.size();

  if (num_points < 1) return;

  path.moveTo(points[0].x, points[0].y);

  for (uint i = 1; i < num_points; ++i)
    path.lineTo(points[i].x, points[i].y);

  path.closeSubpath();

  getQPainter()->fillPath(path, getQPainter()->brush());
}

void
CQPixelRenderer::
drawClippedArc(const CIPoint2D &center, int xr, int yr, double a1, double a2)
{
  QRect rect(center.x - xr, center.y - yr, 2*xr, 2*yr);

  QPainterPath path;

  if (REAL_EQ(a1, 0) && REAL_EQ(a2, 360)) {
    path.addEllipse(rect);
  }
  else {
    path.moveTo(center.x, center.y);
    path.arcTo (rect, a1, a2);
  }

  getQPainter()->strokePath(path, *pen_);
  //getQPainter()->drawPath(path);
}

void
CQPixelRenderer::
fillClippedArc(const CIPoint2D &center, int xr, int yr, double a1, double a2)
{
  QRectF rect(center.x - xr, center.y - yr, 2*xr, 2*yr);

  QPainterPath path;

  if (REAL_EQ(a1, 0) && REAL_EQ(a2, 360)) {
    path.addEllipse(rect);
  }
  else {
    path.moveTo(center.x, center.y);
    path.arcTo (rect, a1, a2);
  }

  getQPainter()->fillPath(path, *brush_);
}

void
CQPixelRenderer::
drawClippedImage(const CIPoint2D &point, CImagePtr image)
{
  QImage &qimage = image.cast<CQImage>()->getQImage();

  //QImage qimage((uchar *) image->getData(), image->getWidth(),
  //              image->getHeight(), QImage::Format_ARGB32);

  drawQImage(point, qimage);
}

void
CQPixelRenderer::
drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image)
{
  drawClippedImage(point, image);
}

void
CQPixelRenderer::
drawQImage(const CIPoint2D &point, QImage &qimage)
{
  getQPainter()->drawImage(point.x, point.y, qimage);
}

void
CQPixelRenderer::
getClip(int *xmin, int *ymin, int *xmax, int *ymax)
{
  *xmin = 0;
  *ymin = 0;
  *xmax = getWidth () - 1;
  *ymax = getHeight() - 1;
}

void
CQPixelRenderer::
strokePath(QPainterPath *path)
{
  QPainterPathStroker stroker;

  stroker.setWidth      (getLineWidth());
  stroker.setCapStyle   (pen_->capStyle());
  stroker.setJoinStyle  (pen_->joinStyle());
  stroker.setDashPattern(pen_->dashPattern());

  getQPainter()->fillPath(stroker.createStroke(*path), *brush_);
}

void
CQPixelRenderer::
fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2,
                    const CIPoint2D &point3)
{
  IPointList points;

  points.resize(3);

  points[0] = point1;
  points[1] = point2;
  points[2] = point3;

  fillClippedPolygon(points);
}

void
CQPixelRenderer::
flush()
{
}

int
CQPixelRenderer::
getCharWidth()
{
  QFontMetricsF fm(getQFont()->getQFont());

  return fm.width('X');
}

int
CQPixelRenderer::
getCharAscent()
{
  QFontMetricsF fm(getQFont()->getQFont());

  return fm.ascent();
}

int
CQPixelRenderer::
getCharDescent()
{
  QFontMetricsF fm(getQFont()->getQFont());

  return fm.descent();
}

int
CQPixelRenderer::
getStringWidth(const std::string &str)
{
  QFontMetricsF fm(getQFont()->getQFont());

  return fm.width(QString(str.c_str()));
}

bool
CQPixelRenderer::
mmToPixel(double mm, double *pixel)
{
  QWidget *w = QApplication::desktop()->screen(0);

  int dpi = (w->logicalDpiX() + w->logicalDpiY())/2;

  double dpmm = dpi/25.4;

  *pixel = mm*dpmm;

  return true;
}

void
CQPixelRenderer::
setQFont()
{
  CFontPtr font;

  getFont(font);

  uint font_id = font->getId();

  if (font_id != font_id_) {
    getQPainter()->setFont(getQFont()->getQFont());

    font_id_ = font_id;
  }
}

CQFont *
CQPixelRenderer::
getQFont() const
{
  CFontPtr pfont;

  getFont(pfont);

  return pfont.cast<CQFont>();
}
