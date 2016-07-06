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
  std::array<QPoint, 3> getPointer() const;
  bool searchLine(int y, std::array<QPoint, 3>& result, std::array<int, 3>& bestScore) const;
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

  static const int ds = 6;

  struct Score
  {
    enum class Color{ red, green, blue };
    explicit Score();

    void addPoint( UserInput::Image::RGB c );

    static int brightScore( UserInput::Image::RGB c );
    static int redScore( UserInput::Image::RGB c );
    static int greScore( UserInput::Image::RGB c );
    static int bluScore( UserInput::Image::RGB c );

    int getScore( Color c ) const;

    std::array<Image::RGB, 3*ds> pixels;
    int scoreBright;
    std::array<int, 3> scoreColor;
    unsigned int oldestPixel;
  };

  Image m_testImage;
  Image m_currentImage;
};

#endif // USERINPUT_H
