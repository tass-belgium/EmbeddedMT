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

// comment/uncomment below, respectively,
// if you want to test the algorithm with a RGB interleaved/planar image data buffer
//#define RGB_PLANAR_IMAGE_DATA_BUFFER
// function affected by the define : void RGB_from_buffer(int offset)

#include "ac_withoutedges_yuv.hpp"

namespace ofeli
{

ACwithoutEdgesYUV::ACwithoutEdgesYUV(const unsigned char* img_rgb_data1, int img_width1, int img_height1) :
    ActiveContour(img_rgb_data1, img_width1, img_height1,
                  true, 0.65, 0.65, 0.0, 0.0, true, 5, 2.0, 30, 3),
    lambda_out(1), lambda_in(1), alpha(1), beta(10), gamma(10)
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

ACwithoutEdgesYUV::ACwithoutEdgesYUV(const unsigned char* img_rgb_data1, int img_width1, int img_height1,
                                     bool hasEllipse1, double init_width1, double init_height1, double center_x1, double center_y1,
                                     bool hasCycle2_1, int kernel_length1, double sigma1, int Na1, int Ns1,
                                     int lambda_out1, int lambda_in1, int alpha1, int beta1, int gamma1) :
    ActiveContour(img_rgb_data1, img_width1, img_height1,
                  hasEllipse1, init_width1, init_height1, center_x1, center_y1,
                  hasCycle2_1, kernel_length1, sigma1, Na1, Ns1),
    lambda_out(lambda_out1), lambda_in(lambda_in1), alpha(alpha1), beta(beta1), gamma(gamma1)
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

ACwithoutEdgesYUV::ACwithoutEdgesYUV(const unsigned char* img_rgb_data1, int img_width1, int img_height1,
                                     const char* phi_init1,
                                     bool hasCycle2_1, int kernel_length1, double sigma1, int Na1, int Ns1,
                                     int lambda_out1, int lambda_in1, int alpha1, int beta1, int gamma1) :
    ActiveContour(img_rgb_data1, img_width1, img_height1,
                  phi_init1,
                  hasCycle2_1, kernel_length1, sigma1, Na1, Ns1),
    lambda_out(lambda_out1), lambda_in(lambda_in1), alpha(alpha1), beta(beta1), gamma(gamma1)
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

ACwithoutEdgesYUV::ACwithoutEdgesYUV(const ACwithoutEdgesYUV& ac) :
    ActiveContour(ac),
    lambda_out(ac.lambda_out), lambda_in(ac.lambda_in), alpha(ac.alpha), beta(ac.beta), gamma(ac.gamma),
    CoutR(ac.CoutR), CoutG(ac.CoutG), CoutB(ac.CoutB),
    CinR(ac.CinR), CinG(ac.CinG), CinB(ac.CinB),
    sum_out_R(ac.sum_out_R), sum_in_R(ac.sum_in_R), sum_out_G(ac.sum_out_G),
    sum_in_G(ac.sum_in_G), sum_out_B(ac.sum_out_B), sum_in_B(ac.sum_in_B),
    n_out(ac.n_out), n_in(ac.n_in), R(ac.R), G(ac.G), B(ac.B)
{
    for( int index = 0; index < 3; index++ )
    {
        CoutYUV[index] = ac.CoutYUV[index];
        CinYUV[index] = ac.CinYUV[index];
    }
}

void ACwithoutEdgesYUV::initialize_for_each_frame()
{
    int it = get_iteration();

    ActiveContour::initialize_for_each_frame();

    // condition for video tracking :
    // If the active contour moves enough ( >120 ), variables sums, n_in, n_out
    // are too different and are recalculated.
    // If the active contour does not evolve ( < 5 because a last cycle 2 is performed )
    // in the case of uniform images (or during the object initialization),
    // variables are (re)calculated for convergence.
    // Otherwise, variables are not calculated.
    if( it > 120 || it < 5 )
    {
        initialize_sums();
    }
    calculate_means();

    return;
}

void ACwithoutEdgesYUV::initialize_sums()
{
    sum_in_R = 0;
    sum_out_R = 0;

    sum_in_G = 0;
    sum_out_G = 0;

    sum_in_B = 0;
    sum_out_B = 0;

    n_in = 0;
    n_out = 0;

    for( int offset = 0; offset < img_size; offset++ )
    {
        RGB_from_buffer(offset);

        if( phi[offset] > 0 )
        {
            sum_out_R += R;
            sum_out_G += G;
            sum_out_B += B;

            n_out++;
        }
        else
        {
            sum_in_R += R;
            sum_in_G += G;
            sum_in_B += B;

            n_in++;
        }
    }

    return;
}

void ACwithoutEdgesYUV::calculate_means()
{
    if( n_out != 0 )
    {
        CoutR = sum_out_R/n_out;
        CoutG = sum_out_G/n_out;
        CoutB = sum_out_B/n_out;
    }

    if( n_in != 0 )
    {
        CinR = sum_in_R/n_in;
        CinG = sum_in_G/n_in;
        CinB = sum_in_B/n_in;
    }

    calculate_YUV(CoutR, CoutG, CoutB, CoutYUV);
    calculate_YUV(CinR, CinG, CinB, CinYUV);

    return;
}

int ACwithoutEdgesYUV::compute_external_speed_Fd(int offset)
{
    RGB_from_buffer(offset);

    int YUV[3];
    calculate_YUV(R, G, B, YUV);

    return    lambda_out*(   alpha*square(YUV[0]-CoutYUV[0])
                           + beta*square(YUV[1]-CoutYUV[1])
                           + gamma*square(YUV[2]-CoutYUV[2]) )

            - lambda_in*(   alpha*square(YUV[0]-CinYUV[0])
                          + beta*square(YUV[1]-CinYUV[1])
                          + gamma*square(YUV[2]-CinYUV[2]) );
}

void ACwithoutEdgesYUV::updates_for_means_in1()
{
    sum_out_R -= R;
    sum_out_G -= G;
    sum_out_B -= B;
    n_out--;

    sum_in_R += R;
    sum_in_G += G;
    sum_in_B += B;
    n_in++;

    return;
}

void ACwithoutEdgesYUV::updates_for_means_out1()
{
    sum_in_R -= R;
    sum_in_G -= G;
    sum_in_B -= B;
    n_in--;

    sum_out_R += R;
    sum_out_G += G;
    sum_out_B += B;
    n_out++;

    return;
}

void ACwithoutEdgesYUV::updates_for_means_in2(int offset)
{
    RGB_from_buffer(offset);

    updates_for_means_in1();

    return;
}

void ACwithoutEdgesYUV::updates_for_means_out2(int offset)
{
    RGB_from_buffer(offset);

    updates_for_means_out1();

    return;
}

int ACwithoutEdgesYUV::get_CoutR() const
{
    return CoutR;
}
int ACwithoutEdgesYUV::get_CoutG() const
{
    return CoutG;
}
int ACwithoutEdgesYUV::get_CoutB() const
{
    return CoutB;
}

int ACwithoutEdgesYUV::get_CinR() const
{
    return CinR;
}
int ACwithoutEdgesYUV::get_CinG() const
{
    return CinG;
}
int ACwithoutEdgesYUV::get_CinB() const
{
    return CinB;
}


const int* ACwithoutEdgesYUV::get_CoutYUV() const
{
    return CoutYUV;
}

const int* ACwithoutEdgesYUV::get_CinYUV() const
{
    return CinYUV;
}

}
