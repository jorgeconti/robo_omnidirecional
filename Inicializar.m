conectado=0;
if(exist('rederobo','var'))
    if(isequal(rederobo.status,'open'))
        disp('Conexão já está aberta');
        conectado=1;
    else
        fclose(rederobo);
        pause(0.2);
        echotcpip('off')
        pause(0.2);
        echotcpip('on',80)
        pause(0.2);
        fopen(rederobo);
        pause(0.2);
    end
else
    rederobo = tcpip('192.168.4.1',80);
    pause(0.2);
    fclose(rederobo);
    pause(0.2);
    echotcpip('off')
    pause(0.2);
    echotcpip('on',80)
    pause(0.2);
    fopen(rederobo);
    pause(0.2);
    
end
if(conectado==0)
    while(rederobo.BytesAvailable~=0 && conectado~=40 )
        conectado = fread(rederobo, 1,'char');
    end
    if(conectado==40)
        disp('Conexão estabelecida com o ESP32 via WIFI');
        if(rederobo.BytesAvailable~=0)
            fread(rederobo, rederobo.BytesAvailable);%%limpar buffer
        end
    else
        disp('Erro de Conexão, Reinicie o Esp32 e faça login no Wifi "Robô-Omnidirecional"');
        
    end
end
clear conectado