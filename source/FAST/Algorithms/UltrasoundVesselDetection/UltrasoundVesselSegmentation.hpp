#ifndef ULTRASOUND_VESSEL_SEGMENTATION_HPP
#define ULTRASOUND_VESSEL_SEGMENTATION_HPP

#include "FAST/ProcessObject.hpp"

namespace fast {

class VesselCrossSection;
class KalmanFilter;
class UltrasoundVesselDetection;
class Image;

class TrackedVessel : public Object {
	FAST_OBJECT(TrackedVessel)
public:
	SharedPointer<KalmanFilter> filter;
	int framesSinceRefshed;
};

class UltrasoundVesselSegmentation : public ProcessObject {
	FAST_OBJECT(UltrasoundVesselSegmentation)
	public:

	private:
		UltrasoundVesselSegmentation();
		void execute();

		int mFramesToKeep;
		/*
		 * This stores all the vessel cross section detected for N frames back
		 */
		std::deque<std::vector<SharedPointer<VesselCrossSection> > > mDetectedVessels;

		/*
		 * This keeps a Kalman filter for all the tracked vessels
		 */
		std::vector<SharedPointer<TrackedVessel> > mKalmanFilters;

		/*
		 * This keeps the ultrasound vessel detection object
		 */
		SharedPointer<UltrasoundVesselDetection> mDetector;

		void checkForNewVessels(float spacing);
		void trackAllVessels(SharedPointer<Image> image);
		void createSegmentation(SharedPointer<Image> image);
};

} // namespace fast


#endif
