SARG( 'p', pattern, "pattern (must contain a %i)", 0 );
SARG( 'f', flat_field, "flat field", 0 );
SARG( 'd', result_file, "result file", 0 );
SARG( 'm', mask, "mask (white=take into account)", 0 );
SARG( 'i', u_init, "initial value for U", 0 );
SARG( 'r', residual_img, "where to save the (png) residual if needed", 0 );
IARG(  0 , x_min, "x pos of the first ROI diag", 0 );
IARG(  0 , y_min, "y pos of the first ROI diag", 0 );
IARG(  0 , x_max, "x pos of the second ROI diag", 100000 );
IARG(  0 , y_max, "y pos of the second ROI diag", 100000 );
DARG(  0 , fc, "filter coeff", 0.1 );

