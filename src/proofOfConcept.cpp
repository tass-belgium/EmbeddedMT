#include <fstream>
#include <time.h>
#include <ctime>
#include <sys/time.h>
#include <sys/stat.h>

#include "log/logging.hpp"
#include "cm/global.hpp"
#include "imProc/imageProcBase.hpp"
#include "descriptor/sift.hpp"
#include "descriptor/surf.hpp"
#include "match/bfMatcher.hpp"
#include "match/flannBasedMatcher.hpp"
#include "draw/draw.hpp"
#include "displacement/displacementBase.hpp"
#include "inputMethod/inputImageSequence.hpp"
#include "inputMethod/inputVideo.hpp"

#include "application/findTheBall.hpp"

static const char_t* getMethodPname(uint8_t matchAlgorithm);
static GBL::CmRetCode_t getInputMethod(const char* file, InputMethod::InputMethodInterface** inputMethod);
static GBL::CmRetCode_t getExecutors(uint32_t matchAlgorithm, const Descriptor::DescriptorInterface** descriptor, const Match::MatcherInterface** matcher);
static GBL::CmRetCode_t writeResults(const char* outputFile, std::vector<GBL::Displacement_t> data);
static int64_t GetTimeMs64();

int main(int argc, char** argv) {
	uint32_t matchAlgorithm = 0;
	const char* outputFile = "displacements.result";
	const char* imageSequenceFolder = nullptr;

	const ImageProc::ImageProc* imProc = new ImageProc::ImageProcBase();
	Draw::DrawInterface* drawer = new Draw::Draw();
	const Displacement::DisplacementInterface* displacement = new Displacement::DisplacementBase();
	InputMethod::InputMethodInterface* inputMethod = nullptr;

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
	if (argc > 3) {
		outputFile = argv[3];
	}
	LOG_INFO("Using %s", getMethodPname(matchAlgorithm));

	if (getInputMethod(imageSequenceFolder, &inputMethod) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not deduce a valid input method");
		return -1;
	}

	if (getExecutors(matchAlgorithm, &descriptor, &matcher) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not get valid descriptor and matcher. Define a valid algorithm to be used.");
		return -1;
	}

	clock_t tStart = clock();
	int64_t startTime = GetTimeMs64();
	std::vector<GBL::Displacement_t> displacements = findTheBall(imageSequenceFolder, imProc, *drawer, *descriptor, *matcher, *displacement, *inputMethod);
	int64_t endTime = GetTimeMs64();
	float_t totalTimeElapsed = (float_t) (clock() - tStart) / CLOCKS_PER_SEC;
	LOG(stdout, "TIME", "CPU time of processing stage: %f s", totalTimeElapsed);
	LOG(stdout, "TIME", "Execution time of processing stage: %f s", (float_t ) (endTime - startTime) / 1000.0f);

	if (writeResults(outputFile, displacements) != GBL::RESULT_SUCCESS) {
		LOG_WARNING("Could not write results to file %s", outputFile);
	}

	delete imProc;
	delete descriptor;
	delete matcher;
	delete displacement;
	delete drawer;
	delete inputMethod;

	return 0;
}

GBL::CmRetCode_t writeResults(const char* outputFile, std::vector<GBL::Displacement_t> data) {
	std::ofstream file;
	file.open(outputFile);
	if (!file.is_open()) {
		return GBL::RESULT_FAILURE;
	}
	for (uint32_t i = 0; i < data.size(); i++) {
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

GBL::CmRetCode_t getExecutors(uint32_t matchAlgorithm, const Descriptor::DescriptorInterface** descriptor, const Match::MatcherInterface** matcher) {
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
			LOG_ERROR("Unknown matching algorithm")
			;
			return GBL::RESULT_FAILURE;
	}
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t getInputMethod(const char* file, InputMethod::InputMethodInterface** inputMethod) {
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	struct stat st_buf;
	uint32_t status = stat(file, &st_buf);
	if (status == 0) {
		if (S_ISDIR (st_buf.st_mode)) {
			LOG_INFO("%s detected to be a directory.", file);
			*inputMethod = new InputMethod::InputImageSequence();
		} else {
			LOG_INFO("%s detected to be a file", file);
			*inputMethod = new InputMethod::InputVideo();
		}
		result = GBL::RESULT_SUCCESS;
	} else {
		LOG_ERROR("Could not get information about %s", file);
	}
	return result;
}

int64_t GetTimeMs64() {
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
