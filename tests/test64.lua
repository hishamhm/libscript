function anirudh.plugin_test(x, d)
	--print (("Lua got %s from C"):format(tostring(x)));
	if d==0 then anirudh.return_val_i(x) else anirudh.return_val_d(x) end
end