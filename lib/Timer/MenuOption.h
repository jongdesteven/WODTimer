#ifndef MENUOPTION_H
#define MENUOPTION_H

class MenuOption {
  const char* displayName; //2 green characters
  int startTimeInterval1Sec;
  int startTimeInterval2Sec;
  int nrOfRounds;
  bool countDirectionUp;
  bool includesInterval;

  public:
    MenuOption(const char* name, int time1, int time2, int rounds, bool countUp, bool interval) :
      displayName(name),
      startTimeInterval1Sec(time1),
      startTimeInterval2Sec(time2),
      nrOfRounds(rounds),
      countDirectionUp(countUp),
      includesInterval(interval)
    {
    }

    const char* getDisplayName(){
      return displayName;
    }
    
    int getStartTime1(){
      return startTimeInterval1Sec;
    }
    
    int getStartTime2(){
      return startTimeInterval2Sec;
    }
    
    int getNrOfRounds(){
      if ( startTimeInterval2Sec != 0 ){
        return nrOfRounds*2;
      }
      else {
        return nrOfRounds ? nrOfRounds : 1;
      }
    }
    
    bool getCountDirectionUp(){
      return countDirectionUp;
    }
    bool hasRounds(){
      return nrOfRounds ? true: false;
    }
    bool hasInterval(){
      return includesInterval;
    }
  
    void changeRounds(int change){
      // do not add rounds if it never had
      if (nrOfRounds > 0 && nrOfRounds <= 99) {
        nrOfRounds += change;
      }
    }
   
    void changeTimeInterval(int interval, int changeSec){
      switch (interval){
        case 1:
          startTimeInterval1Sec += changeSec;
          break;
        case 2:
          startTimeInterval2Sec += changeSec;
          break; 
      }
      startTimeInterval1Sec += changeSec;
      //todo: Check for boundaries
    }
};

#endif