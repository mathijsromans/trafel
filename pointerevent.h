#ifndef POINTEREVENT_H
#define POINTEREVENT_H

#include <array>
#include <QPointF>
#include <QMetaType>
class QTransform;

class PointerEvent
{
public:
  enum class Color{ red, green, blue };
  PointerEvent();
  PointerEvent( const std::array<QPointF, 3>& points );
  QPointF getAny() const;
  QPointF getPoint( Color c ) const;
  const std::array<QPointF, 3>& getPoints() const { return m_points; }
  void transform( const QTransform& t );
  bool differsFrom( const PointerEvent& other ) const;
private:
  std::array<QPointF, 3> m_points;
};

Q_DECLARE_METATYPE(PointerEvent)

#endif // POINTEREVENT_H
