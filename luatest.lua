local mylib = require "tdc_gen"


print(string.format("%x",20))
--print(type(mylib))
--a,b,c,d,e,f,g,h,i,j = mylib.tdc1000_gen(0.7692,1000000,5,8000000)
--a,b,c,d,e,f,g,h,i,j = mylib.tdc7200_gen()
a,b,c,d,e,f,g = mylib.tdc_by_temperature(21.1,8000000,0.7692,1000000,17)
--print(math.floor(a),math.floor(b))
print(a,b,c,d,e)
