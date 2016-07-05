#ifndef USERINPUT_H
#define USERINPUT_H

#include <vector>
#include <QImage>
#include <QObject>
#include <experimental/optional>

class QTextStream;

class UserInput : public QObject
{
  Q_OBJECT

public:
  UserInput();
  virtual ~UserInput();
  void getImage();
signals:
  void signalNewImage(QImage image);
  void signalMouseClick(QPoint point);
private slots:
  void slotCheck();
private:
  std::experimental::optional<QPoint> getPointer() const;
private:

  class Image
  {
  public:
    struct RGB { unsigned char r, g, b; };
    Image() : m_width(0), m_height(0), m_data() {}
    Image( const QImage& image );
    void setSize( unsigned int width, unsigned int height, unsigned int dataSize );
    QImage toImage() const;
    const RGB* scanLine(unsigned int y) const;
    unsigned char* getData() { return reinterpret_cast<unsigned char*>( m_data.data() ); }
    const unsigned char* getConstData() const { return reinterpret_cast<const unsigned char*>( m_data.data() ); }
    unsigned int getWidth() const { return m_width; }
    unsigned int getHeight() const { return m_height; }

  private:
    unsigned int m_width, m_height;
    std::vector<RGB> m_data;
  };

  struct ScoreOnePixel
  {
    ScoreOnePixel() : scoreA(0), scoreB(0), scoreC(0) {}
    explicit ScoreOnePixel( UserInput::Image::RGB c, QPoint p, QTextStream& s );
    UserInput::Image::RGB color;
    int scoreA;
    int scoreB;
    int scoreC;
  };

  static const int ds = 10;

  struct Score
  {

    explicit Score() : scores(), scoreA(0), scoreB(0), scoreC(0), newScoreIndex(0) {}

    void addPoint( UserInput::Image::RGB c, QPoint p, QTextStream& s );

    int getScore() const
    {
      if ( scoreA >= 0 &&
           scoreB >= 0 &&
           scoreC >= 0 )
      {
        return scoreA + scoreB + scoreC;
      }
      return -1;
    }

    std::array<ScoreOnePixel, ds> scores;
    int scoreA;
    int scoreB;
    int scoreC;
    unsigned int newScoreIndex;
  };

  Image m_testImage;
  Image m_currentImage;
};

#endif // USERINPUT_H
