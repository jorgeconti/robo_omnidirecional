fprintf(rederobo, "x0y0t0d");
pause(0.1);
conectado=1;
while(conectado~=41 && rederobo.BytesAvailable~=0)
    conectado = fread(rederobo, 1,'char');
end
if(conectado==41)
    disp('Conexão encerrada com sucesso');
    if(rederobo.BytesAvailable~=0)
        fread(rederobo, rederobo.BytesAvailable);%%limpar buffer
    end
else
    disp('Erro na desconexão');
end
fclose(rederobo);
pause(0.2);
echotcpip('off')
pause(0.2);
clear conectado

