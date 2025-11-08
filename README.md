# -Branch-Prediction
Modeling a gshare branch predictor with parameters {m, n}, where m is the number of low-order PC bits used to form the prediction table index, n is the number of bits in the global branch history register

1. n=0: bimodal branch predictor
 - Determined the branch’s index into the prediction table. 
 - Made a prediction. Used index to get the branch’s counter from the prediction table. If the counter value was greater than or equal to 2, then the branch was predicted taken, else it was predicted not-taken. 
- Updated the branch predictor based on the branch’s actual outcome. The counter in 
prediction table was incremented if branch was taken, else decremented. The counter saturated at extremes.
2. n>0: gshare branch predictor 
- Determined the branch’s index into the prediction table. The current n-bit global branch history register was XORed with the uppermost n bits of the m PC bits. 
- Made a prediction. Used index to get the branch’s counter from the prediction table. If the 
counter value was greater than or equal to 2, then the branch was predicted taken, else it was predicted 
not-taken. 
- Updated the branch predictor based on the branch’s actual outcome. The branch’s counter in 
the prediction table was incremented if the branch was taken, decremented if the branch was not
taken. The counter saturated at the extremes and updated the global branch history register. 
3. Hybrid branch predictor
- Obtained predictions from the gshare predictor and from the bimodal predictor.
- Determined the branch’s index into the chooser table. The index for the chooser table was bit k+1 to bit 2 of the branch PC. 
- Made an overall prediction. Used index to get the branch’s chooser counter from the chooser 
table. If the chooser counter value was greater than or equal to 2, then used the prediction that was 
obtained from the gshare predictor, else used the prediction from the bimodal predictor. 
- Updated the selected branch predictor based on the branch’s actual outcome.
