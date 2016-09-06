# x265-Transcoder

This application, encode an raw image (.yuv) into High Efficiency Video Coding format (x265). After the initial encoding (which generates additional data necessary for the post processing), transcoder can convert the image approx 10 times faster.

### Commands:

*    --transcoder-file {path_to_the_transcoder_temp_folder}
     
*   --transcoder-mode {value}
     * 0 [default] - first step (encoding) 
     * 1 - second step (transcoding)

*   --ones-throw {value}
     * by default "0", it means how many ones you want to remove

*   --throw-prop {value in percent}
     * by defualt 0 (0->100) which means that ones can't be removed in whole TU (PU), this value tells how big is safe  area within TU (ones will not be removed from "x" percent of TU)  e.g "34" means that only 66% of TU will be    scanned in order to remove ones 


### Usage example:
    x265.exe --input D:\Work\Source\Crew_1280x704.yuv --output D:\Work\encCoder.h265  --input-res 1280x704 --frames 25  --fps 25 --no-signhide --transcoder-file D:\Work\RawData --transcoder-mode 0 --ones-throw 0 --throw-prop 1
