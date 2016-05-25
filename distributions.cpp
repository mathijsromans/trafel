#include <cmath>
#include <cassert>
#include <cstdlib>
#include "distributions.h"

//debug:
#include <iostream>

float frandom(float x)
{
  return x * rand() / (float(RAND_MAX) + 1);
}

double my_lambert(const double x)  // from GSL, works for x not close to zero
{
const double q = x + 1 / M_E;
const double r = -sqrt(q);
 static const double c[12] = {
    -1.0,
     2.331643981597124203363536062168,
    -1.812187885639363490240191647568,
     1.936631114492359755363277457668,
    -2.353551201881614516821543561516,
     3.066858901050631912893148922704,
    -4.175335600258177138854984177460,
     5.858023729874774148815053846119,
    -8.401032217523977370984161688514,
     12.250753501314460424,
    -18.100697012472442755,
     27.029044799010561650
  };
  const double t_8 = c[8] + r*(c[9] + r*(c[10] + r*c[11]));
  const double t_5 = c[5] + r*(c[6] + r*(c[7]  + r*t_8));
  const double t_1 = c[1] + r*(c[2] + r*(c[3]  + r*(c[4] + r*t_5)));
  return c[0] + r*t_1;
}

double LambertW1(const double z) {
  /* Lambert W function, -1 branch. 

   K M Briggs 2001 Apr 09
   Keith dot Briggs at bt dot com

   Returned value W(z) satisfies W(z)*exp(W(z))=z
   Valid input range: -1/e <= z    < 0
   Output range:      -1   >= W(z) > -infinity

   See LambertW.c for principal branch.
   Together these form the only purely real branches.

   Test: 
     gcc -DTESTW LambertW1.c -o LambertW1 -lm && LambertW1
   Library:
     gcc -O3 -c LambertW1.c 
*/
  int i; 
  const double eps=4.0e-16, em1=0.3678794411714423215955237701614608; 
  double p,e,t,w,l1,l2;
  if (z < -0.3)
    return my_lambert(z);
  if (z<-em1 || z>=0.0 || isinf(z) || isnan(z))
    throw("LambertW1: bad argument");
  /* initial approx for iteration... */
  if (z<-1e-6) { /* series about -1/e */
    p=-sqrt(2.0*(2.7182818284590452353602874713526625*z+1.0));
    w=-1.0+p*(1.0+p*(-0.333333333333333333333+p*0.152777777777777777777777)); 
  } else { /* asymptotic near zero */
    l1=log(-z);
    l2=log(-l1);
    w=l1-l2+l2/l1;
  }
  if (fabs(p)<1e-4) return w;
  for (i=0; i<10; i++) { /* Halley iteration */
    e=exp(w); 
    t=w*e-z;
    p=w+1.0;
    t/=e*p-0.5*(p+1.0)*t/p; 
    w-=t;
    if (fabs(t)<eps*(1.0+fabs(w))) return w; /* rel-abs error */
  }
  /* should never get here */
  fprintf(stderr, "z = %lf\n", z);
  throw("LambertW1: No convergence");
  return 0; // cannot be reached
}

inline double randomUnitIntervalExclusive()
{
  return (double(rand()) + 1) / (double(RAND_MAX) + 2);  // not 0.0 or 1.0
}

double randomLorentzian(double gamma)
{
  return gamma * tan(M_PI * (randomUnitIntervalExclusive() - 0.5));
}

double inverseLorentzian(double var, double gamma)  // returns result: 0 < result < 1
{
  return (1/ M_PI) * atan(var / gamma) + 0.5;
}
      
void mutateLorentzian( double &var, double mutation, double gamma )
{
  double p, newp;
  p = (1/ M_PI) * atan(var / gamma) + 0.5;
  do
    newp = p - mutation + frandom(2 * mutation);
  while (newp <= 0 || newp >= 1);
  var = gamma * tan(M_PI * (newp - 0.5));
}

double mutateOneOverX(double var, double mutation )
{
  const double x_0 = 1;
  double p = log( var / x_0 );
  p += frandom( 2 * mutation ) - mutation;
  return x_0 * exp( p );
}


double distanceLorentzian(double input1, double input2, double gamma)
{
  return (1/ M_PI) * (atan(input2 / gamma) - atan(input1 / gamma));
}

double randomGammaWithk2(double theta)  // probability distribution (1/theta^2)*x*exp(-x/theta)
{
  return theta * (- 1 - LambertW1((randomUnitIntervalExclusive() - 1) / M_E));
}

double probGammaWithk2(double x, double theta)  // probability distribution (1/theta^2)*x*exp(-x/theta)
{
  return   (1/(theta*theta))*x*exp(-x/theta);
}

double random2dGaussian(double sigma) // r exp(-r^2/sigma^2)
{
  return sigma * sqrt(-log(randomUnitIntervalExclusive()));
}

ChangeResult poissonUpOrDown( int curValue, double lambda )
{
  double test = randomUnitIntervalExclusive();
  if ( test < double( curValue ) / ( 3 * lambda ) )
  {
    return ONE_DOWN;
  }
  if ( test > 2./3. )
  {
    return ONE_UP;
  }
return NO_CHANGE;
}

ChangeResult DiscreteGammaWithk2UpOrDown(int input, double probabilityUp, double theta)
{
  double r;
  assert(probabilityUp <= MAXUP);
  assert(input >= 0);
  r = randomUnitIntervalExclusive() - probabilityUp;
  if (r < 0.0)
    return ONE_UP;
  else if (input > 0 && r < probabilityUp * probGammaWithk2(input - 1, theta) / probGammaWithk2(input, theta))
    return ONE_DOWN;
  else 
    return NO_CHANGE;
}

void DiscreteGammaWithk2Mutate(int &input, double probabilityUp, double theta)
{
  switch (DiscreteGammaWithk2UpOrDown(input, probabilityUp, theta))
  {
    case ONE_UP:
      input++;
      break;
    case ONE_DOWN:
      input--;
      break;
    case NO_CHANGE:
      // do nothing
      break;
  }
}

double randomExponential(double theta)  // exp(- x / theta) / theta
{
  return - theta * log(1 - randomUnitIntervalExclusive());
}

double randomExponentialPlateau(double theta)  // (0.5 * e / theta) * (x < theta ? exp(-1) : exp(- x / theta)) 
{
  double R = randomUnitIntervalExclusive();
  if (R <= 0.5)
    return 2 * theta * R;
  else
    return theta - theta * log(2. * (1. - R));
}

int randomGeometric(int maxTrials, double mean)  // 0 < result < max; mean = 1 / successProb
{
  int n;
  for (n = 1; n < maxTrials - 1; n++)
    if (frandom(mean) < 1.0)
      break;
  return n;
}
