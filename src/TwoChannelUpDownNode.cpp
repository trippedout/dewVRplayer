//
//  TwoChannelUpDownNode.cpp
//  Created by Anthony Tripaldi on 9/3/14.
//

#include "TwoChannelUpDownNode.h"

using namespace ci::app;
using namespace std;

namespace cinder { namespace audio {

TwoChannelUpDownNode::TwoChannelUpDownNode( const Format &format )
: Node( format ), mParam( this, 0.f )
{
}

void TwoChannelUpDownNode::process(Buffer *buffer)
{
    if( mParam.eval() ) {
        for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
            float *channel = buffer->getChannel( ch );
            dsp::sub( channel, mParam.getValueArray(), channel, buffer->getNumFrames() );
        }
    }
    else
    {        
        if(buffer->getNumChannels() == 2)
        {
            //get all channels
            float *upChannel = buffer->getChannel( 0 );
            float *downChannel = buffer->getChannel( 1 );
            
            //shouldnt vary much
            size_t numFrames = buffer->getNumFrames();
            float value = mParam.getValue();
            
            if(value > 0.0)
            {
                dsp::mul(upChannel, value, buffer->getChannel( 0 ), numFrames);
                dsp::mul(upChannel, value, buffer->getChannel( 1 ), numFrames);
            }
            else
            {
                float newVal = fabs(value);
                dsp::mul(downChannel, newVal, buffer->getChannel( 0 ), numFrames);
                dsp::mul(downChannel, newVal, buffer->getChannel( 1 ), numFrames);
            }
        }
    }
}

} } //cinder::audio