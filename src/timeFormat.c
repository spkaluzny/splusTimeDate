/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for converting dates and times
 * to and from strings.
 * 
 * See tiemFormat.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 *************************************************************************/

#include "timeFormat.h" 
#include <stdio.h>

/* internal function declarations -- see end of file for defs/docs */
static int format_style(const char *format_string );
static int old_to_new(const char *old_format, char **new_format, 
		       int isdate, int isout );
int count_out_size( char *new_format, int abb_size, int full_size, 
		    int zone_size );  
static int out_width( char spec_char, int abb_size, int full_size, 
		      int zone_size );
static int output_one( char **out_buf, TIME_DATE_STRUCT td, 
		       TIME_OPT_STRUCT topt, char spec_char, 
		       int field_width, int zero_pad );
static int parse_input( char **input_string, char **format_string, 
			TIME_OPT_STRUCT topt, TIME_DATE_STRUCT *td_output, 
			char stopchar );
static int input_one( char **input_string, TIME_OPT_STRUCT topt, 
		      char spec_char, int width, char delim, 
		      TIME_DATE_STRUCT *td_out );
static int match_index( char **str_array, int array_len, char *match_str );

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME new_out_format

   DESCRIPTION  Take an ``old-style'' or ``new-style'' R date/time
   output format and convert it into a newly allocated new-style format, 
   counting up how Sint formatted dates/times will be.  The string
   is allocated using Ralloc(), so it will be freed when the current
   frame exits in R.

   ARGUMENTS
      IARG  old_format   the input format string
      OARG  new_format   the new-style-output format string
      IARG  abb_size     maximum size of abbreviations
      IARG  full_size    maximum size of day/month words
      IARG  zone_size    maximum size of time zones

   RETURN Returns 0 if there is an error; otherwise, the maximum length to 
   allocate for time strings formatted with this format.

   ALGORITHM The function calls format_style to determine the format 
   type, and then converts it to new format if necessary, using 
   old_to_new.  Then it calculates the string length needed for formatted 
   output by calling count_out_size, using the abb_size, full_size, 
   and zone_size arguments.

   EXCEPTIONS 

   NOTE See also new_in_format, mdyt_format

**********************************************************************/
int new_out_format( const char *old_format, char **new_format, 
		    int abb_size, int full_size, int zone_size )
{
  int style, len;

  //*new_format = NULL;
  len = strlen( old_format );

  if( !len )
    return 0;

  /* see if it's old or new style */

  style = format_style( old_format );

  if(( style < 1 ) || ( style > 3 ))
    return 0;

  /* convert or copy to new format */

  if( style == 2 )
  {

    if( !old_to_new( old_format, new_format, 1, 1 ))
      return 0;
  } else if( style == 3 ) 
  {
    if( !old_to_new( old_format, new_format, 0, 1 ))
      return 0;
  } else /* already was a new format */
  {
    *new_format = R_alloc( len + 1, sizeof(char) );
    strcpy( *new_format, old_format );
  }

  /* count the size required */
  len = count_out_size( *new_format, abb_size, full_size, zone_size );

  if( !len )
    return 0;

  return( len );
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME new_in_format

   DESCRIPTION  Take an ``old-style'' or ``new-style'' R date/time
   input format and convert it into a newly allocated new-style format.  
   The string is allocated using Ralloc(), so it will be freed automatically.

   ARGUMENTS
      IARG  old_format   the input format string
      OARG  new_format   the new-style-input format string

   RETURN Returns 1/0 for success/failure.

   ALGORITHM The function first decides whether the input is an
   old-style date, old-style time, or new-style format by calling
   the format_style function.  Then it converts old-style 
   formats to new by calling the old_to_new function.

   EXCEPTIONS 

   NOTE See also new_out_format, mdyt_input

**********************************************************************/
int new_in_format( const char *old_format, char **new_format )
{

  int style, len;

  if( !old_format ){
    error("null old_format obect");
    return 0;
  }

  if( !new_format ){
    error("null new_format obect");
    return 0;
  }

  *new_format = NULL;
  len = strlen( old_format );
  if( !len ){
    error("old format has zero length");
    return 0;
  }

  /* see if it's old or new style */
  style = format_style( old_format );

  if(( style < 1 ) || ( style > 3 )){
    error("invalid format style");
    return 0;
  }

  /* convert or copy to new format */

  if( style == 2 )
  {
    if( !old_to_new( old_format, new_format, 1, 0 )){
      error("could not convert format style 2 to new style");
      return 0;
    }
  } else if( style == 3 ) 
  {
    if( !old_to_new( old_format, new_format, 0, 0 )){
      error("could not convert format style 3 to new style");
      return 0;
    }
  } else /* already was a new format */
  {
    *new_format = R_alloc( len + 1, sizeof(char) );
    if( !(*new_format )){
      error("unable to allocate space for new format");
      return 0;
    }
    strcpy( *new_format, old_format );
  }

  return 1;

}

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME mdyt_format

   DESCRIPTION  This function formats a date and time (given by 
   a time-date structure) according to the given format string and options.

   ARGUMENTS
      IARG  td_input        the time-date information
      IARG  format_string   the new-style-output format string
      IARG  topt            options struct for printing dates/times
      OARG  ret_string      the formatted time/date string

   RETURN Returns 1/0 for success/failure.

   ALGORITHM The function uses the new-style format string (see the R 
   documentation on time formats) and the options to print the date/time given
   by the structure into the string, which must be pre-allocated to 
   the correct length (which can be calculated by the new_out_format 
   function).   Each format specification is printed into the string
   by calling the output_one function.

   EXCEPTIONS 

   NOTE See also mdyt_input

**********************************************************************/
int mdyt_format( TIME_DATE_STRUCT td_input, const char *format_string, 
		 TIME_OPT_STRUCT topt, char *ret_string )
{
  char *inpos, *endpos;
  char *outpos, tmp;
  int width, zeropad;

  if( !format_string || !ret_string )
    return 0;

  /* since R caches R string objects, need to copy input */
  inpos = acopy_string(format_string);
  outpos = ret_string;

  /* go through the format string and find specs */

  while(*inpos != '\0' )
  {
    if( *(inpos++) != '%' ) /* not a spec, just copy to output */
    {
      *(outpos++) = *(inpos - 1);
      *(outpos) = '\0';
      continue;
    }

    /* we're now in an output spec */
    if( *inpos == '%' ) /* % character */
    {
      *(outpos++) = '%';
      *(outpos) = '\0';
      inpos++;
      continue;
    }

    /* see if it has a width */

    width = -1;
    zeropad = ( *inpos == '0' );

    endpos = inpos;
    while( isdigit( *endpos )) endpos++;
    if( endpos > inpos ) 
    {
      /* read the width */
      tmp = *endpos;
      *endpos = '\0';
      if( sscanf( inpos, "%d", &width ) != 1 )
	return 0;
      *endpos = tmp;
      inpos = endpos;
    } 

    if( !output_one( &outpos, td_input, topt, *(inpos++), width, zeropad ))
      return 0;
  }

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME mdyt_input

   DESCRIPTION  This function reads a date and time (given by 
   the time/date structure) from an input string according to the 
   given format string and options.

   ARGUMENTS
      IARG  input_string    the formatted time/date string to read
      IARG  format_string   the new-style input format string
      IARG  topt            options for parsing times/dates
      OARG  td_output       the time/date information

   RETURN Returns 1/0 for success/failure.  The routine fails if the 
   format does not match the input, or if the input string is not 
   entirely read (except whitespace).

   ALGORITHM The function calls parse_input to parse the input.

   EXCEPTIONS 

   NOTE All information previously in the td_output structure is lost in this 
   function call. A new string may be allocated for the zone component 
   of the td_output structure, using Ralloc so it will be freed automatically.
   \\
   \\
   See also new_in_format, mdyt_format

**********************************************************************/
int mdyt_input( const char *input_string, char *format_string, 
		TIME_OPT_STRUCT topt, TIME_DATE_STRUCT *td_output )
{
  char *input_end;
  char *inpos;

  /* error checking */
  if( !input_string || !format_string || !td_output )
    return 0;

  /* initialize the return structure */

  td_output->month = 1;
  td_output->day = 1;
  td_output->year = 1960;
  td_output->hour = td_output->minute = td_output->second = td_output->ms = 0;
  td_output->weekday = julian_to_weekday( 0 );
  td_output->yearday = 1;
  td_output->zone = NULL;

  /* parse input with format */

  inpos = acopy_string(input_string);
  input_end = inpos + strlen(inpos);

  if( !parse_input( &inpos, &format_string, topt, td_output, '\0' ))
    return 0;

  /* check to see that input string is used up except whitespace */
  while( inpos && ( inpos < input_end ))
    if( !isspace( *(inpos++)))
      return 0;

  return 1;

}


/***********************
  Internal functions
  *********************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME format_style

   DESCRIPTION  Determine the ``style'' of the given format specification.

   ARGUMENTS
      IARG  format_string   the format string

   RETURN Returns 0 if it is not a format string at all, 1 if it is
   a new-style format string, 2 if it is an old-style date format string,
   and 3 if it is an old-style time format string.

   ALGORITHM The format string is determined to be new-style if it
   contains any \% characters.  Otherwise, it is an old-style date format 
   string if it contains an `m', a `d', and a `y' (case insensitive).  It is
   an old-style time format if it contains an `h', an `m' and an `s'.
   If it contains both, it is not a valid old-style format.

   EXCEPTIONS 

   NOTE See also new_in_format, new_out_format, old_to_new

**********************************************************************/
static int format_style(const char *format_string )
{
  int IsTime, IsDate;

  /* invalid if null */
  if( !format_string )
    return 0;
  
  /* new if it has a % character */
  if( strchr( format_string, '%' ))
    return 1;

  /* old time if it has all of "h", "m", and "s" in it */
  /* old date if it has all of "m", "d", and "y" in it */
  /* both of these conditions True would be an error -- dates should not
     have an s, and times should not have a y */
  IsTime = (( strchr( format_string, 'h' ) || strchr( format_string, 'H' )) &&
	    ( strchr( format_string, 'm' ) || strchr( format_string, 'M' )) &&
	    ( strchr( format_string, 's' ) || strchr( format_string, 'S' )));
  IsDate = (( strchr( format_string, 'd' ) || strchr( format_string, 'D' )) &&
	    ( strchr( format_string, 'm' ) || strchr( format_string, 'M' )) &&
	    ( strchr( format_string, 'y' ) || strchr( format_string, 'Y' )));

  if( IsTime && IsDate )
    return 0;
  if( IsDate )
    return 2;
  if( IsTime )
    return 3;
  return 0;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME old_to_new

   DESCRIPTION  Convert an old style date or time format to a new style 
   output  or input format, allocated with Ralloc().

   ARGUMENTS
      IARG  old_format   the old-style date/time format string
      OARG  new_format   the new-style format string
      IARG  isdate       1/0 for date/time input 
      IARG  isout        1/0 for input/output return format


   RETURN Returns 1/0 for success/failure.

   ALGORITHM The function first finds delimeters (which can be
   hyphen, slash, period, colon, vertical bar, or space for old-style
   time and date formats) in the format string.  Then it converts
   the old-style specs to new-style, with the same delimeters.
   For the most part, this is simple substitution, after deciding 
   whether to use 3-letter abbreviations for months, 4 digits for 
   years, etc.  The only exception is that 
   if there is no delimeter character, the fields are always of width
   two with zero padding. 

   EXCEPTIONS 

   NOTE See also new_out_format, new_in_format

**********************************************************************/
static int old_to_new(const char *old_format, char **new_format, 
		       int isdate, int isout )
{

  int len, sepwidth = 1, i;
  const char *sep1 = NULL, *sep2 = NULL, *old_pos, *next_sep;
  char *new_pos;
  static char sepchars[] = "-/.:| ";
  
  if( !old_format || !new_format )
    return 0;

  *new_format = NULL;

  len = strlen( old_format );
  if( len < 3 )
    return 0;

  /* calculate or locate positions of the separator characters in format */

  if( len == 3 )  /* mdy, hms, etc, with no separators */
  {
    /* separators are obvious */
    sep1 = old_format + 1;
    sep2 = old_format + 2;
    sepwidth = 0;
  } else 
  {  
    /* find the separators */
    for( i = 0; (i < 6 ) && !sep1; i++ )
      sep1 = strchr( old_format, sepchars[i] );
    if( !sep1 ) /* found no separator */
      return 0;
    for( i = 0; (i < 6 ) && !sep2; i++ )
      sep2 = strchr( sep1 + 1, sepchars[i] );
    if( !sep2 ) /* found no separator */
      return 0;
  }

  /* create a new format string */
  /* space needed is 4 per piece + 2 * sepwidth + 1 for null char */

  *new_format = R_alloc( 12 + 2 * sepwidth + 1, sizeof(char) );
  if( !( *new_format ))
    return 0;
  
  /* find the fields and convert them */
  /* see documentation above for a few notes on this process */

  /* march through the old format string, and keep track of what
     we've put into new format string */

  old_pos = old_format;
  new_pos = *new_format;
  next_sep = sep1;

  for( i = 0; i < 3; i++ )  /* three fields */
  {

    switch( *old_pos )
    {
    case 'm':  /* minutes or months */
    case 'M':
      *(new_pos++) = '%';

      if( isdate )  /* is it a date or a time m? */
      {
	if( isout ) /* is it input or output we want? */
	{
	  /* it's a date output spec */

	  /* decide if it's a 2-digit month, abbrev, or full name */
	  if(( (old_pos + 2) >= next_sep ) || /* field less than 3 in width */
	     (( *(old_pos + 1 ) != 'o' ) && ( *(old_pos + 1 ) != 'O' )) ||
	     (( *(old_pos + 2 ) != 'n' ) && ( *(old_pos + 2 ) != 'N' )))
	  {
	    /* it's a 2-digit number spec */
	    *(new_pos++) = '0';
	    *(new_pos++) = '2';
	    *(new_pos++) = 'm';
	  } else if( (( *(old_pos + 3 ) != 't' ) && 
		      ( *(old_pos + 3 ) != 'T' )) ||
		     (( *(old_pos + 4 ) != 'h' ) && 
		      ( *(old_pos + 4 ) != 'H' )))
	  {
	    /* it's a month abbrev */
	    *(new_pos++) = 'b';
	  } else
	  {
	    /* full month */
	    *(new_pos++) = 'B';
	  }
	} else /* isout */
	{ 
	  /* date input spec m for months */
	  if( !sepwidth ) /* exactly two chars if there's no separator */
	  {
	    *(new_pos++) = '0';
	    *(new_pos++) = '2';
	  }	    
	  *(new_pos++) = 'm';
	}
      } else /* isdate */
      {
	/* time spec M for minutes */
	*(new_pos++) = '%';

	if( isout || !sepwidth )
	{
	  *(new_pos++) = '0';
	  *(new_pos++) = '2';
	}

	*(new_pos++) = 'M';
      }
      break;

    case 'd':
    case 'D':
      if( !isdate )
	return 0;

      /* always a 2-digit day for output */
      *(new_pos++) = '%';

      if( isout || !sepwidth )
      {
	*(new_pos++) = '0';
	*(new_pos++) = '2';
      }

      *(new_pos++) = 'd';
      break;

    case 'y':
    case 'Y':

      if( !isdate )
	return 0;

      *(new_pos++) = '%';

      if( isout )
      {
	/* decide if it's a 2-digit or 4-digit year */
	if((( *(old_pos + 1 ) == 'r' ) && ( *(old_pos + 1 ) == 'R' )) ||
	   !strncmp( old_pos + 1, "ear", 3 ) ||
	   !strncmp( old_pos + 1, "yyy", 3 ))
        {
	  /* four-digit year */
	  *(new_pos++) = 'Y';
	} else 
        {
	  /* two-digit year */
	  *(new_pos++) = '0';
	  *(new_pos++) = '2';
	  *(new_pos++) = 'C';
	}
      } else /* isout */
      {
	if( !sepwidth )
	{
	  *(new_pos++) = '0';
	  *(new_pos++) = '2';
	}

	*(new_pos++) = 'y';  /* input date always use y */
      }
      break;

    case 'h':
    case 'H':

      if( isdate )
	return 0;

      /* always a 2-digit hour for output */
      *(new_pos++) = '%';

      if( isout || !sepwidth )
      {
	*(new_pos++) = '0';
	*(new_pos++) = '2';
      }

      *(new_pos++) = 'H';

      break;

    case 's':
    case 'S':

      if( isdate )
	return 0;

      /* always a two-digit second for output */
      *(new_pos++) = '%';
      if( isout || !sepwidth )
      {
	*(new_pos++) = '0';
	*(new_pos++) = '2';
      }
      *(new_pos++) = 'S';

      break;

    default:
      /* invalid specification */
      return 0;
    }

    /* put in separator */
    if( sepwidth )
      *(new_pos++) = *next_sep;

    /* calculate next old string and separator positions */
    old_pos = next_sep + sepwidth;

    if( next_sep == sep1 )
      next_sep = sep2;
    else
      next_sep = old_format + len;
  }   /* loop to next field */

  /* put in null character */

  *new_pos = '\0';

  return 1;
}



/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME count_out_size

   DESCRIPTION Count up how much space is needed to output dates/times
   with the given new date/time output format string.

   ARGUMENTS
      IARG  new_format        the new-style format string
      IARG  abb_size          maximum size of abbreviations
      IARG  full_size         maximum size of day/month words
      IARG  zone_size         maximum size of time zones

   RETURN Returns the number of characters needed, or 0 if there is an
   error.

   ALGORITHM The function allots the width calculated by out_width for
   fields without specified widths, passing the size arguments to out_width.

   EXCEPTIONS 

   NOTE See also new_out_format
   
**********************************************************************/
int count_out_size( char *new_format, int abb_size, int full_size, 
			   int zone_size )
{
  char *pos, *endpos, tmp;
  int count, thiscount;

  if( !new_format )
    return 0;

  pos = new_format;
  count = 1; /* for end character */

  /* go through and count characters */

  while( *pos != '\0' )
  {
    if( *(pos++) != '%' )
    {
      /* not an output spec, so it just counts as a character */
      count++;
      continue;
    }

    /* it's an output spec.  */

    if( *pos == '%' )
    {
      /* % character spec */
      count++;
      pos++;
      continue;
    }

    /* see if it has a width spec */
    endpos = pos;
    while( isdigit( *endpos )) endpos++;

    if( endpos > pos )
    {
      if( !out_width( *endpos, abb_size, full_size, zone_size )) 
	return 0; /* wasn't a valid spec after all */

      /* read the width */
      tmp = *endpos;
      *endpos = '\0';
      if( sscanf( pos, "%d", &thiscount ) != 1 )
	return 0;
      *endpos = tmp;

      count += thiscount;
      pos = endpos + 1;
      continue;
    }

    /* no width spec, so just calculate standard width */
    if( !( thiscount = out_width( *pos, abb_size, full_size, zone_size ))) 
	return 0; /* wasn't valid spec */

    count += thiscount;
    pos++;
  }

  return( count );
  
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME out_width

   DESCRIPTION Calculate the maximum output width of a format spec.

   ARGUMENTS
      IARG  spec_char   the character code for formatting
      IARG  abb_size    maximum size of abbreviations
      IARG  full_size   maximum size of day/month words
      IARG  zone_size   maximum size of time zones

   RETURN Returns the width, or 0 if it's not a valid spec.

   ALGORITHM The function returns the appropriate size input argument
   depending on the format specification, and returns it. 
   It allots abb_size characters for abbreviated
   days and months and AM/PM, full_size for full-sized months and days,
   zone_size for time zones, and the appropriate width for numeric fields.

   EXCEPTIONS 

   NOTE See also count_out_size
   
**********************************************************************/
static int out_width( char spec_char, int abb_size, int full_size, 
		      int zone_size )
{
  switch( spec_char )
  {
  case 'a': /* abbreviated weekday */
  case 'b': /* abbreviated month */
  case 'p': /* AM/PM */
    return abb_size;
  case 'A': /* full weekday */
  case 'B': /* full month */
    return full_size;
  case 'Q': /* quarter as roman */
  case 'q': /* quarter as int */
    return 1;
  case 'C': /* 2-digit year */
  case 'd': /* day */
  case 'H': /* hour */
  case 'I': /* hour (12-hr clock) */
  case 'm': /* month */
  case 'M': /* minute */
  case 'S': /* second */
    return 2;
  case 'N': /* milliseconds */
  case 'D': /* day within year */
    return 3;
  case 'y': /* 2 or 4 digit year */
  case 'Y': /* 4 digit year */
    return 4;
  case 'Z': /* time zone */
  case 'z': /* time zone */
    return zone_size;
  default:
    return 0;
  }
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME output_one

   DESCRIPTION Use snprintf to print out one output spec

   ARGUMENTS
      IOARG out_buf      the buffer for printing (set to end of last print)
      IARG  td           structure containing time/date info
      IARG  topt         structure containing options for printing
      IARG  spec_char    the output spec character
      IARG  field_width  width of the field (-1 for default)
      IARG  zero_pad     pad with zeros if true; spaces otherwise

   RETURN Returns 1 if successful, 0 if not.

   ALGORITHM The function figures out what information from the
   td structure it needs to print, and prints it.  If zero_pad is
   true, field_width must be at least 1.  If the field width is
   at least 1, the output will be made exactly that wide, padded
   on the left with spaces, unless it's a numeric field and zero_pad 
   is true, which will cause padding with zeros instead. No attempt is 
   made to ensure that the string doesn't run over the output buffer,
   or that the given time/date information is valid.

   EXCEPTIONS 

   NOTE See also mdyt_format, input_one
   
**********************************************************************/
static int output_one( char **out_buf, TIME_DATE_STRUCT td, 
		       TIME_OPT_STRUCT topt, char spec_char, 
		       int field_width, int zero_pad )
{
  Sint print_val;
  int num_chars, i;
  char *print_str, *slash_pos = NULL;
  size_t n_bytes_remaining = field_width+1; // it is unclear to me if we need to account for trailing null here
    // We assume that all characters are single-byte.

  if( !out_buf || !(*out_buf ))
    return 0;
  if( !field_width )
    return 0;
  if( zero_pad && ( field_width < 1 ))
      return 0;

  /* figure out what to print */
  print_str = NULL;

  switch( spec_char )
  {
 /* first deal with the strings -- calculate value and print below */

  case 'a': /* abbreviated weekday */
    if(( td.weekday < 0 ) || ( td.weekday >= 7 ))
      return 0;
    if( !topt.day_abbs || !( print_str = topt.day_abbs[ td.weekday ] ))
      return 0;
    break;

  case 'A': /* full weekday */
    if(( td.weekday < 0 ) || ( td.weekday >= 7 ))
      return 0;
    if( !topt.day_names || !( print_str = topt.day_names[ td.weekday ] ))
      return 0;
    break;

  case 'b': /* abbreviated month */
    if(( td.month < 0 ) || ( td.month >= 13 ))
      return 0;
    if( !topt.month_abbs || !( print_str = topt.month_abbs[ td.month - 1 ] ))
      return 0;
    break;

  case 'B': /* full month */
    if(( td.month < 0 ) || ( td.month >= 13 ))
      return 0;
    if( !topt.month_names || !(print_str = topt.month_names[ td.month - 1 ] ))
      return 0;
    break;

  case 'Q': /* quarter as roman numeral */
    switch( (td.month-1) / 3 )
    {
    case 0:
      print_str = "I";
      break;
    case 1:
      print_str = "II";
      break;
    case 2:
      print_str = "III";
      break;
    case 3:
      print_str = "IV";
      break;
    default:
      return 0;
    }
    break;

  case 'Z': /* time zone */
    if( td.zone )
      print_str = td.zone;
    else
      return 0; 
    break;

  case 'z': /* time zone with daylight switch */
    if( !td.zone )
      return 0;
    /* print part of zone after slash for daylight, before for std,
       whole zone if no slash */
    slash_pos = strchr( td.zone, '/' );
    if( !slash_pos )
      print_str = td.zone;
    else
    {
      if( td.daylight )
      {
	print_str = slash_pos + 1;
	slash_pos = NULL;
      } else
      {
	print_str = td.zone;
	/* temporarily make slash a '\0' */
	*slash_pos = '\0';
      }
    }

    break;

  case 'p': /* AM/PM */
    if( !topt.am_pm )
      return 0;
    if( td.hour >= 12 )
      print_str = topt.am_pm[1];
    else
      print_str = topt.am_pm[0];
    if( !print_str )
      return 0;
    break;

 /* all the rest are longs; calculate value and print out below */

  case 'C': /* 2-digit year */
    print_val = td.year % 100;
    break;
     
  case 'd': /* day */
    print_val = td.day;
    break;

  case 'D': /* year day */
    print_val = td.yearday;
    break;

  case 'H': /* hour */
    print_val = td.hour;
    break;

  case 'I': /* hour (12-hr clock) */
    print_val = td.hour % 12;
    if( print_val < 1 ) 
      print_val += 12;
    break;

  case 'm': /* month */
    print_val = td.month;
    break;

  case 'M': /* minute */
    print_val = td.minute;
    break;

  case 'q': /* quarter of year */
    print_val = ((td.month-1) / 3 ) + 1;
    break;

  case 'S': /* second */
    print_val = td.second;
    break;

  case 'N': /* milliseconds */
    if( field_width == 1 )
      print_val = td.ms / 100;
    else if( field_width == 2 )
      print_val = td.ms / 10;
    else
      print_val = td.ms;
    break;

  case 'y': /* 2 or 4 digit year */
    if(( td.year >= topt.century ) && ( td.year < ( topt.century + 100 )))
       print_val = td.year % 100;
    else
       print_val = td.year;
    break;

  case 'Y': /* 4 digit year */
    print_val = td.year;
    break;

  default:
    return 0;
  }

  /* see if it's a string */
  if( print_str )
  {
    num_chars = strlen( print_str );

    /* we ignore zero padding for string fields */

    if( field_width > 0 )  /* have a field width */
    {
      if( field_width < num_chars ) /* insufficient space */
	num_chars = snprintf( *out_buf, n_bytes_remaining, "%*.*s", field_width, field_width, 
			     print_str );
      else /* sufficient space */
	num_chars = snprintf( *out_buf, n_bytes_remaining, "%*s", field_width, print_str );
      /* put slash back if necessary */
      if( slash_pos )
	*slash_pos = '/';

      if (num_chars >= n_bytes_remaining){
        return 0;
      }
      *out_buf += num_chars;
      return 1;
    }
      
    /* We have not been told the size of the output buffer, so we need to assume it is big enough */
    n_bytes_remaining = strlen(print_str) + 1;
    num_chars = snprintf( *out_buf, n_bytes_remaining, "%s", print_str );
    if (num_chars >= n_bytes_remaining){
      return 0;
    }
    /* put slash back if necessary */
    if( slash_pos )
      *slash_pos = '/';

    *out_buf += num_chars;
    return 1;
  }

  /* we are now printing out whatever print_val is as a Sint */

   if( zero_pad )
   {
     num_chars = snprintf( *out_buf, n_bytes_remaining, "%0*d", field_width, print_val );
     if( num_chars < field_width ) /* problem printing */
       return 0;
     if( num_chars >= n_bytes_remaining ) /* field too narrow, put in * characters */
       for( i = 0; i < field_width; i++ )
	 *(*out_buf + i) = '*';
     *out_buf += field_width;
     **out_buf = '\0';
     return 1;
   }

   if( field_width > 0 )
   {
     num_chars = snprintf( *out_buf, n_bytes_remaining, "%*d", field_width, print_val );
     if( num_chars < field_width )  /* problem printing */
       return 0;
     if( num_chars >= n_bytes_remaining ) /* field too narrow, put in * characters */
       for( i = 0; i < field_width; i++ )
	 *(*out_buf + i) = '*';
     *out_buf += field_width;
     **out_buf = '\0';
     return 1;
   }

   /* no particular width */
   /* We have not been told the size of the output buffer, so we need to assume it is big enough */
  n_bytes_remaining = 250;
  num_chars = snprintf( *out_buf, n_bytes_remaining, "%d", print_val );
  if( num_chars >= n_bytes_remaining )  /* problem printing */
    return 0;
  *out_buf += num_chars;
  return 1;
}



/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME parse_input

   DESCRIPTION  This function reads a date and time (given by 
   month, day, year, hour, minute, second, millisecond, and time
   zone string) from an input string according to the given format string.

   ARGUMENTS
      IARG  input_string    the formatted time/date string to read
      IARG  format_string   the new-style input format string
      IARG  topt            options for parsing times/dates
      IOARG td_output       the time/date information
      IARG  stopchar        the character to stop at in the string

   RETURN Returns 1/0 for success/failure.

   ALGORITHM The function uses the format string (see the R 
   documentation on time formats) to read the date/time
   from the input string.  It reads only until it gets to the 
   stop character, and fails if the end of the string is reached
   first (unless the null character is the stop character).  
   Optional format specifications are handled
   by calling this function recursively.  Single format specifications
   are read using the input_one function.  If the routine fails,
   the td_output structure may be changed anyway; if it succeeds, only
   members of the structure that were read will be changed.

   EXCEPTIONS 

   NOTE See also mdyt_input

**********************************************************************/
static int parse_input( char **input_string, char **format_string, 
			TIME_OPT_STRUCT topt, TIME_DATE_STRUCT *td_output, 
			char stopchar )
{
  char *formpos, *endpos, *nextlbr, *nextrbr, tmp, delim;
  char *inpos;
  TIME_DATE_STRUCT td_temp;
  int depth, width;

  if( !input_string || !(*input_string) || !format_string || 
      !(*format_string ) || !td_output )
    return 0;

  formpos = *format_string;
  inpos = *input_string;

  /* go through format string and find specs */

  while(( *formpos != stopchar ) && ( *formpos != '0' ))
  {
    if( *formpos == '[' )
    {
      /* optional specification -- parse recursively */

      /* first copy date to tmp td struct */
      memcpy( &td_temp, td_output, sizeof( TIME_DATE_STRUCT ));

      /* see if can parse optional part successfully */
      formpos++;
      if( parse_input( &inpos, &formpos, topt, &td_temp, ']' ))
      {
	/* was successful, so copy in the data */
	memcpy( td_output, &td_temp, sizeof( TIME_DATE_STRUCT ));
	
	continue;
      } else  /* parsing failed */
      {
	/* couldn't parse optional part, so skip it in the format */
	/* be careful about nesting */
	depth = 1;
	nextlbr = strchr( formpos, '[' );
	nextrbr = strchr( formpos, ']' );
	while( depth > 0 )
	{
	  if( !nextrbr ) /* failed to find a matching right bracket */
	    return 0; 

	  if( nextlbr && ( nextlbr < nextrbr ))
	  {
	    /* next bracket is a left, so increase depth */
	    depth++;
	    nextlbr = strchr( nextlbr + 1, '[' );
	    continue;
	  }

	  /* next bracket is a right, but we're not done yet */
	  depth--;
	  if( depth > 0 )
	    nextrbr = strchr( nextrbr + 1, ']' );

	} /* while depth */

	/* put format after position of nextrbr */
	if( nextrbr )
	  formpos = nextrbr + 1;
	else 
	  return 0;
      } /* if parsing worked or not */

      continue;
    } /* parsing turned up a left bracket */

    /* OK, we've dealt with the recursive bit.  Now other specs */

    if( isspace( *formpos )) /* skip white space in formats */
    {
      formpos++;
      continue;
    }

    while( isspace( *inpos )) /* skip white space in input */
      inpos++;

    if( *formpos != '%' ) /* character to match explicitly */
    {
      if( *inpos == *formpos ) /* does match */
      {
	inpos++;
	formpos++;
	continue;
      } else
	return 0;
    }

    /* if we get here, it's a % character, so see what it's for */
    formpos++;

    if(( *formpos == '%' ) || ( *formpos == '[' ) || (*formpos == ']' ))
    {
      /* characters to match explicitly */
      if( *inpos == *formpos ) /* does match */
      {
	inpos++;
	formpos++;
	continue;
      } else
	return 0;
    }

    /* ok, it's an honest to goodness input spec */

    /* check for delimeters */
    width = -1;
    delim = '\0';
    if( *formpos == ':' ) /* field delimeter */
    {
      width = -2;
      delim = *(formpos + 1 );
      formpos += 2;
    } else if( *formpos == '$' ) /* end of string */
    {
      width = -3;
      formpos++;
    } else 
    { /* find width */

      endpos = formpos;
      while( isdigit( *endpos )) endpos++;
      if( endpos > formpos ) 
      {
	/* read the width */
	tmp = *endpos;
	*endpos = '\0';

	if( sscanf( formpos, "%d", &width ) != 1 )
	  return 0;
	*endpos = tmp;
	formpos = endpos;
      } 
    } /* end search for width and delims */

    /* and finally, see if we can get whatever it was from the string */

    if( !input_one( &inpos, topt, *(formpos++), width, delim, td_output ))
      return 0;

  } /* outer while loop */

  if( *formpos != stopchar ) /* got to end of string instead of stop */
    return 0;

  /* set the positions */

  *input_string = inpos;
  *format_string = formpos + 1; /* skip the stop character */

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME input_one

   DESCRIPTION Use sscanf to read a time/date from one input spec

   ARGUMENTS
      IOARG input_string  the buffer for reading (set to end of last read)
      IARG  topt          structure containing options for reading
      IARG  spec_char     the input spec character
      IARG  width         width of the field (see below)
      IARG  delim         delimeter for field
      IOARG td_out        structure for time/date info

   RETURN Returns 1 if successful, 0 if not.

   ALGORITHM The function figures out what it's supposed to try to
   find in the input string, and tries as hard as it can to find it
   and use up as much of the string as possible.  Leading white space
   is skipped first, and any fixed widths start after white space.
   \\
   \\ 
   The w specification means to skip a white space delimited word;
   width and delimeters are ignored.
   \\
   \\
   For other specifications, 
   if width is -2, the field goes to the next occurrence of the delimeter,
   and the input buffer will be set to just before the delimeter.
   If width is -3, the field goes to the end of the string.
   If width is positive, that number of characters are used.
   Otherwise, a c specification skips one character,
   numeric fields use up all contiguous numeric digits and 
   string fields go to the next white space, except that 
   character months go until the next non-letter.
   \\
   \\
   Text strings for things like months and AM/PM are converted to 
   their numerical counterparts by calling the match_index function
   with the options structure.

   EXCEPTIONS 

   NOTE See also parse_input
   
**********************************************************************/
static int input_one( char **input_string, TIME_OPT_STRUCT topt, 
		      char spec_char, int width, char delim, 
		      TIME_DATE_STRUCT *td_out )
{

  char *inpos, *endpos;
  char *pos, tmpchr;
  int isnum, len;
  Sint tmplng;

  if( !input_string || !(*input_string) || !td_out || !width )
    return 0;

  inpos = *input_string;

  /* skip white space */

  while(( *inpos != '\0' ) && ( isspace( *inpos )))
    inpos++;

  /* special case w (skip word) spec, which doesn't support widths/delims */

  if( spec_char == 'w' )
  {
    while(( *inpos != '\0' ) && ( !isspace( *inpos )))
      inpos++;
    *input_string = inpos;
    return( 1 );
  }

  /* locate the end of this field. */
  /* easy if a delimeter is specified or if it's end of string */
  /* endpos variable is actually just after end of field */
  /* also determine if it's numeric or string */

  isnum = -1;

  if( width == -2 )
    endpos = strchr( inpos, delim );
  else if( width == -3 )
    endpos = inpos + strlen( inpos );
  else if( width > 0 )
    endpos = inpos + width;
  else  /* variable width field */
  {
    switch( spec_char )
    {
    case 'c':
      /* c field is always 1 if no width specified */
      isnum = 0;
      endpos = inpos + 1;
      break;

    case 'd':
    case 'H':
    case 'M':
    case 'N':
    case 'n':
    case 'S':
    case 'y':
    case 'Y':
      /* numeric integer fields -- go to next non-digit */
      isnum = 1;
      endpos = inpos;
      while(( *endpos != '\0' ) && isdigit( *endpos ))
	endpos++;
      break;

    case 'p':
    case 'Z':
      /* character fields -- go to next white space */
      isnum = 0;
      endpos = inpos;
      while(( *endpos != '\0' ) && ( !isspace( *endpos )))
	endpos++;
      break;

    case 'm':
      /* months: num or string -- decide by first character */
      endpos = inpos;
      isnum = isdigit( *endpos );
      if( isnum )
      {
	while(( *endpos != '\0' ) && isdigit( *endpos ))
	  endpos++;
	isnum = 1;
      }
      else
	while(( *endpos != '\0' ) && isalpha( *endpos ))
	  endpos++;

      break;

    default: /* unknown spec */
      return 0;
    } /* end of switch statement */
    
  } /* if/else on width */

  /* get isnum right if we didn't set it */

  if( isnum == -1 )
  {
    switch( spec_char )
    {
    case 'c':
    case 'p':
    case 'Z':
      isnum = 0;
      break;

    case 'd':
    case 'H':
    case 'M':
    case 'N':
    case 'n':
    case 'S':
    case 'y':
    case 'Y':
      isnum = 1;
      break;

    case 'm':
      /* months: num or string -- decide by first character */
      isnum = isdigit( *inpos );
      if( isnum )
	isnum = 1;
      break;

    default: /* unknown spec */
      return 0;
    } /* end of switch statement */
  }

  /*  OK, now we should have determined the end position */
  /* verify that it's OK */

  if( endpos <= inpos )
    return 0;

  /*LINTED: cast should be OK here */
  width = (int) (endpos - inpos);

  /* verify that string isn't too short for this */
  if( strlen( inpos ) < width )
    return 0;

  /* OK.  Now convert it to desired input */

  if( isnum )
  {
    /* first one more bit of error checking */
    
    for( pos = inpos; pos < endpos; pos++ )
      if( !isdigit( *pos ))
	return 0;
    
    /* temporarily put a null char at end and read with scanf */
    tmpchr = *endpos;
    *endpos = '\0';

    if( sscanf( inpos, "%d", &tmplng ) != 1 ) /* this should always work */
    {
      *endpos = tmpchr;
      return 0;
    }

    *endpos = tmpchr;
  }

  /* now copy into proper field */

  switch( spec_char )
  {
  case 'c':
    /* skip to end of field is done after this switch */
    break;

  case 'd': /* day */
    td_out->day = tmplng;
    break;
 
  case 'H': /* hour */
    td_out->hour = tmplng;
    break;

  case 'M': /* minute */
    td_out->minute = tmplng;
    break;

  case 'S': /* second */
    td_out->second = tmplng;
    break;

  case 'Y': /* year */
    td_out->year = tmplng;
    break;

  case 'y': /* year with century addition */
    td_out->year = tmplng;

    if( td_out->year < 100 )
    {
      if( td_out->year < ( topt.century % 100 ))
	td_out->year += ( topt.century / 100 ) * 100 + 100;
      else
	td_out->year += ( topt.century / 100 ) * 100;

    }
    break;

  case 'n': /* millisecond but always 1000ths */
    td_out->ms = tmplng;
    break;

  case 'N': /* millisecond */
    /* if field was *specified* 1 or 2 wide read as 10ths or 
       100ths of seconds instead of 1000ths */
    td_out->ms = tmplng;
    if( width == 1 )
      td_out->ms *= 100;
    else if( width == 2 )
      td_out->ms *= 10;
    break;

  case 'Z': /* time zone */
    /* Copy the zone into a new string in td_out */
    /* since we allways use Ralloc, we don't have to worry about freeing */
    td_out->zone = R_alloc( width + 1, sizeof(char) );
    strncpy( td_out->zone, inpos, width );
    td_out->zone[ width ] = '\0';

    break;

  case 'p': /* am/pm */
    /* find matching index in am/pm if possible */

    tmpchr = *endpos;
    *endpos = '\0';
    len = match_index( topt.am_pm, 2, inpos );
    *endpos = tmpchr;

    if(( len != 1 ) && ( len != 2 ))
      return 0;
    if(( len == 2 ) && ( td_out->hour < 12 )) /* pm -- change to 24 hour */
      td_out->hour += 12;
    else if(( len == 1 ) && ( td_out->hour == 12 )) /* am -- change to 0 */
      td_out->hour = 0;

    break;

  case 'm': /* months: num or string */
    if( isnum )
      td_out->month = tmplng;
    else
    {
      tmpchr = *endpos;
      *endpos = '\0';
      td_out->month = match_index( topt.month_names, 12, inpos );
      *endpos = tmpchr;
      if( td_out->month < 1 )
	return 0;
    }

    break;

    default: /* unknown spec */
      return 0;

  }  /* end of switch statement */

  /* reset input string position */
  
  *input_string = endpos;
  return 1;
}  


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME match_index

   DESCRIPTION Find which element in an array matches a string uniquely,
   with case-sensitive sub-string matching.

   ARGUMENTS
      IARG  str_array     the array to look for matches in
      IARG  array_len     the length of the str_array array
      IARG  match_str     the string to try to match

   RETURN Returns the index (between 1 and length) if successful, 
   and 0 if not.

   ALGORITHM The function checks to see how many characters from 
   each element of str_array match (case-insensitive) match_str.  It returns
   the index of the best-matching element if it matches better than any
   of the other elements, and 0 if two or more elements tie for 
   unique matches.  For example, if the input array contains the names
   of the calendar months, the input strings ``Mar'', ``March'', and 
   ``mar'' would all return 3, but ``Ma'' would return 0 because it 
   also matches ``May'', and ``Marchx'' would also return 0 because
   it doesn't match ``March''.

   EXCEPTIONS 

   NOTE See also input_one
   
**********************************************************************/
static int match_index( char **str_array, int array_len, char *match_str )
{
  int most_matched, unique, which_matched, this_matched, i, j, 
    our_len, this_len ;

  if( !str_array || ( array_len <= 0 ) || !match_str )
    return 0;

  which_matched = -1;
  most_matched = 0;
  unique = 0;
  our_len = strlen( match_str );

  for( i = 0; i < array_len; i++ )
  {
    if( !str_array[i] )
      continue;
    this_len = strlen( str_array[i] );

    /* see how many characters we match */
    this_matched = 0;
    for( j = 0; ( j < this_len ) && ( j < our_len ); j++ )
      if( tolower( match_str[j] ) == tolower( str_array[i][j] ))
	this_matched = j;
      else
	break;

    /* verify we got to end of the input string */
    if( j < our_len )
      continue; 

    if( this_matched == most_matched )
      unique = 0;
    else if( this_matched > most_matched )
    {
      unique = 1;
      most_matched = this_matched;
      which_matched = i;
    }
  } /* for loop going through all strings in array */

  if( !unique )
    return 0;
  return( which_matched + 1 );
}
