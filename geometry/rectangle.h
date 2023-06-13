#ifndef RECTANGLE_H
#define RECTANGLE_H


class Rectangle
{
public:
    Rectangle();
    Rectangle(int x, int y, int width, int height);
    ~Rectangle() = default;

    int x, y;
    int width, height;

    bool contains(int x, int y) const;
    int getRight() const;
    int getBottom() const;
};

#endif  // RECTANGLE_H
