#ifndef GRAPHICS_MAP_VIEW_H
#define GRAPHICS_MAP_VIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QWidget>

namespace viewpkg
{
class MainView;

class GraphicsMapView : public QGraphicsView
{
 public:
  GraphicsMapView(QWidget *parent = 0);

  void    setZoom(double zoom);
  void    addZoom(double value);
  double  getZoom() const { return zoom; }

  void    setMainView(MainView *main_view)   { this->main_view = main_view; }

 protected:
  void wheelEvent(QWheelEvent *event) override;

 private:
  void updateMatrix();

  MainView *main_view;

  double zoom;
};

}

#endif // GRAPHICS_MAP_VIEW_H
