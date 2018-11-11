tic
while(rederobo.BytesAvailable~=0)
    preambulo=0;
    while(preambulo~=35)
        preambulo = fread(rederobo, 1,'char');
    end
    
    letra=10;
    odometria1='';
    while(letra~=13)        
        letra=fread(rederobo, 1,'char');        
        if(letra~=13)            
            odometria1 = [odometria1 letra];            
        end
    end
    letra=fread(rederobo, 1,'char');
    
    odometria2='';
    while(letra~=13 )
        letra=fread(rederobo, 1,'char');
        if(letra~=13)
            odometria2 = [odometria2 letra];
        end
    end
    letra=fread(rederobo, 1,'char');
    
    odometria3='';
    while(letra~=13 )
        letra=fread(rederobo, 1,'char');
        if(letra~=13)
            odometria3 = [odometria3 letra];
            
        end
    end
    letra=fread(rederobo, 1,'char');
    
    odometria4='';
    while(letra~=13 )
        letra=fread(rederobo, 1,'char');
        if(letra~=13)
            odometria4 = [odometria4 letra];            
        end
    end
    letra=fread(rederobo, 1,'char');
    
    final=fread(rederobo, 1,'char');
    if(final==33)        
        odometriaDE=str2num(odometria1);
        odometriaDD=str2num(odometria2);
        odometriaTD=str2num(odometria3);
        odometriaTE=str2num(odometria4);
    end
end
toc

clear odometria1 odometria2 odometria3 odometria4 letra final preambulo