// operator== for polygons doesn't work :)
//      Point A(0, 0);
//      Point B(0, 1);
//      Point C(1, 1);
//      Point D(1, 0);
//      Point E(2, -1);
// ABCDE == ABCED


#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>


namespace DoubleArithmetic {

    const double EPS = 1e-12;

    bool is_number(double num) {
        return !std::isnan(num) && !std::isinf(num);
    }

    bool is_equal(double lhs, double rhs) {
        return is_number(lhs) && is_number(rhs) && std::abs(lhs - rhs) < EPS;
    }

    bool is_equal(const std::vector<double>& lhs,
                  const std::vector<double>& rhs) {

        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (size_t i = 0; i < lhs.size(); ++i) {
            if (!is_equal(lhs[i], rhs[i])) {
                return false;
            }
        }

        return true;

    }

    double squared(double x) {
        return x * x;
    }

}
namespace DA = DoubleArithmetic;


struct Line;


// Point is equal to vector
struct Point {

public:

    double x;
    double y;

    Point(): x(0), y(0) {}

    Point(double init_x, double init_y): x(init_x), y(init_y) {}

    Point(const Point& point) = default;

    ~Point() = default;

    Point& operator=(const Point& rhs) = default;

    bool operator==(const Point& rhs) const {
        return DA::is_equal(x, rhs.x) && DA::is_equal(y, rhs.y);
    }

    bool operator!=(const Point& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const Point& rhs) const {
        return x < rhs.x || (x == rhs.x && y < rhs.y);
    }

    Point operator+(const Point& rhs) const {
        return {x + rhs.x, y + rhs.y};
    }

    Point operator-(const Point& rhs) const {
        return {x - rhs.x, y - rhs.y};
    }

    Point operator*(double coefficient) const {
        return {x * coefficient, y * coefficient};
    }

    Point operator/(double coefficient) const {
        return {x / coefficient, y / coefficient};
    }

    bool isOn(const Line&) const;

    Line perpendicularTo(const Line& line) const;

    void rotate(const Point& center, double angle) {
        double x_0 = x;
        x = x * cos(angle / 180 * M_PI) - y * sin(angle / 180 * M_PI) + center.x;
        y = x_0 * sin(angle / 180 * M_PI) + y * cos(angle / 180 * M_PI) + center.y;
    }

    void reflect(const Point& center) {
        scale(center, -1);
    }

    void reflect(const Line& axis);

    void scale(const Point& center, double coefficient) {
        *this = center * (1 - coefficient) + *this * coefficient;
    }

    static double distance(const Point& lhs, const Point& rhs) {
        return sqrt(DA::squared(lhs.x - rhs.x) + DA::squared(lhs.y - rhs.y));
    }

    static double dotProduct(const Point& lhs, const Point& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    static double crossProduct(const Point& lhs, const Point& rhs) {
        return lhs.x * rhs.y - lhs.y * rhs.x;
    }

};


struct Line {

public:

    // Ax + By + C = 0
    double x_coefficient; // A
    double y_coefficient; // B
    double free_term;     // C

    Line(): x_coefficient(0), y_coefficient(1), free_term(0) {}

    Line(const Point& first, const Point& second):
            x_coefficient(), y_coefficient(), free_term() {

        y_coefficient = static_cast<double>(!DA::is_equal(first.x, second.x));

        if (DA::is_equal(first.y, second.y)) {
            x_coefficient = 0;
        } else {
            x_coefficient = DA::is_equal(y_coefficient, 1)
                            ? -(first.y - second.y) / (first.x - second.x)
                            : 1;
        }

        free_term = -(x_coefficient * first.x + y_coefficient * first.y);

    }

    Line(double slope, double intercept)
            : x_coefficient(slope), y_coefficient(-1), free_term(intercept) {}

    Line(const Point& point, double slope):
            x_coefficient(slope), y_coefficient(-1),
            free_term(-(y_coefficient * point.y + x_coefficient * point.x)) {}

    Line(double x_coefficient, double y_coefficient, double free_term):
            x_coefficient(x_coefficient), y_coefficient(y_coefficient), free_term(free_term) {}

    Line(const Line& line) = default;

    ~Line() = default;

    Line& operator=(const Line& rhs) = default;

    bool operator==(const Line& rhs) const {

        if (DA::is_equal(y_coefficient, 0) && DA::is_equal(rhs.y_coefficient, 0)) {
            return DA::is_equal(free_term / x_coefficient, rhs.free_term / rhs.x_coefficient);
        }

        return DA::is_equal(x_coefficient / y_coefficient, rhs.x_coefficient / rhs.y_coefficient) &&
               DA::is_equal(free_term / y_coefficient, rhs.free_term / rhs.y_coefficient);

    }

    bool operator!=(const Line& rhs) const {
        return !(*this == rhs);
    }

    Point directionVector() const {
        return {x_coefficient, y_coefficient};
    }

    static Line segmentPerpendicular(const Point& first, const Point& second) {
        return {first.x - second.x,
                first.y - second.y,
                -(DA::squared(first.x) - DA::squared(second.x) +
                  DA::squared(first.y) - DA::squared(second.y)) / 2};
    }

    static Point intersection(const Line& lhs, const Line& rhs) {

        Point intersection;

        intersection.x = (lhs.y_coefficient * rhs.free_term - rhs.y_coefficient * lhs.free_term) /
                         Point::crossProduct(lhs.directionVector(), rhs.directionVector());
        intersection.y = (rhs.x_coefficient * lhs.free_term - lhs.x_coefficient * rhs.free_term) /
                         Point::crossProduct(lhs.directionVector(), rhs.directionVector());

        return intersection;

    }

private:

};

bool Point::isOn(const Line& line) const {
    return DA::is_equal(line.x_coefficient * x + line.y_coefficient * y + line.free_term, 0);
}

Line Point::perpendicularTo(const Line& line) const {
    return {-line.y_coefficient,
            line.x_coefficient,
            (line.y_coefficient * x - line.x_coefficient * y)};
}

void Point::reflect(const Line &axis) {

    // axis: Ax + By + C = 0
    double A = axis.x_coefficient;
    double B = axis.y_coefficient;
    double C = axis.free_term;

    Point M0 = {(A * (A * x + B * y) -
                B * (A * y - B * x)) /
                (DA::squared(A) + DA::squared(B)),
                (B * (A * x + B * y) +
                A * (A * y - B * x)) /
                (DA::squared(A) + DA::squared(B))};

    Point M1 = {(A * -(A * x + B * y + 2 * C) -
                B * (A * y - B * x)) /
                (DA::squared(A) + DA::squared(B)),
                (B * -(A * x + B * y + 2 * C) +
                A * (A * y - B * x)) /
                (DA::squared(A) + DA::squared(B))};

    *this = *this == M0 ? M1 : M0;

}


class Shape {

public:

    virtual ~Shape() = default;

    virtual double perimeter() const = 0;

    virtual double area() const = 0;

    virtual bool operator==(const Shape& another) const = 0;

    virtual bool operator!=(const Shape& another) const = 0;

    virtual bool isCongruentTo(const Shape& another) const = 0;

    virtual bool isSimilarTo(const Shape& another) const = 0;

    virtual bool containsPoint(const Point& point) const = 0;

    virtual void rotate(const Point& center, double angle) = 0;

    virtual void reflect(const Point& center) = 0;

    virtual void reflect(const Line& axis) = 0;

    virtual void scale(const Point& center, double coefficient) = 0;

protected:

private:

};


class Polygon: public Shape {

public:

    Polygon() = default;

    explicit Polygon(const std::vector<Point>& vertices): vertices(vertices) {}

    template <typename ...Ts>
    explicit Polygon(const Point& point, Ts... args): vertices() {
        push_point(point, args...);
    }

    Polygon(const Polygon& polygon) = default;

    ~Polygon() override = default;

    Polygon& operator=(const Polygon& rhs) = default;

    size_t verticesCount() const {
        return vertices.size();
    }

    // returns sorted vector of lengths of sides
    std::vector<double> getSides() const {

        std::vector<double> sides(verticesCount());

        for (size_t i = 0; i < verticesCount(); ++i) {
            sides[i] = Point::distance(vertices[i], vertices[(i + 1) % verticesCount()]);
        }

        std::sort(sides.begin(), sides.end());

        return sides;

    }

    // returns sorted vector of angles in radians
    std::vector<double> getAngles() const {

        std::vector<double> angles(verticesCount());

        for (size_t i = 0; i < verticesCount(); ++i) {

            size_t next_vertex = (i + 1) % verticesCount();
            size_t prev_vertex = (verticesCount() + i - 1) % verticesCount();

            double cos =
                    Point::dotProduct(vertices[next_vertex] - vertices[i],
                                      vertices[prev_vertex] - vertices[i]) /
                    (Point::distance(vertices[next_vertex], vertices[i]) *
                    Point::distance(vertices[prev_vertex], vertices[i]));

            double sin =
                    Point::crossProduct(vertices[next_vertex] - vertices[i],
                                        vertices[prev_vertex] - vertices[i]) /
                    (Point::distance(vertices[next_vertex], vertices[i]) *
                    Point::distance(vertices[prev_vertex], vertices[i]));

            if (sin >= 0 && cos >= 0) {
                angles[i] = asin(sin);
                continue;
            }
            if (sin >= 0 && cos <= 0) {
                angles[i] = M_PI - asin(sin);
                continue;
            }
            if (sin <= 0 && cos <= 0) {
                angles[i] = M_PI - asin(sin);
                continue;
            }
            if (sin <= 0 && cos >= 0) {
                angles[i] = 2 * M_PI + asin(sin);
                continue;
            }

        }

        std::sort(angles.begin(), angles.end());

        size_t count = angles.end() - std::lower_bound(angles.begin(), angles.end(), M_PI);

        if (count >= verticesCount() / 2) {
            for (size_t i = 0; i < verticesCount(); ++i) {
                angles[i] = 2 * M_PI - angles[i];
            }
        }

        std::sort(angles.begin(), angles.end());

        return angles;

    }

    std::vector<Point> getVertices() const {
        return vertices;
    }

    bool isConvex() const {

        bool sign =
                Point::crossProduct(vertices[0] - vertices.back(),
                                    vertices[verticesCount() - 2] - vertices.back()) > 0;

        for (size_t i = 0; i < verticesCount() - 1; ++i) {
            size_t prev_vertex = i == 0 ? verticesCount() - 1 : i - 1;
            if ((Point::crossProduct(vertices[i + 1] - vertices[i],
                                     vertices[prev_vertex] - vertices[i]) > 0) != sign) {
                return false;
            }
        }

        return true;

    }

    double perimeter() const override {

        double perimeter = 0;

        for (double& side : getSides()) {
            perimeter += side;
        }

        return perimeter;

    }

    double area() const override {

        double res = 0;

        for (size_t i = verticesCount() - 1; i >= 2; --i) {
            res += Point::crossProduct(vertices[0] - vertices[i], vertices[i - 1] - vertices[i]) / 2;
        }

        return std::abs(res);

    }

    bool operator==(const Shape& another) const override {

        if (!isPolygon(another)) {
            return false;
        }

        auto polygon = dynamic_cast<const Polygon&>(another);

        std::vector<Point> this_vertices(vertices);
        std::vector<Point> another_vertices(polygon.vertices);

        std::sort(this_vertices.begin(), this_vertices.end());
        std::sort(another_vertices.begin(), another_vertices.end());

        return this_vertices == another_vertices;

    }

    bool operator!=(const Shape& another) const override {
        return !operator==(another);
    }

    bool isCongruentTo(const Shape& another) const override {

        if (!isPolygon(another)) {
            return false;
        }

        auto polygon = dynamic_cast<const Polygon&>(another);

        return DA::is_equal(getSides(), polygon.getSides()) &&
               DA::is_equal(getAngles(), polygon.getAngles());

    }

    bool isSimilarTo(const Shape& another) const override {

        if (!isPolygon(another)) {
            return false;
        }

        auto polygon = dynamic_cast<const Polygon&>(another);

        if (DA::is_equal(getAngles(), polygon.getAngles())) {

            std::vector<double> this_sides(getSides());
            std::vector<double> another_sides(polygon.getSides());

            for (size_t i = 0; i < verticesCount(); ++i) {
                another_sides[i] /= this_sides[i];
            }

            for (size_t i = 0; i < verticesCount() - 1; ++i) {
                if (!DA::is_equal(another_sides[i], another_sides[i + 1])) {
                    return false;
                }
            }

            return true;

        }

        return false;

    }

    bool containsPoint(const Point& point) const override {

        // ray casting algorithm

        Line ray(point, {point.x + 1, point.y}); // ray straight from the point going along X-axis
        size_t intersection_count = 0;

        for (size_t i = 0; i < verticesCount(); ++i) {

            Point next_vertex = vertices[(i + 1) % verticesCount()];

            if (point.isOn({vertices[i], next_vertex}) &&
                point.y >= std::min(vertices[i].y, next_vertex.y) &&
                point.y <= std::max(vertices[i].y, next_vertex.y) &&
                point.x >= std::min(vertices[i].x, next_vertex.x) &&
                point.x <= std::max(vertices[i].x, next_vertex.x)) {

                return true;

            }

            if (ray == Line(vertices[i], next_vertex)) {
                continue;
            }

            Point intersection = Line::intersection(ray, {vertices[i], next_vertex});

            if (intersection == vertices[i] ||
                intersection == vertices[(i + 1) % verticesCount()]) {

                intersection = Line::intersection(
                        {{point.x, point.y + 10 * DA::EPS}, {point.x + 1, point.y + 10 * DA::EPS}},
                        {vertices[i], next_vertex});

            }

            if (DA::is_number(intersection.x) && intersection.x > point.x &&
                intersection.y >= std::min(vertices[i].y, next_vertex.y) &&
                intersection.y <= std::max(vertices[i].y, next_vertex.y)) {

                ++intersection_count; // counts only sides intersections

            }

        }

        return intersection_count % 2 != 0; // even - false, odd - true

    }

    void rotate(const Point& center, double angle) override {
        for (size_t i = 0; i < verticesCount(); ++i) {
            vertices[i].rotate(center, angle);
        }
    }

    void reflect(const Point& center) override {
        scale(center, -1);
    }

    void reflect(const Line& axis) override {
        for (size_t i = 0; i < verticesCount(); ++i) {
            vertices[i].reflect(axis);
        }
    }

    void scale(const Point& center, double coefficient) override {
        for (size_t i = 0; i < verticesCount(); ++i) {
            vertices[i].scale(center, coefficient);
        }
    }

protected:

    std::vector<Point> vertices;

private:

    void push_point(const Point& point) {
        vertices.push_back(point);
    }

    template <typename ...Ts>
    void push_point(const Point& point, Ts... args) {
        vertices.push_back(point);
        push_point(args...);
    }

    static bool isPolygon(const Shape& shape) {

        try {
            auto polygon = dynamic_cast<const Polygon&>(shape);
        }
        catch (const std::bad_cast&) {
            return false;
        }

        return true;

    }

};


class Ellipse: public Shape {

public:

    Ellipse() = default;

    Ellipse(const Point& first, const Point& second, double dist):
            _focuses({first, second}), semi_major_axis(dist / 2) {}

    Ellipse(const Ellipse& ellipse) = default;

    ~Ellipse() override = default;

    Ellipse& operator=(const Ellipse& rhs) = default;

    // returns pair of semi-major axis and semi-minor axis
    std::pair<double, double> getSemiAxes() const {
        return {semi_major_axis,
                sqrt(DA::squared(semi_major_axis) -
                     DA::squared(Point::distance(_focuses.first, _focuses.second) / 2))};
    }

    std::pair<Point, Point> focuses() const {
        return _focuses;
    }

    double eccentricity() const {
        return Point::distance(center(), _focuses.first) / semi_major_axis;
    }

    std::pair<Line, Line> directrices() const {

        Line y_axis(Line::segmentPerpendicular(_focuses.first, _focuses.second));

        std::pair<Line, Line> directrices(y_axis, y_axis);

        if (y_axis.y_coefficient != 0) {
            double p = (semi_major_axis / eccentricity()) /
                       cos(atan(-y_axis.x_coefficient / y_axis.y_coefficient));
            directrices.first.free_term += p * y_axis.y_coefficient;
            directrices.second.free_term -= p * y_axis.y_coefficient;
        } else {
            directrices.first = {1, 0, semi_major_axis / eccentricity()};
            directrices.second = {1, 0, -semi_major_axis / eccentricity()};
        }

        return directrices;

    }

    virtual Point center() const {
        return (_focuses.first + _focuses.second) / 2;
    }

    double perimeter() const override {
        auto [a, b] = getSemiAxes();
        double smth = 3 * DA::squared((a - b) / (a + b));
        return M_PI * (a + b) * (1 + smth / (10 + sqrt(4 - smth)));
    }

    double area() const override {
        return M_PI * getSemiAxes().first * getSemiAxes().second;
    }

    bool operator==(const Shape& another) const override {

        if (!isEllipse(another)) {
            return false;
        }

        auto ellipse = dynamic_cast<const Ellipse&>(another);
        return _focuses == ellipse._focuses && DA::is_equal(semi_major_axis, ellipse.semi_major_axis);

    }

    bool operator!=(const Shape& another) const override {
        return !operator==(another);
    }

    bool isCongruentTo(const Shape& another) const override {

        if (!isEllipse(another)) {
            return false;
        }

        auto ellipse = dynamic_cast<const Ellipse&>(another);
        return Point::distance(_focuses.first, _focuses.second) ==
               Point::distance(ellipse._focuses.first, ellipse._focuses.second) &&
               semi_major_axis == ellipse.semi_major_axis;

    }

    bool isSimilarTo(const Shape& another) const override {

        if (!isEllipse(another)) {
            return false;
        }

        auto ellipse = dynamic_cast<const Ellipse&>(another);

        if (_focuses.first != _focuses.second) {
            return Point::distance(ellipse._focuses.first, ellipse._focuses.second) /
                   Point::distance(_focuses.first, _focuses.second) ==
                   ellipse.semi_major_axis / semi_major_axis;
        } else {
            return ellipse._focuses.first == ellipse._focuses.second;
        }

    }

    bool containsPoint(const Point& point) const override {
        return Point::distance(point, _focuses.first) + Point::distance(point, _focuses.second) <=
               2 * semi_major_axis;
    }

    void rotate(const Point& center, double angle) override {
        _focuses.first.rotate(center, angle);
        _focuses.second.rotate(center, angle);
    }

    void reflect(const Point& center) override {
        _focuses.first.reflect(center);
        _focuses.second.reflect(center);
    }

    void reflect(const Line& axis) override {
        _focuses.first.reflect(axis);
        _focuses.second.reflect(axis);
    }

    void scale(const Point& center, double coefficient) override {
        _focuses.first.scale(center, coefficient);
        _focuses.second.scale(center, coefficient);
        semi_major_axis *= std::abs(coefficient);
    }

protected:

    std::pair<Point, Point> _focuses;
    double semi_major_axis;

private:

    static bool isEllipse(const Shape& shape) {

        try {
            auto ellipse = dynamic_cast<const Ellipse&>(shape);
        }
        catch (const std::bad_cast&) {
            return false;
        }

        return true;

    }

};


class Circle: public Ellipse {

    // semi-major axis of a circle is a radius of a circle

public:

    Circle() = default;

    Circle(const Point& center, double radius): Ellipse(center, center, 2 * radius) {}

    Circle(const Circle& circle) = default;

    ~Circle() override = default;

    Circle& operator=(const Circle& rhs) = default;

    Point center() const override {
        return _focuses.first;
    }

    double radius() const {
        return semi_major_axis;
    }

    double perimeter() const override {
        return 2 * M_PI * semi_major_axis;
    }

    double area() const override {
        return M_PI * DA::squared(semi_major_axis);
    }

    bool operator==(const Shape& another) const override {
        return Ellipse::operator==(another);
    }

    bool operator!=(const Shape& another) const override {
        return !operator==(another);
    }

    bool isCongruentTo(const Shape& another) const override {
        return Ellipse::isCongruentTo(another);
    }

    bool isSimilarTo(const Shape& another) const override {
        return Ellipse::isSimilarTo(another);
    }

    bool containsPoint(const Point& point) const override {
        return Point::distance(point, _focuses.first) <= semi_major_axis;
    }

    void rotate(const Point& center, double angle) override {
        Ellipse::rotate(center, angle);
    }

    void reflect(const Point& center) override {
        Ellipse::reflect(center);
    }

    void reflect(const Line& axis) override {
        Ellipse::reflect(axis);
    }

    void scale(const Point& center, double coefficient) override {
        Ellipse::scale(center, coefficient);
    }

protected:

private:

};


class Rectangle: public Polygon {

public:

    Rectangle() = default;

    Rectangle(const Point& first, const Point& second, double ratio): Polygon() {

        if (ratio < 1) {
            ratio = 1 / ratio;
        }

        vertices.resize(VERTICES_COUNT);
        vertices[0] = first;
        vertices[1] = {first.x + (second.x - first.x - ratio * (second.y - first.y)) /
                                (1 + DA::squared(ratio)),
                       first.y + (second.y - first.y + ratio * (second.x - first.x)) /
                                (1 + DA::squared(ratio))};
        vertices[2] = second;
        vertices[3] = {second.x + (first.x - second.x - ratio * (first.y - second.y)) /
                                (1 + DA::squared(ratio)),
                       second.y + (first.y - second.y + ratio * (first.x - second.x)) /
                                (1 + DA::squared(ratio))};

    }

    Rectangle(const Rectangle& rectangle) = default;

    ~Rectangle() override = default;

    Rectangle& operator=(const Rectangle& rhs) = default;

    Point center() const {
        return (vertices[0] + vertices[2]) / 2;
    }

    std::pair<Line, Line> diagonals() const {
        return {{vertices[0], vertices[2]}, {vertices[1], vertices[3]}};
    }

    double perimeter() const override {
        return (Point::distance(vertices[0], vertices[1]) +
                Point::distance(vertices[1], vertices[2])) * 2;
    }

    double area() const override {
        return Point::distance(vertices[0], vertices[1]) *
               Point::distance(vertices[1], vertices[2]);
    }

    bool operator==(const Shape& another) const override {
        return Polygon::operator==(another);
    }

    bool operator!=(const Shape& another) const override {
        return !operator==(another);
    }

    bool isCongruentTo(const Shape& another) const override {
        return Polygon::isCongruentTo(another);
    }

    bool isSimilarTo(const Shape& another) const override {
        return Polygon::isSimilarTo(another);
    }

    bool containsPoint(const Point& point) const override {
        return Polygon::containsPoint(point);
    }

    void rotate(const Point& center, double angle) override {
        Polygon::rotate(center, angle);
    }

    void reflect(const Point& center) override {
        Polygon::reflect(center);
    }

    void reflect(const Line& axis) override {
        Polygon::reflect(axis);
    }

    void scale(const Point& center, double coefficient) override {
        Polygon::scale(center, coefficient);
    }

protected:

private:

    // rectangle has 4 vertices :)
    static const size_t VERTICES_COUNT = 4;

};


class Square: public Rectangle {

public:

    Square() = default;

    Square(const Point& first, const Point& second): Rectangle(first, second, 1) {}

    Square(const Square& square) = default;

    ~Square() override = default;

    Square& operator=(const Square& rhs) = default;

    Circle inscribedCircle() const {
        return {center(), Point::distance(vertices[0], vertices[1]) / 2};
    }

    Circle circumscribedCircle() const {
        return {center(), Point::distance(vertices[0], vertices[1]) / 2};
    }

    double perimeter() const override {
        return 4 * Point::distance(vertices[0], vertices[1]);
    }

    double area() const override {
        return DA::squared(Point::distance(vertices[0], vertices[1]));
    }

    bool operator==(const Shape& another) const override {
        return Rectangle::operator==(another);
    }

    bool operator!=(const Shape& another) const override {
        return !operator==(another);
    }

    bool isCongruentTo(const Shape& another) const override {
        return Rectangle::isCongruentTo(another);
    }

    bool isSimilarTo(const Shape& another) const override {
        return Rectangle::isSimilarTo(another);
    }

    bool containsPoint(const Point& point) const override {
        return Rectangle::containsPoint(point);
    }

    void rotate(const Point& center, double angle) override {
        Rectangle::rotate(center, angle);
    }

    void reflect(const Point& center) override {
        Rectangle::reflect(center);
    }

    void reflect(const Line& axis) override {
        Rectangle::reflect(axis);
    }

    void scale(const Point& center, double coefficient) override {
        Rectangle::scale(center, coefficient);
    }

protected:

private:

};


class Triangle: public Polygon {

public:

    Triangle() = default;

    Triangle(const Triangle& triangle) = default;

    Triangle(const Point& first, const Point& second, const Point& third):
            Polygon(first, second, third) {}

    ~Triangle() override = default;

    Triangle& operator=(const Triangle& rhs) = default;

    Circle inscribedCircle() const {

        // using angle bisector theorem

        double ratio_0 = Point::distance(vertices[0], vertices[1]) /
                         Point::distance(vertices[0], vertices[2]);
        Point L0 = (vertices[1] + vertices[2] * ratio_0) / (1 + ratio_0);

        double ratio_1 = Point::distance(vertices[1], vertices[0]) /
                         Point::distance(vertices[1], vertices[2]);
        Point L1 = (vertices[0] + vertices[2] * ratio_1) / (1 + ratio_1);

        return {Line::intersection({vertices[0], L0}, {vertices[1], L1}), 2 * area() / perimeter()};

    }

    Circle circumscribedCircle() const {

        // the perpendicular bisector of the side 1-2
        Line perpendicular_0 = Line::segmentPerpendicular(vertices[1], vertices[2]);

        // the perpendicular bisector of the side 0-2
        Line perpendicular_1 = Line::segmentPerpendicular(vertices[0], vertices[2]);

        Point center = Line::intersection(perpendicular_0, perpendicular_1);

        return {center, Point::distance(center, vertices[0])};

    }

    Point centroid() const {
        return Line::intersection({vertices[0], (vertices[1] + vertices[2]) / 2},
                                  {vertices[1], (vertices[0] + vertices[2]) / 2});
    }

    Point orthocenter() const {
        return Line::intersection(vertices[0].perpendicularTo({vertices[1], vertices[2]}),
                                  vertices[1].perpendicularTo({vertices[0], vertices[2]}));
    }

    Point incenter() const {
        return inscribedCircle().center();
    }

    Point circumcenter() const {
        return circumscribedCircle().center();
    }

    Line EulerLine() const {
        return {orthocenter(), centroid()};
    }

    Circle ninePointsCircle() const {
        return orthicTriangle().circumscribedCircle();
    }

    Triangle orthicTriangle() const {
        return {Line::intersection(vertices[0].perpendicularTo({vertices[1], vertices[2]}),
                                                               {vertices[1], vertices[2]}),
                Line::intersection(vertices[1].perpendicularTo({vertices[0], vertices[2]}),
                                                               {vertices[0], vertices[2]}),
                Line::intersection(vertices[2].perpendicularTo({vertices[0], vertices[1]}),
                                                               {vertices[0], vertices[1]})};
    }

    double perimeter() const override {
        double perimeter = 0;
        for (double& side : getSides()) {
            perimeter += side;
        }
        return perimeter;
    }

    double area() const override {
        return std::abs(Point::crossProduct(vertices[0] - vertices[1], vertices[2] - vertices[1]) / 2);
    }

    bool operator==(const Shape& another) const override {
        return Polygon::operator==(another);
    }

    bool operator!=(const Shape& another) const override {
        return !operator==(another);
    }

    bool isCongruentTo(const Shape& another) const override {
        return Polygon::isCongruentTo(another);
    }

    bool isSimilarTo(const Shape& another) const override {
        return Polygon::isSimilarTo(another);
    }

    bool containsPoint(const Point& point) const override {
        return Polygon::containsPoint(point);
    }

    void rotate(const Point& center, double angle) override {
        Polygon::rotate(center, angle);
    }

    void reflect(const Point& center) override {
        Polygon::reflect(center);
    }

    void reflect(const Line& axis) override {
        Polygon::reflect(axis);
    }

    void scale(const Point& center, double coefficient) override {
        Polygon::scale(center, coefficient);
    }

protected:

private:

};
