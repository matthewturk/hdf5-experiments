#include "hdf5.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/time.h"

#define READ_FIELD(A) field = H5Dopen(grid_id, A, H5P_DEFAULT);\
                      if(field < 0)return 1;\
                      if (to_read == 1) \
                        H5Dread(field, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, \
                                H5P_DEFAULT, data);\
                      H5Dclose(field);

herr_t visit_grid(hid_t group_id, const char *member_name, 
                  const H5L_info_t *info, void *op_data)

{
    if(member_name[0] != 'G') return 0;
    hid_t grid_id = H5Gopen(group_id, member_name, H5P_DEFAULT);
    int to_read = ((int *) op_data)[0];
    /* We do this once for Density, then we will only do open and read for the
       rest of the known fields */
    hid_t field = H5Dopen(grid_id, "Density", H5P_DEFAULT);
    hid_t space = H5Dget_space(field);
    hssize_t np = H5Sget_simple_extent_npoints(space);
    H5Sclose(space);
    H5Dclose(field);
    void *data = malloc(np * sizeof(double));

    READ_FIELD("DII_Density")
    READ_FIELD("DI_Density")
    READ_FIELD("Density")
    READ_FIELD("Electron_Density")
    READ_FIELD("GasEnergy")
    READ_FIELD("H2II_Density")
    READ_FIELD("H2I_Density")
    READ_FIELD("H2I_kdiss")
    READ_FIELD("HDI_Density")
    READ_FIELD("HII_Density")
    READ_FIELD("HI_Density")
    READ_FIELD("HI_kph")
    READ_FIELD("HM_Density")
    READ_FIELD("HeIII_Density")
    READ_FIELD("HeII_Density")
    READ_FIELD("HeII_kph")
    READ_FIELD("HeI_Density")
    READ_FIELD("HeI_kph")
    READ_FIELD("PhotoGamma")
    READ_FIELD("Temperature")
    READ_FIELD("TotalEnergy")
    READ_FIELD("x-velocity")
    READ_FIELD("y-velocity")
    READ_FIELD("z-velocity")

    H5Gclose(grid_id);
    return 0;
}

/* From
http://stackoverflow.com/questions/1468596/c-programming-calculate-elapsed-time-in-milliseconds-unix
*/ 

int timeval_subtract(struct timeval *result, struct timeval *t2,
                     struct timeval *t1)
{
  long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec +
      1000000 * t1->tv_sec);
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;

  return (diff<0);
}

#define NRUNS 1000

int main()
{
    int read, i;
    struct timeval t1, t2, dt;
    H5L_iterate_t vg = visit_grid;
    hid_t file_handle;
    for (read = 0; read < 2; read++) {
        gettimeofday(&t1, NULL);
        for (i = 0; i < NRUNS; i++) {
            file_handle = H5Fopen(
                    "output_0039.cpu0000",
                    H5F_ACC_RDONLY, H5P_DEFAULT);
            if (file_handle < 0) {fprintf(stderr,"BROKEN FILE\n"); return(1);}
            H5Literate(file_handle, H5_INDEX_NAME, H5_ITER_NATIVE, NULL,
                       vg, (void *) &read);
            H5Fclose(file_handle);
        }
        gettimeofday(&t2, NULL);
        timeval_subtract(&dt, &t2, &t1);
        fprintf(stdout, "Read: %d with time %ld.%05ld over %d runs\n", read, 
                dt.tv_sec, dt.tv_usec, NRUNS);
    }
}
