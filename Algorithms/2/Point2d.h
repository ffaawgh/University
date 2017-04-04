//
// Created by ed on 04.04.17.
//

#ifndef LAB2_POINT2D_H
#define LAB2_POINT2D_H

/**
 * Point in two-dimensional space.
 */
struct Point2d {
	/**
	 * Default constructor.
	 */
	Point2d() = default;

	/**
	 * Constructor.
	 *
	 * @param x x coordinate
	 * @param y y coordinate
	 */
	Point2d(double x, double y);

	double x, y;
};

#endif //LAB2_POINT2D_H
