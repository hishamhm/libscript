def Anirudh.plugin_test(x, d)
	#print ("Ruby got "+x.to_s+" from C\n");
	if d==0
		Anirudh.return_val_i(x)
	else
		Anirudh.return_val_d(x)
	end
end