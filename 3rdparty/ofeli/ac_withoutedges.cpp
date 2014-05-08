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

#include "ac_withoutedges.hpp"

namespace ofeli
{

ACwithoutEdges::ACwithoutEdges(const unsigned char* img_data1, int img_width1, int img_height1) :
    ActiveContour(img_data1, img_width1, img_height1,
                  true, 0.65, 0.65, 0.0, 0.0, true, 5, 2.0, 30, 3),
    lambda_out(1), lambda_in(1)
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

ACwithoutEdges::ACwithoutEdges(const unsigned char* img_data1, int img_width1, int img_height1,
                               bool hasEllipse1, double init_width1, double init_height1, double center_x1, double center_y1,
                               bool hasCycle2_1, int kernel_length1, double sigma1, int Na1, int Ns1,
                               int lambda_out1, int lambda_in1) :
    ActiveContour(img_data1, img_width1, img_height1,
                  hasEllipse1, init_width1, init_height1, center_x1, center_y1,
                  hasCycle2_1, kernel_length1, sigma1, Na1, Ns1),
    lambda_out(lambda_out1), lambda_in(lambda_in1)
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

ACwithoutEdges::ACwithoutEdges(const unsigned char* img_data1, int img_width1, int img_height1,
                               const char* phi_init1,
                               bool hasCycle2_1, int kernel_length1, double sigma1, int Na1, int Ns1,
                               int lambda_out1, int lambda_in1) :
    ActiveContour(img_data1, img_width1, img_height1,
                  phi_init1,
                  hasCycle2_1, kernel_length1, sigma1, Na1, Ns1),
    lambda_out(lambda_out1), lambda_in(lambda_in1)
{
    // a virtual function call in a constructor
    // but the virtual function table is solved at this level of the class hierarchy
    initialize_for_each_frame();
}

ACwithoutEdges::ACwithoutEdges(const ACwithoutEdges& ac) :
    ActiveContour(ac),
    lambda_out(ac.lambda_out), lambda_in(ac.lambda_in), Cout(ac.Cout), Cin(ac.Cin),
    sum_out(ac.sum_out), sum_in(ac.sum_in), n_out(ac.n_out), n_in(ac.n_in),
    I(ac.I)
{
}

void ACwithoutEdges::initialize_for_each_frame()
{
    ActiveContour::initialize_for_each_frame();

    initialize_sums();
    calculate_means();

    return;
}

void ACwithoutEdges::initialize_sums()
{
    sum_in = 0;
    sum_out = 0;
    n_in = 0;
    n_out = 0;

    for( int offset = 0; offset < img_size; offset++ )
    {
        if( phi[offset] > 0 )
        {
            sum_out += int(img_data[offset]);
            n_out++;
        }
        else
        {
            sum_in += int(img_data[offset]);
            n_in++;
        }
    }

    return;
}

void ACwithoutEdges::calculate_means()
{
    if( n_out != 0 )
    {
        Cout = sum_out/n_out;
    }

    if( n_in != 0 )
    {
        Cin = sum_in/n_in;
    }

    return;
}

int ACwithoutEdges::compute_external_speed_Fd(int offset)
{
    I = int(img_data[offset]); // intensity of the current pixel

    return lambda_out*square(I-Cout) - lambda_in*square(I-Cin);
}

void ACwithoutEdges::updates_for_means_in1()
{
    sum_out -= I;
    n_out--;

    sum_in += I;
    n_in++;

    return;
}

void ACwithoutEdges::updates_for_means_out1()
{
    sum_in -= I;
    n_in--;

    sum_out += I;
    n_out++;

    return;
}

void ACwithoutEdges::updates_for_means_in2(int offset)
{
    I = int(img_data[offset]); // intensity of the current pixel

    updates_for_means_in1();

    return;
}

void ACwithoutEdges::updates_for_means_out2(int offset)
{
    I = int(img_data[offset]); // intensity of the current pixel

    updates_for_means_out1();

    return;
}

int ACwithoutEdges::get_Cout() const
{
    return Cout;
}

int ACwithoutEdges::get_Cin() const
{
    return Cin;
}

}
