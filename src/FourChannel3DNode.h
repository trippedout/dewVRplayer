//
//  FourChannel3DNode.h
//  Created by Anthony Tripaldi on 9/1/14.
//

#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/Node.h"
#include "cinder/audio/Param.h"

namespace cinder { namespace audio {
    
    typedef std::shared_ptr<class FourChannel3DNode>			FourChannel3DNodeRef;
    
    /*
     Processes a sound file with 4 distinct channels related to 4 directions of viewing, this time
     recorded by dual binaural microphones, ordered specifically in North->East->South->West. North
     being forward facing in channel 0 and west being to your left in channel 3. Setting the value
     will change the sound scape from 0-360 degrees, raising and lowering channels depending on which
     direction you are facing, 90 being east, 180 north, 270 west, 360 south;
     */
    class FourChannel3DNode : public Node {
    public:
        FourChannel3DNode( const Format &format );

        //! Sets the current value to a constant \a value. This is a 0-1 number based on rotation around
        // the camera's axis - .5 being forward
        void setValue( float value )	{ mParam.setValue( value ); }
        //! Returns the current value.
        float getValue() const			{ return mParam.getValue(); }
        
        //! Returns a pointer to the Param that can be used to animate the value.
        Param* getParam()				{ return &mParam; }
        
    protected:
        void process( Buffer *buffer ) override;
        Param       mParam;
    };
    
    
} } //cinder::audio