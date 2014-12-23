#ifndef MEASURE_HPP
#define MEASURE_HPP

#include <chrono>
#include <vector>
#include <string>

namespace EmbeddedMT {
	namespace Test {
		namespace Measure {
			typedef int64_t Time_t; 
			typedef std::chrono::high_resolution_clock Clock_t;
			typedef std::chrono::nanoseconds nseconds;
			typedef std::chrono::microseconds useconds;
			typedef std::chrono::milliseconds mseconds;
			typedef std::chrono::seconds seconds;

			class ObjectToMeasure {
				public:
					virtual void operator()(void) const = 0;
			};

			/**
			 * Template types for automatically retrieving the unit string
			 */
			template <typename duration>
			const std::string getUnit(duration) { return "UNKNOWN UNIT";}

			template <>
			const std::string getUnit(nseconds) { return "ns"; }

			template <>
			const std::string getUnit(useconds) { return "us"; }
			
			template <>
			const std::string getUnit(mseconds) { return "ms"; }

			template <>
			const std::string getUnit(seconds) { return "s";}

			/**
			 * Template types for automatically retrieving the unit string of the unit 1000 times bigger 
			 */
			template <typename duration>
			const std::string getThousandUnit(duration T) { return getUnit(T);}

			template <>
			const std::string getThousandUnit(nseconds) { return getUnit(useconds::zero()); }

			template <>
			const std::string getThousandUnit(useconds) { return getUnit(mseconds::zero()); }
			
			template <>
			const std::string getThousandUnit(mseconds) { return getUnit(seconds::zero()); }

			template <>
			const std::string getThousandUnit(seconds) { return " * 1000 s";}

			template <typename duration>
			class Time {
				public:
					void start(void) {
						reset();
					}
					void reset(void) {
						_startTime = Clock_t::now();
					}
					/**
					 * Returns elapsed time since last call to start() or reset() in nanoseconds
					 */
					duration stop(void) {
						auto endTime = Clock_t::now();
						return std::chrono::duration_cast<duration>(endTime - _startTime);
					}
				private:
					std::chrono::high_resolution_clock::time_point _startTime;
			};


			template <typename duration>
			class TimeMeasurement {
				public:
					static duration measure(const ObjectToMeasure& objectToMeasure, const uint32_t nbOfMeasurements) {
						std::vector<duration> measurements(nbOfMeasurements);						

						for(uint32_t i = 0; i < nbOfMeasurements; ++i) {
							Time<duration> timeMeasurement;
							timeMeasurement.start();
							objectToMeasure();
							measurements.push_back(timeMeasurement.stop());
						}

						// Calculate the average
						duration averageDuration = duration::zero();
						for(auto measurement : measurements) {
							averageDuration += measurement;
						}
						return averageDuration/nbOfMeasurements;
					}
			};
		}
	}
}

#endif
