#include "rectangle.h"

Rectangle::Rectangle()
    : x(0),
      y(0),
      width(0),
      height(0)
{
}

Rectangle::Rectangle(int x, int y, int width, int height)
    : x(x),
      y(y),
      width(width),
      height(height)
{
}

bool Rectangle::contains(int x, int y) const
{
    if (x > this->x && x < this->getRight()) {
        if (y > this->y && y < this->getBottom()) {
            return true;
        }
    }
    return false;
}

int Rectangle::getRight() const
{
    return x + width;
}

int Rectangle::getBottom() const
{
    return y + height;
}
