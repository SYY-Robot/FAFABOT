#include "controller.h"

#include "board.h"
#include "usart_transport.h"
#include "simple_dataframe_slave.h"
#include "data_holder.h"


void Controller::init(){
   
   init_trans();
   

}

void Controller::init_trans(){
   static USART_transport _trans(MASTER_USART,115200);

   static Simple_dataframe _frame(&_trans);

   trans = &_trans;
   frame = &_frame;

   trans->init();
   frame->init();

}


void Controller::check_command(){
   unsigned char ch=0;

   if(trans->read(ch)){
      if(frame->data_recv(ch)){
         frame->data_parse();
      }
   }
}

void Controller::update(const MESSAGE_ID id, void* data){
   switch (id)
   {
   default:
      break;
   }
}
