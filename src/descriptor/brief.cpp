/*
 * brief.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: Bart Verhagen
 */
#include <vector>
#include "brief_opencv/brief.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "brief.hpp"

namespace EmbeddedMT {
	namespace Descriptor {
		GBL::CmRetCode_t Brief::describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
			LOG_ENTER("image = %p", &image);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;

			compute(image, keypoints, descriptor);
			result = GBL::RESULT_SUCCESS;

			LOG_EXIT("result = %d", result);
			return result;
		}

		void Brief::compute( const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptors ) const
		{
			if( image.empty() || keypoints.empty() )
			{
				descriptors.release();
				return;
			}
			
			cv::BriefDescriptorExtractor extractor(16);

			cv::KeyPointsFilter::runByImageBorder( keypoints, image.size(), 0 );
			cv::KeyPointsFilter::runByKeypointSize( keypoints, std::numeric_limits<float>::epsilon() );

			extractor.computeImpl( image, keypoints, descriptors );
		}
	}
}
