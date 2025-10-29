//Apagar

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>

static void msleep(int ms){struct timespec ts={ms/1000,(ms%1000)*1000000L};nanosleep(&ts,NULL);}
static void clear(){printf("\x1b[2J\x1b[H");}
static void altbuf_on(){printf("\x1b[?1049h\x1b[?25l");}
static void altbuf_off(){printf("\x1b[?25h\x1b[?1049l");}
static void rgb(int r,int g,int b){(void)r;(void)g;(void)b;}
static void reset(){;}
static void rep(const char*s,int n){for(int i=0;i<n;i++)fputs(s,stdout);}
static void hr(){rep("─",60);putchar('\n');}

static volatile sig_atomic_t run=1;
static void on_sig(int _){(void)_;run=0;}
static void term_cols_rows(int*C,int*R){struct winsize w; if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&w)==0&&w.ws_col>0&&w.ws_row>0){*C=w.ws_col;*R=w.ws_row;}else{*C=80;*R=24;}}
static unsigned xorshift32(unsigned*s){unsigned x=*s; x^=x<<13; x^=x>>17; x^=x<<5; return *s=x;}
static void hsv2rgb(double h,double s,double v,int*R,int*G,int*B){double c=v*s,hp=fmod(h*6.0,6.0),x=c*(1.0-fabs(fmod(hp,2.0)-1.0)),r=0,g=0,b=0; if(hp<1){r=c;g=x;} else if(hp<2){r=x;g=c;} else if(hp<3){g=c;b=x;} else if(hp<4){g=x;b=c;} else if(hp<5){r=x;b=c;} else {r=c;b=x;} double m=v-c; *R=(int)((r+m)*255.0); *G=(int)((g+m)*255.0); *B=(int)((b+m)*255.0);}
static void box(int w,const char*title){const char *H="─",*V="│",*TL="┌",*TR="┐",*BL="└",*BR="┘"; fputs(TL,stdout);rep(H,w-2);fputs(TR,stdout);putchar('\n');int in=w-2,len=(int)strlen(title),L=(in-len)/2,R=in-len-L; printf("%s",V);rep(" ",L);printf("%s",title);rep(" ",R);printf("%s\n",V);fputs(BL,stdout);rep(H,w-2);fputs(BR,stdout);putchar('\n');}
static void line_kv(int w,const char*k,const char*v){const char *V="│";char b[256];snprintf(b,sizeof b,"%s: %s",k,v);int in=w-2,len=(int)strlen(b); if(len>in)len=in; printf("%s%.*s%*s%s\n",V,len,b,in-len,"",V);}
static void line_kn(int w,const char*k,const char*f,double n){char v[64];snprintf(v,sizeof v,f,n);line_kv(w,k,v);}
static void bar(int W,double t){int n=(int)(t*(W-2));putchar('[');for(int i=0;i<W-2;i++)putchar(i<n?'#':' ');putchar(']');}

static void splash(){
  signal(SIGINT,on_sig); altbuf_on(); clear();
  int cols,rows; term_cols_rows(&cols,&rows);
  const int H=20,W=80; const char*ramp=" .,:;*+oO@#"; const int RAMP=11;
  time_t now=time(NULL); struct tm lt; localtime_r(&now,&lt);
  unsigned seed=(unsigned)(lt.tm_year+1900)*2654435761u ^ (unsigned)(lt.tm_mon+1)*2246822519u ^ (unsigned)lt.tm_mday*3266489917u ^ (unsigned)lt.tm_hour*668265263u ^ (unsigned)lt.tm_min*374761393u ^ (unsigned)lt.tm_sec;
  int fw=3+(seed%3);
  double cx[8],cy[8],hue[8],y0[8]; for(int i=0;i<fw;i++){seed=xorshift32(&seed); cx[i]=2+(seed%(W-4)); seed=xorshift32(&seed); cy[i]=2+(seed%8); y0[i]=H+6+(seed%6); hue[i]=fmod(((lt.tm_min*7+lt.tm_sec*13+i*97)%360)/360.0,1.0);}
  int sp=12+(lt.tm_sec%7); double ang[32]; for(int k=0;k<sp;k++){seed=xorshift32(&seed); ang[k]=((seed%36000)/36000.0)*2.0*M_PI;}
  int ASC=18,EXP=40; const double RMAX=7.0,EPS=0.55;
  for(int frame=0; frame<ASC+EXP && run; ++frame){
    printf("\x1b[H\x1b[2J"); int pad=(cols>W)?(cols-W)/2:0;
    for(int y=0;y<H;y++){
      for(int p=0;p<pad;p++) putchar(' ');
      for(int x=0;x<W;x++){
        double inten=0; if(frame<ASC){
          for(int f=0;f<fw;f++){double ty=y0[f]-(y0[f]-cy[f])*(frame/(double)ASC); double d=hypot(x-cx[f],(y-ty)*1.8); double trail=exp(-(d*d)/0.9); inten=fmax(inten,trail);}
        }else{
          double t=(frame-ASC)/(double)(EXP-1),ease=1.0-pow(1.0-t,2.0),rad=0.5+ease*RMAX;
          for(int f=0;f<fw;f++){double dx=x-cx[f],dy=(y-cy[f])*2.0,r=hypot(dx,dy),band=fabs(r-rad),ap=atan2(dy,dx); if(ap<0)ap+=2*M_PI; double sm=0; for(int k=0;k<sp;k++){double d=fabs(ang[k]-ap); d=fmin(d,2*M_PI-d); double c=exp(-(d*d)/0.10); if(c>sm) sm=c;} double ring=exp(-(band*band)/(EPS*EPS)); double local=ring*sm; double fade=exp(-0.12*r)*(1.0-t*0.6); inten+=local*fade;}
        }
        if(inten<0.05) putchar(' '); else{int idx=(int)(fmin(1.0,inten)*(RAMP-1)+0.5); putchar(ramp[idx]);}
      }
      putchar('\n');
    }
    printf(" ★ SUPER TRUNFO - LOGICA NIVEL MESTRE | %04d-%02d-%02d %02d:%02d:%02d | Ctrl+C\n",
           lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
    fflush(stdout); msleep(90);
  }
  altbuf_off(); clear();
}

typedef struct{char uf;char cod[5];char nome[50];int pop;float area,pib;int pts;} Card;
static float dens(const Card*c){return (c->area>0)?((float)c->pop/c->area):INFINITY;}
static const char* aname(int a){return a==1?"Populacao":a==2?"Area":a==3?"PIB":a==4?"Pontos Turisticos":"Densidade";}
static int cmp1(const Card*a,const Card*b,int atr){switch(atr){case 1: return a->pop==b->pop?-1:(a->pop>b->pop?0:1);case 2: return a->area==b->area?-1:(a->area>b->area?0:1);case 3: return a->pib==b->pib?-1:(a->pib>b->pib?0:1);case 4: return a->pts==b->pts?-1:(a->pts>b->pts?0:1);default:{float d1=dens(a),d2=dens(b); if((isinf(d1)&&isinf(d2))||d1==d2) return -1; return d1<d2?0:1;}}}
static void print_card(const char*t,const Card*c){
  int W=50; const char *H="─",*V="│",*TL="┌",*TR="┐",*BL="└",*BR="┘";
  fputs(TL,stdout);rep(H,W-2);fputs(TR,stdout);putchar('\n');
  int in=W-2,len=(int)strlen(t),L=(in-len)/2,R=in-len-L; printf("%s",V);rep(" ",L);printf("%s",t);rep(" ",R);printf("%s\n",V);
  char b[8]; snprintf(b,sizeof b,"%c",c->uf);
  line_kv(W,"Estado",b); line_kv(W,"Codigo",c->cod); line_kv(W,"Cidade",c->nome);
  line_kn(W,"Populacao","%d",(double)c->pop); line_kn(W,"Area (km2)","%.2f",(double)c->area);
  line_kn(W,"PIB (bi R$)","%.2f",(double)c->pib); line_kn(W,"Pontos Turisticos","%d",(double)c->pts);
  float d=dens(c); if(isinf(d)) line_kv(W,"Densidade","indefinida (area <= 0)"); else line_kn(W,"Densidade (hab/km2)","%.2f",d);
  fputs(BL,stdout);rep(H,W-2);fputs(BR,stdout);putchar('\n');
}
static void side(const Card*a,const Card*b){
  int W=40; const char *H="─",*V="│",*TL="┌",*TR="┐",*BL="└",*BR="┘";
  fputs(TL,stdout);rep(H,W);fputs("┬",stdout);rep(H,W);fputs(TR,stdout);putchar('\n');
  char t1[64],t2[64]; snprintf(t1,sizeof t1,"CARTA 1 - %s",a->nome); snprintf(t2,sizeof t2,"CARTA 2 - %s",b->nome);
  printf("%s%-*.*s┆%-*.*s%s\n",V,W,W,t1,W,W,t2,V);
  fputs("├",stdout);rep(H,W);fputs("┼",stdout);rep(H,W);fputs("┤",stdout);putchar('\n');
  #define R(LB,F1,V1,F2,V2) do{char l[128],r[128];snprintf(l,sizeof l,"%s: " F1,LB,V1);snprintf(r,sizeof r,"%s: " F2,LB,V2);printf("%s%-*.*s┆%-*.*s%s\n",V,W,W,l,W,W,r,V);}while(0)
  R("Estado","%c",a->uf,"%c",b->uf); R("Codigo","%s",a->cod,"%s",b->cod);
  R("Popul.","%d",a->pop,"%d",b->pop); R("Area","%.2f",a->area,"%.2f",b->area);
  R("PIB","%.2f",a->pib,"% .2f",b->pib); R("Pontos","%d",a->pts,"%d",b->pts);
  float d1=dens(a),d2=dens(b); char s1[32],s2[32]; snprintf(s1,sizeof s1,isinf(d1)?"indef.":"%.2f",d1); snprintf(s2,sizeof s2,isinf(d2)?"indef.":"%.2f",d2);
  R("Dens.","%s",s1,"%s",s2);
  {#define SB(LB,A,B) do{double _m=fmax((double)(A),(double)(B));int N=20,kA=(int)((_m?((double)(A)/_m):0)*(N-2)),kB=(int)((_m?((double)(B)/_m):0)*(N-2));char ba[32],bb[32];int i=0;ba[i++]='[';for(int j=0;j<N-2;j++)ba[i++]=j<kA?'#':' ';ba[i++]=']';ba[i]=0;i=0;bb[i++]='[';for(int j=0;j<N-2;j++)bb[i++]=j<kB?'#':' ';bb[i++]=']';bb[i]=0;char l[128],r[128];snprintf(l,sizeof l,"%s %s",LB,ba);snprintf(r,sizeof r,"%s",bb);printf("%s%-*.*s┆%-*.*s%s\n",V,W,W,l,W,W,r,V);}while(0)
   SB("Popul.",a->pop,b->pop); SB("Area  ",a->area,b->area); SB("PIB   ",a->pib,b->pib); SB("Pontos",a->pts,b->pts); SB("Dens. ",isinf(d1)?0:d1,isinf(d2)?0:d2);
   #undef SB}
  fputs(BL,stdout);rep(H,W);fputs("┴",stdout);rep(H,W);fputs(BR,stdout);putchar('\n');
  #undef R
}
static void flushln(){int ch;while((ch=getchar())!='\n'&&ch!=EOF){}}
static void pbar(const char*lab,int ms){int W=30,dt=ms/W; if(dt<10)dt=10; printf("%s [",lab); for(int j=0;j<W;j++) putchar(' '); printf("]  0%%"); fflush(stdout); for(int i=0;i<=W;i++){printf("\r%s [",lab); for(int j=0;j<W;j++) putchar(j<i?'#':' '); printf("] %3d%%",(i*100)/W); fflush(stdout); msleep(dt);} putchar('\n');}
static void show_result(const char*t,const char*sub){rep("=",50);putchar('\n');printf("%s\n",t);if(sub&&*sub)printf("%s\n",sub);rep("=",50);putchar('\n');msleep(600);}

/* SP x RJ reais */
#define POP_SP 11451245
#define POP_RJ  6210000
#define AREA_SP   1521
#define AREA_RJ   1221
#define PIB_SP    1100
#define PIB_RJ     400
static const char* sug(int a){if(a==1)return POP_SP>POP_RJ?"SP":"RJ"; if(a==2)return AREA_SP>AREA_RJ?"SP":"RJ"; if(a==3)return PIB_SP>PIB_RJ?"SP":"RJ"; if(a==4)return "?"; double ds=POP_SP/AREA_SP,dr=POP_RJ/AREA_RJ; return ds<dr?"SP":"RJ";}

int main(void){
  setlocale(LC_ALL,""); splash();
  Card c1={0},c2={0};
  box(50,"Cadastro da Carta 1"); printf("Estado (A-H): "); scanf(" %c",&c1.uf);
  printf("Codigo (ex: A01): "); scanf(" %4s",c1.cod);
  printf("Nome da cidade: "); flushln(); scanf(" %49[^\n]",c1.nome);
  printf("Populacao: "); scanf(" %d",&c1.pop);
  printf("Area em km2: "); scanf(" %f",&c1.area);
  printf("PIB em bilhoes: "); scanf(" %f",&c1.pib);
  printf("Pontos turisticos: "); scanf(" %d",&c1.pts);

  clear(); box(50,"Cadastro da Carta 2"); printf("Estado (A-H): "); scanf(" %c",&c2.uf);
  printf("Codigo (ex: B02): "); scanf(" %4s",c2.cod);
  printf("Nome da cidade: "); flushln(); scanf(" %49[^\n]",c2.nome);
  printf("Populacao: "); scanf(" %d",&c2.pop);
  printf("Area em km2: "); scanf(" %f",&c2.area);
  printf("PIB em bilhoes: "); scanf("
