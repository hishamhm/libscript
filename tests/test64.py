def plugin_test (x, d):
	print("Python got "+str(x)+" from C")
	if d == 0 :
		anirudh.return_val_i(x)
	else:
		anirudh.return_val_d(x)

anirudh.plugin_test = plugin_test