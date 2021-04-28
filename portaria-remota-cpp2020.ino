#include <SPI.h>
#include <Ethernet.h>
#define PANICO 6
#define RELE 7
#define BOTAO 8
#define BUZZER 9
#define LED_VERDE 14
#define LED_VERMELHO 15

// Foram utilizadas as saidas analógicas 14 e 15 

// Endereço MAC, IP para o server side e IP da câmera para o client side
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 10, 0, 0, 50 };
char cam[] = "10.0.0.70"; 

// Biblioteca para server side
EthernetServer server(80); // Servidor "escutando" na porta 80

// Biblioteca para client side
EthernetClient clientside; 

/*
 *O arduino além de mostrar a página Web para o cliente (server-sider) 
 *também faz requisições HTTP para a câmera (client-side)
*/

String readString; // String auxiliar para "ler" o link da página
  
void setup(){

  pinMode(PANICO, INPUT); // HIGH = Ok e LOW = Panico, caso panico estiver em LOW o circuito não irá funcionar
  pinMode(RELE, OUTPUT); 
  pinMode(BOTAO, INPUT); 
  pinMode(BUZZER, OUTPUT); 
  pinMode(LED_VERDE, OUTPUT); 
  pinMode(LED_VERMELHO, OUTPUT);
  
  Ethernet.begin(mac, ip);
  server.begin();
  
}
  
void loop(){

  // Esse led só ficará aceso enquanto o panico for LOW
  // A cada loop ele é retornado para LOW
  digitalWrite(LED_VERMELHO, LOW); 
         
  // Verifica se o botão foi pressionado e se o panico está desligado
  // Aciona o relé, pisca o led verde e envia aviso sonoro
  if(digitalRead(BOTAO) == HIGH && digitalRead(PANICO) == HIGH){
 
    ligaRele();
    piscaLed();
    beepBuzzer();
    beepBuzzer();
    desligaRele();
                               
  } 

  // Se panico for LOW acende led vermelho pra informar
  if(digitalRead(PANICO) == LOW){

    digitalWrite(LED_VERMELHO, HIGH);             
                
  }
  
  // Imprime a página web para o cliente
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
  
        if (readString.length() < 100) {
          readString += c;             
        }
 
        if (c == '\n') {           
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println();
           
          client.println(F("<html lang=\"pt-br\">"));
          client.println(F("<head>"));
          client.println(F("<meta charset=\"utf-8\">")); 
          client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">"));
          client.println(F("<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/gh/christianalexandre/cdncss@2020/cpp.css\"><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\">"));
          client.println(F("<title>Unisul - Programação C++</title>"));
          client.println(F("</head>"));
          client.println(F("<body style=\"background-color:#595959;\">"));                 
          client.println(F("<nav class=\"navbar navbar-expand-md bg-dark navbar-dark\" data-toggle=\"collapse\" data-target=\"#collapse1\">")); 
          client.println(F("<div class=\"container-fluid\"><div class=\"col-2\"><h4 class=\"text-white\">Unisul</h4></div>")); 
          client.println(F("<div class=\"col-8\"><h4 class=\"text-white text-center\">Trabalho 1 - Programação C++</h4></div>")); 
          client.println(F("<div class=\"col-2\"><h6 class=\"text-white text-center\">Christian Alexandre<br>Gustavo Prim</h6></div></div>")); 
          client.println(F("</nav>"));
          client.println(F("<div class=\"container-fluid\">")); 
          client.println(F("<div class=\"row\"><div id=\"ajusteEsquerda\"></div>")); 
          client.println(F("<div class=\"col-8\" style=\"padding-top: 50px;\"><iframe id=\"ajusteIframe\" class=\"text-center border border-dark rounded\" width=\"1282\" height=\"722\" src=\"http://10.0.0.70/cgi-bin/mjpg/video.cgi\"></iframe></div>")); 
          client.println(F("<div class=\"col-3\"><div id=\"ajusteDireita\" class=\"container bg-dark rounded\">"));     

          // Menu porta
          client.println(F("<div class=\"container-fluid text-center text-white rounded border border-white\" style=\"border-color: white; padding-top: 5px; padding-bottom: 10px;\">"));
          client.println(F("<h4 style=\"padding-bottom:5px;\">Portão</h4>"));      
          client.println(F("<a href=\"/?pulsoPortao\"><button id=\"ajusteAncora\" type=\"button\" class=\"btn btn-light\">Enviar pulso</button></a>"));
          client.println(F("</div>")); 
          // Fim do menu porta
          
          client.println(F("<div style=\"height:15px;\"></div>")); 

          // Menu modo de vídeo
          client.println(F("<div class=\"container-fluid text-center text-white rounded border border-white\" style=\"border-color: white; padding-top: 5px; padding-bottom: 10px;\">"));
          client.println(F("<h4 style=\"padding-bottom:5px;\">Modo de vídeo</h4>"));
          client.println(F("<div class=\"row\"><div class=\"col\"><a href=\"/?irOff\"><button id=\"ajusteAncora\" type=\"button\" class=\"btn btn-light\">IR off</button></a></div>"));
          client.println(F("<div class=\"col\"><a href=\"/?irAuto\"><button id=\"ajusteAncora\" type=\"button\" class=\"btn btn-light\">Auto</button></a></div>"));
          client.println(F("<div class=\"col\"><a href=\"/?irOn\"><button id=\"ajusteAncora\" type=\"button\" class=\"btn btn-light\">IR on</button></a></div>"));
          client.println(F("</div></div>"));                                                   
          // Fim menu modo de vídeo
  
          client.println(F("<div style=\"height:15px;\"></div>")); 
          
          // Menu padrão de vídeo
          client.println(F("<div class=\"container-fluid text-center text-white rounded border border-white\" style=\"border-color: white; padding-top: 5px; padding-bottom: 10px;\">"));
          client.println(F("<h4 style=\"padding-bottom:5px;\">Padrão de vídeo</h4>"));
          client.println(F("<div class=\"row\"><div class=\"col\"><a href=\"/?videoPadrao\"><button id=\"ajusteAncora\" type=\"button\" class=\"btn btn-light\">Padrão</button></a></div>"));
          client.println(F("<div class=\"col\"><a href=\"/?videoSuave\"><button id=\"ajusteAncora\" type=\"button\" class=\"btn btn-light\">Suave</button></a></div>"));
          client.println(F("<div class=\"col\"><a href=\"/?videoBrilho\"><button id=\"ajusteAncora\" type=\"button\" class=\"btn btn-light\">Brilho</button></a></div>"));
          client.println(F("</div></div>"));
          // Fim do menu padrão de vídeo
          
          client.println(F("</div>")); 
          client.println(F("</div>"));  
          client.println(F("</div>"));     
          client.println(F("</div>"));  

          // Barra abaixo da câmera
          client.println(F("<div class=\"container-fluid\"><div class=\"row\"><div id=\"ajusteEsquerda\"></div>"));  
          client.println(F("<div id=\"ajusteAbaixo\" class=\"col-8 container bg-dark rounded\" style=\"margin-top: 10px;\">"));
          
          client.println(F("<div class=\"row\"><div class=\"col-1\"><a href=\"http://admin:admin@10.0.0.70/cgi-bin/snapshot.cgi\" target=\"_blank\"><button type=\"button\" class=\"btn btn-light\">Foto</button></a></div>"));
          client.println(F("<div class=\"col-1\"><a href=\"/?turnHd\"><button type=\"button\" class=\"btn btn-light\" style=\"margin-left:10px;\">HD</button></a></div>"));
          client.println(F("<div class=\"col-1\"><a href=\"/?turnD1\"><button type=\"button\" class=\"btn btn-light\" style=\"margin-left:10px;\">D1</button></a></div></div></div>"));          
          client.println(F("<div class=\"col-3\"></div></div></div>"));     
                                 
          client.println(F("</body>"));
          client.println(F("</body>"));
          client.println(F("</html>"));
          // Fim da página web
           
          delay(1);
          client.stop();

/*
              ==============================================================================================================
*/

          // Após imprimir a página html checa as condições                  
          
          // Verifica se o link e o panico está desativado
          // Aciona o relé e pisca o led verde
          if(readString.indexOf("?pulsoPortao") > 0 && digitalRead(PANICO) == HIGH){
            
            ligaRele();
            piscaLed();
            piscaLed();
            beepBuzzer();            
            desligaRele();
    
          }

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e desliga o Infra vermelho
          if(readString.indexOf("?irOff") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&VideoInOptions[0].DayNightColor=0 HTTP/1.1"));
                httpRequest();
                     
              }                                     
          } 

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e seta em automatico o Infra vermelho
          if(readString.indexOf("?irAuto") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&VideoInOptions[0].DayNightColor=1 HTTP/1.1"));
                httpRequest();
                
                      
              }                                     
          } 

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e liga o Infra vermelhoo
          if(readString.indexOf("?irOn") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&VideoInOptions[0].DayNightColor=2 HTTP/1.1"));
                httpRequest();
                               
                }
          }

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e seta resolução em D1
          if(readString.indexOf("?turnD1") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&Encode[0].MainFormat[0].Video.Height=480&Encode[0].MainFormat[0].Video.Width=704&Encode[0].MainFormat[0].Video.BitRate=4096 HTTP/1.1"));
                httpRequest();
          
              }                                     
          }

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e seta resolução em HD
          if(readString.indexOf("?turnHd") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&Encode[0].MainFormat[0].Video.Height=720&Encode[0].MainFormat[0].Video.Width=1280&Encode[0].MainFormat[0].Video.BitRate=12000 HTTP/1.1"));
                httpRequest();
                        
              }                                     
          }

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e seta os padrões de vídeo no perfil padrão
          if(readString.indexOf("?videoPadrao") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&VideoColor[0][0].Brightness=50&VideoColor[0][0].Contrast=50&VideoColor[0][0].Hue=50&VideoColor[0][0].Saturation=50 HTTP/1.1"));
                httpRequest();
                       
              }                                     
          }

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e seta os padrões de vídeo no perfil suave
          if(readString.indexOf("?videoSuave") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&VideoColor[0][0].Brightness=55&VideoColor[0][0].Contrast=50&VideoColor[0][0].Hue=40&VideoColor[0][0].Saturation=40 HTTP/1.1"));
                httpRequest();
                        
              }                                     
          }

          // Verifica o link e panico desativado
          // Envia requisição para a câmera e seta os padrões de vídeo no perfil brilho
          if(readString.indexOf("?videoBrilho") > 0 && digitalRead(PANICO) == HIGH){

              clientside.stop(); // Fecha qualquer conexão aberta
            
              // Verifica se o cliente ta conectado
              if (clientside.connect(cam, 80)) {
                
                // Envia requisição HTTP
                clientside.println(F("GET /cgi-bin/configManager.cgi?action=setConfig&VideoColor[0][0].Brightness=70&VideoColor[0][0].Contrast=50&VideoColor[0][0].Hue=50&VideoColor[0][0].Saturation=60 HTTP/1.1"));
                httpRequest();
                         
              }                                     
          }
           
          
          readString=""; // Seta a string como vazia para um novo loop
        }
      }
    }
  }
}

void piscaLed(){
    
  digitalWrite(LED_VERDE, HIGH);
  delay(50);
  digitalWrite(LED_VERDE, LOW);     
  delay(50);
  digitalWrite(LED_VERDE, HIGH);
  delay(50);
  digitalWrite(LED_VERDE, LOW);
  delay(50);
  digitalWrite(LED_VERDE, HIGH);
  delay(50);
  digitalWrite(LED_VERDE, LOW);
  delay(50);    
  digitalWrite(LED_VERDE, HIGH);   
  delay(50);  
  digitalWrite(LED_VERDE, LOW);      
  delay(50);  
  digitalWrite(LED_VERDE, HIGH);
  delay(50);   
  digitalWrite(LED_VERDE, LOW);   

}

void beepBuzzer(){

  tone(BUZZER, 261);    
  delay(200);
  noTone(BUZZER);  
  
}

void ligaRele(){

  digitalWrite(RELE, HIGH);  
  
}

void desligaRele(){

  digitalWrite(RELE, LOW);  
  
}

void httpRequest(){

  clientside.println(F("Host: 10.0.0.70"));
  clientside.println(F("Authorization: Basic YWRtaW46YWRtaW4="));
  clientside.println(F("User-Agent: arduino-ethernet"));
  clientside.println(F("Connection: close"));     
  clientside.println();     
  
}
