#ifndef CQWIGDET_PIXEL_RENDERER_H
#define CQWIGDET_PIXEL_RENDERER_H

#include <CQPixelRenderer.h>
#include <CAutoPtr.h>

class QWidget;

class CQWidgetPixelRenderer : public CQPixelRenderer {
 private:
  QWidget *widget_;

 public:
  CQWidgetPixelRenderer(QWidget *widget);
 ~CQWidgetPixelRenderer();

  CQWidgetPixelRenderer *dup() const;

  QWidget *getQWidget() const { return widget_; }

  uint getWidth () const;
  uint getHeight() const;

  void updateSize(int width, int height);

  void beginDraw();
  void endDraw();
};

#endif
