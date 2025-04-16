/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "claves-rpc.h"

bool_t
xdr_Coord_RPC (XDR *xdrs, Coord_RPC *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->x))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->y))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_args_in (XDR *xdrs, args_in *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->res))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->value1, 256))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->N_value2))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->V_value2.V_value2_val, (u_int *) &objp->V_value2.V_value2_len, 32,
		sizeof (double), (xdrproc_t) xdr_double))
		 return FALSE;
	 if (!xdr_Coord_RPC (xdrs, &objp->value3))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_key_args_in (XDR *xdrs, key_args_in *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->key))
		 return FALSE;
	 if (!xdr_args_in (xdrs, &objp->tupla))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_args_out (XDR *xdrs, args_out *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->res))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->value1, 256))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->N_value2))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->V_value2.V_value2_val, (u_int *) &objp->V_value2.V_value2_len, 32,
		sizeof (double), (xdrproc_t) xdr_double))
		 return FALSE;
	 if (!xdr_Coord_RPC (xdrs, &objp->value3))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	return TRUE;
}
