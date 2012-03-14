# $MawkId: reg4.awk,v 1.7 2009/09/17 23:29:01 tom Exp $
{
	if ($0 ~/^[-+()0-9.,$%/'"]*$/)
	{
		print ("reg4.1<<:",$0,">>")
	}
	if ($0 ~/^[]+()0-9.,$%/'"-]*$/)
	{
		print ("reg4.2<<:",$0,">>")
	}
	if ($0 ~/^[^]+()0-9.,$%/'"-]*$/)
	{
		print ("reg4.3<<:",$0,">>")
	}
	if ($0 ~/^[[+(){}0-9.,$%/'"-]*$/)
	{
		print ("reg4.4<<:",$0,">>")
	}
	if ($0 ~/^[^[+(){}0-9.,$%/'"-]*$/)
	{
		print ("reg4.5<<:",$0,">>")
	}
}
