#include "a3_omp.h"

inline void bonusVideo(int g, const char *output_file) {
#define BUF_SIZE (512)
  char buf[BUF_SIZE];
  char output_file_basename_increment[BUF_SIZE / 4];
  int  numZeros = 9;
  char zeroFill[12];

  bzero(zeroFill, 12);
  bzero(buf,      BUF_SIZE);

  int count = 0;
  int newg  = g;

  for (; newg / 10 > 0; numZeros--) newg = newg / 10;
  for (; count < numZeros; count++) zeroFill[count] = '0';
  
  sprintf(output_file_basename_increment,
          "%s%d",
          zeroFill,
          g);
  sprintf(buf,
          "convert %s ./bonus_%s/%s.png",
          output_file,
          output_file,
          output_file_basename_increment);
  system(buf);
}

inline void monitor(int         g,
                    const char *output_file,
                    int         width,
                    int         height,
                    int         max,
                    Individual *ind,
                    double      prev_fitness,
                    double     *old_time,
                    double     *new_time) {
  double current_fitness = ind->fitness;
  double change          =
    -(current_fitness - prev_fitness) / current_fitness * 100;

  if (g % MON_GEN == 0) {
    writePPM(output_file,
             width,
             height,
             max,
             ind->image);

#ifdef BONUS_VIDEO
    bonusVideo(g, output_file);
#endif /* ifdef BONUS_VIDEO */

    *old_time = *new_time;
    *new_time = omp_get_wtime();

    printf(
      " generation % 5d fitness %e  change from prev %.2e%c  time [%f] seconds \n",
      g,
      current_fitness,
      change,
      37,
      *new_time - *old_time);
   }
}

