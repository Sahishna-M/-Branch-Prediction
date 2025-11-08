#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"
#include <vector>
#include <utility> // adding this to use std::pair
#include <inttypes.h>
#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

//bimodal branch predictor variables
std::vector<int> bmp_counter_table; 
 
struct bimod
{
    int bimod_predictions = 0;
    int  bimod_mispredictions = 0;
};

bimod bimod_values; //creating variable of type bimod
bimod bimodal_predictor(unsigned long int m_bmp, char branch_type_bmp, unsigned long int address_bmp);

// gshare global variables
std::vector<int> gshare_counter_table;
std::vector<int> global_branch_history;

struct gshare
{
    int gshare_predictions = 0;
    int  gshare_mispredictions = 0;
};
gshare gshare_values;
gshare gshare_predictor(unsigned long int m_gsp, unsigned long int n_gsp, std::vector<int>& gbh, char branch_type_gsp, unsigned long int address_gsp);

// Hybrid global variables
std::vector<int> hybrid_counter_table; 
struct hybrid
{
    int hybrid_predictions = 0;
    int  hybrid_mispredictions = 0;
};
hybrid hybrid_values;
hybrid hybrid_predictor(unsigned long int k_hybrid, unsigned long int m1_hybrid, unsigned long int n_hybrid, unsigned long int m2_hybrid, std::vector<int>& gbh_hybrid, char branch_type_hybrid, unsigned long int address_hybrid);

int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
  
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];

    ////////////////////////////////////////////////////// BIMODAL INITIALIZATIONS ///////////////////////////////////////////////////////////////////
    // sizing the bimodal branch predictor to 2 power m2
    double bp_size = 1 << params.M2;
    bmp_counter_table.resize(bp_size);
    //Initializing all entries in prediction table to 2 - weakly taken
    for (int i = 0; i<bp_size; i++)
    {
        bmp_counter_table[i] = 2;
    }

    ////////////////////////////////////////////////////// GSHARE INITIALIZATIONS ///////////////////////////////////////////////////////////////////
    //sizing the gshare branch predictor to 2 power m1
    double gs_size = 1 << params.M1;
    gshare_counter_table.resize(gs_size);
    //Initializing all entries in prediction table to 2 - weakly taken
    for (int i = 0; i<gs_size; i++)
    {
        gshare_counter_table[i] = 2;
    }
    // sizing the global branch history resgister to n
    if(params.N > 0)
    {
        global_branch_history.resize(params.N);
        //Initializing all entries in global branch history to 0
        for (int i = 0; i<params.N; i++)
        {
            global_branch_history[i] = 0;
        }
    }
   
    ////////////////////////////////////////////////////// HYBRID INITIALIZATIONS ///////////////////////////////////////////////////////////////////
    // sizing the hybrid predictor to 2 power k
    double hybrid_size = 1 << params.K;
    hybrid_counter_table.resize(hybrid_size);
    //Initializing all entries in prediction table to 2 - weakly taken
    for (int i = 0; i<hybrid_size; i++)
    {
        hybrid_counter_table[i] = 1;
    }

    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        
        outcome = str[0];
        /*************************************
            Add branch predictor code here
        **************************************/

        // CALL THE RESPECTIVE FUNCTION BASED ON THE GIVEN INPUT

        if(strcmp(params.bp_name, "bimodal") == 0)
        {
            bimod_values = bimodal_predictor(params.M2,outcome,addr);

        }

        else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
        {
            gshare_values =  gshare_predictor(params.M1,params.N,global_branch_history,outcome,addr);
        }

        else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
        {
            hybrid_values = hybrid_predictor(params.K,params.M1,params.N,params.M2,global_branch_history,outcome,addr);
        }
        
    } 

    ////////////////////////////////////////////////////////////// BIMODAL PRINT ///////////////////////////////////////////////////////////////////
    if(strcmp(params.bp_name, "bimodal") == 0)
    {            
        //Calculation and print for bimod
        double bimod_misprediction_rate = (double(bimod_values.bimod_mispredictions)/double(bimod_values.bimod_predictions)*100);
        cout<<"OUTPUT"<<endl;
        cout<<"number of predictions: "<< bimod_values.bimod_predictions<<endl;
        cout<<"number of mispredictions: "<< bimod_values.bimod_mispredictions<<endl;
        printf("misprediction rate: %.2lf% \n", bimod_misprediction_rate);
        cout<<"FINAL BIMODAL CONTENTS"<<endl;
        for(int i = 0; i<bp_size; i++)
        {
            if(i<10)
            {
                cout<<i<< "     "<<bmp_counter_table[i]<<endl;
            }
            else if(i<100)
            {
                cout<<i<< "    "<<bmp_counter_table[i]<<endl; 
            } 
            else if(i<1000)
            {
                cout<<i<< "   "<<bmp_counter_table[i]<<endl; 
            } 
            else
            {
                cout<<i<< "  "<<bmp_counter_table[i]<<endl; 
            }       
        }
    }

    /////////////////////////////////////////////////////////////// GSHARE PRINT ///////////////////////////////////////////////////////////////////
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        //Calculation and print for gshare
        double gshare_misprediction_rate = (double(gshare_values.gshare_mispredictions)/double(gshare_values.gshare_predictions)*100);
        cout<<"OUTPUT"<<endl;
        cout<<"number of predictions: "<< gshare_values.gshare_predictions<<endl;
        cout<<"number of mispredictions: "<< gshare_values.gshare_mispredictions<<endl;
        printf("misprediction rate: %.2lf% \n", gshare_misprediction_rate);
        cout<<"FINAL GSHARE CONTENTS"<<endl;
        for(int i = 0; i<gs_size; i++)
        {
            if(i<10)
            {
                cout<<i<< "     "<<gshare_counter_table[i]<<endl;
            }
            else if(i<100)
            {
                cout<<i<< "    "<<gshare_counter_table[i]<<endl; 
            } 
            else if(i<1000)
            {
                cout<<i<< "   "<<gshare_counter_table[i]<<endl;
            }  
            else
            {
                cout<<i<< "  "<<gshare_counter_table[i]<<endl;
            }     
        }
    }

    /////////////////////////////////////////////////////////////// HYBRID PRINT ///////////////////////////////////////////////////////////////////
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        //Calculation and print for hybrid
        double hybrid_misprediction_rate = (double(hybrid_values.hybrid_mispredictions)/double(hybrid_values.hybrid_predictions)*100);
        cout<<"OUTPUT"<<endl;
        cout<<"number of predictions: "<< hybrid_values.hybrid_predictions<<endl;
        cout<<"number of mispredictions: "<< hybrid_values.hybrid_mispredictions<<endl;
        printf("misprediction rate: %.2lf% \n", hybrid_misprediction_rate);
        cout<<"FINAL CHOOSER CONTENTS"<<endl;
        for(int i = 0; i<hybrid_size; i++)
        {
            if(i<10)
            {
                cout<<i<< "     "<<hybrid_counter_table[i]<<endl;
            }
            else if(i<100)
            {
                cout<<i<< "    "<<hybrid_counter_table[i]<<endl; 
            } 
            else if(i<1000)
            {
                cout<<i<< "   "<<hybrid_counter_table[i]<<endl;
            }   
            else
            {
                cout<<i<< "  "<<hybrid_counter_table[i]<<endl;
            }    
        }
        cout<<"FINAL GSHARE CONTENTS"<<endl;
        for(int i = 0; i<gs_size; i++)
        {
            if(i<10)
            {
                cout<<i<< "     "<<gshare_counter_table[i]<<endl;
            }
            else if(i<100)
            {
                cout<<i<< "    "<<gshare_counter_table[i]<<endl; 
            } 
            else if(i<1000)
            {
                cout<<i<< "  "<<gshare_counter_table[i]<<endl;
            } 
            else
            {
                cout<<i<< " "<<gshare_counter_table[i]<<endl;
            }      
        } 
        cout<<"FINAL BIMODAL CONTENTS"<<endl;
        for(int i = 0; i<bp_size; i++)
        {
            if(i<10)
            {
                cout<<i<< "     "<<bmp_counter_table[i]<<endl;
            }
            else
            {
                cout<<i<< "    "<<bmp_counter_table[i]<<endl; 
            }        
        } 
    }
    return 0;    
}

///////////////////////////////////////////////////////////////// BIMODAL FUNCTION ///////////////////////////////////////////////////////////////////

bimod bimodal_predictor(unsigned long int m_bmp, char branch_type_bmp, unsigned long int address_bmp)
{
    unsigned long int temp_addr; //stores temporary address that contains m+1 -> 2 bits of address
    char predict;
    int counter;

    // Extraction of bits from m+1 -> 2
    temp_addr = (address_bmp >> 2) & ((1 << m_bmp) - 1);  // removing the last two bits
  
    bimod_values.bimod_predictions++; // counting number of predictions
        
    counter = bmp_counter_table[temp_addr];
        
    // making prediction
    if(counter>=2)
    {
        predict = 't';
    }
    else
    {
        predict = 'n';
    }
        
    //checking for misprediction
    if(predict != branch_type_bmp)
    {
        bimod_values.bimod_mispredictions++;
    }

    // counter increment conditions
    if(branch_type_bmp == 't')
    {
        if(counter < 3)
        {
            counter++;
        }
    }
    else
    {
        if(counter > 0)
        {
            counter--;
        }
    }
    bmp_counter_table[temp_addr] = counter;
    return bimod_values;
}

///////////////////////////////////////////////////////////////// GSHARE FUNCTION ///////////////////////////////////////////////////////////////////

gshare gshare_predictor(unsigned long int m_gsp, unsigned long int n_gsp, std::vector<int>& gbh, char branch_type_gsp, unsigned long int address_gsp)
{
    unsigned long int temp_addr; //stores temporary address that contains m+1 -> 2 bits of address
    char predict;
    int counter;

    if(n_gsp > 0)
    {
        // Extraction of bits from m+1 -> 2
        temp_addr = (address_gsp >> 2) & ((1 << m_gsp) - 1);  // removing the last two bits

        //extract n upper bits of temp_addr and store lower bits
        unsigned long int lower_bits = temp_addr & ((1 << (m_gsp - n_gsp)) - 1);
        unsigned long int upper_bits = temp_addr >> (m_gsp - n_gsp);
    
        // Convert global branch history resgister values to decimal value
        unsigned long int gbh_dec_value = 0;
        for(int i = 0; i<n_gsp; i++)
        {
            gbh_dec_value |= (gbh[i] << (n_gsp - 1 - i));
        }

        // xor the n upper bits of the m bit pc with global branch history register
        unsigned long int result = upper_bits ^ gbh_dec_value;

        // concatenate the xored result with the lower bits of pc
        unsigned long decimal = (result << (m_gsp - n_gsp)) | lower_bits;

        gshare_values.gshare_predictions++; // counting number of predictions
        
        counter = gshare_counter_table[decimal];
        
        // making prediction
        if(counter>=2)
        {
            predict = 't';
        }
        else
        {
            predict = 'n';
        }
        
        //checking for misprediction
        if(predict != branch_type_gsp)
        {
            gshare_values.gshare_mispredictions++;
        }

        // counter increment conditions
        if(branch_type_gsp == 't')
        {
            if(counter < 3)
            {
                counter++;
            }
        }
        else
        {
            if(counter > 0)
            {
                counter--;
            }
        }
        gshare_counter_table[decimal] = counter;

        // right shifting the global branch history and storing the latest prediction in msb
        for(int i = n_gsp - 1; i>0 ; i--)
        {
            gbh[i] = gbh [i-1];
        }
        if(branch_type_gsp == 't')
        {
            gbh[0] = 1;
        }
        else
        {
            gbh[0] = 0;
        }
    }

    else
    {
        // Extraction of bits from m+1 -> 2
        temp_addr = (address_gsp >> 2) & ((1 << m_gsp) - 1);  // removing the last two bits
  
        gshare_values.gshare_predictions++; // counting number of predictions
        
        counter = gshare_counter_table[temp_addr];
        
        // making prediction
        if(counter>=2)
        {
            predict = 't';
        }
        else
        {
            predict = 'n';
        }
        
        //checking for misprediction
        if(predict != branch_type_gsp)
        {
            gshare_values.gshare_mispredictions++;
        }

        // counter increment conditions
        if(branch_type_gsp == 't')
        {
            if(counter < 3)
            {
                counter++;
            }
        }
        else
        {
            if(counter > 0)
            {
                counter--;
            }
        }
        gshare_counter_table[temp_addr] = counter;
    }
    return gshare_values;   

}

///////////////////////////////////////////////////////////////// HYBRID FUNCTION ///////////////////////////////////////////////////////////////////

hybrid hybrid_predictor(unsigned long int k_hybrid, unsigned long int m1_hybrid, unsigned long int n_hybrid, unsigned long int m2_hybrid, std::vector<int>& gbh_hybrid, char branch_type_hybrid, unsigned long int address_hybrid)
{
    // creating chooser table
    unsigned long int temp_addr_hybrid; //stores temporary address that contains m+1 -> 2 bits of address
    char predict_hybrid;
    int counter_hybrid;    

    // Extraction of bits from m+1 -> 2
    temp_addr_hybrid = (address_hybrid >> 2) & ((1 << k_hybrid) - 1);  // removing the last two bits

    hybrid_values.hybrid_predictions++;

    counter_hybrid = hybrid_counter_table[temp_addr_hybrid];

    //BIMODAL
    unsigned long int temp_addr_bimod; //stores temporary address that contains m+1 -> 2 bits of address
    char predict_bimod;
    int counter_bimod;

    // Extraction of bits from m+1 -> 2
    temp_addr_bimod = (address_hybrid >> 2) & ((1 << m2_hybrid) - 1);  // removing the last two bits
  
        
    counter_bimod = bmp_counter_table[temp_addr_bimod];
        
    // making prediction
    if(counter_bimod>=2)
    {
        predict_bimod = 't';
    }
    else
    {
        predict_bimod = 'n';
    }

    //gshare
    unsigned long int temp_addr_gshare; //stores temporary address that contains m+1 -> 2 bits of address
    char predict_gshare;
    int counter_gshare;

    // Extraction of bits from m+1 -> 2
    temp_addr_gshare = (address_hybrid >> 2) & ((1 << m1_hybrid) - 1);  // removing the last two bits

    //extract n upper bits of temp_addr
    unsigned long int lower_bits = temp_addr_gshare & ((1 << (m1_hybrid - n_hybrid)) - 1);
    unsigned long int upper_bits = temp_addr_gshare >> (m1_hybrid - n_hybrid);
 
    unsigned long int gbh_dec_value = 0;
    for(int i = 0; i<n_hybrid; i++)
    {
        gbh_dec_value |= (gbh_hybrid[i] << (n_hybrid - 1 - i));
    }

    unsigned long int result = upper_bits ^ gbh_dec_value;

    unsigned long decimal = (result << (m1_hybrid - n_hybrid)) | lower_bits;

    //gshare_values.gshare_predictions++; // counting number of predictions
        
    counter_gshare = gshare_counter_table[decimal];
        
    // making prediction
    if(counter_gshare>=2)
    {
        predict_gshare = 't';
    }
    else
    {
        predict_gshare = 'n';
    }

    bool gshare_select = false; // to check which predictor is used

    //chooser table prediction
    if(counter_hybrid>=2)
    {
        predict_hybrid = predict_gshare;
        gshare_select = true;
    }
    else
    {
        predict_hybrid = predict_bimod;
    }

    if(gshare_select)
    {

        if(branch_type_hybrid == 't')
        {
            if(counter_gshare < 3)
            {
                counter_gshare++;
            }
        }
        else
        {
            if(counter_gshare > 0)
            {
                counter_gshare--;
            }
        }
        gshare_counter_table[decimal] = counter_gshare;        
    }

    else
    {

        if(branch_type_hybrid == 't')
        {
            if(counter_bimod < 3)
            {
                counter_bimod++;
            }
        }
        else
        {
            if(counter_bimod > 0)
            {
                counter_bimod--;
            }
        }
        bmp_counter_table[temp_addr_bimod] = counter_bimod;
    }

    // updating the global counter irrespective of the selected branch
    for(int i = n_hybrid - 1; i>0 ; i--)
    {
        gbh_hybrid[i] = gbh_hybrid[i-1];
    }
    if(branch_type_hybrid == 't')
    {
        gbh_hybrid[0] = 1;
    }
    else
    {
        gbh_hybrid[0] = 0;
    }   

    //updating chooser
    if(predict_hybrid != branch_type_hybrid)
    {
        hybrid_values.hybrid_mispredictions++;
    }
    if(predict_gshare == branch_type_hybrid && predict_bimod != branch_type_hybrid)
    {
        if(counter_hybrid < 3)
        {
            counter_hybrid++;
        }
    }

    if(predict_gshare != branch_type_hybrid && predict_bimod == branch_type_hybrid)
    {
        if(counter_hybrid > 0)
        {
            counter_hybrid--;
        }
    }
    hybrid_counter_table[temp_addr_hybrid] = counter_hybrid;
    return hybrid_values;  
}