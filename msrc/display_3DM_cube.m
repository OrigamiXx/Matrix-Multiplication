function ret = display_3DM_cube(c)

    clf;
    figure(1);
    format compact 
    %h(1) = axes('Position',[0.2 0.2 0.6 0.6]);
    vert = [1 1 0; 
            0 1 0; 
            0 1 1; 
            1 1 1; 
            0 0 1;
            1 0 1; 
            1 0 0;
            0 0 0];

    fac = [1 2 3 4; 
           4 3 5 6; 
           6 7 8 5; 
           1 2 8 7; 
           6 7 1 4; 
           2 3 5 8];
  
    dims = (size(c));
    s = dims(1);
    if s > 13
        s1 = 13;
    elseif s > 8
        s1 = 8;
    elseif s > 5
        s1 = 5;
    elseif s > 3
        s1 = 3;
    elseif s > 2
        s1 = 2;
    end
    
    s2 = s - s1;
        
    
    hold on;
    axis equal;
    
    for i = 1:s
        for j = 1:s
            for k = 1:s
                if c(i,j,k) > 0
                    offset = [i-1,j-1,k-1];
                    v = vert + ones(8,1) * offset;
                    f = fac;
                    patch('Faces',f,'Vertices',v,'FaceColor','m');
                    hold on; 
                end
            end
        end
    end
    
    patch('Faces',f,'Vertices',vert * (s + 0.08) + ones(8,3) * -0.04 ,'FaceColor','w');
    
    patch('Faces',f,'Vertices',vert * (s1 + 0.02) + ones(8,3) * -0.01,'FaceColor','b');
    patch('Faces',f,'Vertices',vert * (s2 + 0.02) + ones(8,3) *(s1-0.01), 'FaceColor','c');
    
    material metal;
    alpha(0.5);
    alphamap('rampdown');
    view(3);
    xlabel('\pi_1');
    ylabel('\pi_2');
    zlabel('\pi_3');
    rotate3d();
end