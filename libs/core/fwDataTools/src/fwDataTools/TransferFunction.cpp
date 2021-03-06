/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwDataTools/TransferFunction.hpp"

#include <map>

namespace fwDataTools
{

//------------------------------------------------------------------------------

void TransferFunction::generateDrawingTF(::fwData::TransferFunction::sptr tf)
{
    tf->setTFData(TransferFunction::getDrawingTFColors());
    tf->setWLMinMax(::fwData::TransferFunction::TFValuePairType(0, 255));
    tf->setInterpolationMode(::fwData::TransferFunction::NEAREST);
    tf->setName("Drawing");
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::TFDataType TransferFunction::getDrawingTFColors()
{
    typedef ::fwData::TransferFunction::TFColor TFColorType;
    static const ::fwData::TransferFunction::TFDataType dataType = {
        { 0.0, TFColorType(0/255.0, 0/255.0, 0/255.0, 0/255.0 ) },
        { 1.0, TFColorType(255/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 2.0, TFColorType(0/255.0, 0/255.0, 150/255.0, 255/255.0 ) },
        { 3.0, TFColorType(255/255.0, 100/255.0, 0/255.0, 255/255.0 ) },
        { 4.0, TFColorType(0/255.0, 255/255.0, 0/255.0, 255/255.0 ) },
        { 5.0, TFColorType(255/255.0, 255/255.0, 0/255.0, 255/255.0 ) },
        { 6.0, TFColorType(150/255.0, 0/255.0, 150/255.0, 255/255.0 ) },
        { 7.0, TFColorType(0/255.0, 255/255.0, 255/255.0, 255/255.0 ) },
        { 8.0, TFColorType(150/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 9.0, TFColorType(0/255.0, 150/255.0, 0/255.0, 255/255.0 ) },
        { 10.0, TFColorType(255/255.0, 177/255.0, 177/255.0, 255/255.0 ) },
        { 11.0, TFColorType(0/255.0, 150/255.0, 150/255.0, 255/255.0 ) },
        { 12.0, TFColorType(255/255.0, 0/255.0, 255/255.0, 255/255.0 ) },
        { 13.0, TFColorType(150/255.0, 150/255.0, 0/255.0, 255/255.0 ) },
        { 14.0, TFColorType(135/255.0, 135/255.0, 81/255.0, 255/255.0 ) },
        { 15.0, TFColorType(255/255.0, 255/255.0, 177/255.0, 255/255.0 ) },
        { 16.0, TFColorType(177/255.0, 177/255.0, 255/255.0, 255/255.0 ) },
        { 17.0, TFColorType(255/255.0, 200/255.0, 50/255.0, 255/255.0 ) },
        { 18.0, TFColorType(255/255.0, 177/255.0, 255/255.0, 255/255.0 ) },
        { 19.0, TFColorType(177/255.0, 255/255.0, 255/255.0, 255/255.0 ) },
        { 20.0, TFColorType(255/255.0, 50/255.0, 50/255.0, 255/255.0 ) },
        { 21.0, TFColorType(245/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 22.0, TFColorType(0/255.0, 0/255.0, 142/255.0, 255/255.0 ) },
        { 23.0, TFColorType(245/255.0, 95/255.0, 0/255.0, 255/255.0 ) },
        { 24.0, TFColorType(0/255.0, 245/255.0, 0/255.0, 255/255.0 ) },
        { 25.0, TFColorType(245/255.0, 245/255.0, 0/255.0, 255/255.0 ) },
        { 26.0, TFColorType(142/255.0, 0/255.0, 142/255.0, 255/255.0 ) },
        { 27.0, TFColorType(0/255.0, 245/255.0, 245/255.0, 255/255.0 ) },
        { 28.0, TFColorType(142/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 29.0, TFColorType(0/255.0, 142/255.0, 0/255.0, 255/255.0 ) },
        { 30.0, TFColorType(245/255.0, 169/255.0, 169/255.0, 255/255.0 ) },
        { 31.0, TFColorType(0/255.0, 142/255.0, 142/255.0, 255/255.0 ) },
        { 32.0, TFColorType(245/255.0, 0/255.0, 245/255.0, 255/255.0 ) },
        { 33.0, TFColorType(142/255.0, 142/255.0, 0/255.0, 255/255.0 ) },
        { 34.0, TFColorType(169/255.0, 245/255.0, 169/255.0, 255/255.0 ) },
        { 35.0, TFColorType(245/255.0, 245/255.0, 169/255.0, 255/255.0 ) },
        { 36.0, TFColorType(169/255.0, 169/255.0, 245/255.0, 255/255.0 ) },
        { 37.0, TFColorType(245/255.0, 192/255.0, 46/255.0, 255/255.0 ) },
        { 38.0, TFColorType(245/255.0, 169/255.0, 245/255.0, 255/255.0 ) },
        { 39.0, TFColorType(169/255.0, 245/255.0, 245/255.0, 255/255.0 ) },
        { 40.0, TFColorType(245/255.0, 46/255.0, 46/255.0, 255/255.0 ) },
        { 41.0, TFColorType(235/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 42.0, TFColorType(0/255.0, 0/255.0, 134/255.0, 255/255.0 ) },
        { 43.0, TFColorType(235/255.0, 90/255.0, 0/255.0, 255/255.0 ) },
        { 44.0, TFColorType(0/255.0, 235/255.0, 0/255.0, 255/255.0 ) },
        { 45.0, TFColorType(235/255.0, 235/255.0, 0/255.0, 255/255.0 ) },
        { 46.0, TFColorType(134/255.0, 0/255.0, 134/255.0, 255/255.0 ) },
        { 47.0, TFColorType(0/255.0, 235/255.0, 235/255.0, 255/255.0 ) },
        { 48.0, TFColorType(134/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 49.0, TFColorType(0/255.0, 134/255.0, 0/255.0, 255/255.0 ) },
        { 50.0, TFColorType(235/255.0, 161/255.0, 161/255.0, 255/255.0 ) },
        { 51.0, TFColorType(0/255.0, 134/255.0, 134/255.0, 255/255.0 ) },
        { 52.0, TFColorType(235/255.0, 0/255.0, 235/255.0, 255/255.0 ) },
        { 53.0, TFColorType(134/255.0, 134/255.0, 0/255.0, 255/255.0 ) },
        { 54.0, TFColorType(161/255.0, 235/255.0, 161/255.0, 255/255.0 ) },
        { 55.0, TFColorType(235/255.0, 235/255.0, 161/255.0, 255/255.0 ) },
        { 56.0, TFColorType(161/255.0, 161/255.0, 235/255.0, 255/255.0 ) },
        { 57.0, TFColorType(235/255.0, 184/255.0, 42/255.0, 255/255.0 ) },
        { 58.0, TFColorType(235/255.0, 161/255.0, 235/255.0, 255/255.0 ) },
        { 59.0, TFColorType(161/255.0, 235/255.0, 235/255.0, 255/255.0 ) },
        { 60.0, TFColorType(235/255.0, 42/255.0, 42/255.0, 255/255.0 ) },
        { 61.0, TFColorType(225/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 62.0, TFColorType(0/255.0, 0/255.0, 126/255.0, 255/255.0 ) },
        { 63.0, TFColorType(225/255.0, 85/255.0, 0/255.0, 255/255.0 ) },
        { 64.0, TFColorType(0/255.0, 225/255.0, 0/255.0, 255/255.0 ) },
        { 65.0, TFColorType(225/255.0, 225/255.0, 0/255.0, 255/255.0 ) },
        { 66.0, TFColorType(126/255.0, 0/255.0, 126/255.0, 255/255.0 ) },
        { 67.0, TFColorType(0/255.0, 225/255.0, 225/255.0, 255/255.0 ) },
        { 68.0, TFColorType(126/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 69.0, TFColorType(0/255.0, 126/255.0, 0/255.0, 255/255.0 ) },
        { 70.0, TFColorType(225/255.0, 153/255.0, 153/255.0, 255/255.0 ) },
        { 71.0, TFColorType(0/255.0, 126/255.0, 126/255.0, 255/255.0 ) },
        { 72.0, TFColorType(225/255.0, 0/255.0, 225/255.0, 255/255.0 ) },
        { 73.0, TFColorType(126/255.0, 126/255.0, 0/255.0, 255/255.0 ) },
        { 74.0, TFColorType(153/255.0, 225/255.0, 153/255.0, 255/255.0 ) },
        { 75.0, TFColorType(225/255.0, 225/255.0, 153/255.0, 255/255.0 ) },
        { 76.0, TFColorType(153/255.0, 153/255.0, 225/255.0, 255/255.0 ) },
        { 77.0, TFColorType(225/255.0, 176/255.0, 38/255.0, 255/255.0 ) },
        { 78.0, TFColorType(225/255.0, 153/255.0, 225/255.0, 255/255.0 ) },
        { 79.0, TFColorType(153/255.0, 225/255.0, 225/255.0, 255/255.0 ) },
        { 80.0, TFColorType(225/255.0, 38/255.0, 38/255.0, 255/255.0 ) },
        { 81.0, TFColorType(215/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 82.0, TFColorType(0/255.0, 0/255.0, 118/255.0, 255/255.0 ) },
        { 83.0, TFColorType(215/255.0, 80/255.0, 0/255.0, 255/255.0 ) },
        { 84.0, TFColorType(0/255.0, 215/255.0, 0/255.0, 255/255.0 ) },
        { 85.0, TFColorType(215/255.0, 215/255.0, 0/255.0, 255/255.0 ) },
        { 86.0, TFColorType(118/255.0, 0/255.0, 118/255.0, 255/255.0 ) },
        { 87.0, TFColorType(0/255.0, 215/255.0, 215/255.0, 255/255.0 ) },
        { 88.0, TFColorType(118/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 89.0, TFColorType(0/255.0, 118/255.0, 0/255.0, 255/255.0 ) },
        { 90.0, TFColorType(215/255.0, 145/255.0, 145/255.0, 255/255.0 ) },
        { 91.0, TFColorType(0/255.0, 118/255.0, 118/255.0, 255/255.0 ) },
        { 92.0, TFColorType(215/255.0, 0/255.0, 215/255.0, 255/255.0 ) },
        { 93.0, TFColorType(118/255.0, 118/255.0, 0/255.0, 255/255.0 ) },
        { 94.0, TFColorType(145/255.0, 215/255.0, 145/255.0, 255/255.0 ) },
        { 95.0, TFColorType(215/255.0, 215/255.0, 145/255.0, 255/255.0 ) },
        { 96.0, TFColorType(145/255.0, 145/255.0, 215/255.0, 255/255.0 ) },
        { 97.0, TFColorType(215/255.0, 168/255.0, 34/255.0, 255/255.0 ) },
        { 98.0, TFColorType(215/255.0, 145/255.0, 215/255.0, 255/255.0 ) },
        { 99.0, TFColorType(145/255.0, 215/255.0, 215/255.0, 255/255.0 ) },
        { 100.0, TFColorType(215/255.0, 34/255.0, 34/255.0, 255/255.0 ) },
        { 101.0, TFColorType(205/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 102.0, TFColorType(0/255.0, 0/255.0, 110/255.0, 255/255.0 ) },
        { 103.0, TFColorType(205/255.0, 75/255.0, 0/255.0, 255/255.0 ) },
        { 104.0, TFColorType(0/255.0, 205/255.0, 0/255.0, 255/255.0 ) },
        { 105.0, TFColorType(205/255.0, 205/255.0, 0/255.0, 255/255.0 ) },
        { 106.0, TFColorType(110/255.0, 0/255.0, 110/255.0, 255/255.0 ) },
        { 107.0, TFColorType(0/255.0, 205/255.0, 205/255.0, 255/255.0 ) },
        { 108.0, TFColorType(110/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 109.0, TFColorType(0/255.0, 110/255.0, 0/255.0, 255/255.0 ) },
        { 110.0, TFColorType(205/255.0, 137/255.0, 137/255.0, 255/255.0 ) },
        { 111.0, TFColorType(0/255.0, 110/255.0, 110/255.0, 255/255.0 ) },
        { 112.0, TFColorType(205/255.0, 0/255.0, 205/255.0, 255/255.0 ) },
        { 113.0, TFColorType(110/255.0, 110/255.0, 0/255.0, 255/255.0 ) },
        { 114.0, TFColorType(137/255.0, 205/255.0, 137/255.0, 255/255.0 ) },
        { 115.0, TFColorType(205/255.0, 205/255.0, 137/255.0, 255/255.0 ) },
        { 116.0, TFColorType(137/255.0, 137/255.0, 205/255.0, 255/255.0 ) },
        { 117.0, TFColorType(205/255.0, 160/255.0, 30/255.0, 255/255.0 ) },
        { 118.0, TFColorType(205/255.0, 137/255.0, 205/255.0, 255/255.0 ) },
        { 119.0, TFColorType(137/255.0, 205/255.0, 205/255.0, 255/255.0 ) },
        { 120.0, TFColorType(205/255.0, 30/255.0, 30/255.0, 255/255.0 ) },
        { 121.0, TFColorType(195/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 122.0, TFColorType(0/255.0, 0/255.0, 102/255.0, 255/255.0 ) },
        { 123.0, TFColorType(195/255.0, 70/255.0, 0/255.0, 255/255.0 ) },
        { 124.0, TFColorType(0/255.0, 195/255.0, 0/255.0, 255/255.0 ) },
        { 125.0, TFColorType(195/255.0, 195/255.0, 0/255.0, 255/255.0 ) },
        { 126.0, TFColorType(102/255.0, 0/255.0, 102/255.0, 255/255.0 ) },
        { 127.0, TFColorType(0/255.0, 195/255.0, 195/255.0, 255/255.0 ) },
        { 128.0, TFColorType(102/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 129.0, TFColorType(0/255.0, 102/255.0, 0/255.0, 255/255.0 ) },
        { 130.0, TFColorType(195/255.0, 129/255.0, 129/255.0, 255/255.0 ) },
        { 131.0, TFColorType(0/255.0, 102/255.0, 102/255.0, 255/255.0 ) },
        { 132.0, TFColorType(195/255.0, 0/255.0, 195/255.0, 255/255.0 ) },
        { 133.0, TFColorType(102/255.0, 102/255.0, 0/255.0, 255/255.0 ) },
        { 134.0, TFColorType(129/255.0, 195/255.0, 129/255.0, 255/255.0 ) },
        { 135.0, TFColorType(195/255.0, 195/255.0, 129/255.0, 255/255.0 ) },
        { 136.0, TFColorType(129/255.0, 129/255.0, 195/255.0, 255/255.0 ) },
        { 137.0, TFColorType(195/255.0, 152/255.0, 26/255.0, 255/255.0 ) },
        { 138.0, TFColorType(195/255.0, 129/255.0, 195/255.0, 255/255.0 ) },
        { 139.0, TFColorType(129/255.0, 195/255.0, 195/255.0, 255/255.0 ) },
        { 140.0, TFColorType(195/255.0, 26/255.0, 26/255.0, 255/255.0 ) },
        { 141.0, TFColorType(185/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 142.0, TFColorType(0/255.0, 0/255.0, 94/255.0, 255/255.0 ) },
        { 143.0, TFColorType(185/255.0, 65/255.0, 0/255.0, 255/255.0 ) },
        { 144.0, TFColorType(0/255.0, 185/255.0, 0/255.0, 255/255.0 ) },
        { 145.0, TFColorType(185/255.0, 185/255.0, 0/255.0, 255/255.0 ) },
        { 146.0, TFColorType(94/255.0, 0/255.0, 94/255.0, 255/255.0 ) },
        { 147.0, TFColorType(0/255.0, 185/255.0, 185/255.0, 255/255.0 ) },
        { 148.0, TFColorType(94/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 149.0, TFColorType(0/255.0, 94/255.0, 0/255.0, 255/255.0 ) },
        { 150.0, TFColorType(185/255.0, 121/255.0, 121/255.0, 255/255.0 ) },
        { 151.0, TFColorType(0/255.0, 94/255.0, 94/255.0, 255/255.0 ) },
        { 152.0, TFColorType(185/255.0, 0/255.0, 185/255.0, 255/255.0 ) },
        { 153.0, TFColorType(94/255.0, 94/255.0, 0/255.0, 255/255.0 ) },
        { 154.0, TFColorType(121/255.0, 185/255.0, 121/255.0, 255/255.0 ) },
        { 155.0, TFColorType(185/255.0, 185/255.0, 121/255.0, 255/255.0 ) },
        { 156.0, TFColorType(121/255.0, 121/255.0, 185/255.0, 255/255.0 ) },
        { 157.0, TFColorType(185/255.0, 144/255.0, 22/255.0, 255/255.0 ) },
        { 158.0, TFColorType(185/255.0, 121/255.0, 185/255.0, 255/255.0 ) },
        { 159.0, TFColorType(121/255.0, 185/255.0, 185/255.0, 255/255.0 ) },
        { 160.0, TFColorType(185/255.0, 22/255.0, 22/255.0, 255/255.0 ) },
        { 161.0, TFColorType(175/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 162.0, TFColorType(0/255.0, 0/255.0, 86/255.0, 255/255.0 ) },
        { 163.0, TFColorType(175/255.0, 60/255.0, 0/255.0, 255/255.0 ) },
        { 164.0, TFColorType(0/255.0, 175/255.0, 0/255.0, 255/255.0 ) },
        { 165.0, TFColorType(175/255.0, 175/255.0, 0/255.0, 255/255.0 ) },
        { 166.0, TFColorType(86/255.0, 0/255.0, 86/255.0, 255/255.0 ) },
        { 167.0, TFColorType(0/255.0, 175/255.0, 175/255.0, 255/255.0 ) },
        { 168.0, TFColorType(86/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 169.0, TFColorType(0/255.0, 86/255.0, 0/255.0, 255/255.0 ) },
        { 170.0, TFColorType(175/255.0, 113/255.0, 113/255.0, 255/255.0 ) },
        { 171.0, TFColorType(0/255.0, 86/255.0, 86/255.0, 255/255.0 ) },
        { 172.0, TFColorType(175/255.0, 0/255.0, 175/255.0, 255/255.0 ) },
        { 173.0, TFColorType(86/255.0, 86/255.0, 0/255.0, 255/255.0 ) },
        { 174.0, TFColorType(113/255.0, 175/255.0, 113/255.0, 255/255.0 ) },
        { 175.0, TFColorType(175/255.0, 175/255.0, 113/255.0, 255/255.0 ) },
        { 176.0, TFColorType(113/255.0, 113/255.0, 175/255.0, 255/255.0 ) },
        { 177.0, TFColorType(175/255.0, 136/255.0, 18/255.0, 255/255.0 ) },
        { 178.0, TFColorType(175/255.0, 113/255.0, 175/255.0, 255/255.0 ) },
        { 179.0, TFColorType(113/255.0, 175/255.0, 175/255.0, 255/255.0 ) },
        { 180.0, TFColorType(175/255.0, 18/255.0, 18/255.0, 255/255.0 ) },
        { 181.0, TFColorType(165/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 182.0, TFColorType(0/255.0, 0/255.0, 78/255.0, 255/255.0 ) },
        { 183.0, TFColorType(165/255.0, 55/255.0, 0/255.0, 255/255.0 ) },
        { 184.0, TFColorType(0/255.0, 165/255.0, 0/255.0, 255/255.0 ) },
        { 185.0, TFColorType(165/255.0, 165/255.0, 0/255.0, 255/255.0 ) },
        { 186.0, TFColorType(78/255.0, 0/255.0, 78/255.0, 255/255.0 ) },
        { 187.0, TFColorType(0/255.0, 165/255.0, 165/255.0, 255/255.0 ) },
        { 188.0, TFColorType(78/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 189.0, TFColorType(0/255.0, 78/255.0, 0/255.0, 255/255.0 ) },
        { 190.0, TFColorType(165/255.0, 105/255.0, 105/255.0, 255/255.0 ) },
        { 191.0, TFColorType(0/255.0, 78/255.0, 78/255.0, 255/255.0 ) },
        { 192.0, TFColorType(165/255.0, 0/255.0, 165/255.0, 255/255.0 ) },
        { 193.0, TFColorType(78/255.0, 78/255.0, 0/255.0, 255/255.0 ) },
        { 194.0, TFColorType(105/255.0, 165/255.0, 105/255.0, 255/255.0 ) },
        { 195.0, TFColorType(165/255.0, 165/255.0, 105/255.0, 255/255.0 ) },
        { 196.0, TFColorType(105/255.0, 105/255.0, 165/255.0, 255/255.0 ) },
        { 197.0, TFColorType(165/255.0, 128/255.0, 14/255.0, 255/255.0 ) },
        { 198.0, TFColorType(165/255.0, 105/255.0, 165/255.0, 255/255.0 ) },
        { 199.0, TFColorType(105/255.0, 165/255.0, 165/255.0, 255/255.0 ) },
        { 200.0, TFColorType(165/255.0, 14/255.0, 14/255.0, 255/255.0 ) },
        { 201.0, TFColorType(155/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 202.0, TFColorType(0/255.0, 0/255.0, 70/255.0, 255/255.0 ) },
        { 203.0, TFColorType(155/255.0, 50/255.0, 0/255.0, 255/255.0 ) },
        { 204.0, TFColorType(0/255.0, 155/255.0, 0/255.0, 255/255.0 ) },
        { 205.0, TFColorType(155/255.0, 155/255.0, 0/255.0, 255/255.0 ) },
        { 206.0, TFColorType(70/255.0, 0/255.0, 70/255.0, 255/255.0 ) },
        { 207.0, TFColorType(0/255.0, 155/255.0, 155/255.0, 255/255.0 ) },
        { 208.0, TFColorType(70/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 209.0, TFColorType(0/255.0, 70/255.0, 0/255.0, 255/255.0 ) },
        { 210.0, TFColorType(155/255.0, 97/255.0, 97/255.0, 255/255.0 ) },
        { 211.0, TFColorType(0/255.0, 70/255.0, 70/255.0, 255/255.0 ) },
        { 212.0, TFColorType(155/255.0, 0/255.0, 155/255.0, 255/255.0 ) },
        { 213.0, TFColorType(70/255.0, 70/255.0, 0/255.0, 255/255.0 ) },
        { 214.0, TFColorType(97/255.0, 155/255.0, 97/255.0, 255/255.0 ) },
        { 215.0, TFColorType(155/255.0, 155/255.0, 97/255.0, 255/255.0 ) },
        { 216.0, TFColorType(97/255.0, 97/255.0, 155/255.0, 255/255.0 ) },
        { 217.0, TFColorType(155/255.0, 120/255.0, 10/255.0, 255/255.0 ) },
        { 218.0, TFColorType(155/255.0, 97/255.0, 155/255.0, 255/255.0 ) },
        { 219.0, TFColorType(97/255.0, 155/255.0, 155/255.0, 255/255.0 ) },
        { 220.0, TFColorType(155/255.0, 10/255.0, 10/255.0, 255/255.0 ) },
        { 221.0, TFColorType(145/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 222.0, TFColorType(0/255.0, 0/255.0, 62/255.0, 255/255.0 ) },
        { 223.0, TFColorType(145/255.0, 45/255.0, 0/255.0, 255/255.0 ) },
        { 224.0, TFColorType(0/255.0, 145/255.0, 0/255.0, 255/255.0 ) },
        { 225.0, TFColorType(145/255.0, 145/255.0, 0/255.0, 255/255.0 ) },
        { 226.0, TFColorType(62/255.0, 0/255.0, 62/255.0, 255/255.0 ) },
        { 227.0, TFColorType(0/255.0, 145/255.0, 145/255.0, 255/255.0 ) },
        { 228.0, TFColorType(62/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 229.0, TFColorType(0/255.0, 62/255.0, 0/255.0, 255/255.0 ) },
        { 230.0, TFColorType(145/255.0, 89/255.0, 89/255.0, 255/255.0 ) },
        { 231.0, TFColorType(0/255.0, 62/255.0, 62/255.0, 255/255.0 ) },
        { 232.0, TFColorType(145/255.0, 0/255.0, 145/255.0, 255/255.0 ) },
        { 233.0, TFColorType(62/255.0, 62/255.0, 0/255.0, 255/255.0 ) },
        { 234.0, TFColorType(89/255.0, 145/255.0, 89/255.0, 255/255.0 ) },
        { 235.0, TFColorType(145/255.0, 145/255.0, 89/255.0, 255/255.0 ) },
        { 236.0, TFColorType(89/255.0, 89/255.0, 145/255.0, 255/255.0 ) },
        { 237.0, TFColorType(145/255.0, 112/255.0, 6/255.0, 255/255.0 ) },
        { 238.0, TFColorType(145/255.0, 89/255.0, 145/255.0, 255/255.0 ) },
        { 239.0, TFColorType(89/255.0, 145/255.0, 145/255.0, 255/255.0 ) },
        { 240.0, TFColorType(145/255.0, 6/255.0, 6/255.0, 255/255.0 ) },
        { 241.0, TFColorType(135/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 242.0, TFColorType(0/255.0, 0/255.0, 54/255.0, 255/255.0 ) },
        { 243.0, TFColorType(135/255.0, 40/255.0, 0/255.0, 255/255.0 ) },
        { 244.0, TFColorType(0/255.0, 135/255.0, 0/255.0, 255/255.0 ) },
        { 245.0, TFColorType(135/255.0, 135/255.0, 0/255.0, 255/255.0 ) },
        { 246.0, TFColorType(54/255.0, 0/255.0, 54/255.0, 255/255.0 ) },
        { 247.0, TFColorType(0/255.0, 135/255.0, 135/255.0, 255/255.0 ) },
        { 248.0, TFColorType(54/255.0, 0/255.0, 0/255.0, 255/255.0 ) },
        { 249.0, TFColorType(0/255.0, 54/255.0, 0/255.0, 255/255.0 ) },
        { 250.0, TFColorType(135/255.0, 81/255.0, 81/255.0, 255/255.0 ) },
        { 251.0, TFColorType(0/255.0, 54/255.0, 54/255.0, 255/255.0 ) },
        { 252.0, TFColorType(135/255.0, 0/255.0, 135/255.0, 255/255.0 ) },
        { 253.0, TFColorType(54/255.0, 54/255.0, 0/255.0, 255/255.0 ) },
        { 254.0, TFColorType(81/255.0, 135/255.0, 81/255.0, 255/255.0 ) },
        { 255.0, TFColorType(177/255.0, 255/255.0, 177/255.0, 255/255.0 ) }
    };
    return dataType;
}

} // namespace fwDataTools
