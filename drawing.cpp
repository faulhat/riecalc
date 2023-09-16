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

#include "drawing.hpp"
#include <cmath>

Point goes_pos_inf(Func fn, Point A, Point B) {
   Point mid = Point::of(fn, (A.x + B.x) / 2.0);
   Point lo = A.y < B.y? A: B,
         hi = A.y < B.y? B: A;
   
   if (mid.y >= 1e+11) {
      return mid;
   } else if (mid.y > hi.y) {
      // If the asymptote goes to +inf, Then the lower y must be on the other side
      // So we search between mid and lo.
      return goes_pos_inf(fn, mid, lo);
   } else if (mid.y < lo.y) {
      return goes_pos_inf(fn, mid, hi);
   } else {
      // mid.y is either less than lo.y or between lo.y and hi.y
      // Either way we fail to conclude the function goes to positive infinity.
      // This may not be true if the asymptote goes negative on one side
      // and positive on the other, such as with tan(x), but if that is the
      // case, goes_neg_inf will return a value and then we can search again.

      return { 0, 0 };
   }
}

Point goes_neg_inf(Func fn, Point A, Point B) {
   Point mid = Point::of(fn, (A.x + B.x) / 2.0);
   Point lo = A.y < B.y? A: B,
         hi = A.y < B.y? B: A;
   
   if (mid.y <= -1e+15) {
      return mid;
   } else if (mid.y < lo.y) {
      // If the asymptote goes to -inf, Then the lower y must be on the other side
      // So we search between mid and hi.
      return goes_neg_inf(fn, mid, hi);
   } else if (mid.y > hi.y) {
      return goes_neg_inf(fn, mid, lo);
   } else {
      // Same logic as positive equivalent.
      return { 0, 0 };
   }
}

