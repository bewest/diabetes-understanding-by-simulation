#if !defined(PROGRAM_OPTIONS_HH)
#define PROGRAM_OPTIONS_HH

#include <map>

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGNumberEntry.h"
#include "RQ_OBJECT.h"

// so I should have the gui in a seperate file, but in the interest of easy
//  programming, I'll do this later (maybe)

//We have to hide anything boost from ROOTS CINT
#ifndef __CINT__
#include "ArtificialPancrease.hh"
#include "boost/program_options.hpp"

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/version.hpp>
#endif //__CINT__


namespace ProgramOptions
{
  #ifndef __CINT__ //We have to hide anything boost from ROOTS CINT
  namespace po = boost::program_options;

  //I want to make 'ProgramOptions' a class, and the below maps static members
  //  and each instance of a NLSimple model will hold it's own ProgramOptions object
  extern po::variables_map                   ns_poVariableMap;
  extern po::options_description             ns_poDescription;
  extern po::positional_options_description  ns_posDescripton; //positional decrip

  TimeDuration roundToNearestSecond( const TimeDuration &td );
  TimeDuration roundToNearestSecond( const double nMinutes );

  void declareOptions();
  void decodeOptions( int argc, char **argv );
  #endif //__CINT__

  //Below here is variables that may be specified via the command line when
  //  starting the program.  You typically won't need to specify any of them

  //in case you open a Model file imediately from the command line
  //  Can have as first argument, or use with '--file=name', or '-f name'
  extern std::string ns_defaultModelFileName;

  //All variables below here have an equivalent variable in the 'ModelSettings'
  //  class.  'ModelSettings' default value of variables is derived from the
  //  values of the below variables

  extern double kPersonsWeight;          //kg, command line '--weight=78', or '-kg 78'
  extern double kBasalGlucConc;          //

  extern double kCgmsIndivReadingUncert;

  #ifndef __CINT__ //We have to hide anything boost from ROOTS CINT
  extern TimeDuration kDefaultCgmsDelay; //minutes, ('--cgmsdelay=15')
  extern TimeDuration kPredictAhead;     //default how far to predict ahead of cgms
  extern TimeDuration kIntegrationDt;    //Integration tiemstep in minutes ('--dt=1.0')
  #endif //__CINT__

  extern double kLastPredictionWeight;   //for calc. chi2 of model ('--last_pred_weight=0.25')

  extern double kTargetBG;               //target blood glucose, ('--target=100')
  extern double kBGLowSigma;             //uncert on low BG, ('--lowsigma=10')
  extern double kBGHighSigma;            //uncert on high BG, ('--lowsigma=20')

  //Genetic minimization paramaters
  extern int kGenPopSize;                //(--genetic_pop_size=100)
  extern int kGenConvergNsteps;          //(--genetic_conv_steps=10)
  extern int kGenNStepMutate;            //(--genetic_nstep_track=6)
  extern int kGenNStepImprove;           //(--genetic_ngen_improve=3)
  extern double kGenSigmaMult;           //(--genetic_mutate_sigma=0.5)
  extern double kGenConvergCriteria;     //(--genetic_convergence_chi2=1.0)
  //When the number of improvments within the last kGenNStepMutate generations is:
  //  a) smaller than kGenNStepImprove, then divide current sigma by kGenSigmaMult
  //  b) equal, do nothing
  //  c) larger than kGenNStepImprove, then multiple current sigma by kGenSigmaMult
  //
  //If convergence hasn't improvedmore than kGenConvergCriteria in the last
  //  kGenConvergNsteps, then consider minimization complete


}//namespace ProgramOptions



 #ifndef __CINT__ //We have to hide anything boost from ROOTS CINT
class ModelSettings
{
  public:
    ModelSettings();
    ~ModelSettings();


    double m_personsWeight;          //ProgramOptions::kPersonsWeight
    //double m_basalGlucConc;          //ProgramOptions::kBasalGlucConc

    double m_cgmsIndivReadingUncert; //ProgramOptions::kCgmsIndivReadingUncert

    TimeDuration m_defaultCgmsDelay; //ProgramOptions::kDefaultCgmsDelay
    TimeDuration m_cgmsDelay;        //what is actually used for the delay
    TimeDuration m_predictAhead;     //ProgramOptions::kPredictAhead
    TimeDuration m_dt;               //ProgramOptions::kIntegrationDt

    PosixTime m_endTrainingTime;
    PosixTime m_startTrainingTime;


    double m_lastPredictionWeight;   //ProgramOptions::kLastPredictionWeight

    double m_targetBG;               //ProgramOptions::kTargetBG
    double m_bgLowSigma;             //ProgramOptions::kBGLowSigma
    double m_bgHighSigma;            //ProgramOptions::kBGHighSigma

  //Genetic minimization paramaters
    int m_genPopSize;                //ProgramOptions::kGenPopSize
    int m_genConvergNsteps;          //ProgramOptions::kGenConvergNsteps
    int m_genNStepMutate;            //ProgramOptions::kGenNStepMutate
    int m_genNStepImprove;           //ProgramOptions::kGenNStepImprove
    double m_genSigmaMult;           //ProgramOptions::kGenSigmaMult
    double m_genConvergCriteria;     //ProgramOptions::kGenConvergCriteria

    //Will leave serialization funtion in header
    friend class boost::serialization::access;
    template<class Archive>
    void serialize( Archive &ar, const unsigned int version );
};//class ModelSettings
#endif //__CINT__

//depreciated below here
//This class seems a bit awkward, and is error prone for adding
//  future options into it, so be careful
//Also, THE MAJOR PROBLEM with this class is that it forces
//  namespace::PersonConstants and namespace::ModelDefaults
//  variables to have the same value as the model you pass in
//  Also, model updating is done via
//  ProgramOptionsGui::valueChanged(UInt_t bitmask) emitting a signal
//    which this seems less than ideal, but to be worried about later
class NLSimple;
class ProgramOptionsGui : public TGCompositeFrame
{
    RQ_OBJECT("ProgramOptionsGui")
  public:
    enum ProgOptionEnum
    {
      E_kPersonsWeight, E_kBasalGlucConc,
      E_kDefaultCgmsDelay, E_kCgmsIndivReadingUncert,
      E_kPredictAhead, E_kIntegrationDt,
      E_kLastPredictionWeight, E_kTargetBG,
      E_kBGLowSigma, E_kBGHighSigma,
      E_kGenPopSize, E_kGenConvergNsteps,
      E_kGenNStepMutate, E_kGenNStepImprove,
      E_kGenSigmaMult, E_kGenConvergCriteria,
      E_NUMBER_PROGRAM_OPTIONS
    };//enum ProgOptionEnum

    NLSimple *m_model;
    std::vector<TGNumberEntry *> m_entries;
    const bool m_debug;

    //Pass in  w and h of the parent window, if model!=NULL, then model will
    //  be updated when the gui is changed
    ProgramOptionsGui( const TGWindow *parentW, NLSimple *model, UInt_t w, UInt_t h );

    TGNumberEntry *addNewEntryField( TGVerticalFrame *parentFrame,
                                     const char *label, double defaultNumer,
                                     TGNumberFormat::EStyle format,
                                     const char *connect = NULL );

    //The below both actually change the value of an option
    void valueChanged(UInt_t bitmask); // *SIGNAL*
    void optionValueChanged();         // *SIGNAL*
    void modelCalcValueChanged();      // *SIGNAL*
    void personConstChanged();         // *SIGNAL*

    ClassDef(ProgramOptionsGui, 0 );
};


#endif //PROGRAM_OPTIONS_HH
