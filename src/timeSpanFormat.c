/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

/*************************************************************************
 *
 * It contains C code utility functions for converting time spans
 * to and from strings.
 * 
 * See timeSpanFormat.h for a more compact listing of the included functions.
 * They were written as auxiliary functions for R code
 * dealing with times and dates.
 *************************************************************************/

#include "timeSpanFormat.h" 

/* internal function declarations -- see end of file for defs/docs */
static int out_width( char spec_char );
static int output_one( char **out_buf, TIME_DATE_STRUCT td,
		       char spec_char, int field_width, int zero_pad );
static int parse_input( char **input_string, char **format_string, 
			Sint *julian, Sint *ms, char stopchar );
static int input_one( char **input_string, char spec_char, int width, 
		      char delim, Sint *julian, Sint *ms );

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME tspan_format

   DESCRIPTION  This function formats a date and time (given by 
   a time-date structure) according to the given format string and options.

   ARGUMENTS
      IARG  format_string   the new-style-output format string
      IARG  julian          the number of days
      IARG  ms              the number of milliseconds
      OARG  ret_string      the formatted time/date string

   RETURN Returns 1/0 for success/failure.

   ALGORITHM The function uses the time span format string (see the R 
   documentation on time span formats) to print the given time span
   into the string, which must be pre-allocated to 
   the correct length (which can be calculated by the tspan_output_length
   function).   Each format specification is printed into the string
   by calling the output_one function.

   EXCEPTIONS 

   NOTE See also tspan_input

**********************************************************************/
int tspan_format(const char *format_string, Sint julian, Sint ms,
		  char *ret_string )
{
  char *inpos, *outpos, *endpos, tmp;
  int width, zeropad;
  TIME_DATE_STRUCT td;

  if( !format_string || !ret_string )
    return 0;

  /* since R caches R string objects, need to copy input */
  inpos = acopy_string(format_string);

  outpos = ret_string;

  /* convert ms to h/m/s/ms and put days into struct */
  td.day = julian;
  if( ms > 0 )
  {
    if( !ms_to_hms( ms, &td ))
      return 0;
  } else {
    if( !ms_to_hms( - ms, &td ))
      return 0;
    td.hour = - td.hour;
    td.minute = - td.minute;
    td.second = - td.second;
    td.ms = - td.ms;
  }
  
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
    while( isdigit( *endpos ) || ( *endpos == '-' )) endpos++;
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

    if( !output_one( &outpos, td, *(inpos++), width, zeropad ))
      return 0;
  }

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME tspan_input

   DESCRIPTION  This function reads a time span from an input string 
   according to the given format string.

   ARGUMENTS
      IARG  input_string    the formatted string to read
      IARG  format_string   the time span input format string
      OARG  julian          the number of days in the span
      OARG  ms              the number of milliseconds in the span

   RETURN Returns 1/0 for success/failure.  The routine fails if the 
   format does not match the input, or if the input string is not 
   entirely read (except whitespace).

   ALGORITHM The function calls parse_input to parse the input.

   EXCEPTIONS 

   NOTE   See also tspan_format

**********************************************************************/
int tspan_input( const char *input_string, const char *format_string, 
		 Sint *julian, Sint *ms )
{
  char *in_str, *in_fmt, *input_end;

  /* error checking */
  if( !input_string || !format_string || !julian || !ms )
    return 0;

  /* zero out the return values */
  *julian = *ms = 0;

  /* since R caches R string objects, need to copy input */
  in_str = acopy_string(input_string);
  in_fmt = acopy_string(format_string);

  /* parse input with format */
  input_end = in_str + strlen( in_str );

  if( !parse_input( &in_str, &in_fmt, julian, ms, '\0' ))
    return 0;

  /* check to see that input string is used up except whitespace */
  while( in_str && ( in_str < input_end ))
    if( !isspace( *(in_str++)))
      return 0;

  return 1;
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME tspan_output_length

   DESCRIPTION Count up how much space is needed to output time spans
   with the given output format string.

   ARGUMENTS
      IARG  format_string     the time span format string

   RETURN Returns the number of characters needed, or 0 if there is an
   error.

   ALGORITHM The function allots the width calculated by out_width for
   fields without specified widths.

   EXCEPTIONS 

   NOTE
   
**********************************************************************/
int tspan_output_length(const char *format_string )
{
  char *pos, *endpos, tmp;
  int count, thiscount;

  if( !format_string )
    return 0;

  /* since R caches R string objects, need to copy input */
  pos = acopy_string(format_string);

  count = 0;

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
    while( isdigit( *endpos ) || ( *endpos == '-' )) endpos++;

    if( endpos > pos )
    {
      if( !out_width( *endpos ))
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
    if( !( thiscount = out_width( *pos )))
	return 0; /* wasn't valid spec */

    count += thiscount;
    pos++;
  }

  return( count );
  
}


/***************************
 Internal Functions
 ***************************/

/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME out_width

   DESCRIPTION Calculate the maximum output width of a format spec.

   ARGUMENTS
      IARG  spec_char   the character code for formatting

   RETURN Returns the width, or 0 if it's not a valid spec.

   ALGORITHM The function returns the appropriate size input argument
   depending on the format specification, and returns it. 

   EXCEPTIONS 

   NOTE See also count_out_size
   
**********************************************************************/
static int out_width( char spec_char )
{
  switch( spec_char )
  {
  case 'd': /* total number of days -- allow for, say, 2000 yrs * 365 days = 
	       730,000  + 1 for sign */
  case 'W': /* total number of weeks */
  case 'y': /* years + 1 for sign */
    return 10; /* for all of these build in some slack */

  case 's': /* seconds in 1 day max 86400 + 1 for sign */
    return 6;

  case 'D': /* days in 1 year max + 1 for sign */
  case 'N': /* ms in 1 sec max + 1 for sign */
    return 4;

  case 'H': /* hours in 1 day max + 1 for sign */
  case 'M': /* minutes in 1 hour max + 1 for sign */
  case 'S': /* seconds in 1 minute max + 1 for sign */
    return 3;

  case 'E': /* days in 1 week max + 1 for sign */
    return 2;

  default:
    return 0;
  }
}


/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME output_one

   DESCRIPTION Use sprintf to print out one output spec

   ARGUMENTS
      IOARG out_buf      the buffer for printing (set to end of last print)
      IARG  td           structure containing time span
      IARG  spec_char    the output spec character
      IARG  field_width  width of the field (-1 for default)
      IARG  zero_pad     pad with zeros if true; spaces otherwise

   RETURN Returns 1 if successful, 0 if not.

   ALGORITHM The function prints the needed portion of the day, hour, 
   minute, second, and ms elements of the td structure according
   to the spec character.  If zero_pad is
   true, field_width must be at least 1.  If the field width is
   at least 1, the output will be made exactly that wide, padded
   on the left with spaces, unless it's a numeric field and zero_pad 
   is true, which will cause padding with zeros instead. No attempt is 
   made to ensure that the string doesn't run over the output buffer,
   or that the given time span is valid.

   EXCEPTIONS 

   NOTE See also tspan_format, input_one
   
**********************************************************************/
static int output_one( char **out_buf, TIME_DATE_STRUCT td,
		       char spec_char, int field_width, int zero_pad )
{
  Sint print_val;
  int num_chars;

  if( !out_buf || !(*out_buf ))
    return 0;
  if( !field_width )
    return 0;
  if( zero_pad && ( field_width < 1 ))
      return 0;

  /* figure out what to print */
  switch( spec_char )
  {
     
  case 'd': /* days */
    print_val = td.day;
    break;

  case 'D': /* days subtracting off years */
    print_val = td.day % 365;
    /* get sign correct */
    if(( td.day < 0 ) && ( print_val > 0 ))
      print_val -= 365;
    break;

  case 'E': /* days subtracting off weeks */
    print_val = td.day % 7;
    /* get sign correct */
    if(( td.day < 0 ) && ( print_val > 0 ))
      print_val -= 7;
    break;

  case 'H': /* hour */
    print_val = td.hour;
    break;

  case 'M': /* minute */
    print_val = td.minute;
    break;

  case 'N': /* milliseconds */
    print_val = td.ms;
    break;

  case 'S': /* second */
    print_val = td.second;
    break;

  case 's': /* total seconds in day*/
    print_val = td.second + 60 * td.minute + 3600 * td.hour;
    break;

  case 'W': /* weeks */
    /* first calculate as in 'E' days subtracting weeks */
    print_val = td.day % 7;
    if(( td.day < 0 ) && ( print_val > 0 ))
      print_val -= 7;
    /* now go back to td.day and subtract this calculated value 
       and divide by 7 */
    print_val = ( td.day - print_val ) / 7;
    break;

  case 'y': /* 365-day years */
    /* first calculate as in 'D' days subtracting years */
    print_val = td.day % 365;
    if(( td.day < 0 ) && ( print_val > 0 ))
      print_val -= 365;
    /* now go back to td.day and subtract this calculated value 
       and divide by 365 */
    print_val = ( td.day - print_val ) / 365;    
    break;

  default:
    return 0;
  }

  /* now print out whatever print_val is as a Sint */

   if( zero_pad )
   {
     num_chars = sprintf( *out_buf, "%0*d", field_width, print_val );
     if( num_chars != field_width ) 
       return 0;
     *out_buf += num_chars;
     return 1;
   }

   if( field_width > 0 )
   {
     num_chars = sprintf( *out_buf, "%*d", field_width, print_val );
     if( num_chars != field_width )
       return 0;
     *out_buf += num_chars;
     return 1;
   }

   /* no particular width */
  num_chars = sprintf( *out_buf, "%d", print_val );
  if( num_chars < 1 )
    return 0;
  *out_buf += num_chars;
  return 1;
}



/**********************************************************************
 * C Code Documentation ************************************************
 **********************************************************************
   NAME parse_input

   DESCRIPTION  This function reads a time span
   from an input string according to the given format string.

   ARGUMENTS
      IARG  input_string    the formatted time/date string to read
      IARG  format_string   the new-style input format string
      IOARG julian          the number of days in the span
      IARG  ms              the number of milliseconds in the span
      IARG  stopchar        the character to stop at in the string

   RETURN Returns 1/0 for success/failure.

   ALGORITHM The function uses the format string (see the R 
   documentation on time span formats) to read the date/time
   from the input string.  It reads only until it gets to the 
   stop character, and fails if the end of the string is reached
   first (unless the null character is the stop character).  
   Optional format specifications are handled
   by calling the function recursively.  Individual format specifications
   are read using the input_one function. 

   EXCEPTIONS 

   NOTE See also mdyt_input

**********************************************************************/
static int parse_input( char **input_string, char **format_string, 
			Sint *julian, Sint *ms, char stopchar )
{
  char *formpos, *inpos, *endpos, *nextlbr, *nextrbr, tmp, delim;
  Sint juladd, msadd;
  int depth, width;

  if( !input_string || !(*input_string) || !format_string || 
      !(*format_string ) || !julian || !ms )
    return 0;

  formpos = *format_string;
  inpos = *input_string;

  /* go through format string and find specs */

  while(( *formpos != stopchar ) && ( *formpos != '0' ))
  {
    if( *formpos == '[' )
    {
      /* optional specification -- parse recursively */

      /* see if can parse optional part successfully */
      formpos++;
      juladd = msadd = 0;
      if( parse_input( &inpos, &formpos, &juladd, &msadd, ']' ))
      {
	/* was successful, so add in the new spans */
	*ms += msadd;
	*julian += juladd;

	/* carry ms into julian if necessary */
	if( !adjust_span( julian, ms ))
	  return 0;
	
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

    if( *formpos != '%' ) /* character to match explicitely */
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
      while( isdigit( *endpos ) || ( *endpos == '-' )) endpos++;
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

    if( !input_one( &inpos, *(formpos++), width, delim, julian, ms ))
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

   DESCRIPTION Use sscanf to read a time span from one input spec

   ARGUMENTS
      IOARG input_string  the buffer for reading (set to end of last read)
      IARG  spec_char     the input spec character
      IARG  width         width of the field (see below)
      IARG  delim         delimeter for field
      IOARG julian        number of days in span
      IOARG ms            number of ms in span

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
   Otherwise, a c specification skips one character, and 
   numeric fields use up all contiguous numeric digits.

   EXCEPTIONS 

   NOTE See also parse_input
   
**********************************************************************/
static int input_one( char **input_string, char spec_char, int width, 
		      char delim, Sint *julian, Sint *ms )
{
  char *inpos, *endpos, *pos, tmpchr;
  Sint tmplng;

  if( !input_string || !(*input_string) || !julian || !ms || !width )
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

  if( width == -2 )
    endpos = strchr( inpos, delim );
  else if( width == -3 )
    endpos = inpos + strlen( inpos );
  else if( width > 0 )
    endpos = inpos + width;
  else  /* variable width field */
  {
    if( spec_char == 'c' ) /* c field is always 1 if no width specified */
      endpos = inpos + 1;
    else 
    {
      /* numeric integer fields -- go to next non-digit */
      endpos = inpos;
      while(( *endpos != '\0' ) && ( isdigit( *endpos ) || ( *endpos == '-' )))
	endpos++;
    }
  } /* if/else on width */

  /*  OK, now we should have determined the end position */
  /* verify that it's OK */

  if( endpos <= inpos )
    return 0;

  /*LINTED: cast should be OK here */
  width = (int) (endpos - inpos);

  /* verify that string isn't too short for this */
  if( strlen( inpos ) < width )
    return 0;

  /* special case 'c' field -- skip input */
  if( spec_char == 'c' )
  {
    *input_string = endpos;
    return 1;
  }

  /* OK.  Now convert to desired input */
  /* first one more bit of error checking */
    
  for( pos = inpos; pos < endpos; pos++ )
    if( !isdigit( *pos ) && ( *pos != '-' ))
      return 0;
    
  /* temporarily put a null char at end and read with scanf */
  tmpchr = *endpos;
  *endpos = '\0';

  if( sscanf( inpos, "%d", &tmplng ) != 1 ) /* this may fail */
  {
    *endpos = tmpchr;
    return 0;
  }

  *endpos = tmpchr;

  /* now add in proper amount */
  switch( spec_char )
  {

  case 'd': /* day */
    *julian += tmplng;
    break;
 
  case 'H': /* hour */
    /* Need to carry hours to days now, before muliplying by 3600000.
     * Would need to fix up for negative tmplng, except that adjust_span()
     * will normalize julian,ms later.
     */
    *julian += tmplng / 24 ;
    tmplng %= 24 ;
    *ms += tmplng * 3600000;
    break;

  case 'M': /* minute */
    *julian += tmplng / (24 * 60) ;
    tmplng %= 24 * 60 ;
    *ms += tmplng * 60000;
    break;

  case 'N': /* milliseconds */
    *ms += tmplng;
    break;

  case 'S': /* second */
    *julian += tmplng / (24 * 60 * 60) ;
    tmplng %= 24 * 60 * 60 ;
    *ms += tmplng * 1000;
    break;

  case 'W': /* weeks */
    *julian += 7 * tmplng;
    break;

  case 'y': /* 365-day years */
    *julian += tmplng * 365;
    break;

  default: /* unknown spec */
    return 0;

  }  /* end of switch statement */

  /* reset input string position */
  
  *input_string = endpos;

  /* carry ms into julian if necessary */
  if( !adjust_span( julian, ms ))
    return 0;

  return 1;
}  


