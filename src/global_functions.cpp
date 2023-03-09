#include "global_functions.h"


///  append or delete the given [octave,key_index] from the global sound_map
void modified_soundMap(uint8_t local_octave, uint8_t key_index, bool append){
  xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  if (append) {
    //pressed
    sound_table.find(local_octave)->second.push_back(key_index);
    // Serial.print("PUSH BACK ");
    // Serial.println(key_index);
    // for (int inc_vec = 0; inc_vec < sound_table.find(local_octave)->second.size(); inc_vec ++ ){
    //   Serial.print(sound_table.find(local_octave)->second[inc_vec]);
    // }
  }else{
    //release
    sound_table.find(local_octave)->second.erase(std::remove(sound_table.find(local_octave)->second.begin(), sound_table.find(local_octave)->second.end(), key_index), sound_table.find(local_octave)->second.end());// Erase-remove_idiom
    // Serial.print("ERASE ");
    // Serial.println(key_index);
    // for (int inc_vec = 0; inc_vec < sound_table.find(local_octave)->second.size(); inc_vec ++ ){
    //   Serial.print(sound_table.find(local_octave)->second[inc_vec]);
    // }
  }
  xSemaphoreGive(sound_tableMutex);
}
