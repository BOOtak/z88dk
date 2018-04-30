/*
Z88DK Z80 Macro Assembler

Copyright (C) Gunther Strube, InterLogic 1993-99
Copyright (C) Paulo Custodio, 2011-2017
License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
Repository: https://github.com/pauloscustodio/z88dk-z80asm

Handle library file contruction, reading and writing
*/

#include "errors.h"
#include "fileutil.h"
#include "zfileutil.h"
#include "libfile.h"
#include "zobjfile.h"
#include "options.h"

char Z80libhdr[] = "Z80LMF" OBJ_VERSION;

/*-----------------------------------------------------------------------------
*	define a library file name from the command line
*----------------------------------------------------------------------------*/
static char *search_libfile( char *filename )
{
	if ( filename != NULL && *filename != '\0' )	/* not empty */
		return get_lib_filename( filename );		/* add '.lib' extension */
	else
	{
		error_not_lib_file(filename);
        return NULL;
	}
}

/*-----------------------------------------------------------------------------
*	make library from list of files; convert each source to object file name 
*----------------------------------------------------------------------------*/
void make_library(char *lib_filename, UT_array *src_files)
{
	ByteArray *obj_file_data;
	FILE	*lib_file;
	char	*obj_filename;
	size_t	 fptr, obj_size;
	char **pfile;

	lib_filename = search_libfile(lib_filename);
	if ( lib_filename == NULL )
		return;					/* ERROR */

    if ( opts.verbose )
        printf("Creating library '%s'\n", lib_filename );

	/* write library header */
	lib_file = xfopen( lib_filename, "wb" );	
	xfput_strz( lib_file, Z80libhdr );

	/* write each object file */
	for (pfile = NULL; (pfile = (char **)utarray_next(src_files, pfile)) != NULL; )
	{
		fptr = ftell( lib_file );

		/* read object file */
		obj_filename  = get_obj_filename( *pfile );
		obj_file_data = read_obj_file_data( obj_filename );
		if ( obj_file_data == NULL )
		{
			xfclose(lib_file);			/* error */
			remove(lib_filename);
			return;
		}

		/* write file pointer of next file, or -1 if last */
		obj_size = ByteArray_size( obj_file_data );
		if (pfile == (char **)utarray_back(src_files))
			xfput_uint32(lib_file, -1);
        else
            xfput_uint32( lib_file, fptr + 4 + 4 + obj_size ); 

		/* write module size */
        xfput_uint32( lib_file, obj_size );

		/* write module */
		xfwrite_bytes((char *)ByteArray_item(obj_file_data, 0), obj_size, lib_file);
	}

	/* close and write lib file */
	xfclose( lib_file );
}

bool check_library_file(char *src_filename)
{
	return check_obj_lib_file(
		get_lib_filename(src_filename),
		Z80libhdr,
		error_not_lib_file,
		error_lib_file_version);
}
