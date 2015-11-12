#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <pthread.h>
#include <stdint.h>

#include "mandelCore.h"

#define WinW 300
#define WinH 300
#define ZoomStepFactor 0.5
#define ZoomIterationFactor 2

static Display *dsp = NULL;
static unsigned long curC;
static Window win;
static GC gc;

/* basic win management rountines */

static void openDisplay() {
    if (dsp == NULL) {
        dsp = XOpenDisplay(NULL);
    }
}

static void closeDisplay() {
    if (dsp != NULL) {
        XCloseDisplay(dsp);
        dsp=NULL;
    }
}

void openWin(const char *title, int width, int height) {
    unsigned long blackC,whiteC;
    XSizeHints sh;
    XEvent evt;
    long evtmsk;

    whiteC = WhitePixel(dsp, DefaultScreen(dsp));
    blackC = BlackPixel(dsp, DefaultScreen(dsp));
    curC = blackC;

    win = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, WinW, WinH, 0, blackC, whiteC);

    sh.flags=PSize|PMinSize|PMaxSize;
    sh.width=sh.min_width=sh.max_width=WinW;
    sh.height=sh.min_height=sh.max_height=WinH;
    XSetStandardProperties(dsp, win, title, title, None, NULL, 0, &sh);

    XSelectInput(dsp, win, StructureNotifyMask|KeyPressMask);
    XMapWindow(dsp, win);
    do {
        XWindowEvent(dsp, win, StructureNotifyMask, &evt);
    } while (evt.type != MapNotify);

    gc = XCreateGC(dsp, win, 0, NULL);

}

void closeWin() {
    XFreeGC(dsp, gc);
    XUnmapWindow(dsp, win);
    XDestroyWindow(dsp, win);
}

void flushDrawOps() {
    XFlush(dsp);
}

void clearWin() {
    XSetForeground(dsp, gc, WhitePixel(dsp, DefaultScreen(dsp)));
    XFillRectangle(dsp, win, gc, 0, 0, WinW, WinH);
    flushDrawOps();
    XSetForeground(dsp, gc, curC);
}

void drawPoint(int x, int y) {
    XDrawPoint(dsp, win, gc, x, WinH-y);
    flushDrawOps();
}

void getMouseCoords(int *x, int *y) {
    XEvent evt;

    XSelectInput(dsp, win, ButtonPressMask);
    do {
        XNextEvent(dsp, &evt);
    } while (evt.type != ButtonPress);
    *x=evt.xbutton.x; *y=evt.xbutton.y;
}

/* color stuff */

void setColor(char *name) {
    XColor clr1,clr2;

    if (!XAllocNamedColor(dsp, DefaultColormap(dsp, DefaultScreen(dsp)), name, &clr1, &clr2)) {
        printf("failed\n"); return;
    }
    XSetForeground(dsp, gc, clr1.pixel);
    curC = clr1.pixel;
}

char *pickColor(int v, int maxIterations) {
    static char cname[128];

    if (v == maxIterations) {
        return("black");
    }
    else {
        sprintf(cname,"rgb:%x/%x/%x",v%64,v%128,v%256);
        return(cname);
    }
}

mandel_Pars *slices;
int *res, maxIterations, num_work, nofslices, e;
pthread_mutex_t work_status, draw;

void *workers(void *i){
    int j;
    j=(intptr_t)i;


    while(1){

        if((nofslices >= num_work) && (num_work>-1)){
            mandel_Calc(&slices[j],maxIterations,&res[j*slices[j].imSteps*slices[j].reSteps]);
            pthread_mutex_lock(&work_status);
            num_work++;
            e=j;
            if(num_work<=0)pthread_mutex_unlock(&draw);
            pthread_mutex_unlock(&work_status);

        }

    }


}


int main(int argc, char *argv[]) {
    mandel_Pars pars;
    int i, j, x, y, k, level, thread_status;
    int xoff,yoff;
    long double reEnd,imEnd,reCenter,imCenter;
    pthread_t *thread_workers;

    printf("\n");
    printf("This program starts by drawing the default Mandelbrot region\n");
    printf("When done, you can click with the mouse on an area of interest\n");
    printf("and the program will automatically zoom around this point\n");
    printf("\n");
    printf("Press enter to continue\n");
    getchar();

    pars.reSteps = WinW; /* never changes */
    pars.imSteps = WinH; /* never changes */

    /* default mandelbrot region */

    pars.reBeg = (long double) -2.0;
    reEnd = (long double) 1.0;
    pars.imBeg = (long double) -1.5;
    imEnd = (long double) 1.5;
    pars.reInc = (reEnd - pars.reBeg) / pars.reSteps;
    pars.imInc = (imEnd - pars.imBeg) / pars.imSteps;

    printf("enter max iterations (50): ");
    scanf("%d",&maxIterations);
    printf("enter no of slices: ");
    scanf("%d",&nofslices);

    /* adjust slices to divide win height */

    while (WinH % nofslices != 0) { nofslices++;}

    /* allocate slice parameter and result arrays */

    slices = (mandel_Pars *) malloc(sizeof(mandel_Pars)*nofslices);
    res = (int *) malloc(sizeof(int)*pars.reSteps*pars.imSteps);



    //workers init
    thread_workers = (pthread_t*)malloc(sizeof(pthread_t)*nofslices);

    for(i=0;i<nofslices;i++){
        thread_status=pthread_create(&thread_workers[i], NULL, workers, (void*)(intptr_t)i);
        if(thread_status){
            perror("Fail create thread\n");
            exit(1);
        }
    }
    pthread_mutex_init(&draw, NULL);
    pthread_mutex_init(&work_status, NULL);

    /* open window for drawing results */

    openDisplay();
    openWin(argv[0], WinW, WinH);

    level = 1;
    thread_status=0;
    num_work=-1;
    while (1) {

        clearWin();

        mandel_Slice(&pars,nofslices,slices);

        num_work=0;
        i=0;
        y=0;
        k=0;
        while(i<nofslices){

            pthread_mutex_lock(&work_status);
            num_work--;
            if(num_work<0){
                pthread_mutex_unlock(&work_status);
                pthread_mutex_lock(&draw);
                pthread_mutex_lock(&work_status);
            }
            pthread_mutex_unlock(&work_status);
            //start drawing
            for(i++, j=0;j<slices[e].imSteps;j++,y++){
                for(x=0;x<slices[e].reSteps;x++){
                    setColor(pickColor(res[y*slices[e].reSteps+x],maxIterations));
                    drawPoint(x, y);
                }
            }
        }

        /* get next focus/zoom point */
        
        getMouseCoords(&x,&y);
        xoff = x;
        yoff = WinH-y;
        
        /* adjust region and zoom factor  */
        
        reCenter = pars.reBeg + xoff*pars.reInc;
        imCenter = pars.imBeg + yoff*pars.imInc;
        pars.reInc = pars.reInc*ZoomStepFactor;
        pars.imInc = pars.imInc*ZoomStepFactor;
        pars.reBeg = reCenter - (WinW/2)*pars.reInc;
        pars.imBeg = imCenter - (WinH/2)*pars.imInc;
        
        maxIterations = maxIterations*ZoomIterationFactor;
        level++;
        
    }
    
    /* never reach this point; for cosmetic reasons */
    
    free(thread_workers);
    free(slices);
    free(res);
    
    closeWin();
    closeDisplay();
    
}
