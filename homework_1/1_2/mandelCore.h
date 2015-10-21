typedef struct {
  long double reBeg,reInc;  /* beginning and increment for real values */
  long double imBeg,imInc;  /* beginning and increment for imaginary values */
  int reSteps,imSteps;           /* number of steps/points per range */
} mandel_Pars;

extern void mandel_Slices(mandel_Pars *pars, int n, mandel_Pars slices[]);
/* len(slices) == n */

extern void mandel_Calc(mandel_Pars *pars, int maxIterations, int res[]);
/* len(res) = pars->reSteps*pars->imSteps */

typedef struct{
   mandel_Pars *pars;
   int *res;
   int maxIterations;
}work;

void *calc(void *);
