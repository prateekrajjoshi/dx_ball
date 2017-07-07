#include "SDL.h"
#include"SDL_image.h"
#include"SDL_mixer.h"
#include"SDL_ttf.h"
#include<iostream>
#include<cstdlib>
#include<time.h>
using namespace std;
int forrandom=0,incpts=0,lives=10,forbr=1;


void resetall()
{
    lives=10;
    incpts=0;
    forbr=0;
}
char* convert(int x)
{
    char buf[10];
    itoa(x,buf,10);
    return buf;
}

SDL_Surface* load_image(const char* c,Uint32 colorkey=0)
{
        SDL_Surface* tmp=SDL_LoadBMP(c);
        if(colorkey!=0)
        {
                SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,colorkey);
        }
        return tmp;
}

int showmenu(SDL_Surface* screen, TTF_Font* font,char *points)
{
        Uint32 time;
        int x, y;
        const int NUMMENU = 4;
        const char* labels[NUMMENU] = {"Continue","Exit","YOUR SCORE IS",points};
        SDL_Surface* menus[NUMMENU];
        bool selected[NUMMENU] = {0,0,0,0};
        SDL_Color color[2] = {{255,255,255},{255,0,0}};

        menus[0] = TTF_RenderText_Solid(font,labels[0],color[0]);
        menus[1] = TTF_RenderText_Solid(font,labels[1],color[0]);
        menus[2] = TTF_RenderText_Solid(font,labels[2],color[0]);
        menus[3] = TTF_RenderText_Solid(font,labels[3],color[0]);
        SDL_Rect pos[NUMMENU];
        pos[0].x = screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
        pos[0].y = screen->clip_rect.h/2 - menus[0]->clip_rect.h;
        pos[1].x = screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
        pos[1].y = screen->clip_rect.h/2 + menus[0]->clip_rect.h;
        pos[2].x = screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
        pos[2].y = screen->clip_rect.h/2 + menus[0]->clip_rect.h+50;
        pos[3].x = 500;
        pos[3].y = screen->clip_rect.h/2 + menus[0]->clip_rect.h+50;

        SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0x00,0x00,0x00));

        SDL_Event event;
        while(1)
        {
                time = SDL_GetTicks();
                while(SDL_PollEvent(&event))
                {
                        switch(event.type)
                        {
                                case SDL_QUIT:
                                    for(int k=0;k<NUMMENU;k++)
                                        SDL_FreeSurface(menus[k]);
                                        return 1;
                                case SDL_MOUSEMOTION:
                                        x = event.motion.x;
                                        y = event.motion.y;
                                        for(int i = 0; i < NUMMENU; i += 1) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                        if(!selected[i])
                                                        {
                                                                selected[i] = 1;
                                                                SDL_FreeSurface(menus[i]);
                                                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[1]);
                                                        }
                                                }
                                                else
                                                {
                                                        if(selected[i])
                                                        {
                                                                selected[i] = 0;
                                                                SDL_FreeSurface(menus[i]);
                                                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[0]);
                                                        }
                                                }
                                        }
                                        break;
                                case SDL_MOUSEBUTTONDOWN:
                                        x = event.button.x;
                                        y = event.button.y;
                                        for(int i = 0; i < NUMMENU-2; i += 1) {
                                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h)
                                                {
                                                for(int k=0;k<NUMMENU;k++)
                                                  SDL_FreeSurface(menus[k]);
                                                return i;
                                                }
                                        }
                                        break;
                                case SDL_KEYDOWN:
                                        if(event.key.keysym.sym == SDLK_ESCAPE)
                                        {
                                        for(int k=0;k<NUMMENU;k++)
                                           SDL_FreeSurface(menus[k]);
                                        return 0;
                                        }
                        }
                }
                for(int i = 0; i < NUMMENU; i ++)
                {
                        SDL_BlitSurface(menus[i],NULL,screen,&pos[i]);
                }
                SDL_Flip(screen);
                if(1000/30 > (SDL_GetTicks()-time))
                        SDL_Delay(1000/30 - (SDL_GetTicks()-time));
        }
}

class paddle
{
private:
SDL_Surface* stick;
SDL_Rect rectstick;

public:
    paddle(SDL_Surface* temp,int p,int q,int r,int s)
    {
        stick=temp;
        rectstick.x=p;
        rectstick.y=q;
        rectstick.w=r;
        rectstick.h=s;
    }

~paddle()
{
    SDL_FreeSurface(stick);
}

void move_left()
{
    rectstick.x-=5;
}
void move_right()
{
    rectstick.x+=5;
    if(rectstick.x>=500)
        rectstick.x=500;
}

void show_paddle()
{
    SDL_BlitSurface(stick,NULL,SDL_GetVideoSurface(),&rectstick);
}

SDL_Rect* getRect()//to pass the position of paddle for collision checking
{
        return &rectstick;
}
};


class ball
{
private:
SDL_Surface* ba;
SDL_Rect rectball;
int xvel,yvel;
public:
    ball(SDL_Surface* bal,int q,int w,int e,int r,int u, int o)
    {
        ba=bal;
        rectball.x=q;
        rectball.y=w;
        rectball.h=e;
        rectball.w=r;
        xvel=u;
        yvel=o;
    }
    void set_velocity(int velx,int vely)
    {
        xvel=velx;
        yvel=vely;
    }
    ~ball()
    {
        SDL_FreeSurface(ba);
    }
    int collision(SDL_Rect* rec1,SDL_Rect* rec2)
    {
    if((rec1->x+(rec1->w)/2)>=rec2->x && (rec1->x+(rec1->w)/2)<=rec2->x+rec2->w && (rec1->y+rec1->h)>=rec2->y)
        return 1;
    else if(((rec1->x+(rec1->w)/2)<rec2->x || (rec1->x+(rec1->w)/2)>rec2->x+rec2->w) && (rec1->y+rec1->h)>=rec2->y)
        return 2;
    else if(rec1->x>=600||rec1->x<=0)
        return 3;
    else if(rec1->y<=70)
        return 4;
    else
        return 5;
    }


    void show_ball()
    {
        SDL_BlitSurface(ba,NULL,SDL_GetVideoSurface(),&rectball);
    }

    void move_ball(int k)
    {
        if(k==1)
        {
            yvel=-yvel;
        }
        if(k==3)
        {
            xvel=-xvel;
        }
        if(k==4)
        {
            yvel=-yvel;
        }
        rectball.x+=xvel;
        rectball.y+=yvel;
    }



    void ball_reset(int q,int w)
    {
        rectball.x=q;
        rectball.y=w;
        xvel=-xvel;
        yvel=-yvel;
        lives--;
    }

    SDL_Rect* getRect()//to pass the position of paddle for collision checking
{
        return &rectball;
}
};


class brick1
{
private:
SDL_Surface* br1;
SDL_Rect rectbr;
int col_counts;

public:
    brick1()
    {
        br1=NULL;
        rectbr.x=0;
        rectbr.y=0;
        rectbr.w=0;
        rectbr.h=0;

    }

    ~brick1()
    {
        SDL_FreeSurface(br1);
    }
    void getinfo(SDL_Surface* br,int q,int w,int e,int r)
    {
        br1=br;
        rectbr.x=q;
        rectbr.y=w;
        rectbr.w=e;
        rectbr.h=r;

    }

    void brick_random()
    {
        int abcd=0;
        while(1)
        {
            abcd=rand()%6;
            if(abcd!=forrandom)
            {
                forrandom=abcd;
                break;
            }
        }
        rectbr.x=50+forrandom*100;
    }

    void show_brick()
    {
        SDL_BlitSurface(br1,NULL,SDL_GetVideoSurface(),&rectbr);
    }

    int col_brick(SDL_Rect* rec1,SDL_Rect* rec2)
    {

      if(rec1->x+rec1->w>=rec2->x && rec1->x<=rec2->x+rec2->w && rec1->y+rec1->h>=rec2->y && rec1->y+rec1->h<=rec2->y+2)
            return 1;
      if(rec1->x+rec1->w>=rec2->x && rec1->x<=rec2->x+rec2->w && rec1->y>=rec2->y+rec2->h-2 && rec1->y<=rec2->y+rec2->h)
            return 2;
      if(rec1->y+rec1->h>=rec2->y && rec1->y<=rec2->y+rec2->h && rec1->x+rec1->w>=rec2->x && rec1->x+rec1->w<=rec2->x+2)
         return 3;
      if(rec1->y+rec1->h>=rec2->y && rec1->y<=rec2->y+rec2->h && rec1->x<=rec2->x+rec2->w && rec1->x>=rec2->x+rec2->w-2)
        return 4;
        else
            return 5;
    }
  SDL_Rect* getrect()
  {
      return &rectbr;
  }

};


class brick2
{
private:
  SDL_Surface* br2;
  SDL_Rect rectbr;
  int col_counts;
  Mix_Chunk* eff1;

public:
    brick2()
    {
        br2=NULL;
        rectbr.x=0;
        rectbr.y=0;
        rectbr.w=0;
        rectbr.h=0;
        eff1=NULL;
    }

    ~brick2()
    {
        SDL_FreeSurface(br2);
        Mix_FreeChunk(eff1);
    }
    brick2(SDL_Surface* br,Mix_Chunk* mu,int q,int w,int e,int r)
    {
        br2=br;
        eff1=mu;
        rectbr.x=q;
        rectbr.y=w;
        rectbr.w=e;
        rectbr.h=r;

    }


  int settarget(SDL_Rect* r1,SDL_Rect* r2)
      {
    if((r1->x+r1->w) >= r2->x && r1->x <= (r2->x+r2->w) && (r1->y+r1->h)>=r2->y && r1->y<=r2->y+r2->h)
        return 1;
    else
        return 0;
      }


 void show_brick()
    {
        SDL_BlitSurface(br2,NULL,SDL_GetVideoSurface(),&rectbr);
    }

    int col_brick(SDL_Rect* rec1,SDL_Rect* rec2)
    {
      if(rec1->x+rec1->w>=rec2->x && rec1->x<=rec2->x+rec2->w && rec1->y+rec1->h>=rec2->y && rec1->y+rec1->h<=rec2->y+3)
            return 1;
      if(rec1->x+rec1->w>=rec2->x && rec1->x<=rec2->x+rec2->w && rec1->y>=rec2->y+rec2->h-3 && rec1->y<=rec2->y+rec2->h)
            return 2;
      if(rec1->y+rec1->h>=rec2->y && rec1->y<=rec2->y+rec2->h && rec1->x+rec1->w>=rec2->x && rec1->x+rec1->w<=rec2->x+3)
         return 3;
      if(rec1->y+rec1->h>=rec2->y && rec1->y<=rec2->y+rec2->h && rec1->x<=rec2->x+rec2->w && rec1->x>=rec2->x+rec2->w-3)
        return 4;
        else
            return 0;
    }
  SDL_Rect* getrect()
  {
      return &rectbr;
  }

void incopts()
{
    incpts++;
    Mix_PlayChannel(-1,eff1,0);
}

int whatisscore()
{
    return incpts;
}
void breaking_ball()
{
    rectbr.y=80+rand()%300;
    rectbr.x=rand()%500;

}


};

int checklives(int x)
{
    if(x==0)
        {

            return 1;
        }
        return 0;
}


int main(int argc,char* argv[])
{
    srand( time( NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Surface* screen, *bg,*icon,*title,*scorebg,*sco,*li,*bar,*incr,*lidecr,*gameover;
    TTF_Font *fnt=TTF_OpenFont("adamwarrenpro-bold.ttf",25);
    screen=SDL_SetVideoMode(640,580,32,SDL_SWSURFACE);
    icon=SDL_LoadBMP("icon.bmp");
    bar=SDL_LoadBMP("bar.bmp");
    SDL_BlitSurface(bg,NULL,screen,NULL);
    scorebg=SDL_LoadBMP("scorebg.bmp");
    SDL_Color coloring={255,255,255};
    SDL_Color coloring1={255,0,0};
    title=TTF_RenderText_Solid(fnt,"DX-BALL",coloring);
    sco=TTF_RenderText_Solid(fnt,"SCORE",coloring);
    li=TTF_RenderText_Solid(fnt,"LIVES",coloring);
    gameover=TTF_RenderText_Solid(fnt,"GAME-OVER   HIT SPACE TO CONTINUE NEW GAME!!!!!!!!!!!!!",coloring1);
    SDL_WM_SetCaption("DXBALL",NULL);
    SDL_WM_SetIcon(icon,NULL);
    bool running=true;
    const int FPS=80;
    Uint32 start,start1;
    int pqr[12],temp1=0,r[6];

    SDL_Rect rectlidcr;
    rectlidcr.x=120;
    rectlidcr.y=0;
    rectlidcr.w=20;
    rectlidcr.h=20;

    SDL_Rect recttext;
    recttext.x=250;
    recttext.y=0;
    recttext.w=50;
    recttext.h=40;

    SDL_Rect rectsco;
    rectsco.x=480;
    rectsco.y=0;
    rectsco.w=50;
    rectsco.h=40;

    SDL_Rect rectli;
    rectli.x=50;
    rectli.y=0;
    rectli.w=50;
    rectli.h=40;

    SDL_Rect rectbar;
    rectbar.x=0;
    rectbar.y=60;
    rectbar.w=50;
    rectbar.h=40;

    SDL_Rect rectinc;
    rectinc.x=570;
    rectinc.y=0;
    rectinc.w=20;
    rectinc.h=20;

    SDL_Rect rectgameov;
    rectgameov.x=0;
    rectgameov.y=35;
    rectgameov.w=50;
    rectgameov.h=20;



    Uint32 col=SDL_MapRGB(screen->format,0x00,0x00,0x00);
    int arr[3] = {0,0,0},col_check=0,col_bricks[6]={0,0,0,0,0,0},col_tar=0,vel1=3,vel2=3;
    bg=SDL_LoadBMP("bg.bmp");
    Mix_Music* bgmus;
    Mix_Chunk* tring,*tuing;
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,2048);
    bgmus=Mix_LoadMUS("Big Guns.wav");
    tring=Mix_LoadWAV("smb_coin.wav");
    tuing=Mix_LoadWAV("smb_jump-small.wav");
    paddle pad(load_image("sticks.bmp",SDL_MapRGB(screen->format,0xff,0xff,0xff)),0,530,50,50);
    ball ball1(load_image("balls.bmp",SDL_MapRGB(screen->format,0xff,0xff,0xff)),50+rand()%500,100,20,20,3,3);
    brick2 bri2(load_image("brick2.bmp",SDL_MapRGB(screen->format,0xff,0xff,0xff)),Mix_LoadWAV("smb_breakblock.wav"),50+rand()%500,rand()%250,30,30);
    brick1 brs[6];
    for(int coun=0;coun<2;coun++)
         brs[coun].getinfo(load_image("brick1.bmp",SDL_MapRGB(screen->format,0xff,0xff,0xff)),150+rand()%500,100,30,30);
    for(int coun=2;coun<4;coun++)
         brs[coun].getinfo(load_image("brick1.bmp",SDL_MapRGB(screen->format,0xff,0xff,0xff)),50+rand()%500,200,30,30);
    for(int coun=4;coun<6;coun++)
         brs[coun].getinfo(load_image("brick1.bmp",SDL_MapRGB(screen->format,0xff,0xff,0xff)),50+rand()%500,300,30,30);
    Mix_PlayMusic(bgmus,-1);



    while(running)
    {
        start=SDL_GetTicks();
        SDL_Event event,ev;
        incr=TTF_RenderText_Solid(fnt,convert(incpts),coloring1);
        lidecr=TTF_RenderText_Solid(fnt,convert(lives),coloring1);


        if(checklives(lives))
            {

              int i = showmenu(screen, fnt,convert(bri2.whatisscore()));
                  if(i==1)
                    running = false;

              resetall();
            }

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                running=false;
                break;
                case SDL_KEYDOWN:
                   switch(event.key.keysym.sym)
                                        {
                                                case SDLK_LEFT:
                                                        arr[0]=1;
                                                        break;
                                                case SDLK_RIGHT:
                                                        arr[1]=1;
                                                        break;

                                                case SDLK_ESCAPE:
                                                        int i = showmenu(screen, fnt,convert(bri2.whatisscore()));
                                                        if(i==1)
                                                                running = false;
                                                        break;
                                        }
                                        break;
                case SDL_KEYUP:
                    switch(event.key.keysym.sym)
                                        {
                                                case SDLK_LEFT:
                                                        arr[0]=0;
                                                        break;
                                                case SDLK_RIGHT:
                                                        arr[1]=0;
                                                        break;

                                        }
                                        break;
                        }

            }


            if(arr[0])
            {
                pad.move_left();
            }
            else if(arr[1])
            {
                pad.move_right();
            }

            SDL_BlitSurface(bg,NULL,screen,NULL);

            col_check=ball1.collision(ball1.getRect(),pad.getRect());
            switch(col_check)
            {
            case 1:
                Mix_PlayChannel(-1,tuing,0);
                ball1.move_ball(col_check);
                break;
            case 3:
                ball1.move_ball(col_check);

                break;
            case 4:
                ball1.move_ball(col_check);

                break;
            case 2:
                ball1.ball_reset(rand()%500,400);
                break;
            default:
                ball1.move_ball(5);
            }



            if(forbr%10==0)
            {
                forbr++;

                 while(1)
                {
                temp1=0;
                for(int re=0;re<6;re++)
                  brs[re].brick_random();

                pqr[0]=bri2.settarget(bri2.getrect(),brs[0].getrect());
                pqr[1]=bri2.settarget(bri2.getrect(),brs[1].getrect());
                pqr[2]=bri2.settarget(bri2.getrect(),brs[2].getrect());
                pqr[3]=bri2.settarget(bri2.getrect(),brs[3].getrect());
                pqr[4]=bri2.settarget(bri2.getrect(),brs[4].getrect());
                pqr[5]=bri2.settarget(bri2.getrect(),brs[5].getrect());
                for(int qw=6;qw<12;qw++)
            {
                pqr[qw]=brs[qw-6].col_brick(ball1.getRect(),brs[qw-6].getrect());
            }

                if(pqr[0]==1||pqr[1]==1||pqr[2]==1||pqr[3]==1||pqr[4]==1||pqr[5]==1||pqr[6]!=5||pqr[7]!=5||pqr[8]!=5||pqr[9]!=5||pqr[10]!=5||pqr[11]!=5)
                        {
                            temp1++;
                        }
                else
                break;
                }

            }

            for(int ab=0;ab<6;ab++)
            {
                brs[ab].show_brick();
            }

            for(int qw=0;qw<6;qw++)
            {
                col_bricks[qw]=brs[qw].col_brick(ball1.getRect(),brs[qw].getrect());
            }


            for(int y=0;y<6;y++)
            {
                if(col_bricks[y]==1)
                    {
                        ball1.move_ball(1);
                        Mix_PlayChannel(-1,tring,0);
                        forbr++;
                    }
                if(col_bricks[y]==2)
                {
                    ball1.move_ball(1);
                    Mix_PlayChannel(-1,tring,0);
                    forbr++;
                }
                if(col_bricks[y]==3)
                    {
                        ball1.move_ball(3);
                        Mix_PlayChannel(-1,tring,0);
                        forbr++;
                    }
                if(col_bricks[y]==4)
                    {
                        ball1.move_ball(3);
                        Mix_PlayChannel(-1,tring,0);
                        forbr++;
                    }

            }

            col_tar=bri2.col_brick(ball1.getRect(),bri2.getrect());
            if(col_tar!=0)
            {
                bri2.incopts();
                while(1)
                {
                temp1=0;
                bri2.breaking_ball();

                pqr[0]=bri2.settarget(bri2.getrect(),brs[0].getrect());
                pqr[1]=bri2.settarget(bri2.getrect(),brs[1].getrect());
                pqr[2]=bri2.settarget(bri2.getrect(),brs[2].getrect());
                pqr[3]=bri2.settarget(bri2.getrect(),brs[3].getrect());
                pqr[4]=bri2.settarget(bri2.getrect(),brs[4].getrect());
                pqr[5]=bri2.settarget(bri2.getrect(),brs[5].getrect());
                if(pqr[0]==1||pqr[1]==1||pqr[2]==1||pqr[3]==1||pqr[4]==1||pqr[5]==1)
                        {
                            temp1++;
                        }
                else
                break;
                }

                if(col_tar==1)
                    ball1.move_ball(1);
                if(col_tar==2)
                    ball1.move_ball(1);
                if(col_tar==3)
                    ball1.move_ball(3);
                if(col_tar==4)
                    ball1.move_ball(3);
            }
            pad.show_paddle();
            ball1.show_ball();
            bri2.show_brick();

    SDL_BlitSurface(scorebg,NULL,screen,NULL);
    SDL_BlitSurface(bar,NULL,screen,&rectbar);
    SDL_BlitSurface(lidecr,NULL,screen,&rectlidcr);
    SDL_BlitSurface(incr,NULL,screen,&rectinc);
    SDL_BlitSurface(title,NULL,screen,&recttext);
    SDL_BlitSurface(sco,NULL,screen,&rectsco);
    SDL_BlitSurface(li,NULL,screen,&rectli);





        SDL_Flip(screen);
        if(1000/FPS>SDL_GetTicks()-start)
    SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
    }
    SDL_FreeSurface(gameover);
    SDL_FreeSurface(bg);
    SDL_FreeSurface(scorebg);
    SDL_FreeSurface(lidecr);
    SDL_FreeSurface(incr);
    SDL_FreeSurface(icon);
    SDL_FreeSurface(sco);
    SDL_FreeSurface(bar);
    SDL_FreeSurface(li);
    SDL_FreeSurface(icon);
    SDL_FreeSurface(gameover);
    SDL_FreeSurface(title);
    Mix_FreeMusic(bgmus);
    Mix_FreeChunk(tring);
    Mix_CloseAudio();
    TTF_CloseFont(fnt);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}


