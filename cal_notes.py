base_freq = 440

const_ratio = pow(2, 1/12)

freq_ratio = pow(2, 32) / 22000

def output_freq(base, base_index, size):
    result = []
    for i in range (size):
        result.append( freq_ratio * base * pow(const_ratio, i - base_index))
        
        print("index ", i, result[i])



output_freq(440, 9, 12)