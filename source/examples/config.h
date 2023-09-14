#ifndef CONFIG_H
#define CONFIG_H

/*!
 *  \file pakcage debugging config file
 *  \details the file defines log writing directive macro,
 *  directories and basic log files, that are written
*/

// write logs
#define WRITE_LOGS

// define names of logs directory and output signals
#define LOGS_DIRECTORY (string)"C:\\Qt_projects\\DSP\\logs"
#define OUTPUT_STREAM_INPUT (string)"\\xt.txt"
#define OUTPUT_STREAM_OUTPUT (string)"\\yt.txt"
#define OUTPUT_STREAM_TIME (string)"\\dt.txt"
#define OUTPUT_STREAM_PHASE_RESPONSE (string)"\\pH.txt"
#define OUTPUT_STREAM_AMPLITUDE_RESPONSE (string)"\\Km.txt"
#define OUTPUT_STREAM_REAL_COMPONENT (string)"\\re.txt"
#define OUTPUT_STREAM_IMAG_COMPONENT (string)"\\im.txt"
#define OUTPUT_STREAM_SIGNAL_AMPLITUDE (string)"\\am.txt"

#endif // CONFIG_H
