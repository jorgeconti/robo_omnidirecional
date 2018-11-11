Inicializar

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%tempo de simula��o
t_sim=50;
%tempo de amostragem
Ts=0.1;
%quantidade de amostras da simula��o
fim=t_sim/Ts;
%inicializa vetor de odometrias
odometriaTodas=zeros(fim,4);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%% inicializa��o de vari�veis aqui
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

for i=1:fim
    tic    
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%%%%%%% C�digo de controle de trajeto aqui
    %%%%%%%%% Exemplo de envio de comando
    %data=["x"+num2str(velocidadeX)+"y"+num2str(velocidadeY)+"t"+num2str(velocidadeAngular)];
    %fprintf(rederobo, data);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    odometria
    odometriaTodas(i,:)=[odometriaDE odometriaDD odometriaTE odometriaTD];
    
    while (toc < Ts) %aguarda periodos de 100 millissegundos
    end
end

Encerrar
