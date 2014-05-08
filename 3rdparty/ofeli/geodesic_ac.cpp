/****************************************************************************
**
** Copyright (C) 2010-2012 Fabien Bessy.
** Contact: fabien.bessy@gmail.com
**
** This file is part of project Ofeli.
**
** http://www.cecill.info/licences/Licence_CeCILL_V2-en.html
** You may use this file under the terms of the CeCILL license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Fabien Bessy and its Subsidiary(-ies) nor the
**     names of its contributors may be used to endorse or promote products
**     derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/

#include "geodesic_ac.hpp"

namespace ofeli
{

GeodesicAC::GeodesicAC(const unsigned char* img_gradient_data1, int img_width1, int img_height1) :
    ActiveContour(img_gradient_data1, img_width1, img_height1,
                  true, 1.25, 1.25, 0.0, 0.0, true, 5, 2.0, 30, 3),
    otsu_threshold1( otsu_method(img_gradient_data1,img_width1*img_height1) )
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

GeodesicAC::GeodesicAC(const unsigned char* img_gradient_data1, int img_width1, int img_height1,
                       bool hasEllipse1, double init_width1, double init_height1, double center_x1, double center_y1,
                       bool hasCycle2_1, int kernel_length1, double sigma1, int Na1, int Ns1) :
    ActiveContour(img_gradient_data1, img_width1, img_height1,
                  hasEllipse1, init_width1, init_height1, center_x1, center_y1, hasCycle2_1, kernel_length1, sigma1, Na1, Ns1),
    otsu_threshold1( otsu_method(img_gradient_data1,img_width1*img_height1) )
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

GeodesicAC::GeodesicAC(const unsigned char* img_gradient_data1, int img_width1, int img_height1,
                       const char* phi_init1,
                       bool hasCycle2_1, int kernel_length1, double sigma1, int Na1, int Ns1) :
    ActiveContour(img_gradient_data1, img_width1, img_height1,
                  phi_init1,
                  hasCycle2_1, kernel_length1, sigma1, Na1, Ns1),
    otsu_threshold1( otsu_method(img_gradient_data1,img_width1*img_height1) )
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

GeodesicAC::GeodesicAC(const GeodesicAC& ac) :
    ActiveContour(ac),
    otsu_threshold1(ac.otsu_threshold1), isInside(ac.isInside)
{
}

void GeodesicAC::initialize_for_each_frame()
{
    if( get_Lout().empty() && get_Lin().empty() )
    {
        std::cerr << std::endl <<
        "The both lists Lout and Lin are empty so the algorithm could not converge. The active contour is initialized with an ellipse."
        << std::endl;

        initialize_phi_with_a_shape(true, 1.25, 1.25, 0.0, 0.0);
        initialize_lists();
    }

    isInside = find_direction_evolution();

    ActiveContour::initialize_for_each_frame();

    return;
}

int GeodesicAC::compute_external_speed_Fd(int offset)
{
    int x, y;
    find_xy_position(offset,x,y); // x and y passed by reference

    int sum_local_out, n_local_out, sum_local_in, n_local_in;

    // if high gradient value
    if( double(img_data[offset]) > 0.7*double(otsu_threshold1) )
    {
        sum_local_out = 0;
        n_local_out = 0;
        sum_local_in = 0;
        n_local_in = 0;

        if( y > 0 )
        {
            if( phi[ find_offset(x,y-1) ] < 0 )
            {
                sum_local_in += int( img_data[ find_offset(x,y-1) ] );
                n_local_in++;
            }
            else
            {
                sum_local_out += int( img_data[ find_offset(x,y-1) ] );
                n_local_out++;
            }
        }
        if( x > 0 )
        {
            if( phi[ find_offset(x-1,y) ] < 0 )
            {
                sum_local_in += int( img_data[ find_offset(x-1,y) ] );
                n_local_in++;
            }
            else
            {
                sum_local_out += int( img_data[ find_offset(x-1,y) ] );
                n_local_out++;
            }
        }
        if( x < img_width-1 )
        {
            if( phi[ find_offset(x+1,y) ] < 0 )
            {
                sum_local_in += int( img_data[ find_offset(x+1,y) ] );
                n_local_in++;
            }
            else
            {
                sum_local_out += int( img_data[ find_offset(x+1,y) ] );
                n_local_out++;
            }
        }
        if( y < img_height-1 )
        {
            if( phi[ find_offset(x,y+1) ] < 0 )
            {
                sum_local_in += int( img_data[ find_offset(x,y+1) ] );
                n_local_in++;
            }
            else
            {
                sum_local_out += int( img_data[ find_offset(x,y+1) ] );
                n_local_out++;
            }
        }

        return sum_local_out*n_local_in - sum_local_in*n_local_out;
    }
    else // if low gradient value
    {
        if( isInside ) // if high gradient values are inside the curve
        {
            return -1; // inward movement
        }
        else
        {
            return 1; // outward movement
        }
    }
}

// to find the high gradient values and to converge
unsigned char GeodesicAC::otsu_method(const unsigned char* img_data1, int img_size1)
{
    int histogram[256];

    for( int I = 0; I < 256; I++ )
    {
        histogram[I] = 0;
    }

    for( int offset = 0; offset < img_size1; offset++ )
    {
        histogram[ img_data1[offset] ]++;
    }

    int sum = 0;
    for( int I = 0; I < 256; I++ )
    {
        sum += I*histogram[I];
    }

    int weight1, weight2, sum1, threshold;
    double mean1, mean2, var_t, var_max;

    threshold = 127; // value returned in the case of an totally homogeneous image

    weight1 = 0;
    sum1 = 0;
    var_max = -1.0;

    // 256 values ==> 255 thresholds t evaluated
    // class1 <= t and class2 > t
    for( int t = 0; t < 255; t++ )
    {
        weight1 += histogram[t];
        if( weight1 == 0 )
        {
            continue;
        }

        weight2 = img_size1-weight1;
        if( weight2 == 0 )
        {
            break;
        }

        sum1 += t*histogram[t];

        mean1 = sum1/weight1;
        mean2 = (sum-sum1)/weight2; // sum2 = sum-sum1

        var_t = double(weight1)*double(weight2)*(mean1-mean2)*(mean1-mean2);

        if( var_t > var_max )
        {
            var_max = var_t;
            threshold = t;
        }
    }

    return (unsigned char)(threshold);
}

bool GeodesicAC::find_direction_evolution()
{
    int sum_in = 0;
    int sum_out = 0;

    for( int offset = 0; offset < img_size; offset++ )
    {
        if( phi[offset] < 0 )
        {
            sum_in += int(img_data[offset]);
        }
        else
        {
            sum_out += int(img_data[offset]);
        }
    }

    if( sum_in > sum_out )
    {
        return true;
    }
    else
    {
        return false;
    }
}

unsigned char GeodesicAC::get_otsu_threshold() const
{
    return otsu_threshold1;
}

}
