#include <fstream>
#include <string>
#include <time.h>
#include <dirent.h>
#include <ctime>
#include <sys/time.h>

#include "log/logging.hpp"
#include "cm/global.hpp"
#include "imProc/imageProcBase.hpp"
#include "descriptor/sift.hpp"
#include "descriptor/surf.hpp"
#include "match/bfMatcher.hpp"
#include "match/flannBasedMatcher.hpp"
#include "draw/draw.hpp"
#include "displacement/displacementBase.hpp"

#include "application/findTheBall.hpp"

static const char_t* getMethodPname(uint8_t matchAlgorithm);
static GBL::CmRetCode_t getExecutors(uint32_t matchAlgorithm, const Descriptor::DescriptorInterface** descriptor,
		const Match::MatcherInterface** matcher);
static GBL::CmRetCode_t writeResults(const char* outputFile, std::vector<GBL::Displacement_t> data);
static GBL::CmRetCode_t setFiles (const char* const dir, GBL::ImageSequence_t& imageSequence);
static bool isImage(struct dirent* dirp);
static int64_t GetTimeMs64();

int main(int argc, char** argv) {
	uint32_t matchAlgorithm = 0;
	const char* outputFile = "displacements.result";
	const char* imageSequenceFolder = nullptr;

	const ImageProc::ImageProc* imProc = new ImageProc::ImageProcBase();
	const Draw::DrawInterface* drawer = new Draw::Draw();
	const Displacement::DisplacementInterface* displacement = new Displacement::DisplacementBase();

	const Descriptor::DescriptorInterface* descriptor = nullptr;
	const Match::MatcherInterface* matcher = nullptr;

	if (argc > 1) {
		imageSequenceFolder = argv[1];
	} else {
		LOG_ERROR("Image sequence folder not defined");
		return -1;
	}
	if (argc > 2) {
		matchAlgorithm = (uint32_t) std::strtoul(argv[2], NULL, 10);
	}
	if(argc > 3) {
		outputFile = argv[3];
	}
	LOG_INFO("Using %s", getMethodPname(matchAlgorithm));

	if(getExecutors(matchAlgorithm, &descriptor, &matcher) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not get valid descriptor and matcher. Define a valid algorithm to be used.");
		return -1;
	}

	GBL::ImageSequence_t imageSequence;
	if(setFiles(imageSequenceFolder, imageSequence) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not retrieve content of %s", imageSequenceFolder);
	}
	// Sort files
	std::sort(imageSequence.images.begin(), imageSequence.images.end());

	// Print files
	for(uint32_t i = 0; i < imageSequence.images.size(); i++) {
		LOG_INFO("Added %s to imageSequence.images", imageSequence.images[i].c_str());
	}

	clock_t tStart = clock();
	int64_t startTime = GetTimeMs64();
	std::vector<GBL::Displacement_t> displacements = findTheBall(imageSequence, imProc, *drawer, *descriptor, *matcher, *displacement);
	int64_t endTime = GetTimeMs64();
	float_t totalTimeElapsed = (float_t) (clock() - tStart)/ CLOCKS_PER_SEC;
	LOG(stdout,"TIME","CPU time of processing stage: %f s", totalTimeElapsed);
	LOG(stdout,"TIME","Execution time of processing stage: %f s", (float_t) (endTime-startTime)/1000.0f);

	if(writeResults(outputFile, displacements) != GBL::RESULT_SUCCESS) {
		LOG_WARNING("Could not write results to file %s", outputFile);
	}

	delete imProc;
	delete descriptor;
	delete matcher;
	delete displacement;
	delete drawer;

	return 0;
}

GBL::CmRetCode_t writeResults(const char* outputFile, std::vector<GBL::Displacement_t> data) {
	std::ofstream file;
	file.open(outputFile);
	if(! file.is_open()) {
		return GBL::RESULT_FAILURE;
	}
	for(uint32_t i = 0; i < data.size(); i++) {
		file << data[i].x << "\t" << data[i].y << std::endl;
	}
	file.close();
	return GBL::RESULT_SUCCESS;
}

const char_t* getMethodPname(uint8_t matchAlgorithm) {
	const char_t* algoName = "Unknown";
	switch (matchAlgorithm) {
	case 0:
		algoName = "SURF+brute force";
		break;
	case 1:
		algoName = "SURF+FLANN";
		break;
	case 2:
		algoName = "SIFT+brute force";
		break;
	case 3:
		algoName = "SIFT+FLANN";
		break;
	}
	return algoName;
}

GBL::CmRetCode_t getExecutors(uint32_t matchAlgorithm, const Descriptor::DescriptorInterface** descriptor,
		const Match::MatcherInterface** matcher) {
	switch (matchAlgorithm) {
	case 0:
		*descriptor = new Descriptor::Surf();
		*matcher = new Match::BfMatcher();
		break;
	case 1:
		*descriptor = new Descriptor::Surf();
		*matcher = new Match::FlannBasedMatcher();
		break;
	case 2:
		*descriptor = new Descriptor::Sift();
		*matcher = new Match::BfMatcher();
		break;
	case 3:
		*descriptor = new Descriptor::Sift();
		*matcher = new Match::FlannBasedMatcher();
		break;
	default:
		LOG_ERROR("Unknown matching algorithm");
		return GBL::RESULT_FAILURE;
	}
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t setFiles (const char* const dir, GBL::ImageSequence_t& imageSequence) {
	snprintf(imageSequence.backgroundImage, (size_t) GBL::maxFilenameLength, "%s/background.jpg", dir);
	LOG_INFO("Used background image: %s", imageSequence.backgroundImage);

    DIR *dp;
    struct dirent *dirp;
    dp  = opendir(dir);
    if(dp == nullptr) {
        LOG_ERROR("Error opening %s", dir);
        return GBL::RESULT_FAILURE;
    }

    dirp = readdir(dp);
    while (dirp != nullptr) {
    	if(isImage(dirp)) {
    		char imagePath[GBL::maxFilenameLength];
    		snprintf(imagePath, (size_t) GBL::maxFilenameLength, "%s/%s", dir, dirp->d_name);
    		imageSequence.images.push_back(imagePath);
    	}
    	dirp = readdir(dp);
    }
    closedir(dp);
    return GBL::RESULT_SUCCESS;
}

bool isImage(struct dirent* dirp) {
	// Check if file
	if(dirp->d_type != DT_REG) {
		return false;
	}

	size_t len = strlen(dirp->d_name);
	// Check for length -> point to "." and ".."
	if(len <= 2) {
		return false;
	}

	// We use filenames in stead of MIME-type for the moment
	if(std::strncmp(dirp->d_name, "background.jpg", sizeof("background.jpg")) == 0) {	// Check if it is the background image
		return false;
	}

	if(std::strncmp(&dirp->d_name[len-4], ".jpg", sizeof(".jpg")) == 0) {
		return true;
	}
	return false;
}

int64_t GetTimeMs64()
{
#ifdef WIN32
 /* Windows */
 FILETIME ft;
 LARGE_INTEGER li;

 /* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
  * to a LARGE_INTEGER structure. */
 GetSystemTimeAsFileTime(&ft);
 li.LowPart = ft.dwLowDateTime;
 li.HighPart = ft.dwHighDateTime;

 uint64 ret = li.QuadPart;
 ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
 ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

 return ret;
#else
 /* Linux */
 struct timeval tv;

 gettimeofday(&tv, NULL);

 uint64_t ret = tv.tv_usec;
 /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
 ret /= 1000;

 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 ret += (tv.tv_sec * 1000);

 return ret;
#endif
}
