#include <mu_sensor_megapi.h>

MuSensorMegapi::MuSensorMegapi(int32_t mu_address):MU(mu_address),kalman_servo1(2, 2, 0.01),kalman_servo2(2, 2, 0.01){
   
}

void MuSensorMegapi::init(uint16_t version){
   xAxis = yAxis = 0;
   this->version = version;
   Wire.begin();
   err = MU.begin(&Wire);
   if(err == MU_OK){

   }else{
      do
      {
         delay(5000);
      } while (1);
   }
   MU.VisionBegin(version);
}

void MuSensorMegapi::init(uint16_t version,uint8_t color){
   xAxis = yAxis = WidthValue = HeightValue = 0;
   this->version = version;
   this->color = color;
   Wire.begin();
   err = MU.begin(&Wire);
   if(err == MU_OK){

   }else{
      do
      {
         delay(5000);
      } while (1);
   }
   MU.VisionBegin(version);
   MU.write(version,kLabel,color);
   MU.CameraSetAwb(kLockWhiteBalance);
}

bool MuSensorMegapi::Have_someone_in_front_of_camera(){  
   return MU.GetValue(version,kStatus);
}

int MuSensorMegapi::GetValue(short object_inf){
   if(MU.GetValue(version,kStatus)){
      if(object_inf == X){
         float x = MU.GetValue(version,kXValue);
         xAxis = kalman_servo1.updateEstimate(0.01268 * (x * x) + 0.487 * x - 4.681);
         return xAxis;
      }
      else if(object_inf == Y){
         float y = MU.GetValue(version,kYValue);
         yAxis = kalman_servo2.updateEstimate(0.0144 * (y * y) + 0.4061 * y - 6.21);
         return yAxis;
      }
      else if(object_inf == Width)
         return WidthValue = MU.GetValue(version,kWidthValue);
      else if(object_inf == High)
         return HeightValue = MU.GetValue(version,kHeightValue);
   }
   else{
      switch (object_inf)
      {
      case 1:
         return xAxis;
      
      case 2:
         return yAxis;

      case 3:
         return WidthValue;
      
      case 4:
         return HeightValue;
      
      default:
         break;
      }
   }
}