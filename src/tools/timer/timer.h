/** vpr_relocalization: a library for visual place recognition in changing 
** environments with efficient relocalization step.
** Copyright (c) 2017 O. Vysotska, C. Stachniss, University of Bonn
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**/

#ifndef TIMER_H
#define TIMER_H
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point TimePoint;
enum TimeExt { Sec, MSec, MicroSec, NSec}; // time extensions

class Timer
{
    TimePoint start_;
    TimePoint end_;
public:
    Timer();
    void start();
    void stop();
    std::chrono::seconds get_elapsed_s();
    std::chrono::milliseconds get_elapsed_ms();
    std::chrono::microseconds get_elapsed_micros();
    std::chrono::nanoseconds get_elapsed_ns();
    void print_elapsed_time(TimeExt ext);
};

#endif // TIMER_H
