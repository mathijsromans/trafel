#ifndef DEFDISTRIBUTIONS
#define DEFDISTRIBUTIONS

static const double MAXUP = 0.2;	// if increased this may give stability problems for the algorithm
enum ChangeResult {NO_CHANGE, ONE_UP, ONE_DOWN};

double randomLorentzian(double gamma = 1.0);
double inverseLorentzian(double var, double gamma = 1.0);	// returns result: 0 < result < 1
void mutateLorentzian(double &var, double mutation, double gamma = 1.0);

// consider var to be a value in the non-normalized 'probability distribution' P(x) = 1/x
// mutate var while maintaining the correct probability distribution
double mutateOneOverX( double var, double mutation );

double distanceLorentzian(double input1, double input2, double gamma = 1.0);
double randomGammaWithk2(double theta = 1.0);
double random2dGaussian(double sigma); // r exp(-r^2/sigma^2)
double randomExponential(double theta);	// exp(- x / theta) / theta
double randomExponentialPlateau(double theta);  // (0.5 * e / theta) * (x < theta ? exp(-1) : exp(- x / theta)) 
int randomGeometric(int maxTrials, double mean);	// 0 < result < max; mean = 1 / successProb

ChangeResult poissonUpOrDown( int curValue, double lambda );
ChangeResult DiscreteGammaWithk2UpOrDown(int input, double probabilityUp, double theta = 1.0);
void DiscreteGammaWithk2Mutate(int &input, double probabilityUp, double theta = 1.0);

#endif
