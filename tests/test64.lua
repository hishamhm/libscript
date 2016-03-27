
print("Hello from Lua!\n")
function test(x)
	print("Lua is sending "..tostring(x).." to C [int64 fun(int64)]")
	y = anirudh.hello(x)
	print("Lua got "..tostring(y).." from C [int64 fun(int64)]")
	print("Lua is sending "..tostring(x).." to C [double64 fun(int64)]")
	y = anirudh.hello2(x)
	print("Lua got "..tostring(y).." from C [double64 fun(double64)]")
end
test(20.412)
test(9007199254740995) --64-bit integer in Lua 5.3
test(9007199254740995213)
test(3743832098472309478323287435)