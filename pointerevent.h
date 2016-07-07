#ifndef POINTEREVENT_H
#define POINTEREVENT_H

#include <array>
#include <vector>
#include <QColor>
#include <QPointF>
#include <QMetaType>
class QTransform;

class PointerEvent
{
public:
  enum class Color{ red, green, blue };
  struct CPoint { Color color; QColor qcolor; QPointF point; };
  PointerEvent();
  PointerEvent( const std::array<QPointF, 3>& points );
  QPointF getAny() const;
  std::vector<CPoint> getPoints() const;
  void transform( const QTransform& t );
  bool differsFrom( const PointerEvent& other ) const;
private:
  std::array<QPointF, 3> m_points;
  static const std::array<QColor, 3> ms_qcolors;
};

Q_DECLARE_METATYPE(PointerEvent)

#endif // POINTEREVENT_H
