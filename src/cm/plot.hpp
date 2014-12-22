#ifndef PLOT_HPP
#define PLOT_HPP

#include <vector>

#include "cm/point.hpp"
#include "outputMethod/outputMethodInterface.hpp"

#include "opencv2/core.hpp"

namespace EmbeddedMT {
	namespace Utils {
		class Plot {
			public:
				static void drawContours(const GBL::Frame_t& image, const std::vector<std::vector<GBL::Point> >& contours, OutputMethod::OutputMethodInterface& outputMethod);
			private:
				static const std::vector<cv::Vec3b> colors;
		};
	}
}

#endif
