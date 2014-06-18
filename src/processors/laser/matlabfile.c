#include <matlabfile.h>


FILE *pFileMatlab;

void cabezera_matlab(const char* directorio, long timestamp, long timestamp_old, const char* time, const char* archivo, const char* archivo_old,float temprx,float  tempry,float  tempth) {
    /*
         Cabezera Matlab
         
     */
    char filename_matlab[100];

    sprintf(filename_matlab, "mat_%s_%06ld.m", time, timestamp);
    printf("Archivo Matlab: %s", filename_matlab);
    pFileMatlab = fopen(filename_matlab, "w");

    fprintf(pFileMatlab, "if not(exist('poses'))\n poses= [];\nend\n");
    fprintf(pFileMatlab, "filename = '%s';\n", archivo_old);
    fprintf(pFileMatlab, "filename = unix_to_win(filename);\n");
    fprintf(pFileMatlab, "p%s_%06ld=leer_archivo(filename);\n", time, timestamp_old);
    fprintf(pFileMatlab, "pose=[%f,%f,%f];\n", temprx, tempry, tempth);
    fprintf(pFileMatlab, "poses = [poses;pose];\n");
    fprintf(pFileMatlab, "p%s_%06ld = transform_to_global(p%s_%06ld,pose);\n", time, timestamp_old, time, timestamp_old);

    //fprintf(pFileMatlab, "% Primer Archivo:%s \n", archivo_old);

    fprintf(pFileMatlab, "if not(exist('h'))\n");
    fprintf(pFileMatlab, "      figure, axis equal, grid, hold on;\n");
    fprintf(pFileMatlab, "      h = plot(0,0,'r','erasemode','xor');\n");
    fprintf(pFileMatlab, "      set(h(1),'xdata',p%s_%06ld(1,:),'ydata',p%s_%06ld(2,:),'color','green')\n", time, timestamp_old, time, timestamp_old);
    fprintf(pFileMatlab, "else\n");
    fprintf(pFileMatlab, "h= [h; plot(0,0,'.','xdata',p%s_%06ld(1,:),'ydata',p%s_%06ld(2,:),'color','red','displayname','Ref_%s_%06ld','marker','o','markersize',4)]\n", time, timestamp_old, time, timestamp_old, time, timestamp_old);
    fprintf(pFileMatlab, "end\n");
    /**********************************/
}
void final_matlab(
        int timestamp,
        const char* archivo,
        const char* time,
        PMScan *segundo,
        float x,
        float y,float temprx,float  tempry,float  tempth) {

    fprintf(pFileMatlab, "filename = '%s';\n", archivo);
    fprintf(pFileMatlab, "filename = unix_to_win(filename);\n");
    fprintf(pFileMatlab, "p%s_%06d=leer_archivo(filename);\n", time, timestamp);


    // printf("pose2=[%f,%f,%f]+ultimapose2\n", primero->rx, primero->ry, primero->th);
    //printf("pose=[%f,%f,%f]+ultimapose\n", primero->rx, primero->ry, primero->th);


    fprintf(pFileMatlab, "if not(exist('movimientos'))\n movimientos= [];\nend\n");
    fprintf(pFileMatlab, "if not(exist('movimientos_raw'))\n movimientos_raw= [];\nend\n");

    fprintf(pFileMatlab, "movimiento_raw=[%f,%f,%f];\n", x, y, segundo->th);
    fprintf(pFileMatlab, "movimientos_raw = [movimientos_raw;movimiento_raw]\n");

    fprintf(pFileMatlab, "movimiento=[%f,%f,%f];\n", segundo->rx, segundo->ry, segundo->th);
    fprintf(pFileMatlab, "movimientos = [movimientos;movimiento]\n");
    fprintf(pFileMatlab, "pose=[%f,%f,%f];\n", segundo->rx + temprx, segundo->ry + tempry, segundo->th + tempth);
    //fprintf(pFileMatlab, "pose = transform_to_global(movimiento,pose)+pose\n");
    fprintf(pFileMatlab, "poses = [poses;pose]\n");
    fprintf(pFileMatlab, "p%s_%06d = transform_to_global(p%s_%06d,pose);\n", time, timestamp, time, timestamp);
    fprintf(pFileMatlab, "h= [h; plot(0,0,'.','xdata',p%s_%06d(1,:),'ydata',p%s_%06d(2,:),'displayname','Pos_%s_%06d','marker','o','markersize',4)]\n", time, timestamp, time, timestamp, time, timestamp);

    //fprintf(pFileMatlab, "h= [h; plot(0,0,'.','xdata',poses(:,1),'ydata',poses(:,2),'displayname','pose','marker','.','markersize',1)];\n");
    fclose(pFileMatlab);
}
