
def an_exception():
   raise Exception("An exception raised from Python code")

testexc.an_exception = an_exception

def oops():
   testexc.an_error = None
   foobarbla()

testexc.an_error = oops
oops = None
