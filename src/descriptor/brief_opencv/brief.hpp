#ifndef BRIEF_OPENCV_HPP
#define BRIEF_OPENCV_HPP

#include <vector>
//#include "opencv2/core/core.hpp"
#include "opencv2/features2d.hpp"

using std::vector;

namespace cv
{
	/*
	* BRIEF Descriptor
	*/
	class CV_EXPORTS BriefDescriptorExtractor
	{
		public:
			static const int PATCH_SIZE = 48;
			static const int KERNEL_SIZE = 9;
			// bytes is a length of descriptor in bytes. It can be equal 16, 32 or 64 bytes.
			BriefDescriptorExtractor( int bytes = 32 );
			void read( const FileNode& );
			void write( FileStorage& ) const;
			int descriptorSize() const;
			int descriptorType() const;
			int defaultNorm() const;
			void computeImpl( const Mat& image, std::vector<KeyPoint>& keypoints, Mat& descriptors) const;
			/// @todo read and write for brief
//			AlgorithmInfo* info() const;
		protected:
			typedef void(*PixelTestFn)(const Mat&, const vector<KeyPoint>&, Mat&);
			int bytes_;
			PixelTestFn test_fn_;
	};
}

#endif
