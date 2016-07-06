#ifndef POINTEREVENT_H
#define POINTEREVENT_H

#include <array>
#include <QPoint>
#include <QMetaType>
class QTransform;

class PointerEvent
{
public:
  enum class Color{ red, green, blue };
  PointerEvent();
  PointerEvent( const std::array<QPoint, 3>& points );
  QPoint getAny() const;
  QPoint getPoint( Color c ) const;
  const std::array<QPoint, 3>& getPoints() const { return m_points; }
  void transform( const QTransform& t );
private:
  std::array<QPoint, 3> m_points;
};

Q_DECLARE_METATYPE(PointerEvent)

#endif // POINTEREVENT_H
