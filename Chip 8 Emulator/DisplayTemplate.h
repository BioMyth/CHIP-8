#pragma once
#include <stdexcept>
#include <cstring>
/* Abstract Class for handling display functionality
 * Building it out this way so that linux/windows/command line implmentations
 * can co-exist in the same codebase */
namespace Display {

typedef unsigned char byte;

struct Pixel {
  Pixel(byte r = 0, byte g = 0, byte b = 0, byte a = 1) : r(r), g(g), b(b), a(a){};
  byte r, g, b, a;
};

class Display {
public:
  Display(const int x, const int y) : xResolution(x), yResolution(y){
    displayBuffer = new Pixel[x * y];
  };

  /*virtual void setResolution(const int x, const int y);*/
  void setPixels(const int x, const int y, const Pixel* pixels, const int numPixels){
    std::memcpy(getDisplayBufferLocation(x, y), pixels, numPixels * sizeof(Pixel));
  };
  void setPixel(const int x, const int y, const Pixel p) {
    (*getDisplayBufferLocation(x, y)) = p;
  };
  Pixel getPixel(const int x, const int y) const {
    return *getDisplayBufferLocation(x, y);
  };
  virtual Pixel* getDisplayBuffer(void *&buffer, int &size) const;
  /* Must be implmented by the specific display technology at use */
  virtual void displayCurrentBuffer() const;

private:
  Pixel* getDisplayBufferLocation(const int x, const int y) const{
    if(x > xResolution || y > yResolution)
      throw std::out_of_range("Index for display buffer write out of range");
    return &displayBuffer[x + y * yResolution];
  };
  int xResolution;
  int yResolution;
  int framesDisplayed;
  Pixel *displayBuffer;
  Pixel *lastFrame;
};
} // namespace Display
