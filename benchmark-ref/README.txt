To obtain the results of the primate referential compression execute run-primates.sh on the current folder.

This will print one line per chromosome pair with the following format: r, t, r_id, t_id, ac2_bps_rt, ac_bps_rt

where:
	* r          = referential file name
	* t          = target file name
	* r_id       = the referential species id (HS)
	* t_id       = the target species id (GG, PT, PA)
	* ac2_bps_rt = number of bits per symbol without header produced by AC2
	* ac_bps_rt  = number of bits per symbol without header produced by AC
