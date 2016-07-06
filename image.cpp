#include "image.h"

#include <QImage>

Image::Image(const QImage& image)
  : m_width(image.width()),
    m_height(image.height()),
    m_data(m_width*m_height)
{
  for ( unsigned int y = 0; y < m_height; ++y )
  {
    for ( unsigned int x = 0; x < m_width; ++x )
    {
      QRgb c = image.pixel( x, y );
      m_data[y * m_width + x] = RGB{ static_cast<unsigned char>(qRed(c)),
                                     static_cast<unsigned char>(qGreen(c)),
                                     static_cast<unsigned char>(qBlue(c)) };
    }
  }
}

void Image::setSize(unsigned int width, unsigned int height, unsigned int dataSize)
{
  if ( dataSize != sizeof(RGB)*width*height)
  {
    throw std::logic_error("incorrect image byte size");
  }
  m_width = width;
  m_height = height;
  m_data.resize(m_width*m_height);
}

QImage Image::toImage() const
{
  return QImage(getConstData(), m_width, m_height, QImage::Format_RGB888).copy();
}

const Image::RGB* Image::scanLine(unsigned int y) const
{
  return &m_data[y * m_width];
}
