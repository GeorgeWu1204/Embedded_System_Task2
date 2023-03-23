#include "global_functions.h"

///  append or delete the given [octave,key_index] from the global sound_map
void modified_soundMap(uint8_t local_octave, uint8_t key_index, bool append){

  xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  
  if (append) {
    //pressed
    for (int g = 0; g < 12 ;g++){
      if (sound_table[g].key_index == 0){
        sound_table[g].key_index = key_index;
        sound_table[g].octave = local_octave;
        break;

      }
    }

  }else{
    for (int g = 0; g < 12 ;g++){
      if (sound_table[g].key_index == key_index){
        sound_table[g].key_index = 0;
        sound_table[g].octave = 0;
        break;
      }
    }
  }
  xSemaphoreGive(sound_tableMutex);
}
