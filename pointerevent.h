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
  enum class Color{ red, green, blue, MAX };
  struct CPoint { Color color; QPointF point; bool ping; };
  PointerEvent();
  PointerEvent( const std::array<QPointF, 3>& points );
  QPointF getAny() const;
  QPointF getPoint(Color c) const;
  std::vector<CPoint> getPoints() const;
  void transform( const QTransform& t );
  bool compareTo( const PointerEvent& other );
  static QColor getQColor( Color color );
private:
  std::array<QPointF, 3> m_points;
  std::array<bool, 3> m_ping;
  static const std::array<QColor, 3> ms_qcolors;
};

Q_DECLARE_METATYPE(PointerEvent)

#endif // POINTEREVENT_H
