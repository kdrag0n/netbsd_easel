/*	$NetBSD: audio.h,v 1.1.1.1 2000/03/29 12:38:48 simonb Exp $	*/

/*
 * Header file for audio drivers
 */
#include "ntp_types.h"

#define AUDIO_BUFSIZ    160     /* codec buffer size (Solaris only) */

/*
 * Function prototypes
 */
int	audio_init		P((void));
int	audio_gain		P((int, int));
void	audio_show		P((void));
