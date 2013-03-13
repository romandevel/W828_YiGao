

void (*gps_reinit)(void) ;
void (*gps_deinit)(void) ;

void gps_reintHandle(  void (*preint) (void))
{
	gps_reinit = preint;
}

void gps_deintHandle(  void (*pdeint) (void))
{
	gps_deinit = pdeint;
}