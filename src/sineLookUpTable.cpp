#include "global_variables.h"
#include <math.h>
# define M_PI           3.14159265358979323846

const int32_t tableSizes [] = { 
     // we have to know the size at comile time.
    84,
    79,
    75,
    71,
    67,
    63,
    59,
    56,
    53,
    50,
    47,
    45,
};

class CentralOctaveLookUpTable{
    private:
    // There sizes have to be manually calculated. :(

    int32_t C[84];
    int32_t C_sharp[79];
    int32_t D[75];
    int32_t D_sharp[71];
    int32_t E[67];
    int32_t F[63];
    int32_t F_sharp[59];
    int32_t G[56];
    int32_t G_sharp[53];
    int32_t A[50];
    int32_t A_sharp[47];
    int32_t B[45];

    const int32_t baseHz[12] = { 
        261,
        277,
        293,
        311,
        329,
        349,
        369,
        391,
        415,
        440,
        466,
        493,
    };

    void sineTable(int32_t tableHz[], uint8_t keyIndex){
            uint8_t tableSize = tableSizes[keyIndex];
            Serial.println(sizeof(tableSize));
            for(int i=0; i<tableSize;i++){
                tableHz[i] = 127*sin(2*M_PI*((baseHz[keyIndex])/22000.0)*i);
                Serial.println(tableHz[i]);
            }
        }

    public:

        void initializeTable(){
            Serial.print("Init");
            sineTable(C,0);
            sineTable(C_sharp,1);
            sineTable(D,2);
            sineTable(D_sharp,3);
            sineTable(E,4);
            sineTable(F,5);
            sineTable(F_sharp,6);
            sineTable(G,7);
            sineTable(G_sharp,8);
            sineTable(A,9);
            sineTable(A_sharp,10);
            sineTable(B,11);
        }

        int32_t accessTable(uint32_t keyIndex, uint8_t timeStepIndex){
            switch (keyIndex)
            {
                case(0):
                    // timeStepIndex = timeStepIndex % 84;
                    return C[timeStepIndex];
                    break;
                case(1):
                    // timeStepIndex = timeStepIndex % 79;
                    return C_sharp[timeStepIndex];
                    break;
                case(2):
                    timeStepIndex = timeStepIndex % 75;
                    return D[timeStepIndex];
                    break;
                case(3):
                    timeStepIndex = timeStepIndex % 71;
                    return D_sharp[timeStepIndex];
                    break;
                case(4):
                    timeStepIndex = timeStepIndex % 67;
                    return E[timeStepIndex];
                    break;
                case(5):
                    timeStepIndex = timeStepIndex % 63;
                    return F[timeStepIndex];
                    break;
                case(6):
                    // timeStepIndex = timeStepIndex % 59;
                    return F_sharp[timeStepIndex];
                    break;
                case(7):
                    timeStepIndex = timeStepIndex % 56;
                    return G[timeStepIndex];
                    break;
                case(8):
                    timeStepIndex = timeStepIndex % 53;
                    return G_sharp[timeStepIndex];
                    break;
                case(9):
                    timeStepIndex = timeStepIndex % 50;
                    return A[timeStepIndex];
                    break;
                case(10):
                    timeStepIndex = timeStepIndex % 47;
                    return A_sharp[timeStepIndex];
                    break;
                case(11):
                    timeStepIndex = timeStepIndex % 45;
                    return B[timeStepIndex];
                    break;
                default:
                    break;
                }
        }

};

