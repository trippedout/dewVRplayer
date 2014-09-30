#include "OculusRift.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace cinder{
	namespace ovr {

		OculusRift::OculusRift()
		{
			//init
			ovr_Initialize();

			//create hmd 
			mHmd = ovrHmd_Create(0);
			if (mHmd)
			{
				// Get more details about the HMD.
				ovrSizei resolution = mHmd->Resolution;
				console() << "hmd detected! " << mHmd->ProductName << " size: " << resolution.w << ", " << resolution.h << " reco Fov: " << mHmd->DefaultEyeFov[0].LeftTan << endl;
			}
			else
			{
				//no oculus
				console() << "No Oculus found! Check your settings." << endl;
				return;
			}

			ovrHmd_SetEnabledCaps(mHmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

			ovrHmd_ConfigureTracking(mHmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
		}

		OculusRift::~OculusRift()
		{

		}
        
        void OculusRift::keyDown(KeyEvent event)
        {
            //recenter camera forward
            if(event.isControlDown() && event.getChar() == 'r') ovrHmd_RecenterPose(mHmd);
        }

		void OculusRift::update()
		{
			ovrTrackingState state = ovrHmd_GetTrackingState(mHmd, ovr_GetTimeInSeconds());
			if ((state.StatusFlags & ovrStatus_OrientationTracked) | ovrStatus_PositionTracked)
			{
				/*console() << "trackingState::Quatf" <<
					state.HeadPose.ThePose.Orientation.x << ", " <<
					state.HeadPose.ThePose.Orientation.y << ", " <<
					state.HeadPose.ThePose.Orientation.z << ", " <<
					state.HeadPose.ThePose.Orientation.w << ", " <<
					" position: " <<
					state.HeadPose.ThePose.Position.x << ", " <<
					state.HeadPose.ThePose.Position.y << ", " <<
					state.HeadPose.ThePose.Position.z << ", " <<
					endl;*/

				mHeadPose = state.HeadPose;
			}
			else
			{
				console() << state.StatusFlags << endl;
			}
		}

		void OculusRift::shutdown()
		{
			ovrHmd_Destroy(mHmd);
			ovr_Shutdown();
		}


		/**
		* Helper Utilities for transfering between OVR and Cinder
		*/
		Quatf OculusRift::OculusToCinderQuatf(ovrQuatf ovrQuat)
		{
			return Quatf(ovrQuat.w, ovrQuat.x, ovrQuat.y, ovrQuat.z);
		}



	} /* namespace ovr */
} /* namespace cinder */