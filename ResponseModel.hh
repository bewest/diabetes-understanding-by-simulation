#if !defined(RESPONSE_MODEL_HH)
#define RESPONSE_MODEL_HH

#include <vector>
#include <utility> //for pair<,>
#include "boost/function.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "KineticModels.hh"
#include "ConsentrationGraph.hh"
#include "ArtificialPancrease.hh" //contains useful typedefs and constants

//Eventually, when I add mmore models, I'll set up a inheritance hiegharchy
//  to improve organization


typedef std::pair<boost::posix_time::ptime, boost::posix_time::ptime> TimeRange;

//My non-linear simple model, it is a inspired by the Bergman model
class NLSimple
{
  public:
  
    //Parameters used to compute the diferential of Blood Glucose
    enum NLSimplePars
    {
      // dGdT = -BGMultiplier*G - X(G + G_basal) + CarbAbsorbMultiplier*CarbAbsorbRate
      BGMultiplier,   
      CarbAbsorbMultiplier,
      
      // dXT = -XMultiplier*X + PlasmaInsulinMultiplier*I
      XMultiplier,
      PlasmaInsulinMultiplier,
      
      //
      NumNLSimplePars
    };//enum NLSimplePars
    
    
    std::string m_description;                  //Useful for later checking
    
    double m_cgmsDelay;                         //initially set to 15 minutes
    double m_basalInsulinConc;                  //units per kilo per hour
    double m_basalGlucoseConcentration;

    boost::posix_time::ptime m_t0;
    boost::posix_time::time_duration m_dt;
    
    std::vector<double> m_paramaters;           //size == NumNLSimplePars
    std::vector<double> m_paramaterErrorPlus;
    std::vector<double> m_paramaterErrorMinus;
    
    //If you add CGMS data from Isig, then below converts from CGMS reading
    //  to a corrected reading
    double m_currCgmsCorrFactor[2];
    
    ConsentrationGraph m_cgmsData;
    ConsentrationGraph m_freePlasmaInsulin;
    ConsentrationGraph m_glucoseAbsorbtionRate;
    
    ConsentrationGraph m_predictedInsulinX;
    ConsentrationGraph m_predictedBloodGlucose;
    
    
    std::vector<boost::posix_time::ptime> m_startSteadyStateTimes;
    
    NLSimple( const std::string &description = "", 
              double basalUnitsPerKiloPerhour = 0.9,
              double basalGlucoseConcen = 120.0, 
              boost::posix_time::ptime t0 = kGenericT0 );
    const NLSimple &operator=( const NLSimple &rhs );
    
    ~NLSimple() {};
    
    double getOffset( const boost::posix_time::ptime &absoluteTime ) const;
    boost::posix_time::ptime getAbsoluteTime( double nOffsetMinutes ) const;
    
    
    //Functions for integrating the kinetic equations
    double dGdT( const boost::posix_time::ptime &time, double G, double X ) const;
    double dXdT( const boost::posix_time::ptime &time, double G, double X ) const;
    double dXdT_usingCgmsData( double time, double X ) const;
    
    std::vector<double> dGdT_and_dXdT( double time, std::vector<double> G_and_X ) const;
    RK_DerivFuntion getRKDerivFunc() const;
    
    static double getBasalInsulinConcentration( double unitesPerKiloPerhour );
    void addCgmsData( const ConsentrationGraph &newData, 
                        bool findNewSteadyState = false );
    void addCgmsData( boost::posix_time::ptime, double value );
    void addCgmsDataFromIsig( const ConsentrationGraph &isigData,
                              const ConsentrationGraph &calibrationData,
                              bool findNewSteadyState = false );
    void addBolusData( const ConsentrationGraph &newData, 
                       bool finNewSteadyStates = false );
    
    //uses default absorption rates
    void addConsumedGlucose( boost::posix_time::ptime time, double amount ); 
    
    //if you pass in carbs consumed, just uses default absorption rate
    //  passing in glucose absorption rate is preffered method
    void addGlucoseAbsorption( const ConsentrationGraph &newData ); 
    
    
    void setModelParameters( std::vector<double> &newPar );
    void setModelParameterErrors( std::vector<double> &newParErrorLow, 
                                  std::vector<double> &newParErrorHigh );
    
    void findSteadyStateBeginings( double nHoursNoInsulinForFirstSteadyState = 3.0 );
    
    double performModelGlucosePrediction( boost::posix_time::ptime t_start = kGenericT0,
                                          boost::posix_time::ptime t_end = kGenericT0,
                                          double bloodGlucose_initial = kFailValue,
                                          double bloodX_initial = kFailValue      );
    
    double getChi2ComparedToCgmsData( const ConsentrationGraph &inputData,
                                      boost::posix_time::ptime t_start = kGenericT0,
                                      boost::posix_time::ptime t_end = kGenericT0 );
    
    double fitModelToData( std::vector<TimeRange> timeRanges = std::vector<TimeRange>(0) );
    
    void draw( boost::posix_time::ptime t_start = kGenericT0,
               boost::posix_time::ptime t_end = kGenericT0 );
    
};//class NLSimple



#endif //RESPONSE_MODEL_HH
