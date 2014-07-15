#include "AelEffects.h"

using namespace Ael;

//////////////////////////////////////////////////////////////////
// Classe AelEffect
//////////////////////////////////////////////////////////////////


	int AelEffect::ID = 0;


/******************************************************************
 *Função membro processStream da classe AelEffect
 *Parâmetros: AelAudioSteam&
 *Responsável por processar todas as frames de uma stream de audio
 *  por um efeito
 *Retorna stream processada
 *******************************************************************/
AelAudioStream& AelEffect::processStream(AelAudioStream &mystream){
	
	AelAudioStream *temp = new AelAudioStream(mystream.getchannels());    //cria nova stream
    int init_pos = mystream.getCurrPosition();     //guarda posicionamento actual no ficheiro audioFstream
    mystream.rewind();     //posiciona stream para o íncio do ficheiro
    
	for (int i = 0; i < mystream.getnframes(); i++){    //percorre todas as frames da stream
		AelFrame aux = mystream.getNextFrame();     //obtem próxima frame
		aux = processFrame(aux);       //processa a frame obtida (processFrame puramente virtual)
		temp->AddFrames(aux);       //adiciona frame processada à nova stream
	}
    
    mystream.setCurrPosition(init_pos);    //reposiciona stream para posição inicial
	
	return *temp;   //retorma stream processada
}

/******************************************************************
 *Função membro setWetLevel da classe AelEffect
 *Parâmetros: double (level)
 *Responsável por atribuir um valor [0.0, 1.0] à variável wetlevel, 
 *  que traduz a proporção do sinal original face ao sinal processado
 *  numa stream processada
 *******************************************************************/
void AelEffect::setWetLevel(double level){
	
	if(level > 1.0 || level < 0.0)
		return;
	
	wet_level = level;
	
}



