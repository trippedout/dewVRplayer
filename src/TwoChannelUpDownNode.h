//
//  TwoChannelUpDownNode.h
//  Created by Anthony Tripaldi on 9/3/14.
//

#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/audio/Node.h"
#include "cinder/audio/Param.h"
#include "cinder/audio/dsp/Dsp.h"

namespace cinder { namespace audio {
    
    typedef std::shared_ptr<class TwoChannelUpDownNode>			TwoChannelUpDownNodeRef;
    
    /*
     Processes a sound file with 2 distinct channels for sky and ground. Plays them fully depending on 
     value of param, -1 being full ground and 1 being full sky, with 0 as neither.
     */
    class TwoChannelUpDownNode : public Node
    {
    public:
        TwoChannelUpDownNode( const Format &format );
        
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