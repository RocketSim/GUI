#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <grx20.h>
#include <string.h>
char* tit[7];  /* titles*/
float* dat[6]; /* data */
int vars,line; /* global for ease of access */ 
struct Screen_Dim{
       int x;
       int y;       
}size;
struct _GR_mouseEvent mouse;
void line_drawer( float* x, float* y, int color);void GRX_int(); void graph_key();
int var_stat[4] = {1,1,1,1};
void reading_file();
int updater();
int min_finder(float* data);int max_finder(float* data);
void draw_graph();
int main(){
    int close=0;
    reading_file();
    GRX_int();
    while(close==0 )close = updater();
    return 0;
}
void GRX_int(){
     int i; char buf[BUFSIZ],buf1[BUFSIZ],buf2[BUFSIZ];
     int min,max,x,y;
     size.x = 0;
     size.y = 0;
     sprintf(buf, "%s", tit[0]);
    /************************* Initializing Graphics **************************/          
    GrSetMode( GR_biggest_graphics );
    GrClearScreen(GrWhite());
    GrSetWindowTitle(buf);
    size.x = GrScreenX();
    size.y = GrScreenY();
    GrMouseDisplayCursor();
    /********************** Drawing Axis and Grid and key *****************************/      
    draw_graph();
    graph_key();
    
     }
/**************************************************************** key *****************************************************************************/
void graph_key(){
    char step[100],start[100],end[100],buf[BUFSIZ]; int min,max,i;
    GrTextOption grt;
    grt.txo_font = &GrDefaultFont;
    grt.txo_fgcolor.v = GrBlack();
    grt.txo_bgcolor.v = GrWhite();
    grt.txo_direct = GR_TEXT_UP;
    grt.txo_xalign = GR_TEXT_UP;
    grt.txo_yalign = GR_TEXT_UP;
    grt.txo_chrtype = GR_BYTE_TEXT;


    
    for(i=1;i<vars+2;i++){
                        sprintf(step,"");sprintf(start,"");sprintf(end,""); /* clearing strings */
                        sprintf(buf, "%s", tit[i]);strcat(step, buf);strcat(start, buf);strcat(end, buf); /* adding the title */
                        if(i==1)GrTextXY(7*size.x/20 + 5,1*size.y*37/40 + (i-1)*size.y/7,start,GrBlack(),GrWhite()); /* x axis title */
                        if(i>1){
                                    grt.txo_fgcolor.v = i-1;
                                    grt.txo_bgcolor.v = GrWhite();
                                    GrDrawString( step ,strlen( step ),size.x/30 ,1*size.y/20 + (i-2)*size.y/4,&grt );/* y axis title */
                                }
                        strcat(step, " step: ");strcat(start," min : ");strcat(end," max : ");    
                        min = min_finder(dat[i-1]); max  = max_finder(dat[i-1]);sprintf(buf, "%d", (max-min)/10);strcat(step, buf);                    
                        sprintf(buf, "%d", min); strcat(start, buf); sprintf(buf, "%d", max); strcat(end, buf);      
                        GrTextXY(size.x*14/20 + 5,1*size.y/20 + (i-1)*size.y/7,start,GrBlack(),GrWhite()); /* printing */
                        GrTextXY(size.x*14/20 + 5,3*size.y/40 + (i-1)*size.y/7,end,GrBlack(),GrWhite());
                        GrTextXY(size.x*14/20 + 5,2*size.y/20 + (i-1)*size.y/7,step,GrBlack(),GrWhite());
                        GrHLine(size.x * 14/20+5, size.x, size.y* 1/20 + (i-1)*size.y/7, i-1);GrHLine(size.x * 14/20+10, size.x, size.y* 5/40+(i-1)*size.y/7, i-1);
                        }
    
    GrFilledBox(size.x*1/80,size.y * 1/160,size.x*2/20,size.y*3/80 ,7);  /* adding the buttons at the top of the window */
    GrTextXY(size.x*2/80 + 5,1*size.y/80 ," CLOSE ",GrBlack(),7); /* printing the close button */    
    for(i=0;i<vars;i++){
                        sprintf(buf, "TOGGLE");   
                        GrFilledBox(size.x*4/20 + size.x/80 + i*size.x*4/20,size.y * 1/160,size.x*6/20 + size.x/80 + i*size.x*4/20,size.y*3/80 ,7);          
                        GrTextXY(size.x*4/20 +size.x*1/20+ i*size.x*4/20,1*size.y/80 ,buf,GrBlack(),7); /* printing the toggle button */   
                        GrTextXY(size.x*3/20 +size.x*5/80+ i*size.x*4/20,2*size.y/80 ,tit[i+2],GrBlack(),7); /* printing the toggle button */    
     
     }
    }
/*********************************************** Plotting Data **********************************************************************/
void line_drawer(float* x, float* y, int color){
    float x_step,y_step, x0_pix,y0_pix,x1_pix,y1_pix;int xmin, ymin, i;
    x_step =(0.65*size.x)/ (max_finder(x)-min_finder(x)); y_step = (0.85*size.y)/(max_finder(y)-min_finder(y)) ;  /* what each pixel means */
    xmin = min_finder(x); ymin = min_finder(y);
    for(i = 2; i<line-3-vars; i++){ /* inverse of the function in updater */
          x0_pix = x_step * (x[i-1] - xmin) ; y0_pix = y_step * (y[i-1]- ymin);/* how many pixels are needed */
          x1_pix = x_step * (x[i] - xmin) ; y1_pix = y_step * (y[i]-ymin);
    GrLine((int)x0_pix + size.x/20,size.y*18/20 - (int)y0_pix,(int)x1_pix + size.x/20,size.y*18/20 - (int)y1_pix,color);  
    }
}
/**************************************************** Data Import *****************************************************************/
void reading_file(){
	 FILE* data;
     const char* tok; /* for reading the csv part of the .usm */ 
     char buffer [1000]; int i;	 int j;
     data = fopen("send_data.usm","r"); line = 0;
     while(fgets(buffer, 1000, data) != NULL){ /* when it = NULL it has reached the EOF */
     i=0;
         while( buffer[i] != 0 && buffer[i]!=10 && line<8 ){ /* Reading the title lines */
                if(line == 0)vars = atoi(buffer);
                if(line > 0 && line < vars + 3){
                        tit[line-1] = (char*) realloc(tit[line-1], (i+2)*sizeof(char));
                        tit[line-1][i] = buffer[i];                tit[line-1][i+1] =0;
                        }i++;    
               }
	    if(line > vars + 2){ /*csv reading bit */
		        j=0;
		        tok = strtok(buffer,",");
                while(tok != NULL){	
                          dat[j] = (float*) realloc(dat[j], (line-1-vars)*sizeof(float));
                          dat[j][line-3-vars] = atof(tok);
				          tok = strtok(NULL,",");j++;
			           }
	               }
	   line++;
   }
   fclose (data);
}
/************************************************************************************************************************************/
int updater(){
     int i,min,max,point; char  buf[BUFSIZ];
     GrMouseGetEvent(0, &mouse); /* gets mouse info */              
     GrFilledBox(size.x*15/20,size.y * 15/20,size.x,size.y ,GrWhite()); /* 'refeshes' the part of the screen */             
     if (mouse.buttons == 1 && mouse.x>size.x*1/80 && mouse.x<size.x*2/20 && mouse.y>size.y * 1/160 && mouse.y<size.y*3/80 )return 1; /* check if close button is pressed */
     
     if (mouse.buttons == 1 && mouse.x>size.x*17/80 && mouse.x<size.x*25/80 && mouse.y>size.y * 1/160 && mouse.y<size.y*3/80 ){
                       if(var_stat[0]==1)var_stat[0]=0;                        /* if first toggle pressed */
                       else var_stat[0]=1;
                       draw_graph();
                    
                       }
     if (mouse.buttons == 1 && mouse.x>size.x*33/80 && mouse.x<size.x*41/80 && mouse.y>size.y * 1/160 && mouse.y<size.y*3/80 ){
                       if(var_stat[1]==1)var_stat[1]=0;                        /* if 2nd toggle pressed */
                       else var_stat[1]=1;
                       draw_graph();
                       }
     if (mouse.buttons == 1 && mouse.x>size.x*49/80 && mouse.x<size.x*57/80 && mouse.y>size.y * 1/160 && mouse.y<size.y*3/80 ){
                       if(var_stat[2]==1)var_stat[2]=0;                        /* if 3rd toggle pressed */
                       else var_stat[2]=1;
                       draw_graph();
                       }
     if (mouse.buttons == 1 && mouse.x>size.x*65/80 && mouse.x<size.x*73/80 && mouse.y>size.y * 1/160 && mouse.y<size.y*3/80 ){
                       if(var_stat[3]==1)var_stat[3]=0;                        /* if 4th toggle pressed */
                       else var_stat[3]=1;
                       draw_graph();
                       }
     for(i=0;i<vars+1;i++){ /* turning pixel to value and printing */
                      min = min_finder(dat[i]); max = max_finder(dat[i]);
                      if(i==0)point = ((mouse.x - (size.x/20))/((0.65*size.x)/(max-min))) + min;
                      else point =(((size.y*18/20) - mouse.y)/((0.85*size.y)/ (max-min))) + min;
                      sprintf(buf, "%d  ", point); strcat(buf, tit[i+1]);
                      GrTextXY(size.x*14/20 + 10,size.y * 15/20 + i*size.y*1/40,buf,i,GrWhite());
                     }
     return GrKeyPressed(); /* if you hold down a button it should close the programme */
     }

void draw_graph(){
     int i;     
     i=0;
     GrFilledBox(size.x*14/20,size.y * 18/20,size.x/20,size.y/20 ,GrWhite());
     while( i<11){
                  GrHLine(size.x * 14/20, size.x * 1/20, size.y/20 + i*size.y*85/1000, 0);
                  GrVLine(size.x*14/20 - i*size.x*65/1000, size.y * 18/20 , size.y/20, 0);        
                  i++;     
                  }
                       
     for(i=1;i<vars+1;i++)if(var_stat[i-1]==1)line_drawer(dat[0],dat[i],i);
     }

/***************************Range Calculations*********************************/
int min_finder(float* data){  
	float min;int i;min = data[0];
	for(i=2; i<line-3-vars; i++)if(data[i] < min)min = data[i];
    return (int)min;
 } 
int max_finder(float* data){
    float max;int i; max = data[0];
	for(i=2; i<line-3-vars; i++)if(data[i] > max)max = data[i];
    return (int)max;
}
/******************************************************************************/             
