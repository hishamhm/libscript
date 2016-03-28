sub Anirudh::plugin_test
{
	$x = $_[0];
	$d = $_[1];
	if ($d==0)
	{
		Anirudh::return_val_i($x);
	}
	else
	{
		Anirudh::return_val_d($x);
	}
}