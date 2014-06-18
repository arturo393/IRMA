sp=x1';
st=x2;

in4=ones(1,15);
for i=1:m,
  for j=1:m,    
    in4=[1,xx(1,i),yy(1,j),xx(1,i)^2,xx(1,i)*yy(1,j),yy(1,j)^2,xx(1,i)^3,xx(1,i)^2*yy(1,j),xx(1,i)*yy(1,j)^2,yy(1,j)^3,xx(1,i)^4,xx(1,i)^3*yy(1,j),xx(1,i)^2*yy(1,j)^2,xx(1,i)*yy(1,j)^3,yy(1,j)^4];
    in3=in4(1,1:10);
    in2=in3(1,1:6);
    in1=in2(1,1:3);
    
    // Matrices para grafica de modelo LSQ de cuarto orden
    modeloX4(i,j)=(in4*b4x);
    modeloY4(i,j)=(in4*b4y);
    modeloRot4(i,j)=(in4*b4rot);

    // Matrices para grafica de modelo LSQ de tercer orden
    modeloX3(i,j)=(in3*b3x);
    modeloY3(i,j)=(in3*b3y);
    modeloRot3(i,j)=(in3*b3rot);

    // Matrices para grafica de modelo LSQ de segundo orden
    modeloX2(i,j)=(in2*b2x);
    modeloY2(i,j)=(in2*b2y);
    modeloRot2(i,j)=(in2*b2rot);

    // Matrices para grafica de modelo LSQ de primer orden
    modeloX1(i,j)=(in1*bx);
    modeloY1(i,j)=(in1*by);
    modeloRot1(i,j)=(in1*brot);
  end
end

// Graficos de mediciones

// USO:
// plot3d(sp,st,[Medicion])

// Donde [medicion] puede ser:
//    VarX = Mediciones variacion real en el eje X
//    VarY = Mediciones variacion real en el eje Y
//    Rot = Mediciones variacion real de rotacion
plot3d(sp,st,Rot)
title('Mediciones tomadas');

xbasc(1);
scf(1)
// Graficos de modelos LSQ

// USO:
// plot3d(xx,yy,[LSQ])

// Donde [LSQ] puede ser:
//    modeloX4 = Modelo de cuarto orden de la variacion en X
//    modeloY3 = Modelo de tercer orden de la variacion en Y
//    modeloRot2 = Modelo de segundo orden de la variacion de rotacion
//    etc...

plot3d(xx, yy, modeloRot2)
title('Modelo LSQ cuadratico');

xbasc(2);
scf(2)
// Graficos de modelos SPLINES

// USO:
// plot3d(xx,yy,[SPL])

// Donde [SPL] puede ser:
//    zzX = Modelo de la variacion en X
//    zzY = Modelo de la variacion en Y
//    zzRot = Modelo de la variacion de rotacion
plot3d(xx, yy, zzRot)
title('Modelo Splines Bicubicas');



// Comandos graficos
// xbasr(N) abre una nueva ventana, cuyo numero es N
// plot3d(x,y,z) Grafico en 3d
// surf(x,y,z) grafica una superficie, mejor plot3d
// scf(N) selecciona la ventana N
// clf(N) limpia la ventana N

