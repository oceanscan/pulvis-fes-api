/* This file is part of FES library.

   FES is free software: you can redistribute it and/or modify
   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FES is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU LESSER GENERAL PUBLIC LICENSE for more details.

   You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
   along with FES.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fes.h"

// Path to the configuration file and data used to test the library
// Change these settings to your liking.
#ifndef INI
#define INI "../data/fes2022b/fes2022.ini"
#endif
#ifndef FES_DATA
#define FES_DATA "../data/fes2022b"
#endif

// Function to convert epoch time to Julian days
double epoch_to_julian_days(double epoch_time)
{
  // Calculate Modified Julian Day (MJD)
  double mjd = (epoch_time / 86400.0) + 40587.0;

  // Convert to CNES Julian days
  double cnes_julian_days = mjd - 33282.0;

  return cnes_julian_days;
}

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    fprintf(stderr, "Usage: %s <epoch_time> <latitude> <longitude>\n", argv[0]);
    return 1;
  }

  // Parse command-line arguments
  double epoch_time = atof(argv[1]);
  // Latitude and longitude of the point where the ocean tide will be
  // evaluated.
  double lat = atof(argv[2]);
  double lon = atof(argv[3]);

  // Convert epoch time to Julian days
  // Time in CNES Julian days, defined as Modified Julian Day minus 33282.
  // Thus CNES 0 is at midnight between the 31 December and 01 January 1950
  // AD Gregorian.
  double time = epoch_to_julian_days(epoch_time);
  //printf("CNES Julian days = %f\n", time);
  // The return code
  int rc = 0;

  // Short tides (semi_diurnal and diurnal tides)
  double tide;
  // Long period tides
  double lp;
  // Loading effects for short tide
  double load;
  // Loading effects for long period tides (is always equal to zero)
  double loadlp;
  // FES handlers
  FES short_tide;
  FES radial_tide = NULL;

  setenv("FES_DATA", FES_DATA, 1);

  // Creating the FES handler to calculate the ocean tide
  if (fes_new(&short_tide, FES_TIDE, FES_IO, INI))
  {
    printf("fes error : %s\n", fes_error(short_tide));
    goto error;
  }

  // Creating the FES handler to calculate the loading tide
  if (fes_new(&radial_tide, FES_RADIAL, FES_IO, INI))
  {
    printf("fes error : %s\n", fes_error(radial_tide));
    goto error;
  }

  // Compute ocean tide
  if (fes_core(short_tide, lat, lon, time, &tide, &lp))
  {
    fprintf(stderr, "%s\n", fes_error(short_tide));
    goto error;
  }

  // Compute loading tide
  if (fes_core(radial_tide, lat, lon, time, &load, &loadlp))
  {
    fprintf(stderr, "%s\n", fes_error(radial_tide));
    goto error;
  }

  // tide + lp        = pure tide (as seen by a tide gauge)
  // tide + lp + load = geocentric tide (as seen by a satellite)
  printf("%f\n", tide + lp + load);

  goto finish;

error:
  rc = 1;

finish:
  // Release the memory used by the FES handlers.
  fes_delete(short_tide);
  fes_delete(radial_tide);

  return rc;
}
