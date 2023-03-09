#include "sine.h"

CentralOctaveLookUpTable centralOctaveLookUpTable;

uint16_t local_keyArray_concatenate;
const uint8_t octave_freq[7] = {1, 2, 4, 8, 16, 32, 64};

std::map<uint8_t, std::vector<uint16_t> > local_sound_table; 


void initialize_table(){
  centralOctaveLookUpTable.initializeTable(); 
  Serial.println("intialize_table done");
}



int16_t calculate_vout(){
  
  int16_t tmp_vout = 0;
  uint8_t count = 0;
  uint8_t step_size = 0;
  uint32_t tmp_key_index = 0;
  std::vector<uint16_t> local_pressed_keys;
  // local_keyArray_concatenate = __atomic_load_n(&global_keyArray_concated, __ATOMIC_RELAXED);
  
  // TODO: find a better method than copying
  xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
  local_sound_table = sound_table;
  xSemaphoreGive(sound_tableMutex);

  // iterate map
  for (auto it = local_sound_table.begin(); it != local_sound_table.end(); ++it) {
    local_pressed_keys = it->second;
    if (!local_pressed_keys.empty()){
      step_size = octave_freq[it->first];
      // iterate vector
      for (int vec_index = 0; vec_index < local_pressed_keys.size(); vec_index ++){
          tmp_key_index = local_pressed_keys[vec_index];
          local_timestep[tmp_key_index] += step_size;
          if(local_timestep[tmp_key_index] >= tableSizes[tmp_key_index]) local_timestep[tmp_key_index] = 0;
          tmp_vout += centralOctaveLookUpTable.accessTable(tmp_key_index,local_timestep[tmp_key_index]);
          count+=1;
      }
      // for(int i=0; i<12; i++){
      //   if((local_keyArray_concatenate & 1) == 0){
      //     local_timestep[i] += step_size;
      //     if(local_timestep[i] >= tableSizes[i]) local_timestep[i] = 0;
          
      //     tmp_vout += centralOctaveLookUpTable.accessTable(i,local_timestep[i]);
      //     count+=1;
      // }
      // local_keyArray_concatenate = local_keyArray_concatenate >> 1;
    }

  }
  if(count == 0)
    tmp_vout = 0; 
  else 
    tmp_vout = tmp_vout/count;
  tmp_vout = tmp_vout >> (8 - knob3Rotation);
  return tmp_vout;
}


void write_to_double_buffer(void *pvParameters){

  while(1){
	xSemaphoreTake(sampleBufferSemaphore, portMAX_DELAY);
	for (uint32_t writeCtr = 0; writeCtr < SAMPLE_BUFFER_SIZE; writeCtr++) {
		int16_t Vout = calculate_vout(); 
		if(writeBuffer1){
            sampleBuffer1[writeCtr] = Vout + 128;
      // Serial.println(" ");
      // Serial.print("buffer input");
      // Serial.println( sampleBuffer1[writeCtr]);
    }
		else{
			sampleBuffer0[writeCtr] = Vout + 128;
      // Serial.println(sampleBuffer0[writeCtr]);
    }
	} 
  }
}





