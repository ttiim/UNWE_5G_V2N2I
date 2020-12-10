/**
 * TSCBM Message parser example
 * Tyler Worman (tworman@umich.edu)
 * 8/8/19 - Initial version of test script.
 * 
 *  Credit goes to Authors at: https://github.com/mcity/Mcity-V2X-TSCBM 
 *  used code from: https://www.geeksforgeeks.org/program-to-convert-hexadecimal-number-to-binary/ 
 *   Re-used for a demo by UBC RSL Group
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE* openFileRead(char* filename);

/*
 * Attempt to open the file for read access.
 * Peforms error check and exits if file is not accessible
 * Never actually called this function...
 */
 FILE* openFileRead(char* filename)
 {
    FILE* inFile = fopen(filename, "r" );
    if( inFile == NULL) {
       printf( "Error opening input file %s, program terminating!\n", filename);
       exit(EXIT_FAILURE);
    }
    return inFile;
 }

/**
    bin2int(const char *s)
    Takes a null terminated bit string and converst it to an integer
    11111111 = 256
    00000000 = 0
 */



int bin2int(const char *s) {
  return (int) strtol(s, NULL, 2);
}

char *parseTSCBM(char *in, char *id, char* in_time)
{
    char sub[25]; //Use this as temp storage for mem copy.
    sub[24] = '\0';
    char *buf = "{";
   // printf("DEBUG_TEST_PASS_1:\n");
    

    //GNU C string builder
    //Get number of phases in this packet (should always be 16 but check..)
    memcpy(sub, &in[8], 8);
    sub[8] = '\0';
    int phase_cnt = bin2int(sub);
    if (phase_cnt != 16) {
        exit(1);
    }

    //Build the message format we expect.
    asprintf(&buf,"%s%s%s%s",buf,"\"id\": \"", id, "\", \"messageSet\": \"NTCIP_UNWE_UBCV\", ");
    asprintf(&buf,"%s%s%s%s",buf,"\"updated\": \"", in_time, "\" , ");

    // Byte 236-238: SystemSeconds (3 byte)	(sys-clock seconds in day 0-84600)     
    // Byte 239-240: SystemMilliSeconds (2 byte)	(sys-clock milliseconds 0-999)  
    memcpy(sub, &in[8*236], 24);
    asprintf(&buf,"%s%s%d%s",buf,"\"timeSystem\": \"", bin2int(sub), ".");
    memcpy(sub, &in[8*239], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%d%s",buf, bin2int(sub), "\", ");

    //Colors
    // bytes 210-215: PhaseStatusReds, Yellows, Greens	(2 bytes bit-mapped for phases 1-16)
    memcpy(sub, &in[8*214], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%s%s%s", buf, "\"green\": \"", sub, "\", ");

    memcpy(sub, &in[8*212], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%s%s%s", buf, "\"yellow\": \"", sub, "\", ");

    memcpy(sub, &in[8*210], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%s%s%s", buf, "\"red\": \"", sub, "\", ");

    //Peds
    //# bytes 216-221: PhaseStatusDontWalks, PhaseStatusPedClears, PhaseStatusWalks (2 bytes bit-mapped for phases 1-16)
    memcpy(sub, &in[8*220], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%s%s%s", buf, "\"walk\": \"", sub, "\", ");

    memcpy(sub, &in[8*216], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%s%s%s", buf, "\"walkDont\": \"", sub, "\", ");

    memcpy(sub, &in[8*218], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%s%s%s", buf, "\"pedestrianClear\": \"", sub, "\", ");
    
    //Flash
    //# bytes 228-229: FlashingOutputPhaseStatus	(2 bytes bit-mapped for phases 1-16)
    //# bytes 230-231: FlashingOutputOverlapStatus	(2 bytes bit-mapped for overlaps 1-16)
    memcpy(sub, &in[8*228], 16);
    sub[16] = '\0';
    asprintf(&buf,"%s%s%s%s", buf, "\"flash\": \"", sub, "\", ");

    //Loop phases
    //Get number of phases in this packet (should always be 16 but check..)
    //'phases': [
    asprintf(&buf,"%s%s", buf, "\"phases\": [");

    int i;
    for (i=0;i < phase_cnt; i++) {
        if (i >  0) {
            asprintf(&buf,"%s%s%d%s",buf, ",{\"phase\": ", i + 1, ", ");
        } else {
            asprintf(&buf,"%s%s%d%s",buf, "{\"phase\": ", i + 1, ", ");
        }
        //Each Phase is 13 bytes (8 bits)
        
        //Color is stored in multiple places.
        //If it's not Green/Yellow it's red, so must only check 2.
        memcpy(sub, &in[8*214+15-i], 1);
        sub[2] = '\0';
        if (sub[0] == '0') {
            //It's not Green, check yellow
            memcpy(sub, &in[8*212+15-i], 1);
            if (sub[0] == '0') {
                //It's not yellow or green
                asprintf(&buf,"%s%s", buf, "\"color\": \"RED\", ");
            } else {
                asprintf(&buf,"%s%s", buf, "\"color\": \"YELLOW\", ");
            }
        } else {
            asprintf(&buf,"%s%s", buf, "\"color\": \"GREEN\", ");
        }

        //Flash is stored at byte 228. 
        //Its format is 16 bit decending. So Phase 1 is stored at 228 + 15 bits
        memcpy(sub, &in[8*228+15-i], 1);
        sub[1] = '\0';
        if (sub[0] == '0') {
            asprintf(&buf,"%s%s", buf, "\"flash\": false, ");
        } else {
            asprintf(&buf,"%s%s", buf, "\"flash\": true, ");
        }
        
        memcpy(sub, &in[8*3+(i*13*8)], 16);
        sub[16] = '\0';
        asprintf(&buf,"%s%s%d%s", buf, "\"vehTimeMin\": ", bin2int(sub), ", ");
        
        memcpy(sub, &in[8*5+(i*13*8)], 16);
        sub[16] = '\0';
        asprintf(&buf,"%s%s%d%s", buf, "\"vehTimeMax\": ", bin2int(sub), ", ");
                
        memcpy(sub, &in[8*7+(i*13*8)], 16);
        sub[16] = '\0';
        asprintf(&buf,"%s%s%d%s", buf, "\"pedTimeMin\": ", bin2int(sub), ", ");
                
        memcpy(sub, &in[8*218], 16);
        sub[16] = '\0';
        asprintf(&buf,"%s%s%d%s", buf, "\"pedTimeMax\": ", bin2int(sub), "}");
        
    }
    asprintf(&buf,"%s]}", buf);
    return buf;
}


// C program to convert
// Hexadecimal number to Binary
 
// function to convert Hexadecimal to Binary Number
// void HexToBin(char* hexdec)
// {
 
//     long int i = 0;
 
//     while (hexdec[i]) {
 
//         switch (hexdec[i]) {
//         case '0':
//              // strcpy(binAddress + j, "0000");
//             printf("0000");
//             break;
//         case '1':
//             printf("0001");
//             break;
//         case '2':
//             printf("0010");
//             break;
//         case '3':
//             printf("0011");
//             break;
//         case '4':
//             printf("0100");
//             break;
//         case '5':
//             printf("0101");
//             break;
//         case '6':
//             printf("0110");
//             break;
//         case '7':
//             printf("0111");
//             break;
//         case '8':
//             printf("1000");
//             break;
//         case '9':
//             printf("1001");
//             break;
//         case 'A':
//         case 'a':
//             printf("1010");
//             break;
//         case 'B':
//         case 'b':
//             printf("1011");
//             break;
//         case 'C':
//         case 'c':
//             printf("1100");
//             break;
//         case 'D':
//         case 'd':
//             printf("1101");
//             break;
//         case 'E':
//         case 'e':
//             printf("1110");
//             break;
//         case 'F':
//         case 'f':
//             printf("1111");
//             break;
//         default:
//             printf("\nInvalid hexadecimal digit %c",
//                    hexdec[i]);
//         }
//         i++;
//     }
// }
 //void HexToBin(char* hexdec)
 void hexToBin(char* address, char* binAddress){
    int i, j;

    for(j = i = 0; address[i]; ++i, j += 4){
        switch(address[i]){
        case '0': 
            strcpy(binAddress + j, "0000"); 
            break;
        case '1': 
            strcpy(binAddress + j, "0001");
            break;
        case '2': 
            strcpy(binAddress + j, "0010"); 
            break;
        case '3':
             strcpy(binAddress + j, "0011"); 
             break;
        case '4': 
            strcpy(binAddress + j, "0100"); 
            break;
        case '5': 
            strcpy(binAddress + j, "0101"); 
            break;
        case '6': 
            strcpy(binAddress + j, "0110"); 
            break;
        case '7': 
            strcpy(binAddress + j, "0111"); 
            break;
        case '8': 
            strcpy(binAddress + j, "1000"); 
            break;
        case '9': 
            strcpy(binAddress + j, "1001"); 
            break;
        case 'a': 
            strcpy(binAddress + j, "1010"); 
            break;
        case 'b': 
            strcpy(binAddress + j, "1011"); 
            break;
        case 'c': 
            strcpy(binAddress + j, "1100"); 
            break;
        case 'd': 
            strcpy(binAddress + j, "1101"); 
            break;
        case 'e': 
            strcpy(binAddress + j, "1110"); 
            break;
        case 'f': 
            strcpy(binAddress + j, "1111"); 
            break;
        default:
            printf("invalid character %c\n", address[i]);
            strcpy(binAddress + j, "0000"); 
            break;
        }
    }
}


/**
 * Sample code building a string/char array buffer with 
 * resulting JSON formatted text resulting from the parsing of
 * a sample TSCBM formatted binary input string.
 * 
 * This is not the most efficiently written version, we attempted easy to read.
 * Easy improvements you could make:
 * - Combine buffer appends
 * - Move away from GNU apsrintf to a POSIX version with a fixed/non-dynamic buffer sizes
 * - Directly access the values from the char* input string through index instead of memcopy for each value.
 * - Return a native typed object to your calling code instead of a char * [].
 * -
 */
int main(int ac, char *av[])
{
  //  char* in = "1100110100010000000000010000000011011100000000101010101000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000001111101000000001101110000000010101010100000000000000000000000000000000000000011000000001101110000000001110110110000000000000000000000000000000000000000000000000000000000000000000001000000000000111111000000001011110000000000001111110000000010111100000000000000000000000000000000000000010100000000001111110000001011010100000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000000000100100110000000000111111000000101101010000000000000000000000000000000000000001110000000000111111000000001101001000000000000000000000000000000000000000000000000000000000000000000000100000000000001111110000000110100001000000000011111100000001101000010000000000000000000000000000000000001001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000101100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011011101000000000000000000000000001000100000000011111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001011101000000000011111011001010000000111100111000000000000000000000000000000000";
    //char* in =  "cd1001002804920000000000000000020000015e000000000000000003002801860000000000000000040028030c0028030c0000000005002804920000000000000000060000015e000000000000000007002801860000000000000000080028030c0028030c00000000090000000000000000000000000a0000000000000000000000000b0000000000000000000000000c0000000000000000000000000d0000000000000000000000000e0000000000000000000000000f00000000000000000000000010000000000000000000000000ffdd00000022ffff00000000ffff000000000000000000000880011c4d0000 "; //"11001101000100000000000100000000001010000000010010010010000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000001010111100000000000000000000000000000000000000000000000000000000000000000000000110000000000101000000000011000011000000000000000000000000000000000000000000000000000000000000000000000010000000000001010000000001100001100000000000010100000000011000011000000000000000000000000000000000000000101000000000010100000000100100100100000000000000000000000000000000000000000000000000000000000000000000001100000000000000000000000010101111000000000000000000000000000000000000000000000000000000000000000000000011100000000001010000000000110000110000000000000000000000000000000000000000000000000000000000000000000001000000000000010100000000011000011000000000000101000000000110000110000000000000000000000000000000000000010010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000101000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001011000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111110111010000000000000000000000000010001011111111111111110000000000000000000000000000000011111111111111110000000000000000000000000000000000000000000000000000000000000000000000000000000000001000101010110000000100011101101110000000000000000000";
     // Get the Hexadecimal number
    //char* in = "cd1001002804920000000000000000020000015e000000000000000003002801860000000000000000040028030c0028030c0000000005002804920000000000000000060000015e000000000000000007002801860000000000000000080028030c0028030c00000000090000000000000000000000000a0000000000000000000000000b0000000000000000000000000c0000000000000000000000000d0000000000000000000000000e0000000000000000000000000f00000000000000000000000010000000000000000000000000ffdd00000022ffff00000000ffff000000000000000000000880011c4d0000";
    //printf("\n%s\n",in);
    
    FILE *fp;
    char str[482+1];

   /* opening file for reading */
     fp = fopen("data_UDP", "r");
     if(fp == NULL) {
       perror("Error opening file");
       return(-1);
     }

    if( fgets (str, 482+1, fp)!=NULL ) {
      /* writing content to stdout */
    //  puts(str);
    }
    fclose(fp);
   
    //printf("\n %s\n",str);
    // printf("\nLength of hex string from file= %zu \n",strlen(str));

  //   printf("\nLength of hex string = %zu \n",strlen(in));
     int bin_len = 4*strlen(str);
  //   printf("\nLength of binary string = %u \n",bin_len);
   
    // // Convert HexaDecimal to Binary
  //   printf("\nEquivalent Binary value is : \n");
     //HexToBin(in);  // TEST_FUNCTION
     char bin_out[1+bin_len];
     hexToBin(str, bin_out);
   //  printf("%s\n", bin_out);
     char* bin_in= bin_out;
   //  printf("\nLength of binary string operated on = %zu \n",strlen(bin_in));
     char *id = "1";
     char *time = "optional_insert_time_string";
     
     /*
        This is the parser/ decoder function:
     */
     char* out= parseTSCBM(bin_in, id, time);
    printf("\n %s\n",out);
   //  printf("Length of Parsed_TSCBM= %ld\n",sizeof(out));


    // // creating file pointer to work with files
    FILE *fp_out;
    // // opening file in writing mode
     fp_out = fopen("Parsed_TSCBM.txt", "w");
     if (fp_out == NULL) {
         printf("Error!");
         exit(1);
     }
     fputs(out, fp_out);
     fclose(fp_out);
    return 0;
}
