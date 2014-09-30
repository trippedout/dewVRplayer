#pragma once

#include "cinder/app/AppNative.h"
#include "OVR.h"

namespace cinder{ namespace ovr {

class OculusRift;
typedef std::shared_ptr<OculusRift> OculusRiftRef;

class OculusRift
{
public:
	OculusRift();
	~OculusRift();

	static OculusRiftRef	create() { return std::shared_ptr<OculusRift>( new OculusRift()); }

    // Handle all normal cinder KeyEvents for default handling
    // See implementation for details (ctrl-R resets view forward)
    void keyDown(app::KeyEvent event);
    
	void update();
	void shutdown();

protected:
	ovrHmd				mHmd;
	ovrPoseStatef		mHeadPose;

private:
	//helpers
	Quatf				OculusToCinderQuatf(ovrQuatf ovrQuat);

public:	//getters 

	float				getRecoFOV() { return 120.0f; }

    ovrQuatf*           getOvrHeadPose() { return &mHeadPose.ThePose.Orientation; };
    
	/*
	Returns a cinder::Quatf that is perfect for passing to a cinder::CameraPersp or Ortho with setOrientation();
	*/
    ci::Quatf			getCurrentHeadPose() { return OculusToCinderQuatf(mHeadPose.ThePose.Orientation); }
};

} /* namespace ovr */
} /* namespace cinder */
