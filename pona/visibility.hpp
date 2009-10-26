#ifdef PONA_SHARED_LIBRARY
	#ifdef __GNUC__
		#if __GNUC__ >= 4
			#define PONA_API __attribute__((visibility("default")))
			#define PONA_INTERN __attribute__((visibility("hidden")))
		#endif
	#endif
#endif
#ifndef PONA_API
	#define PONA_API
#endif
#ifndef PONA_INTERN
	#define PONA_INTERN
#endif
