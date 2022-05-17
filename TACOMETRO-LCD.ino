// C++ code
//
//OS botões seguem a seguinte ordem de acordo com o posicionamento abaixo do LCD no esquema:  B1---B2---B3---B4
#define B1 13 
#define B2 12
#define B3 11
#define B4 10

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);


//--CRIAÇÃO DE CARACTERES PARA O LCD

byte BACK[] = {
  B11111,
  B10001,
  B10001,
  B00101,
  B01101,
  B11111,
  B01100,
  B00100
};

byte CIMA[] = {
  B00000,
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B00000
};

byte BAIXO[] = {
  B00000,
  B11111,
  B11111,
  B01110,
  B01110,
  B00100,
  B00100,
  B00000
};

byte ENTER[] = {
  B00100,
  B00110,
  B11111,
  B10110,
  B10100,
  B10001,
  B10001,
  B11111
};

byte STOP[] = {
  B10001,
  B11011,
  B01110,
  B00100,
  B01110,
  B11011,
  B10001,
  B00000
};

byte CHECK[] = {
  B00000,
  B00001,
  B00011,
  B10110,
  B11100,
  B01000,
  B00000,
  B00000
};

byte INC[] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000
};

byte C_INC[] = { //C_INC == "C"hange "INC"rement unit
  B01111,
  B01001,
  B11101,
  B01000,
  B00010,
  B10111,
  B10010,
  B11110
};

int typeTest = 0;			//ARMAZENA O TIPO DE TESTE QUE SERÁ FEITO:  0 --> SIMPLES || 1 --> COMPLETO.
int incremento = 1;			//VARIÁVEL DE INCREMENTO. USADA PARA AJUSTAR VALORES ANTES DO INICIO DO TESTE (SOMENTE NO TESTE COMPLETO).
int numIMAS = 0;			//VARIÁVEL DE CONTROLE. DETERMINA O NÚMERO DE PULSOS POR ROTAÇÃO QUE O SENSOR IRÁ LER.
int pressButton = 0;		//VARIÁVEL DE CONTROLE. ARMAZENA UM VALOR DIFERENTE DEPENDENDO DE QUAL BOTÃO FOI PRESSIONADO.
int stage = 1;				//VARIÁVEL DE CONTROLE. DETERMINA SUBTELAS UTILIZADA NO TESTE COMPLETO (SOMENTE NO TESTE COMPLETO).
int tela = 1;				//VARIÁVEL DE CONTROLE. ARMAZENA O VALOR DA TELA QUE SERÁ MOSTRADA. CADA TELA POSSUI UM VALOR DIFERENTE. 
int lastTela = 0;			//VARIÁVEL DE CONTROLE. ARMAZENA O ÚLTIMO VALOR DA TELA ANTES DE MUDAR PARA OUTRA TELA. USADA QUANDO QUEREMOS INTERROMPER O TESTE.

char textInc = 'U';			//VARIÁVEL DE INFORMAÇÃO. USADA PARA INDICAR A UNIDADE CORRESPONDENTE AO INCREMENTO. POR EXEMPLO: "U" SIGNIFICA "UNIDADE", "C" CENTENA E ASSIM POR DIANTE...

unsigned long pressTime = 0;//VARIÁVEL DE CONTROLE: TEMPO EM QUE O BOTÃO PERMANECE APERTADO.
unsigned long startTime = 0;//VARIÁVEL DE CONTROLE. ARMAZENA O VALOR DO TEMPO NO INICIO DO TESTE (SOMENTE NO TESTE COMPLETO).

unsigned int testTime = 0;	//TEMPO DE DURAÇÃO DO TESTE (SEGUNDO E MINUTOS)(SOMENTE NO TESTE COMPLETO).
unsigned int atualRPM = 0;	//VALOR DE RPM ATUAL.
unsigned int valueRPM = 0;	//VALOR DE RPM QUE SERÁ COLETADO ATRAVÉS DA LEITURA DO SNSOR. ATUALIZADO A CADA CICLO DO ARDUINO.
unsigned int maxRPM = 0;	//VALOR DE RPM MÁXIMO (SOMENTE NO TESTE COMPLETO).
unsigned int nominalRPM = 0;//VALOR DE RPM NOMINAL, PARA COMPARAÇÃO APÓS O FINAL DO TESTE (SOMENTE NO TESTE COMPLETO).


void setup()
{
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  pinMode(B4, INPUT);
  
  Serial.begin(9600); //INICIA A SERIAL PARA "DEBUGAR"
  lcd.begin(16, 2);
  
  //ARMAZENA OS CARACTERES CRIADOS PARA O LCD
  lcd.createChar(0, BACK);	
  lcd.createChar(1, CIMA);
  lcd.createChar(2, BAIXO);
  lcd.createChar(3, ENTER);
  lcd.createChar(4, STOP);
  lcd.createChar(5, CHECK);
  lcd.createChar(6, INC);
  lcd.createChar(7, C_INC);
  
}

// ---------- IMPORANTE ---------- //
//Os testes podem ser interrompidos em qualquer tela se pressionar o botão respectivo indicado em cada tela por 3 segundos
//Ou pelo menos era pra funcionar assim kkkkkkk
//Se não ocorrer, é pq o código pode estar precisando de uma revisão



void loop()
{
  lcd.setCursor(0, 0); //lcd.setCursor(coluna,linha)
  
  switch (tela){
   case 1:
    testType(); 	//TELA PRINCIPAL
    break;
   
   case 2:
    inicialTest();	//INICIANDO O TESTE
    break;
    
   case 3:
    simpleTest();	//TESTE SIMPLES
    break;
   
   case 4:
    completeTest();	//TESTE COMPLETO
    break;
   
   case 5:
    interromper();	//DESEJA INTERROMPER O TESTE
    break;
  }
}


//████████████████||                                 ||████████████████
//████████████████|| --- TELA 1 - TELA PRINCIPAL --- ||████████████████
//████████████████||                                 ||████████████████
void testType(){
  
  //mostra os caracteres na segunda linha do LCD, posicionados acima da posição dos botões
  lcd.setCursor(2,1);
  lcd.write(byte(0));
  lcd.setCursor(6,1);
  lcd.write(byte(1));
  lcd.setCursor(9,1);
  lcd.write(byte(2));
  lcd.setCursor(13,1);
  lcd.write(byte(3));
  
  //retorna o cursor para o canto superior esquerdo da tela
  lcd.setCursor(0,0);
  
  while(digitalRead(B2)==1 || digitalRead(B3)==1){
    pressButton = 1;
  }
  
  //se qualquer um dos botões B2 ou B3 for pressionado o tipo do teste muda
  if(pressButton == 1){
    if(typeTest==0){
    	typeTest = 1;
    }else{
      	typeTest = 0;
    }
    pressButton = 0;
  }
  
  //mostra o tipo de teste selecionado
  if(typeTest == 0){
  	lcd.print("     SIMPLES    ");
  }else {
    lcd.print("    COMPLETO    ");
  }
  
  //Toda vez que aparecer um "while" é para impedir que a pessoa selecione algo errado por ficar segurando o botão apertado
  while(digitalRead(B4)==1){
   	pressButton = 2; 
  }
  
  //se apertar o botão B4, vai para a tela 2
  if(pressButton == 2){
    tela = 2;
    pressButton = 0;
  }
}


//████████████████||                                    ||████████████████
//████████████████|| --- TELA 2 - INICIANDO O TESTE --- ||████████████████
//████████████████||                                    ||████████████████
void inicialTest(){
  //Espera 2 segundos nessa tela para prosseguir
  if(pressTime == 0){
    pressTime = millis();
  }else{
    if((millis()-pressTime)>=2000){
      //Vai para a tela do teste selecionado
      if(typeTest==0){
        tela=3; //teste simples
      }else{
        tela=4; //teste completo
        stage=1;
      }
      pressTime=0;
      numIMAS=2;  //no teste simples, assumimos que teremos 2 pulsos por rotação
      lcd.clear();
      return;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("  INICIANDO O   ");
  lcd.setCursor(0,1);
  if(typeTest == 0){
  	lcd.print("  TESTE SIMPLES ");
  }else{
    lcd.print(" TESTE COMPLETO ");
  }
}

//████████████████||                                ||████████████████
//████████████████|| --- TELA 3 - TESTE SIMPLES --- ||████████████████
//████████████████||                                ||████████████████
void simpleTest(){
  //monitora o valor de RPM atual apenas
  lcd.setCursor(0,0);
  lcd.print("RPM:  ");
  lcd.print(atualRPM);
  
  //Deixa o botão B4 habilitado para PARAR o teste
  lcd.setCursor(13,1);
  lcd.write(byte(4));
  
  if(digitalRead(B4)==1){
    pressButton = 1;
  }else{
    pressButton = 0;
  }
  
  //Conta quanto tempo o botão B4 está senso pressionado, reseta se soltar o botão
  if(pressButton == 1){
    if(pressTime == 0){
      pressTime = millis();
    }else if((millis() - pressTime)>=3000){ //interrompe o teste se pressionado por 3 segundos
      lastTela = tela;
      tela = 5;
      lcd.clear();
      pressButton=0;
    }
  }else{
    pressTime = 0;
  }
  //atualiza o valor de RPM - ESSA PARTE É SÓ PARA TESTE E TER UM VALOR SE ALTERANDO NA TELA - APENAS PARA VISUALIZAÇÃO
  //No teste real os valors serão coletados através do sensor de efeito Hall
  atualRPM = valueRPM;
  valueRPM++;
}


//████████████████||                                 ||████████████████
//████████████████|| --- TELA 4 - TESTE COMPLETO --- ||████████████████
//████████████████||                                 ||████████████████
void completeTest(){
  
  	//impede que o usuário selecione algo errado por manter o botão pressionado
    while(digitalRead(B3)==1 && stage<5){
    	pressButton = 1;
    }
    
    while(digitalRead(B2)==1 && stage<5){
        pressButton = 2;
    }
  
  	while(digitalRead(B4)==1 && stage<5){
        pressButton = 4;
    }
    
    while(digitalRead(B1)==1 && stage<5){//BACK OU EXIT
        //Se o botão B1 for pressionado por 3 segundos, então vamos para a tela de "interromper o teste"
      	pressButton = 3;
        if(pressTime==0){
          pressTime=millis();
        }else if((millis()-pressTime)>=3000){
          lastTela=tela;
          tela=5;
          pressTime=0;
          return;
        }
    }
    
    
    //retorna para a tela anterior se o botão B1 for pressionado
    if(pressButton==3 && stage<4){
      if(stage>1){
        stage--;
        lcd.clear();
      }else{
        stage=1;
      }
      pressTime=0;
    }
    
    //vai para a próxima tela se o botão B4 for pressionado
    if(pressButton==4 && stage<4){
      stage++;
      lcd.clear();
    }
  
  if(stage<4){
    //mostra os caracteres na segunda linha do LCD, posicionados acima da posição dos botões
  	lcd.setCursor(2,1);
  	lcd.write(byte(0)); //retorna ou interrompe o teste
  	lcd.setCursor(6,1);
  	lcd.write(byte(7)); //muda o tipo de incremento
  	lcd.setCursor(9,1); 
  	lcd.write(byte(6)); //incrementa
  	lcd.setCursor(13,1);
  	lcd.write(byte(3)); //próxima tela
  }
  
  //Se não estiver realizando os teste, então deve ser "setados" alguns valores iniciais
  switch (stage){
   
   case 1: //███████████████ - SUB TELA 4.1 - Informar o valor de RPM Nominal do motor que está sendo testado
    
    //Se estiver retornando de alguma tela posterior, verifica se as unidade de incremento estão corretas
    if(textInc=='s' || textInc=='m' || textInc=='h'){
      incremento=1;
      textInc='U';
    }
      
    //Mostra as informações atuais do sistema
    lcd.setCursor(0,0);
    lcd.print("RPM Nom.: ");
    lcd.print(nominalRPM);
    lcd.setCursor(15,0);
    lcd.print(textInc);
      
     
    //Incrementa o valor de RPM nominal de acordo com a unidade de incremento: unidade, dezena, centena ou milhar (U,D,C ou M)
    //Para facilitar o incremento, pode-se incrementar cada casa deciamal separadamente mudando o valor da unidade do incremento
    if(pressButton == 1){
      if((nominalRPM%(incremento*10))<(9*incremento)){
        nominalRPM = nominalRPM + incremento;
      }else{
        nominalRPM = nominalRPM-9*incremento;
        if(incremento>=10){
          lcd.setCursor(11,0);
          lcd.print("   ");
        }    
      }
    }
      
    //Muda o valor da unidade do incremento e o valor que será incrementado
    if(pressButton == 2){
      if(incremento==1){
        incremento=10;
        textInc='D'; 	//DEZENA -- AUMENTA O VALOR EM 10 UNIDADES CADA VEZ QUE INCREMENTAR
      }else if(incremento==10){
        incremento=100;
        textInc='C';	//CENTENA -- AUMENTA O VALOR EM 100 UNIDADES CADA VEZ QUE INCREMENTAR
      }else if(incremento==100){
        incremento=1000;
        textInc='M';	//MILHAR -- AUMENTA O VALOR EM 1000 UNIDADES CADA VEZ QUE INCREMENTAR
      }else{
        incremento=1;
        textInc='U';	//UNIDADE -- AUMENTA O VALOR EM 1 UNIDADE CADA VEZ QUE INCREMENTAR
      }
    }
    
    pressButton = 0;
    break;
    
    
   case 2://███████████████ - SUB TELA 4.2 - Informar o valor de tempo de duração do teste
    
    //Se estiver retornando de alguma tela posterior, verifica se as unidade de incremento estão corretas
    if(textInc=='U' || textInc=='D' || textInc=='C' ||textInc=='M'){
      incremento=1;
      textInc='s';
    }
      
    //Mostra as informações atuais do sistema
    lcd.setCursor(0,0);
    lcd.print("Time: ");
    lcd.print(testTime/3600);
    lcd.print("h");
	lcd.print((testTime%3600)/60);
    lcd.print("m");
    lcd.print((testTime%3600)%60);
    lcd.setCursor(15,0);
    lcd.print(textInc);
      
     
    //Incrementa o valor de tempo de acordo com a unidade de incremento: segundo, minuto ou horas (s, m ou h)
    //Para facilitar o incremento, pode-se incrementar cada parte do tempo de acordo com a unidade selecionada do incremento
    //O TEMPO MÁXIMO É: 9h59m59s --> 35999 segundos
    if(pressButton == 1){
      if(textInc=='s'){
        if((testTime%60)<59){
          testTime = testTime + incremento;
        }else{
          testTime = testTime - 59;
        }
      }else if(textInc=='m'){
        if(((testTime%3600)/60)<59){
          testTime = testTime + incremento;
        }else{
          testTime = testTime - 59*incremento;
        }
      }else{
        if((testTime/3600)<9){
          testTime = testTime + incremento;
        }else{
          testTime = testTime - 9*incremento;
       }
      }
      lcd.setCursor(11,0);
      lcd.print("   ");
    }
    
    //Muda o valor da unidade do incremento e o valor que será incrementado
    if(pressButton == 2){
      if(incremento==1){
        incremento=60;
        textInc='m';	//MINUTO -- AUMENTA O VALOR EM 60 SEGUNDOS CADA VEZ QUE INCREMENTAR 
      }else if(incremento==60){
        incremento=3600;
        textInc='h';	//HORA -- AUMENTA O VALOR EM 3600 SEGUNDOS CADA VEZ QUE INCREMENTAR
      }else{
        incremento=1;
        textInc='s';	//SEGUNDO -- AUMENTA O VALOR EM 1 SEGUNDO CADA VEZ QUE INCREMENTAR
      }
    }
    pressButton = 0;
    break;
    
    
   case 3: //███████████████ - SUB TELA 4.3 - Informar o número de pulsos por rotação que o sensor vai ler
    //Mostra as informações atuais do sistema
    lcd.setCursor(0,0);
    lcd.print("Pulsos/rot.:  ");
    lcd.print(numIMAS);
    lcd.print("  ");
    
    if(pressButton==1){
      if(numIMAS<9){
        numIMAS++;
      }else{
        numIMAS = 0;
      }
    }
    pressButton = 0;
    break;
   
   case 4: //███████████████ - SUB TELA 4.4 - TELA DE TRANSIÇÃO
    lcd.setCursor(0,0);
  	lcd.print("CARREGANDO AS   ");
  	lcd.setCursor(0,1);
  	lcd.print("     INFORMACOES");
    //Espera 2 segundos nessa tela para prosseguir
  	if(pressTime == 0){
    	pressTime = millis();
  	}else if((millis()-pressTime)>=2000){
        stage=5;
      	lcd.clear();
      	pressTime=0;
    }
    break;
   
   
   case 5: //███████████████ - SUB TELA 4.5 - MOSTRA OS VALORES QUE ESTÃO SENDO MEDIDOS NA TELA
    lcd.setCursor(0,0);
  	lcd.print("RPM:");
  	lcd.print(atualRPM); //Informa o RPM atual na tela
    
    lcd.setCursor(2,1);
  	lcd.write(byte(4)); //EXIT
  	lcd.setCursor(4,1);
  	lcd.print("EXIT"); 
    
    if(digitalRead(B1)==1){ //Habilita o botão B1 para interromper o teste
    	pressButton = 3;
  	}else{
    	pressButton = 0;
  	}
  
  	//Conta quanto tempo o botão B1 está senso pressionado, reseta se soltar o botão
  	if(pressButton == 3){
    	if(pressTime == 0){
      		pressTime = millis();
    	}else if((millis() - pressTime)>=3000){ //interrompe o teste se pressionado por 3 segundos
      		lastTela = tela;
      		tela = 5;
      		lcd.clear();
          	pressTime=0;
    	}
  	}else{
    	pressTime = 0;
  	}
    
    atualRPM = valueRPM; //atualiza o valor do RPM atual
    valueRPM++;
    
    if(atualRPM > maxRPM){ //Armazena o valor de RPM Máximo do teste
      maxRPM = atualRPM;
    }
    
    if(startTime==0){
      startTime=millis();
    }
    if(testTime < ((millis()-startTime)/1000)){
      stage = 6;
      lcd.clear();
    }
    break;
   
   case 6: //███████████████ - SUB TELA 4.6 - TELA DE TRANSIÇÃO
    lcd.setCursor(0,0);
  	lcd.print("  FINALIZANDO   ");
  	lcd.setCursor(0,1);
  	lcd.print("      TESTE     ");
    
    //Espera 2 segundos nessa tela para prosseguir
  	if(pressTime == 0){
    	pressTime = millis();
  	}else if((millis()-pressTime)>=2000){
        stage=7;
      	lcd.clear();
      	pressTime=0;
      	startTime = 0;
    }
    break;
    
   case 7://███████████████ - SUB TELA 4.7 - TELA DE RESULTADOS
    
    if(digitalRead(B1)==1){ //Botão B1 pressionadoa (EXIT)
      pressButton = 3;
      if(startTime==0){
        startTime=millis();
      }
    }
    
    if(digitalRead(B3)==1){//Botão B3 pressionadoa (AGAIN)
      pressButton = 1;
      if(startTime==0){
        startTime=millis();
      }
    }
    
    if(pressButton==3 && startTime!=0){ //Se o botão B1 for pressionado por 3 segundos, vai para tela de "interromper"
      if((millis()-startTime)>=3000){
        lastTela = tela;
        tela = 5;
      	lcd.clear();
        pressTime=0;
        startTime=0;
      }
    }else if(pressButton==1 && startTime!=0){ //Se o botão B3 for pressionado por 3 segundos, vai para tela de "novo teste?"
      if((millis()-startTime)>=3000){
        stage=8;
      	lcd.clear();
        pressTime=0;
        startTime=0;
      }
    }else{
      startTime=0;
      pressButton=0;
    }
          
    //Espera 2 segundos em cada tela para mostrar os resultados do teste
  	if(pressTime == 0){
    	pressTime = millis();
  	}else if((millis()-pressTime)<=2000){ //Mostra o valor de RPM nominal informado no inicio do teste
      lcd.setCursor(0,0);
      lcd.print("RPM Nom.: ");
      lcd.print(nominalRPM);
    }else if((millis()-pressTime)<=4000){ //Mostra o valor do RPM máximo medido no teste
      lcd.setCursor(0,0);
      lcd.print("RPM Max.: ");
      lcd.print(maxRPM);
    }else{
      lcd.clear();
      pressTime = 0;
    }
    //mostra os caracteres na segunda linha do LCD, posicionados acima da posição dos botões
    lcd.setCursor(2,1);
    lcd.write(byte(4)); //EXIT
    lcd.setCursor(4,1);
    lcd.print("EXIT");
    lcd.setCursor(9,1);
    lcd.write(byte(3)); //AGAIN
    lcd.setCursor(11,1);
    lcd.print("AGAIN");
    break;
    
   case 8: //███████████████ - SUB TELA 4.8 - TELA "NOVO TESTE?"
    lcd.setCursor(0,0);
    lcd.print("  NOVO  TESTE?  ");
  
    //mostra os caracteres na segunda linha do LCD, posicionados acima da posição dos botões
    lcd.setCursor(2,1);
    lcd.write(byte(5)); //OK
    lcd.setCursor(3,1);
    lcd.print("OK   ");
    lcd.setCursor(9,1);
    lcd.write(byte(4)); //CANCEL
    lcd.setCursor(10,1);
    lcd.print("CANCEL");
  
    //Caso a pessoa chegue nessa tela segurando o botão B3 ainda, esta parte impede que ela selecione algo errado
    //Essa condição só ocorre quando a pessoa tenta interromper o TESTE COMPLETO, pois o botão de interromper o TESTE SIMPLES é o B4
    while(digitalRead(B3)==1 && pressButton==1){
    }
    
    pressButton = 0;
  
    while(digitalRead(B1)==1 ){
      pressButton = 1;
    }
  
    while(digitalRead(B3)==1){
      pressButton = 2;
    }
  
    if(pressButton == 1){//OK --> Retorna para o inicio do teste completo com os mesmos parâmetros
  	  stage = 4;
      pressTime = 0;
      startTime = 0;
      maxRPM = 0;
      atualRPM = 0;
      valueRPM = 0;
      pressButton = 0;
      lcd.clear();  
    }else if(pressButton == 2){ //CANCEL --> Retorna para a tela anterior
      stage=7;
      pressButton = 0;
      lcd.clear();
    }else{
      pressButton = 0;
    }
    pressTime = 0;
    break;
  }
}
  


//████████████████||                                    ||████████████████
//████████████████|| --- TELA 5 - INTERROMPER TESTE --- ||████████████████
//████████████████||                                    ||████████████████
void interromper(){
  lcd.setCursor(0,0);
  lcd.print(" PARAR O TESTE? ");
  
  //mostra os caracteres na segunda linha do LCD, posicionados acima da posição dos botões
  lcd.setCursor(2,1);
  lcd.write(byte(5));
  lcd.setCursor(3,1);
  lcd.print("OK   ");
  lcd.setCursor(9,1);
  lcd.write(byte(4));
  lcd.setCursor(10,1);
  lcd.print("CANCEL");
  
  //Caso a pessoa chegue nessa tela segurando o botão B1 ainda, esta parte impede que ela selecione algo errado
  //Essa condição só ocorre quando a pessoa tenta interromper o TESTE COMPLETO, pois o botão de interromper o TESTE SIMPLES é o B4
  while(lastTela>=4 && digitalRead(B1)==1 && pressButton==3){
  }
  
  while(digitalRead(B1)==1 ){
    pressButton = 1;
  }
  
  while(digitalRead(B3)==1){
    pressButton = 2;
  }
  
  
  if(pressButton == 1){//OK --> Reseta os valores e retorna para a TELA PRINCIPAL
  	tela = 1;
    nominalRPM = 0;
    pressTime = 0;
    startTime = 0;
    testTime = 0;
    valueRPM = 0;
    maxRPM = 0;
    atualRPM = 0;
    pressButton = 0;
    stage = 1;
    lcd.clear();  
  }else if(pressButton == 2){ //CANCEL --> Retorna para a tela anterior
    tela = lastTela;
    pressButton = 0;
    lcd.clear();
  }else{
    pressButton = 0;
  }
  pressTime=0;
}