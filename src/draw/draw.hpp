/*
 * draw.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include "cm/global.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "drawInterface.hpp"

namespace Draw {
class Draw : public DrawInterface {
public:
	virtual GBL::CmRetCode_t draw(const GBL::Image_t& img1, const GBL::Image_t& img2, std::vector<GBL::Match_t> good_matches,
			std::vector<GBL::KeyPoint_t> keypoints1, std::vector<GBL::KeyPoint_t> keypoints2, GBL::Image_t& outputFrame) const;
	virtual GBL::CmRetCode_t openFile(const char* file, uint16_t width, uint16_t height);
	virtual GBL::CmRetCode_t closeFile(void);
private:
	cv::VideoWriter _videoFile;
};
}
