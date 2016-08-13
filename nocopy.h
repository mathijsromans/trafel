#ifndef NOCOPY_H
#define NOCOPY_H

class NoCopy
{
public:
  NoCopy() = default;
  NoCopy(const NoCopy&) = delete;
  NoCopy& operator=(const NoCopy&) = delete;
};

#endif // NOCOPY_H

