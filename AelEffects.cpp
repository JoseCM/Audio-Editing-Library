#include "AelEffects.h"

using namespace Ael;
	
	
	int AelEffect::ID = 0;
	
AelAudioStream& AelEffect::processStream(AelAudioStream &mystream){
	
	AelAudioStream *temp = new AelAudioStream(mystream.getchannels());
	
	for (int i = 0; i < mystream.getnframes(); i++){
		AelFrame aux = mystream.getNextFrame();
		aux = processFrame(aux);
		temp->AddFrames(aux);
	}
	
	return *temp;
}

void AelEffect::setWetLevel(double level){
	
	if(level > 1.0 || level < 0.0)
		return;
	
	wet_level = level;
	
}



