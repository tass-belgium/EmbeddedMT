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
	class CV_EXPORTS BriefDescriptorExtractor : public DescriptorExtractor
	{
		public:
			static const int PATCH_SIZE = 48;
			static const int KERNEL_SIZE = 9;
			// bytes is a length of descriptor in bytes. It can be equal 16, 32 or 64 bytes.
			BriefDescriptorExtractor( int bytes = 32 );
			virtual void read( const FileNode& );
			virtual void write( FileStorage& ) const;
			virtual int descriptorSize() const;
			virtual int descriptorType() const;
			virtual int defaultNorm() const;
			/// @todo read and write for brief
//			AlgorithmInfo* info() const;
		protected:
			void computeImplHelper(InputArray image, vector<KeyPoint>& keypoints, Mat& descriptors) const;
			virtual void computeImpl( InputArray image, std::vector<KeyPoint>& keypoints, OutputArray descriptors) const;

			typedef void(*PixelTestFn)(const Mat&, const vector<KeyPoint>&, Mat&);
			int bytes_;
			PixelTestFn test_fn_;
	};
}

#endif
