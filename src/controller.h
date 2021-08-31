#pragma once

#include "dataframe.h"

class Transport;
class Dataframe;


class Controller:public Notify{
public:
   static Controller* get(){
      static Controller controller;
      return &controller;
   }

   void init();

   void check_command();

   void update(const MESSAGE_ID id, void* data);

private:
   Controller(){}

   void init_trans();

private:
   

private:
   Transport*  trans;

   Dataframe*  frame;

};