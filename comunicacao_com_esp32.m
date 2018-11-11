Inicializar

% fprintf(rederobo, "x0.5");
% 
% pause(0.1);
% 
% odometria
% [odometriaDE odometriaDD;odometriaTE odometriaTD]
% 
% fprintf(rederobo, "x0a");
% 
% pause(0.1);
% 
% odometria
% [odometriaDE odometriaDD;odometriaTE odometriaTD]

% for i=10:1200
%     tic
%
%     %odometria = fread(rederobo, rederobo.BytesAvailable);
%     vel=floor(i/10);
%     data=["x"+num2str(vel)+"y"+num2str(vel)+"a"];
%     fprintf(rederobo, data);
%
%     while (toc < 0.1)
%     end
% end
%
% if(exist('rederobo','var'))
%     if(isequal(rederobo.status,'open'))
%         fprintf(rederobo, "x0y0d");
%         pause(1);
%     end
%     fclose(rederobo);
%     pause(1);
%     echotcpip('off')
%     pause(1);
% end

Encerrar
