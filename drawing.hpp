/*
 * This file is part of the Riemann Project.
 * Developed by Tom Faulhaber for personal use.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#ifndef DRAWING_HPP
#define DRAWING_HPP

#include "compile.hpp"

struct Point {
   double x, y;

   /**
    * Create an (x, y) point for a function of x.
    */
   static inline Point of(Func fn, double x) {
      return { x, fn(x) };
   }

   /**
    * Scale the point to a given window
    *
    * @param xmin The lower x bound
    * @param xmax The upper x bound
    * @param width The window width
    * @param ymin The lower y bound
    * @param ymax The upper y bound
    * @param height The window height
    *
    * @return The scaled point
    */
   inline Point scale(double xmin, double xmax, double width,
                      double ymin, double ymax, double height) const
   {
      return { width * (x - xmin) / (xmax - xmin),
               height * (1 - (y - ymin) / (ymax - ymin)) };
   }
};

/**
 * Finds the midpoint between two provided points.
 *
 * @param A The first point
 * @param B The second point
 *
 * @return The midpoint
 */
inline Point midpoint(Point A, Point B) {
   return { (A.x + B.x) / 2.0, (A.y + B.y) / 2.0 };
}

/**
 * Determines if the function goes to positive infinity in the interval
 *
 * @param fn The function
 * @param A The lower bound point
 * @param B The upper bound point
 *
 * @return The point where fn(x) is infinity, or (0, 0)
 */
Point goes_pos_inf(Func fn, Point A, Point B);

/**
 * Determines if the function goes to negative infinity in the interval
 *
 * @param fn The function
 * @param A The lower bound point
 * @param B The upper bound point
 *
 * @return The point where fn(x) is -inf, or (0, 0) 
 */
Point goes_neg_inf(Func fn, Point A, Point B);

#endif

