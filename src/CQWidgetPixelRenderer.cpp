#include <CQWidgetPixelRenderer.h>
#include <QWidget>
#include <QPainter>

CQWidgetPixelRenderer::
CQWidgetPixelRenderer(QWidget *widget) :
 CQPixelRenderer(), widget_(widget)
{
}

CQWidgetPixelRenderer::
~CQWidgetPixelRenderer()
{
}

CQWidgetPixelRenderer *
CQWidgetPixelRenderer::
dup() const
{
  return new CQWidgetPixelRenderer(widget_);
}

uint
CQWidgetPixelRenderer::
getWidth() const
{
  return widget_->width();
}

uint
CQWidgetPixelRenderer::
getHeight() const
{
  return widget_->height();
}

void
CQWidgetPixelRenderer::
updateSize(int, int)
{
}

void
CQWidgetPixelRenderer::
beginDraw()
{
  getQPainter()->begin(widget_);

  CQPixelRenderer::beginDraw();
}

void
CQWidgetPixelRenderer::
endDraw()
{
  getQPainter()->end();

  CQPixelRenderer::endDraw();
}
