#ifndef IMAGE_H
#define IMAGE_H

#include <vector>

class QImage;

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


#endif // IMAGE_H
