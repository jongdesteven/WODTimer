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
      return nrOfRounds;
    }
    
    bool getCountDirectionUp(){
      return countDirectionUp;
    }
  
    void changeRounds(int change){
      // do not add rounds if it never had
      if (nrOfRounds > 0){
        nrOfRounds += change;
        if (nrOfRounds < 1) { nrOfRounds = 1;}
        if (nrOfRounds > 99) { nrOfRounds = 99; }
      }
    }
   
    void changeTimeInterval(int interval, int changeSec){
      switch (interval){
      case 1:
        startTimeInterval1Sec += changeSec;
        if (startTimeInterval1Sec < 0){ startTimeInterval1Sec = 0; }
        if (startTimeInterval1Sec > 3600){ startTimeInterval1Sec = 3600; }
        break;
      case 2:
        startTimeInterval2Sec += changeSec;
        if (startTimeInterval2Sec < 0){ startTimeInterval2Sec = 0; }
        if (startTimeInterval2Sec > 3600){ startTimeInterval2Sec = 3600; }
        break; 
      }
    }
};

#endif