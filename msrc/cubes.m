clf;
figure(1);
format compact 
%h(1) = axes('Position',[0.2 0.2 0.6 0.6]);
vert = [1 1 -1; 
        -1 1 -1; 
        -1 1 1; 
        1 1 1; 
        -1 -1 1;
        1 -1 1; 
        1 -1 -1;
        -1 -1 -1];

fac = [1 2 3 4; 
       4 3 5 6; 
       6 7 8 5; 
       1 2 8 7; 
       6 7 1 4; 
       2 3 5 8];
  
% I defined a new cube whose length is 1 and centers at the origin.
vert2 = vert * 0.5 + ones(8,1) *[-1, -1, -1];  
fac2 = fac;

patch('Faces',fac,'Vertices',vert,'FaceColor','b');  % patch function
axis(2*[-1, 1, -1, 1, -1, 1]);
axis equal;


hold on;

patch('Faces', fac2, 'Vertices', vert2, 'FaceColor', 'k');
material metal;
%alpha('color');
alpha(0.5);
alphamap('rampdown');
view(3);