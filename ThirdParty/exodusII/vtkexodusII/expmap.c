/*
 * Copyright (c) 2005 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Governement
 * retains certain rights in this software.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.  
 * 
 *     * Neither the name of Sandia Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*****************************************************************************
*
* expmap - ex_put_map
*
* entry conditions - 
*   input parameters:
*       int     exoid                   exodus file id
*       int*    elem_map                element order map array
*
* exit conditions - 
*
* revision history - 
*
*
*****************************************************************************/

#include "exodusII.h"
#include "exodusII_int.h"
#include <stdlib.h> /* for free() */

/*!
\deprecated Use ex_put_num_map() instead.

The function ex_put_map() writes out the optional element order map to
the database. See #ElementOrderMap for a description of the element
order map. The function ex_put_init() must be invoked before this call
is made.

In case of an error, ex_put_map() returns a negative number; a warning
will return a positive number.  Possible causes of errors include:
  -  data file not properly opened with call to ex_create() or ex_open()
  -  data file opened for read only.
  -  data file not initialized properly with call to ex_put_init().
  -  an element map already exists in the file.

\param[in]   exoid    exodus file ID returned from a previous call to ex_create() or ex_open().
\param[in]  elem_map  The element order map.

The following code generates a default element order map and outputs
it to an open exodus file. This is a trivial case and included just
for illustration. Since this map is optional, it should be written out
only if it contains something other than the default map.

\code
int error, exoid;
int *elem_map = (int *)calloc(num_elem, sizeof(int));
for (i=0; i < num_elem; i++) {
   elem_map[i] = i+1;
}
error = ex_put_map(exoid, elem_map);
\endcode

 */

int ex_put_map (int  exoid,
                const int *elem_map)
{
  int numelemdim, dims[1], mapid, status;
  char errmsg[MAX_ERR_LENGTH];

  exerrval = 0; /* clear error code */

  /* inquire id's of previously defined dimensions  */

  /* determine number of elements. Return if zero... */
  if (nc_inq_dimid(exoid, DIM_NUM_ELEM, &numelemdim) != NC_NOERR)
    {
      return (EX_NOERR);
    }

  /* put netcdf file into define mode  */

  if ((status = nc_redef (exoid)) != NC_NOERR)
    {
      exerrval = status;
      sprintf(errmsg,
        "Error: failed to put file id %d into define mode",
        exoid);
      ex_err("ex_put_map",errmsg,exerrval);
      return (EX_FATAL);
    }


  /* create a variable array in which to store the element map  */

  dims[0] = numelemdim;

  if ((status = nc_def_var(exoid, VAR_MAP, NC_INT, 1, dims, &mapid)) != NC_NOERR)
    {
      if (status == NC_ENAMEINUSE)
  {
    exerrval = status;
    sprintf(errmsg,
      "Error: element map already exists in file id %d",
      exoid);
    ex_err("ex_put_map",errmsg,exerrval);
  }
      else
  {
    exerrval = status;
    sprintf(errmsg,
      "Error: failed to create element map array in file id %d",
      exoid);
    ex_err("ex_put_map",errmsg,exerrval);
  }
      goto error_ret;         /* exit define mode and return */
    }


  /* leave define mode  */
  if ((status = nc_enddef (exoid)) != NC_NOERR)
    {
      exerrval = status;
      sprintf(errmsg,
        "Error: failed to complete definition in file id %d",
        exoid);
      ex_err("ex_put_map",errmsg,exerrval);
      return (EX_FATAL);
    }


  /* write out the element order map  */
  status = nc_put_var_int(exoid, mapid, elem_map);

  if (status != NC_NOERR)
    {
      exerrval = status;
      sprintf(errmsg,
        "Error: failed to store element map in file id %d",
        exoid);
      ex_err("ex_put_map",errmsg,exerrval);
      return (EX_FATAL);
    }

  return (EX_NOERR);

  /* Fatal error: exit definition mode and return */
 error_ret:
  if (nc_enddef (exoid) != NC_NOERR)     /* exit define mode */
    {
      sprintf(errmsg,
        "Error: failed to complete definition for file id %d",
        exoid);
      ex_err("ex_put_map",errmsg,exerrval);
    }
  return (EX_FATAL);
}

